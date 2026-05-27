# cmake/deps.cmake

set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "" FORCE)

# -----------------------------
# OpenGL
# -----------------------------
find_package(OpenGL REQUIRED)
if(WIN32)
    set(OPENGL_LIBRARIES opengl32)
else()
    set(OPENGL_LIBRARIES OpenGL::OpenGL)
endif()

# -----------------------------
# SDL3
# -----------------------------
find_package(SDL3 CONFIG REQUIRED)

# -----------------------------
# Glad
# -----------------------------
find_package(glad CONFIG REQUIRED)

# -----------------------------
# GLM
# -----------------------------
find_package(glm CONFIG REQUIRED)

# -----------------------------
# Assimp
# -----------------------------
find_package(assimp CONFIG REQUIRED)

# -----------------------------
# ImGui
# -----------------------------
find_package(imgui CONFIG REQUIRED)

# -----------------------------
# stb_image
# -----------------------------
find_package(stb REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_INCLUDE_DIR})
