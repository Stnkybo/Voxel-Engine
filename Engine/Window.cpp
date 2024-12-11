//
// Created by Lamad on 9/12/2024.
//

#include "Window.h"
#include "stdexcept"

Window::Window(const std::string &title, int width, int height, SDL_WindowFlags flags)
    : m_window(SDL_CreateWindow(title.c_str(), width, height, flags), &SDL_DestroyWindow) {

    if (!m_window)
        throw std::runtime_error(SDL_GetError());
}

std::expected<void, const char *> Window::init() {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
        return std::unexpected(SDL_GetError());
    else
        return {};
}

void Window::quit() {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

std::optional<SDL_Event> Window::pollEvents() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        return event;
    } else {
        return std::nullopt;
    }

}
