cmake_minimum_required(VERSION 3.28.6)
project(SDL3PRJ)

include(FetchContent)

find_package(OpenGL REQUIRED)

# Assimp
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
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
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
        FetchContent_Populate(imgui)
        add_library(imgui STATIC
                ${imgui_SOURCE_DIR}/imgui.cpp
                ${imgui_SOURCE_DIR}/imgui_draw.cpp
                ${imgui_SOURCE_DIR}/imgui_widgets.cpp
                ${imgui_SOURCE_DIR}/imgui_tables.cpp
                ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
                ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp  # Add this line
        )
        target_include_directories(imgui PUBLIC
                ${imgui_SOURCE_DIR}
                ${imgui_SOURCE_DIR}/backends
        )
        target_link_libraries(imgui PRIVATE
                SDL3::SDL3
                OpenGL::GL  # Add OpenGL linking
        )
endif()


# Set Release build for SDL3 (even if main project is Debug)
set(SDL3_BUILD_TYPE "Release")  # Force SDL3 to build in Release mode
# SDL3 (Replaced ExternalProject_Add with FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.8
)
FetchContent_GetProperties(SDL3)
if(NOT SDL3_POPULATED)
        FetchContent_Populate(SDL3)

        # Optionally force static build if you prefer
        # set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

        add_subdirectory(${SDL3_SOURCE_DIR} ${SDL3_BINARY_DIR})

        # Handle different SDL3 target names
        if(TARGET SDL3::SDL3)
                # Modern target exists
        elseif(TARGET SDL3-shared)
                add_library(SDL3::SDL3 ALIAS SDL3-shared)
        elseif(TARGET SDL3-static)
                add_library(SDL3::SDL3 ALIAS SDL3-static)
        endif()
endif()

# stb_image
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Create an interface library for stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})

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

if(WIN32)
        # get the right .dll and put next to .exe
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                set(SDL3_DLL_NAME "SDL3d.dll")
        else()
                set(SDL3_DLL_NAME "SDL3.dll")
        endif()

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL3_BINARY_DIR}/${SDL3_DLL_NAME}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying ${SDL3_DLL_NAME} to output directory"
        )
endif()

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        assimp
        fmt::fmt
        glad
        glm
        imgui
        SDL3::SDL3
        stb_image
        OpenGL::GL
)
