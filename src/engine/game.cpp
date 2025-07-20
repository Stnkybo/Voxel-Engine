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

#include "rendering/shader.h"
#include "../config.h"
#include "camera/camera.h"
#include "rendering/cube.h"
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
                m_width = ev.window.data1;
                m_height = ev.window.data2;
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
                    Shader *tempShader = ourShader;
                    ourShader = otherShader;
                    otherShader = tempShader;

                }
                if (keycode == SDLK_TAB) {

                    // Toggle Mouse cursor
                    SDL_SetWindowRelativeMouseMode(m_window, !SDL_GetWindowRelativeMouseMode(m_window));

                    eventStates["MenuMode"] = !eventStates["MenuMode"];

                }

                if (keycode == SDLK_F3) {

                    //Enable window
                    m_boolDebugMenu = !m_boolDebugMenu;

                }if (keycode == SDLK_R) {

                    //Test
                    camera->Position = glm::vec3(0.0f, 5.0f, 10.0f);

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
            camera->ProcessKeyboard(FORWARD, m_deltaTime);
        if (keyState[SDL_SCANCODE_S])
            camera->ProcessKeyboard(BACKWARD, m_deltaTime);
        if (keyState[SDL_SCANCODE_A])
            camera->ProcessKeyboard(LEFT, m_deltaTime);
        if (keyState[SDL_SCANCODE_D])
            camera->ProcessKeyboard(RIGHT, m_deltaTime);
        if (keyState[SDL_SCANCODE_LSHIFT]) {

            camera->MovementSpeed = 20.0f;
        }
        else {
            camera->MovementSpeed = 5.0f;
        }

}
void Game::processMouseMotion(const SDL_Event& event) const {
    // Mouse motion event
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        const float xOffset = event.motion.xrel; // Relative x motion
        const float yOffset = event.motion.yrel; // Relative y motion

        // Process mouse movement in the camera
        camera->ProcessMouseMovement(xOffset, yOffset);
    }
}

void Game::onStart() {
    cout << "onStart" << endl;
    SDL_SetWindowRelativeMouseMode(m_window, true);

    m_boolDebugMenu = true;

    // Cheeky duplicate bad mem
    terrainTexture = new Texture;
    terrainTexture->path = "wall.jpg"; //awesomeface.png
    terrainTexture->type = "texture_diffuse";
    terrainTexture->id =  TextureFromFile(terrainTexture->path.c_str(), "resources/textures");
    Cube::setTexture(*terrainTexture);

    for (int i = 0; i < 11; i++) { //Make floor
        for (int j = 0; j < 11; j++) {
            if ((i != (11-1)/2) || (j != (11-1)/2)) {
                const auto newCube = new Cube(i-5,0,j-5);
                m_cubes.emplace_back(newCube);

            }
        }
    }

    for (int i = 0; i < 10; i++) {
        const auto newCube = new Cube(0,i+1,0);
        m_cubes.emplace_back(newCube);
        penith.emplace_back(newCube);
    }

    const auto newCube = new Cube(0,13,0);
    m_cubes.emplace_back(newCube);


    ourShader = new Shader("./resources/shaders/modelShader.vert", "./resources/shaders/modelShader.frag");
    otherShader = new Shader("./resources/shaders/shader.vert", "./resources/shaders/shader.frag");


    camera = new Camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    auto world = &World::getInstance();
    ChunkCoord chunk_coords = {1,-1};
    world->generateChunk(chunk_coords);
    Chunk* chunk = world->getChunk(chunk_coords);

    for (int x = 0; x < CHUNK_SIZE_X; x++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                if (chunk->at(x,y,z).type != 0) {
                    m_cubes.emplace_back(new Cube(x + chunk_coords.x * CHUNK_SIZE_X, y, z + chunk_coords.z * CHUNK_SIZE_Z));
                }
            }
        }
    }

}

void Game::update() {
    const Uint32 currentTick = SDL_GetTicks();
    m_deltaTime = currentTick - m_lastTick;
    m_lastTick = currentTick;
    //unprocessedTime += m_deltaTime;
    //frameCounter += m_deltaTime;

    for (int i = 0 ; i < penith.size(); i++) {
            penith[i]->setPosition(penith_offset[0], 1 + i + penith_offset[1], penith_offset[2]);
    }

    //PHYSICS


}

void Game::render() {
    // Render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader->use();

    // Get camera matrices
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix(static_cast<float>(m_width) / static_cast<float>(m_height));
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    // Draw cubes
    // render the loaded model
    auto model = glm::mat4(1.0f);
    model = translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    ourShader->setMat4("model", model);

    for (const Cube* cube : m_cubes) {
        if (cube->cubeMesh != nullptr) {
            // cout << cube->cubeMesh->vertices[0].Position.y << endl;
            cube->cubeMesh->Draw(*ourShader);
        } else {
            cout << "cubeMesh is nullptr!" << endl;
        }
    }


    // Text
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (m_boolDebugMenu)
    {
        imguiUI(*m_imguiIO);
    }


    SDL_RenderPresent(m_renderer);

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
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        //static int counter = 0;

        ImGui::Begin("Debug Menu");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("Cube Count %d ", m_cubes.size());               // Display some text (you can use a format strings too)
        ImGui::SliderInt3("penith", penith_offset, -25.0f, 25.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        ImGui::Text("Camera Pos %.3f, %.3f, %.3f ", camera->Position.x, camera->Position.y, camera->Position.z);               // Display some text (you can use a format strings too)
        ImGui::SliderFloat("Camera Speed", &camera->MovementSpeed, 1.0f, 25.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", reinterpret_cast<float *>(&clear_color)); // Edit 3 floats representing a color

        // if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //     counter++;
        // ImGui::SameLine();
        // ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Application TIME  (%d ms), Delta: (%d ms)", SDL_GetTicks(), m_deltaTime);
        ImGui::Text("Terrain Texture:  (%d, %s)", terrainTexture->id, terrainTexture->path.c_str());
        if (ImGui::Button("Close Me"))
            m_boolDebugMenu = false;
        ImGui::End();
    }

}
