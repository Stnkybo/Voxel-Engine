#include <iostream>
#include <SDL3/SDL.h>

#include "Mesh.h"
#include "Renderer.h"
#include "Triangle.h"
#include "Window.h"

int main() {

    bool quit = false;

    SDL_Window *window = SDL_CreateWindow("Triangle Example", 800, 600, 0);

    Renderer renderer(SDL_CreateRenderer(window, nullptr));


#define vertLen 3

    // Create a triangle
    const auto *triangle = new Triangle;

    // Add triangle to mesh so that it will be drawn

    auto mesh = new Mesh();
    mesh->addTriangle(*triangle);

    renderer.addMesh(*mesh);


    while (!quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev) != 0) {
            switch(ev.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;

                case SDL_EVENT_KEY_DOWN: {
                    const SDL_Keycode keycode = SDL_GetKeyFromScancode(ev.key.scancode, ev.key.mod, false);
                        if (keycode == 'a' || keycode == 'A') {
                            std::cout << "A" << std::endl;
                        }
                    }
                    break;
                default: ;
            }
        }

        // Renderers
        renderer.render();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
