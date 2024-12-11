//
// Created by Lamad on 11/12/2024.
//

#ifndef GAME_H
#define GAME_H
#include <SDL3/SDL.h>

#include "Renderer.h"


class Game {
    SDL_Window *m_window;
    Renderer *m_renderer;

    public:
    bool isRunning;

    Game(const char *title, int width, int height);

    ~Game();

    void handleEvents();

    void onStart();

    void update();

    void render();

    void clean();
};



#endif //GAME_H
