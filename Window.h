//
// Created by Lamad on 9/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <expected>
#include <SDL3/SDL.h>
#include <memory>
#include <optional>


class Window {
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window;

public:
    explicit Window(const std::string & title, int width = 800, int height = 600, SDL_WindowFlags flags = (SDL_WindowFlags) 0);

    static std::expected<void, const char *> init();
    static void quit();

    static std::optional<SDL_Event> pollEvents();
};



#endif //WINDOW_H
