#include <iostream>
#include <SDL3/SDL.h>


#include "Config.h"
#include "Engine/Game.h"

int main() {
    const auto game = new Game("Cube Example", WIDTH, HEIGHT);

    game->onStart();
    while (game->isRunning) {
        cout << "Is Running" << endl;
        game->handleEvents();
        game->update();
        game->render();

    }
    game->clean();

    return 0;
}
