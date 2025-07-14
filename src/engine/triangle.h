//
// Created by Lamad on 10/12/2024.
//

#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <SDL3/SDL_render.h>
#define vertLen 3

class Triangle {
    public:
    SDL_Vertex vert[vertLen]{};
    Triangle();

};



#endif //TRIANGLE_H
