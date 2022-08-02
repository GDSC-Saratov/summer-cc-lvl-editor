cmake_minimum_required(VERSION 3.10)
project (summer-cc-lvl-editor)
set (CMAKE_CXX_STANDARD 17)

add_executable (Game
    Source/main.cpp
    Source/components.cpp
    Source/json_loader.cpp
    Source/render.cpp
    Source/editor.cpp
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)

add_subdirectory (Dependencies)

target_link_libraries (Game LINK_PRIVATE raylib-ext)
target_link_libraries (Game LINK_PRIVATE JSON)
target_link_libraries (Game LINK_PRIVATE box2d)
