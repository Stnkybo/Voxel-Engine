//
// Created by Lamad on 11/12/2024.
//

#ifndef GAME_H
#define GAME_H
#include <imgui.h>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>

#include <memory>


#include "rendering/shader.h"
#include "../entities/player.h"
#include "rendering/cube.h"
#include "skybox/skybox.h"
#include "terrain/world.h"


class Game {
    SDL_Window *m_window;
    SDL_Renderer *m_renderer{};
    std::shared_ptr<Player> player{};

    const double FRAME_TIME = 1.0f / 60.0f;
    Uint64  m_lastTick = 0.0f;
    Uint64 unprocessedTime = 0;
    double frameCounter = 0;
    int frames = 0;

    Shader *ourShader{};
    Shader *terrainShader{};

    bool drawWireframe = false;

    std::vector<std::unique_ptr<Cube>> m_cubes;
    Texture *terrainTexture{};
    Skybox* skybox{};
    bool m_boolDebugMenu{};
    SDL_GLContext m_glContext;
    ImGuiIO* m_imguiIO;

    vector<Cube *> penith;
    int penith_offset[3] = {0, 0, 0};
    World *world{};

    void vkPickPhysicalDevice();

    void vkCreateLogicalDevice();

public:
    unordered_map<std::string, bool> eventStates;

    PhysicsSystem physicsSystem;
    float  m_deltaTime = 0;
    bool isRunning = true;
    int m_width;
    int m_height;


    Game(const char *title, int width, int height);

    ~Game();

    void handleEvents();

    void processMouseMotion(const SDL_Event &event) const;

    void vkSetupDebugMessenger();

    void vkCreateInstance();

    void initVulkan();

    void onStart();

    void update();

    void render();

    void clean() const;

    void imguiUI(const ImGuiIO& io);

    void initWindow();

private:
    const char *m_title;

    // Vulkan Backend Stuff
    vk::raii::Context  m_vkContext;
    vk::raii::Instance m_vkInstance{nullptr};
    vk::raii::DebugUtilsMessengerEXT m_vkDebugMessenger = nullptr;
    vk::raii::PhysicalDevice m_vkPhysicalDevice = nullptr;
    vk::raii::Device m_vkDevice = nullptr;
    vk::raii::Queue m_vkGraphicsQueue = nullptr;;

};



#endif //GAME_H
