cmake_minimum_required(VERSION 3.30)
project(SDL3PRJ)

include(FetchContent)

# Assimp
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.3
)
FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
        FetchContent_Populate(assimp)
        add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
endif()

# fmt
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.0.2
)
FetchContent_MakeAvailable(fmt)

# glad
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# imgui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.6
)
FetchContent_MakeAvailable(imgui)

# SDL3 (Replaced ExternalProject_Add with FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.1.6
)
FetchContent_MakeAvailable(SDL3)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
        main.cpp
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
        Temp/stb_image.h
        Engine/Rendering/Cube.cpp
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        assimp
        fmt::fmt
        glad
        glm
        imgui
        SDL3
)
