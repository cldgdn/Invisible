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

# --- SoLoud ---
set(SOLOUD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/soloud/soloud)
set(STB_VORBIS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_vorbis)

file(GLOB_RECURSE SOLOUD_SOURCES
        ${SOLOUD_DIR}/src/core/*.cpp
        ${SOLOUD_DIR}/src/audiosource/*.cpp
        ${SOLOUD_DIR}/src/filter/*.cpp
)

file(GLOB SOLOUD_BACKEND
        ${SOLOUD_DIR}/src/backend/winmm/soloud_winmm.cpp
)

add_library(soloud STATIC
        ${SOLOUD_SOURCES}
        ${SOLOUD_DIR}/src/backend/winmm/soloud_winmm.cpp
        ${STB_VORBIS_DIR}/stb_vorbis.c
)

target_include_directories(soloud PUBLIC
        ${SOLOUD_DIR}/include
        ${STB_VORBIS_DIR}
)

target_compile_definitions(soloud PRIVATE
        WITH_WINMM
        SOLOUD_NO_WASAPI
        SOLOUD_NO_OPENAL
        SOLOUD_NO_SDL2
        SOLOUD_NO_ALSA
        SOLOUD_NO_JACK
        SOLOUD_NO_PORTAUDIO
        SOLOUD_NO_COREAUDIO
)

target_link_libraries(soloud winmm)

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
        dynamic/Guard.h
        dynamic/Player.cpp
        dynamic/Player.h
        Pathfinder.cpp
        Pathfinder.h
        raycasting.cpp
        raycasting.h
        dynamic/Bullet.cpp
        dynamic/Bullet.h
        AudioManager.cpp
        AudioManager.h
        dynamic/Prop.cpp
        dynamic/Prop.h
        Menu.cpp
        Menu.h
        dynamic/RoomExit.cpp
        dynamic/RoomExit.h
        text/Glyph.h
        text/Font.cpp
        text/Font.h
        text/TextObject.cpp
        text/TextObject.h
        text/TextManager.cpp
        text/TextManager.h
)

#STB IMAGE
target_include_directories(Invisible PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image
)

#FreeType
add_subdirectory(external/freetype-2.14.1)

target_include_directories(Invisible PRIVATE external/freetype-2.14.1/include)

target_link_libraries(Invisible PRIVATE glfw glad glm soloud freetype)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()
