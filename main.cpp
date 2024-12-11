#include <iostream>
#include <SDL3/SDL.h>


#include "Engine/Game.h"

int main() {
    const auto game = new Game("Triangle Example", 800, 600);

    game->onStart();
    while (game->isRunning) {
        game->handleEvents();
        game->update();
        game->render();

    }
    game->clean();

    return 0;
}
