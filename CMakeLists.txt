cmake_minimum_required(VERSION 3.30)

project(SDL3PRJ)

find_package(fmt REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)


add_executable(${PROJECT_NAME} main.cpp
        Engine/Window.cpp
        Engine/Window.h
        Engine/Triangle.cpp
        Engine/Triangle.h
        Engine/Rendering/Cube.h
        Entities/Player.cpp
        Entities/Player.h
        Entities/Entity.cpp
        Entities/Entity.h
        Engine/Game.cpp
        Engine/Game.h
        Engine/Camera/Camera.cpp
        Engine/Camera/Camera.h
        Config.h
        Engine/Shader.h
        Engine/Rendering/Mesh.hpp
        Engine/Rendering/Model.hpp
        Temp/imgui_impl_sdl3.cpp
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        fmt::fmt
        glad::glad
        glm::glm
        imgui::imgui
        SDL3::SDL3
        )