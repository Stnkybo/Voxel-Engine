#include <iostream>


#include "Config.h"
#include "Engine/Game.h"

int main() {
    const auto game = new Game("Cube Example", WIDTH, HEIGHT);

    game->onStart();
    while (game->isRunning) {
        game->handleEvents();
        game->update();
        game->render();

    }
    game->clean();

    return 0;
}
