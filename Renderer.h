//
// Created by Lamad on 10/12/2024.
//

#ifndef RENDERER_H
#define RENDERER_H
#include <vector>

#include "Mesh.h"
#include "Triangle.h"


class Renderer {

    std::vector<Mesh> m_meshes;
    SDL_Renderer *renderer;


    void drawMesh() {

        for (const Mesh& mesh : m_meshes) {
            for (Triangle t : mesh.m_triangles) {
                SDL_RenderGeometry(renderer, NULL, t.vert, vertLen, NULL, 0);
            }
        }
    }

public:
    explicit Renderer(SDL_Renderer *renderer) {
        this->renderer = renderer;
}
    ~Renderer() {
        SDL_DestroyRenderer(renderer);
    }
    void addMesh(const Mesh& mesh) {
        m_meshes.push_back(mesh);
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawMesh();
        SDL_RenderPresent(renderer);
    }

};



#endif //RENDERER_H
