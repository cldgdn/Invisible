cmake_minimum_required(VERSION 3.16)
project(Invisible)

set(CMAKE_CXX_STANDARD 20)

# -- FetchContent --
include(FetchContent)

#GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

#GLAD
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/glad)
add_library(glad ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

#GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# -- Executable --
add_executable(Invisible main.cpp
        dynamic/Transform.cpp
        dynamic/Transform.h
        Shape.cpp
        Shape.h
        Shader.cpp
        Shader.h
        dynamic/Collider.cpp
        dynamic/Collider.h
        data/structs.h
        static/TileMap.cpp
        static/TileMap.h
        static/Texture.cpp
        static/Texture.h
        dynamic/Animation.cpp
        dynamic/Animation.h
        globals.h
        dynamic/Sprite.cpp
        dynamic/Sprite.h
        Game.cpp
        Game.h
        static/Room.cpp
        static/Room.h
        dynamic/Guard.cpp
        dynamic/Guard.h)

#STB IMAGE
target_include_directories(Invisible PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image
)

target_link_libraries(Invisible PRIVATE glfw glad glm)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()
