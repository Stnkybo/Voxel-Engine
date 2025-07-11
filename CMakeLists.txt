cmake_minimum_required(VERSION 3.28)
project(SDL3PRJ)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

find_package(OpenGL REQUIRED)
# Check for OpenGL in Linux
if (UNIX AND NOT APPLE)
        if (NOT OpenGL_FOUND)
                message(FATAL_ERROR "OpenGL not found. On Debian/Ubuntu, run:
  sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev")
        else()
                message(STATUS "OpenGL found: ${OpenGL_LIBRARIES}")
        endif()
endif()

# OpenGL message for Windows
if (WIN32)
        message(STATUS "Running on Windows. Make sure GPU drivers are installed to get OpenGL 4.6+ support.")
endif()

# Assimp
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.3
)

FetchContent_MakeAvailable(assimp)

# glad
set(GLAD_GLAPI_EXPORT OFF CACHE BOOL "" FORCE)
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
        FetchContent_MakeAvailable(imgui)
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
                glad  # Add OpenGL linking
        )
endif()

# After find_package(OpenGL REQUIRED)
if(WIN32)
        # On Windows, explicitly link to opengl32
        set(OPENGL_LIBRARIES opengl32)
else()
        set(OPENGL_LIBRARIES OpenGL::OpenGL)
endif()

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.8
)
FetchContent_MakeAvailable(SDL3)

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

# Copy Shaders
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
        COMMENT "Copying Shaders to output directory"
)
# Copy Textures
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Textures
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Textures
        COMMENT "Copying Textures to output directory"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        find_package(Threads REQUIRED)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lpthread -Wl,-Bdynamic")
endif()

if(WIN32)
        # Define icon and resource file paths
        set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/appicon.ico")  # Your .ico file
        set(RES_FILE "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")  # Generated .rc file
        # Create the resource file dynamically
        file(WRITE ${RES_FILE}
                "IDI_ICON1 ICON DISCARDABLE \"${APP_ICON}\"\n"
        )
        # Force rebuild when icon changes
        set_source_files_properties(
                ${RES_FILE}
                PROPERTIES
                OBJECT_DEPENDS "${APP_ICON}"
        )
        # Add to executable
        target_sources(${PROJECT_NAME} PRIVATE ${RES_FILE})
        # Hide the console window for Release builds
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mwindows")

        if(MINGW)
                # Find MinGW libraries # Might be ass code
                find_library(STDCXX_LIB stdc++ PATHS "${CMAKE_CXX_COMPILER_PREFIX}/lib")
                find_library(WINPTHREAD_LIB winpthread PATHS "${CMAKE_CXX_COMPILER_PREFIX}/lib")

                if(STDCXX_LIB AND WINPTHREAD_LIB)
                        target_link_libraries(${PROJECT_NAME} PRIVATE
                                "${STDCXX_LIB}"
                                "${WINPTHREAD_LIB}"
                        )
                else()
                        message(WARNING "Could not find static stdc++ or winpthread libraries - DLLs may be required")
                endif()

                # get the right .dll and put next to .exe
                if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                        set(SDL3_DLL_NAME "SDL3d.dll")
                        set(GLAD_DLL_NAME "libgladd.dll")  # Debug version
                else()
                        set(SDL3_DLL_NAME "SDL3.dll")
                        set(GLAD_DLL_NAME "libglad.dll")   # Release version
                endif()

        endif()
        if(MSVC)
                # get the right .dll and put next to .exe
                if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                        set(SDL3_DLL_NAME "SDL3d.dll")
                        set(GLAD_DLL_NAME "gladd.dll")  # Debug version
                else()
                        set(SDL3_DLL_NAME "SDL3.dll")
                        set(GLAD_DLL_NAME "glad.dll")   # Release version
                endif()
                # Add Sanitization to Debug Builds
                target_compile_options(${PROJECT_NAME} PRIVATE
                        $<$<CONFIG:Debug>:-g>
                        $<$<CONFIG:Debug>:-O0>
                        $<$<CONFIG:Debug>:-fsanitize=address>
                )

                target_link_options(${PROJECT_NAME} PRIVATE
                        $<$<CONFIG:Debug>:-fsanitize=address>
                )
        endif ()

        # Copy dll for SDL3
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL3_BINARY_DIR}/${SDL3_DLL_NAME}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying ${SDL3_DLL_NAME} to output directory"
        )
        # Copy glad DLL
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${GLAD_BINARY_DIR}/${GLAD_DLL_NAME}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying ${GLAD_DLL_NAME} to output directory"
        )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${glm_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        assimp
        glad
        imgui
        SDL3::SDL3
        stb_image
        OpenGL::GL
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:Threads::Threads>  # Only link if not MSVC
)

# Link dl only on UNIX platforms (Linux, BSD, etc.)
if(UNIX AND NOT APPLE)
        target_link_libraries(${PROJECT_NAME} PUBLIC dl)
endif()
