//
// Created by Lamad on 11/12/2024.
//

#include "Game.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <iostream>
#include <ostream>

#include "Shader.h"
#include "../Config.h"
#include "Camera/Camera.h"
#include "Rendering/Cube.h"


Game::Game(const char* title, int width, int height) {
    if (SDL_INIT_STATUS_INITIALIZED) {
        std::cout << "Initializing SDL - OK" << std::endl;
        m_window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
        if (m_window == nullptr) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return;
        }

        // Create OpenGL Context
        SDL_GLContext glContext = SDL_GL_CreateContext(m_window);
        if (!glContext) {
            std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return;
        }

        // Load OpenGL Functions using GLAD
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            SDL_GL_DestroyContext(glContext);
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return;
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    }
    else {
        isRunning = false;
    }
}

Game::~Game() {
    clean();
}

void Game::handleEvents() {
    cout << "Events" << endl;
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
            }
            break;
            default: ;
        }

        processMouseMotion(ev);

        const Uint8* keyState = reinterpret_cast<const Uint8 *>(SDL_GetKeyboardState(nullptr));

        if (keyState[SDL_SCANCODE_W])
            camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
        if (keyState[SDL_SCANCODE_S])
            camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
        if (keyState[SDL_SCANCODE_A])
            camera->processKeyboard(CameraMovement::LEFT, deltaTime);
        if (keyState[SDL_SCANCODE_D])
            camera->processKeyboard(CameraMovement::RIGHT, deltaTime);


    }
}
void Game::processMouseMotion(SDL_Event& event) {
    // Mouse motion event
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        float xOffset = event.motion.xrel; // Relative x motion
        float yOffset = event.motion.yrel; // Relative y motion

        // Process mouse movement in the camera
        camera->processMouseMovement(xOffset, yOffset);
    }
}

void Game::onStart() {
    cout << "onStart" << endl;
    SDL_SetWindowRelativeMouseMode(m_window, true);

    Cube cube;
    m_cubes.emplace_back(cube);

    ourShader = new Shader("./Shaders/modelShader.vs", "./Shaders/modelShader.fs");


    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

}

void Game::update() {
    cout << "Update" << endl;
    float currentFrame = SDL_GetTicks() / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    printf("Camera Vec3: %f, %f, %f\n", camera->position.x, camera->position.y, camera->position.z);

}

void Game::render() {
    cout << "Render" << endl;
    // Render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader->use();
    cout << "Shader used" << endl;

    // Get camera matrices
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix(static_cast<float>(WIDTH) / static_cast<float>(HEIGHT));
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
        cout << "Cube Drawn" << endl;
    }

    //ourModel->Draw(ourShader);

    // Use view and projection matrices in your shader
    // ...

    cout << "swap Windoww " << endl;
    if (!SDL_GL_SwapWindow(m_window)) {
        cout << "SDL_GL_SwapWindow error: "<< SDL_GetError() << endl;
        isRunning = false;
    }
    cout << "Windoww swapped" << endl;
}

void Game::clean() {
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    std::cout << "Cleaning up..." << std::endl;
}
