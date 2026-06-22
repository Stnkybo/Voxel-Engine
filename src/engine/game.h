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
    uint64_t m_lastTick = 0.0f;
    uint64_t unprocessedTime = 0;
    double frameCounter = 0;
    int frames = 0;

    Shader *ourShader{};
    Shader *terrainShader{};

    bool drawWireframe = false;

    Texture *terrainTexture{};
    Skybox *skybox{};
    bool m_boolDebugMenu{};
    SDL_GLContext m_glContext;
    ImGuiIO *m_imguiIO;

    World *world{};

    void vkPickPhysicalDevice();

    void vkCreateLogicalDevice();

    void vkCreateSurface();

    vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities);

public:
    unordered_map<std::string, bool> eventStates;

    PhysicsSystem physicsSystem;
    float m_deltaTime = 0;
    bool isRunning = true;
    int m_width;
    int m_height;


    Game(const char *title, int width, int height);

    ~Game();

    void handleEvents();

    void processMouseMotion(const SDL_Event &event) const;

    void vkSetupDebugMessenger();

    void vkCreateInstance();

    void vkCreateSwapchain();

    void vkCreateImageViews();

    void vkCreateGraphicsPipeline();

    [[nodiscard]] vk::raii::ShaderModule vkCreateShaderModule(const std::vector<char> &code) const;

    void vkCreateCommandPool();

    void vkCreateCommandBuffer();

    void vkRecordCommandBuffer(uint32_t imageIndex);

    void vkTransition_image_layout(uint32_t imageIndex, vk::ImageLayout old_layout, vk::ImageLayout new_layout,
                                   vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask,
                                   vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask);

    void vkCreateSyncObjects();

    void initVulkan();

    void onStart();

    void update();

    void drawFrame();

    void render();

    void clean() const;

    void imguiUI(const ImGuiIO &io);

    void initWindow();

    void initImGui();

private:
    const char *m_title;

    // Vulkan Backend Stuff
    vk::raii::Context m_vkContext;
    vk::raii::Instance m_vkInstance{nullptr};
    vk::raii::DebugUtilsMessengerEXT m_vkDebugMessenger = nullptr;
    vk::raii::SurfaceKHR m_vkSurface = nullptr;
    vk::raii::PhysicalDevice m_vkPhysicalDevice = nullptr;
    vk::raii::Device m_vkDevice = nullptr;
    uint32_t m_vkQueueIndex = ~0;
    vk::raii::Queue m_vkGraphicsQueue = nullptr;

    vk::raii::SwapchainKHR m_vkSwapChain = nullptr;
    std::vector<vk::Image> m_vkSwapChainImages;
    vk::SurfaceFormatKHR m_vkSwapChainSurfaceFormat;
    vk::Extent2D m_vkSwapChainExtent;

    std::vector<vk::raii::ImageView> m_vkSwapChainImageViews;

    vk::raii::PipelineLayout m_vkPipelineLayout = nullptr;
    vk::raii::Pipeline m_vkGraphicsPipeline = nullptr; // The Holy Grail

    vk::raii::CommandPool m_vkCommandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_vkCommandBuffers;

    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    uint32_t m_frameIndex = 0;

};


#endif //GAME_H
