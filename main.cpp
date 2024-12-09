#include <iostream>
#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "Window.h"

int main() {

    auto result = Window::init();
    if (!result) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, result.error());
    }

    {
        try {
            Window window("Window Title");
            bool isRunning = true;

            while (isRunning) {
               std::optional<SDL_Event> event;
                while ((event = Window::pollEvents()))
                    switch (event->type) {
                        case SDL_EventType::SDL_EVENT_QUIT:
                            isRunning = false;
                            break;
                    }


                // RENDER HERE
            }
        } catch (const std::exception& e) {
            SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, e.what());
        }



    }

    Window::quit();

    return 0;
}
