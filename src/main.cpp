#include "config.h"
#include "engine/game.h"

int main() {
    const auto game = new Game("Chunk Demo", WIDTH, HEIGHT);

    game->onStart();
    while (game->isRunning) {
        game->handleEvents();
        game->update();
        game->render();

    }
    game->clean();

    return 0;
}
