//
// Created by Lamad on 11/12/2024.
//

#include "Game.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>


#include <iostream>
#include <ostream>

#include "Shader.h"
#include "../Config.h"
#include "Camera/Camera.h"
#include "Rendering/Cube.h"


Game::Game(const char* title, int width, int height) {
    if (SDL_INIT_STATUS_INITIALIZED) {
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
        m_window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);

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
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForOpenGL(m_window, m_glContext);

        //Might cause errors
        ImGui_ImplOpenGL3_Init();

        // Load OpenGL Functions using GLAD
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
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
    else {
        isRunning = false;
    }
}

Game::~Game() {
    clean();
}



void Game::handleEvents() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0) {
        switch(ev.type) {
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

                }
            }
            break;
            default: ;
        }

        processMouseMotion(ev);

        const Uint8* keyState = reinterpret_cast<const Uint8 *>(SDL_GetKeyboardState(nullptr));

        if (keyState[SDL_SCANCODE_W])
            camera->ProcessKeyboard(FORWARD, m_deltaTime);
        if (keyState[SDL_SCANCODE_S])
            camera->ProcessKeyboard(BACKWARD, m_deltaTime);
        if (keyState[SDL_SCANCODE_A])
            camera->ProcessKeyboard(LEFT, m_deltaTime);
        if (keyState[SDL_SCANCODE_D])
            camera->ProcessKeyboard(RIGHT, m_deltaTime);


    }


}
void Game::processMouseMotion(SDL_Event& event) {
    // Mouse motion event
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        float xOffset = event.motion.xrel; // Relative x motion
        float yOffset = event.motion.yrel; // Relative y motion

        // Process mouse movement in the camera
        camera->ProcessMouseMovement(xOffset, yOffset);
    }
}

void Game::onStart() {
    cout << "onStart" << endl;
    SDL_SetWindowRelativeMouseMode(m_window, true);

    show_another_window = true;

    Cube cube;
    m_cubes.emplace_back(cube);

    ourShader = new Shader("./Shaders/modelShader.vert", "./Shaders/modelShader.frag");
    otherShader = new Shader("./Shaders/shader.vert", "./Shaders/shader.frag");


    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

}

void Game::update() {
    m_deltaTime = (SDL_GetTicks() - m_lastTick);
    m_lastTick = SDL_GetTicks();
    unprocessedTime += m_deltaTime;
    frameCounter += m_deltaTime;

    while (unprocessedTime > FRAME_TIME)
    {
        unprocessedTime -= FRAME_TIME;
    }


}

void Game::render() {
    // Render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader->use();

    // Get camera matrices
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix(static_cast<float>(WIDTH) / static_cast<float>(HEIGHT));
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    // Draw cubes
    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    ourShader->setMat4("model", model);

    for (const Cube& cube : m_cubes) {
        if (cube.cubeMesh != nullptr) {
            cube.cubeMesh->Draw(*ourShader);
        } else {
            cout << "cubeMesh is nullptr!" << endl;
        }
    }


    // Text
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("FPS: %i | %f ms\n", frames, 1000.0 / static_cast<double>(frames));
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }


    SDL_RenderPresent(m_renderer);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (!SDL_GL_SwapWindow(m_window)) {
        cout << "SDL_GL_SwapWindow error: "<< SDL_GetError() << endl;
        isRunning = false;
    }
}

void Game::clean() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    std::cout << "Cleaning up..." << std::endl;
}
