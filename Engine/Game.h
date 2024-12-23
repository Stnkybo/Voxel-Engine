//
// Created by Lamad on 11/12/2024.
//

#ifndef GAME_H
#define GAME_H
#include <SDL3/SDL.h>


#include "Shader.h"
#include "Camera/Camera.h"
#include "Rendering/Cube.h"


class Game {
    SDL_Window *m_window;
    Camera *camera;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    Shader *ourShader;
    std::vector<Cube> m_cubes;

public:
    bool isRunning = true;

    Game(const char *title, int width, int height);

    ~Game();

    void handleEvents();

    void processMouseMotion(SDL_Event &event);

    void onStart();

    void update();

    void render();

    void clean();
};



#endif //GAME_H
