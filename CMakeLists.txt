cmake_minimum_required(VERSION 3.30)

project(SDL3PRJ)

find_package(fmt REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp
        Window.cpp
        Window.h
        Triangle.cpp
        Triangle.h
        Renderer.cpp
        Renderer.h
        Mesh.cpp
        Mesh.h
        Cube.cpp
        Cube.h
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        fmt::fmt
        glad::glad
        SDL3::SDL3
        )