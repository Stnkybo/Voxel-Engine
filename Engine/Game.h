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
    SDL_Renderer *m_renderer;
    Camera *camera;

    const double FRAME_TIME = 1.0f / 60.0f;
    double  m_deltaTime = 0.0f;
    double  m_lastTick = 0.0f;
    double unprocessedTime = 0;
    double frameCounter = 0;
    int frames = 0;

    Shader *ourShader;
    Shader *otherShader;
    std::vector<Cube> m_cubes;
    bool show_another_window;
    SDL_GLContext m_glContext;

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
