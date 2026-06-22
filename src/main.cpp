#include <imgui_impl_sdl3.h>

#include "config.h"
#include "engine/game.h"

int main() {
    const auto game = new Game("Chunk Demo", WIDTH, HEIGHT);

    try {
        game->initWindow();
        game->initVulkan();
        game->initImGui();
        // game->onStart();
        while (game->isRunning) {
            game->handleEvents();
            // game->update();
            game->drawFrame();
            // game->render();
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    game->clean();

    return EXIT_SUCCESS;
}
