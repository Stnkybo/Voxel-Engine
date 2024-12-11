cmake_minimum_required(VERSION 3.30)

project(SDL3PRJ)

find_package(fmt REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp
        Engine/Window.cpp
        Engine/Window.h
        Engine/Triangle.cpp
        Engine/Triangle.h
        Engine/Renderer.cpp
        Engine/Renderer.h
        Engine/Mesh.cpp
        Engine/Mesh.h
        Engine/Cube.cpp
        Engine/Cube.h
        Entities/Player.cpp
        Entities/Player.h
        Entities/Entity.cpp
        Entities/Entity.h
        Engine/Game.cpp
        Engine/Game.h
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        fmt::fmt
        glad::glad
        SDL3::SDL3
        )