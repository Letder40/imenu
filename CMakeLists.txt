cmake_minimum_required(VERSION 3.30...4.4)

project (imenu
    VERSION 0.1.0
    DESCRIPTION "Configurable quick menu with IMGUI paradigm"
    LANGUAGES CXX
)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

file (CREATE_LINK
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
    SYMBOLIC
)

set (IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    OpenGL::GL
)

add_executable(imenu 
    src/imenu.cpp
)

target_compile_features(imenu PRIVATE cxx_std_26)

target_link_libraries(imenu PRIVATE
    imgui
)
