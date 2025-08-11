cmake_minimum_required(VERSION 3.28)
project(SDL3PRJ)

set(CMAKE_CXX_STANDARD 23)

# Load all Project Dependencies
include(cmake/deps.cmake)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/engine/window.cpp
        src/engine/window.h
        src/engine/demo/triangle.cpp
        src/engine/demo/triangle.h
        src/engine/rendering/cube.h
        src/entities/player.cpp
        src/entities/player.h
        src/entities/entity.cpp
        src/entities/entity.h
        src/engine/game.cpp
        src/engine/game.h
        src/engine/camera/camera.cpp
        src/engine/camera/camera.h
        src/config.h
        src/engine/rendering/shader.h
        src/engine/rendering/mesh.hpp
        src/engine/rendering/model.hpp
        src/engine/rendering/cube.cpp
        src/engine/terrain/chunk.h
        src/engine/terrain/voxel.h
        src/engine/terrain/world.cpp
        src/engine/terrain/world.h
        src/engine/physics/worldCollision.cpp
        src/engine/physics/worldCollision.h
        src/engine/physics/AABB.cpp
        src/engine/physics/AABB.h
        src/engine/math/mod.h
        src/engine/rendering/terrain/ChunkMesh.cpp
        src/engine/rendering/terrain/ChunkMesh.h
        src/engine/rendering/terrain/GreedyMesher.cpp
        src/engine/rendering/terrain/GreedyMesher.h
        src/engine/rendering/terrain/BlockTextureAtlas.cpp
        src/engine/rendering/terrain/BlockTextureAtlas.h
)

# Copy Shaders
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/shaders
        COMMENT "Copying Shaders to output directory"
)
# Copy Textures
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/textures
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/textures
        COMMENT "Copying Textures to output directory"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        find_package(Threads REQUIRED)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lpthread -Wl,-Bdynamic")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-g -O0 -fsanitize=address>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address>
        )
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

        if(MINGW)
                # Hide the console window for Release builds
                set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mwindows")
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
                        $<$<CONFIG:Debug>:/Od /Zi>
                )
                target_sources(${PROJECT_NAME} PRIVATE src/main_win32.cpp)

                target_link_options(${PROJECT_NAME} PRIVATE
                        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
                        $<$<CONFIG:Debug>:/DEBUG>
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
