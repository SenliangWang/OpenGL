cmake_minimum_required(VERSION 3.16)
project(selection_demo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)
find_package(OpenGL REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imgui)
set(GLAD_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/glad)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp
)

add_executable(selection_demo
    main.cpp
    ${GLAD_DIR}/src/glad.c
    ${IMGUI_SOURCES}
)

target_include_directories(selection_demo PRIVATE
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLFW3_INCLUDE_DIRS}
)

target_link_libraries(selection_demo PRIVATE
    ${GLFW3_LIBRARIES}
    OpenGL::GL
    OpenGL::GLU
    ${CMAKE_DL_LIBS}
)
