//
// Created by Lamad on 11/12/2024.
//

#include "game.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>


#include <iostream>
#include <ostream>

#include "../config.h"

#include "rendering/shader.h"
#include "camera/camera.h"
#include "physics/collisions/worldCollision.h"
#include "rendering/model.hpp"
#include "terrain/world.h"

void imguiUI(ImGuiIO& io);

Game::Game(const char* title, const int width, const int height): m_imguiIO() {
    if constexpr (SDL_INIT_STATUS_INITIALIZED) {
        std::cout << "Initializing SDL - OK" << std::endl;


        // Set OpenGL version (e.g., 3.3 Core)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // Enable double buffering
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // Set the depth buffer size (optional but recommended for 3D rendering)
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        bool fuck = false;
        m_height = height;
        m_width = width;
        m_window = SDL_CreateWindow(title, m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (m_window == nullptr) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            fuck = true;
        }

        // Create OpenGL Context
        m_glContext = SDL_GL_CreateContext(m_window);
        if (!m_glContext) {
            std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            fuck = true;
        }
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &imguiIO = ImGui::GetIO();
        (void) imguiIO;
        imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

        m_imguiIO = &imguiIO;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForOpenGL(m_window, m_glContext);

        //Might cause errors
        ImGui_ImplOpenGL3_Init();

        // Load OpenGL Functions using GLAD
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            SDL_GL_DestroyContext(m_glContext);
            SDL_DestroyWindow(m_window);
            fuck = true;
        }

        if (!ImGui::CreateContext()) {
            std::cerr << "Failed to create ImGui context" << std::endl;
            fuck = true;
        }

        if (fuck) {
            clean();
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
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

                }if (keycode == SDLK_P) {

                    //toggle Player gravity
                    player->physics->affectedByGravity = !player->physics->affectedByGravity;
                    player->physics->velocity = glm::vec3(0.0f);
                    std::cout << "Player Gravity: " << player->physics->affectedByGravity << std::endl;

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

        // Process mouse movement in the camera
        player->camera->ProcessMouseMovement(xOffset, yOffset);
    }
}

void Game::onStart() {
    cout << "onStart" << endl;
    // To get relative motion without hiding cursor
    SDL_SetWindowRelativeMouseMode(m_window, true);

    m_boolDebugMenu = true;

    ourShader = new Shader("./resources/shaders/modelShader.vert", "./resources/shaders/modelShader.frag");
    terrainShader = new Shader("./resources/shaders/terrainShader.vert", "./resources/shaders/terrainShader.frag");


    // Setup Player
    player = std::make_shared<Player>(glm::vec3(8.0f, 9.0f, 8.0f));
    player->camera->Zoom = FOV;

    world = &World::getInstance();

    world->mesher.blockTextureAtlas.LoadFromFile("resources/textures/debug_texture.png");

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
                    setBlockType(modifyChunk->at(x,y,z), BlockType::AIR);
                }
            }
        }
    }

    // Make Physics
    physicsSystem.RegisterEntity(std::shared_ptr<Entity>(player));

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

    // 3. Mark chunks dirty when modified
    // if (playerModifiedBlock) {
    //     ChunkCoord coord = getChunkCoord(modifiedBlockPos);
    //     world.dirtyChunks.push_back(coord);
    // }


    // Text
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
        ImGui::Text("Cube Count %d ", m_cubes.size());

        ImGui::Text("Player Head %.3f, %.3f, %.3f ", player->getHead().x, player->getHead().y, player->getHead().z);
        ImGui::Text("Player Pos %.3f, %.3f, %.3f ", player->getPosition().x, player->getPosition().y, player->getPosition().z);
        ImGui::Text("Player Cam %.3f, %.3f, %.3f ", player->camera->Position.x, player->camera->Position.y, player->camera->Position.z);
        ImGui::Text("Player Feet %.3f, %.3f, %.3f ", player->getFeet().x, player->getFeet().y, player->getFeet().z);
        ImGui::Text("Player Speed %.2f", player->getMovementSpeed());

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Application TIME  (%d ms), Delta: (%.4f s)", SDL_GetTicks(), m_deltaTime);
        if (ImGui::Button("Close Me"))
            m_boolDebugMenu = false;
        ImGui::End();
    }

}
