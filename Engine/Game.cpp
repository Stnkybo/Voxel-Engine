//
// Created by Lamad on 11/12/2024.
//

#include "Game.h"

#include <iostream>
#include <ostream>


Game::Game(const char* title, int width, int height) {
    if (SDL_INIT_STATUS_INITIALIZED) {
        std::cout << "Initializing SDL - OK" << std::endl;
        m_window = SDL_CreateWindow(title, width, height, 0);
        if (m_window == nullptr) {
            std::cout << "Window not created!" << std::endl;
        }
        m_renderer = new Renderer(SDL_CreateRenderer(m_window, nullptr));
        if (m_renderer == nullptr) {
            std::cout << "Renderer Not created!" << std::endl;
        }
        isRunning = true;
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
                if (keycode == 'a' || keycode == 'A') {
                    std::cout << "A" << std::endl;
                }
            }
            break;
            default: ;
        }
    }
}

void Game::onStart() {
    // Create a triangle
    const auto *triangle = new Triangle;

    // Add triangle to mesh so that it will be drawn
    auto mesh = new Mesh();
    mesh->addTriangle(*triangle);
    m_renderer->addMesh(*mesh);
}

void Game::update() {
}

void Game::render() {
    m_renderer->render();
}

void Game::clean() {
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    std::cout << "Cleaning up..." << std::endl;
}
