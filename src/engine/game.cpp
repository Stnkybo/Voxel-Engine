//
// Created by Lamad on 11/12/2024.
//

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>


#include <iostream>
#include <ostream>

#include "../config.h"

#include "game.h"
#include "rendering/backends/vulkanBackend.h"

#include "rendering/shader.h"
#include "camera/camera.h"
#include "client/playerRaycaster.h"
#include "physics/collisions/worldCollision.h"
#include "rendering/model.hpp"
#include "terrain/world.h"

void imguiUI(ImGuiIO& io);

Game::Game(const char* title, const int width, const int height): m_imguiIO() {
        m_title = title;
        m_height = height;
        m_width = width;
}

Game::~Game() {
    clean();
}



void Game::handleEvents() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0) {

        // This Allows Imgui to handle events
        ImGui_ImplSDL3_ProcessEvent(&ev);

        switch(ev.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                // Set Window Dimensions
                SDL_GetWindowSizeInPixels(m_window, &m_width, &m_height);
                glViewport(0, 0, m_width, m_height);  // Update the OpenGL viewport
                break;


            case SDL_EVENT_QUIT:
                isRunning = false;
            break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN: {

                auto blockraycast = playerVoxelInteractionRaycast(player->camera->Position, player->camera->Front, 5.0f, world);

                if (ev.button.button == SDL_BUTTON_LEFT) {
                    // The left button was pressed or released
                    // destory block

                    if (blockraycast.hit) {
                        world->setBlock(blockraycast.blockPosition, BlockType::AIR);
                        std::cout << "Destoryed Block" << std::endl;


                    }
                }
                else if (ev.button.button == SDL_BUTTON_RIGHT) {
                    // The right button was pressed or released
                    // Place block
                    if (blockraycast.hit) {

                        world->setBlock(blockraycast.blockPosition + blockraycast.normal, player->m_selected_block_type);
                        std::cout << "Placed Block" << std::endl;

                    }
                }
            }
                break;


            case SDL_EVENT_KEY_DOWN: {
                const SDL_Keycode keycode = SDL_GetKeyFromScancode(ev.key.scancode, ev.key.mod, false);
                if (keycode == SDLK_ESCAPE) {
                    isRunning = false;
                }
                if (keycode == SDLK_SPACE) {
                    drawWireframe = !drawWireframe;
                    switch (drawWireframe) {
                        case true:
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe gl
                            break;
                        case false:
                            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // disable wireframe gl
                            break;
                    }

                    // swap to debug shader
                    // Shader *tempShader = ourShader;
                    // ourShader = otherShader;
                    // otherShader = tempShader;

                }
                if (keycode == SDLK_TAB) {

                    // Toggle Mouse cursor
                    SDL_SetWindowRelativeMouseMode(m_window, !SDL_GetWindowRelativeMouseMode(m_window));

                    eventStates["MenuMode"] = !eventStates["MenuMode"];

                }

                if (keycode == SDLK_F3) {
                    //Enable window
                    m_boolDebugMenu = !m_boolDebugMenu;
                }

                if (keycode == SDLK_F11) {
                    //Toggle FullScreen
                    static bool fullscreen = false;
                    fullscreen = !fullscreen;
                    if (!SDL_SetWindowFullscreen(m_window, fullscreen)) {
                        SDL_Log("Failed to toggle fullscreen: %s", SDL_GetError());
                    }
                }

                if (keycode == SDLK_R) {

                    //reset player position
                    player->setPosition(glm::vec3(8.0f, 9.0f, 8.0f));

                }
                if (keycode == SDLK_N) {

                    //toggle Player Noclip
                    player->toggleNoclip();

                }
                if (keycode == SDLK_1) {

                    //Cycle Selected Block
                    player->m_selected_block_type = static_cast<BlockType>((static_cast<uint8_t>(player->m_selected_block_type) + 1) % 5 );
                    std::cout << "selected blockType: " << static_cast<unsigned int>(player->m_selected_block_type) << std::endl;

                }
                
            }

            break;
            default: ;
        }


        if (!eventStates["MenuMode"]) {
            processMouseMotion(ev);
        }



    }

        const auto* keyState = reinterpret_cast<const Uint8 *>(SDL_GetKeyboardState(nullptr));

        if (keyState[SDL_SCANCODE_W])
            player->ProcessMovement(FORWARD, m_deltaTime);
        if (keyState[SDL_SCANCODE_S])
            player->ProcessMovement(BACKWARD, m_deltaTime);
        if (keyState[SDL_SCANCODE_A])
            player->ProcessMovement(LEFT, m_deltaTime);
        if (keyState[SDL_SCANCODE_D])
            player->ProcessMovement(RIGHT, m_deltaTime);
        if (keyState[SDL_SCANCODE_LSHIFT]) {

            player->setMovementSpeed(20.0f);
        }
        else {
            player->setMovementSpeed(5.0f);
        }

}
void Game::processMouseMotion(const SDL_Event& event) const {
    // Mouse motion event
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        const float xOffset = event.motion.xrel; // Relative x motion
        const float yOffset = event.motion.yrel; // Relative y motion

        // Process mouse movement in the camera, does not rotate player as player has no rotation
        player->camera->ProcessMouseMovement(xOffset, yOffset);
    }
}



void Game::initVulkan() {
    vkCreateInstance();
    vkSetupDebugMessenger();
    vkPickPhysicalDevice();
    vkCreateLogicalDevice();

}

void Game::onStart() {
    cout << "onStart" << endl;
    // To get relative motion without hiding cursor
    SDL_SetWindowRelativeMouseMode(m_window, true);

    m_boolDebugMenu = true;

    // glProvokingVertex(GL_FIRST_VERTEX_CONVENTION); // uses information from the first vertex for a plane

    ourShader = new Shader("./resources/shaders/modelShader.vert", "./resources/shaders/modelShader.frag");
    terrainShader = new Shader("./resources/shaders/terrainShader.vert", "./resources/shaders/terrainShader.frag");


    // Setup Player
    player = std::make_shared<Player>(glm::vec3(8.0f, 9.0f, 8.0f));
    player->camera->Zoom = FOV;

    world = &World::getInstance();
    skybox = new Skybox();

    GreedyMesher::blockTextureAtlas.LoadFromFile("resources/textures/texture_atlas.png");

    // Generate some chunks
    ChunkCoord chunk_coords = {0,0};
    world->generateChunk(chunk_coords);

    chunk_coords = {-3, -3};
    world->generateChunk(chunk_coords);
    for (int i = -1; i <= 1; i++) {
        for (int j = 1; j <= 3; j++) {

            chunk_coords = {i,j};
            world->generateChunk(chunk_coords);
        }
    }

    auto modifyChunk = world->getChunk({-3,-3});
    for (int x=0; x < CHUNK_SIZE_X; x++) {
        for (int y=0; y < CHUNK_SIZE_Y; y++) {
            for (int z=0; z < CHUNK_SIZE_Z; z++) {
                if (x < 5 && z < 2) {
                    setBlockType(modifyChunk->at(x,y,z), BlockType::AIR);
                }
                if (x < 11 && z < 13) {
                    setBlockType(modifyChunk->at(x,y,z), BlockType::DIRT);
                }
            }
        }
    }
    setBlockType(modifyChunk->at(15, 10, 15), BlockType::STONE);
    
    // generate and modify for testing
    world->generateChunk({ 3,3 });
    modifyChunk = world->getChunk({3,3});
    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                if (x < 5 && z < 2) {
                    setBlockType(modifyChunk->at(x, y, z), BlockType::AIR);
                }
                if (x < 11 && z < 13) {
                    setBlockType(modifyChunk->at(x, y, z), BlockType::STONE);
                }
            }
        }
    }

    // add test dirt
    Voxel *v = world->getBlock(0, 15, 0);
    setBlockType(*v, BlockType::DIRT);



    // Make Physics
    physicsSystem.RegisterEntity(std::shared_ptr<Entity>(player));
    player->physics->affectedByGravity = false;

}

void Game::update() {
    const Uint64 currentTick = SDL_GetTicks();
    m_deltaTime = (currentTick - m_lastTick) / 1000.0f;
    m_lastTick = currentTick;

    // Update world (chunk loading/unloading)
    world->updateDirtyChunks();

    //PHYSICS
    physicsSystem.Update(m_deltaTime);
    worldCollision::resolveCollisions(*player, world);

}

void Game::render() {
    // Render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    terrainShader->use();

    // Get camera matrices
    glm::mat4 view = player->camera->GetViewMatrix();
    glm::mat4 projection = player->camera->GetProjectionMatrix(static_cast<float>(m_width) / static_cast<float>(m_height));
    terrainShader->setMat4("projection", projection);
    terrainShader->setMat4("view", view);

    // 2. Rendering
    world->renderVisibleChunks(*terrainShader);

    // 3. Mark chunks dirty when modified and reupload geometry
    world->updateDirtyChunks();
 
    skybox->drawSkybox(view, projection);


    // UI
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (m_boolDebugMenu)
    {
        imguiUI(*m_imguiIO);
    }


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (!SDL_GL_SwapWindow(m_window)) {
        cout << "SDL_GL_SwapWindow error: "<< SDL_GetError() << endl;
        isRunning = false;
    }
}

void Game::clean() const {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    std::cout << "Cleaning up..." << std::endl;
}

void Game::imguiUI(const ImGuiIO& io) {
    {
        int fbWidth, fbHeight;

        ImGui::Begin("Debug Menu");

        ImGui::Text("Window Size: %d x %d", m_width, m_height);
        ImGui::Text("Cube Count %ld ", m_cubes.size());

        ImGui::Text("Selected Block Type %d ", static_cast<int>(player->m_selected_block_type));

        ImGui::Text("Player Head %.3f, %.3f, %.3f ", player->getHead().x, player->getHead().y, player->getHead().z);
        ImGui::Text("Player Pos %.3f, %.3f, %.3f ", player->getPosition().x, player->getPosition().y, player->getPosition().z);
        ImGui::Text("Player Cam %.3f, %.3f, %.3f ", player->camera->Position.x, player->camera->Position.y, player->camera->Position.z);
        ImGui::Text("Player Feet %.3f, %.3f, %.3f ", player->getFeet().x, player->getFeet().y, player->getFeet().z);
        ImGui::Text("Player Speed %.2f", player->getMovementSpeed());

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Application TIME  (%ld ms), Delta: (%.4f s)", SDL_GetTicks(), m_deltaTime);
        if (ImGui::Button("Close Me"))
            m_boolDebugMenu = false;
        ImGui::End();
    }

}

void Game::initWindow() {
    m_window = SDL_CreateWindow(m_title, m_width, m_height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }
}
