cmake_minimum_required(VERSION 3.28)
project(SDL3PRJ)

set(CMAKE_CXX_STANDARD 23)


add_executable(${PROJECT_NAME}
        src/main.cpp
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
        src/engine/physics/collisions/worldCollision.cpp
        src/engine/physics/collisions/worldCollision.h
        src/engine/physics/collisions/AABB.cpp
        src/engine/physics/collisions/AABB.h
        src/engine/math/mod.h
        src/engine/rendering/terrain/ChunkMesh.cpp
        src/engine/rendering/terrain/ChunkMesh.h
        src/engine/rendering/terrain/GreedyMesher.cpp
        src/engine/rendering/terrain/GreedyMesher.h
        src/engine/rendering/terrain/BlockTextureAtlas.cpp
        src/engine/rendering/terrain/BlockTextureAtlas.h
        src/engine/physics/PhysicsSystem.cpp
        src/engine/physics/PhysicsSystem.h
        src/engine/raycasting/raycast.cpp
        src/engine/raycasting/raycast.h
        src/engine/raycasting/raycast.cpp
        src/engine/client/playerRaycaster.h
)

# Load all Project Dependencies
include(cmake/deps.cmake)

# Copy Shaders
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/shaders
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/shaders
    COMMENT "Copying Shaders to output directory"
)

# Copy Textures
add_custom_target(copy_textures ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/textures
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/textures
        COMMENT "Copying Textures to output directory"
)

add_dependencies(copy_shaders ${PROJECT_NAME})
add_dependencies(copy_textures ${PROJECT_NAME})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        find_package(Threads REQUIRED)
#        Static linking was breaking the release build so it has been disabled
#        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++")
#        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lpthread -Wl,-Bdynamic")
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
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/appicon.ico")
    set(RES_FILE "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
    
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
        
        # Find MinGW libraries
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
    endif()

    if(MSVC)
        # Add Sanitization to Debug Builds
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/Od /Zi>
        )
        target_sources(${PROJECT_NAME} PRIVATE src/main_win32.cpp)

        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
            $<$<CONFIG:Debug>:/DEBUG>
        )
    endif()

   # # Copy SDL3 DLL (check if it exists first)
   # if(EXISTS "${SDL3_BINARY_DIR}/SDL3.dll" OR EXISTS "${SDL3_BINARY_DIR}/SDL3d.dll")
   #     if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   #         set(SDL3_DLL_NAME "SDL3d.dll")
   #     else()
   #         set(SDL3_DLL_NAME "SDL3.dll")
   #     endif()
   #     
   #     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   #         COMMAND ${CMAKE_COMMAND} -E copy
   #         "${SDL3_BINARY_DIR}/${SDL3_DLL_NAME}"
   #         $<TARGET_FILE_DIR:${PROJECT_NAME}>
   #         COMMENT "Copying ${SDL3_DLL_NAME} to output directory"
   #     )
   # endif()
   #
   # # Copy Glad DLL only if it exists (handle both static and shared linking)
   # get_target_property(GLAD_TYPE glad TYPE)
   # 
   # if(GLAD_TYPE STREQUAL "SHARED_LIBRARY")
   #     # Glad is built as shared library - copy the DLL
   #     if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   #         set(GLAD_DLL_NAME "gladd.dll")
   #     else()
   #         set(GLAD_DLL_NAME "glad.dll")
   #     endif()
   #     
   #     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   #         COMMAND ${CMAKE_COMMAND} -E copy
   #         "${GLAD_BINARY_DIR}/${GLAD_DLL_NAME}"
   #         $<TARGET_FILE_DIR:${PROJECT_NAME}>
   #         COMMENT "Copying ${GLAD_DLL_NAME} to output directory"
   #     )
   # else()
   #     # Glad is built as static library - no need to copy DLL
   #     message(STATUS "Glad is built as static library, no DLL to copy")
   # endif()
endif()

#target_include_directories(${PROJECT_NAME} PRIVATE ${glm_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        assimp::assimp
        glad::glad
        imgui::imgui
        SDL3::SDL3
        OpenGL::GL
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:Threads::Threads>  # Only link if not MSVC
)

# Link dl only on UNIX platforms (Linux, BSD, etc.)
if(UNIX AND NOT APPLE)
        target_link_libraries(${PROJECT_NAME} PUBLIC dl)
endif()
