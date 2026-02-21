cmake_minimum_required(VERSION 3.16)
project(selection_demo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------
# GLFW: 优先查找本地安装，找不到则自动下载
# ------------------------------------------------------------------
find_package(glfw3 3.3 QUIET)

if(NOT glfw3_FOUND)
    message(STATUS "glfw3 not found locally, fetching via FetchContent ...")
    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.10
        GIT_SHALLOW    TRUE
    )
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

# ------------------------------------------------------------------
# OpenGL
# ------------------------------------------------------------------
find_package(OpenGL REQUIRED)

# ------------------------------------------------------------------
# ImGui
# ------------------------------------------------------------------
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

# ------------------------------------------------------------------
# 可执行目标
# ------------------------------------------------------------------
add_executable(selection_demo
    main.cpp
    ${GLAD_DIR}/src/glad.c
    ${IMGUI_SOURCES}
)

target_include_directories(selection_demo PRIVATE
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(selection_demo PRIVATE
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)
