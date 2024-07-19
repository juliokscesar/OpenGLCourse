cmake_minimum_required(VERSION 3.28)
project(OpenGLCourse)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_CXX_STANDARD 20)

set(GLAD_DIR ext/glad)
set(GLAD_SRC ${GLAD_DIR}/src/glad.c)

set(IMGUI_DIR ext/imgui)
set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(ASSIMP_DIR ext/assimp)

set(SRC
    main.cpp
    Shader.cpp
    Window.cpp
    Input.cpp
    TransformComponent.cpp
    Camera.cpp
    UIHelper.cpp
    ModelFactory.cpp
    StaticMesh.cpp
)

add_executable(prog
    ${GLAD_SRC}
    ${IMGUI_SRC}
    ${SRC}
)

target_include_directories(prog PUBLIC
    ext
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${ASSIMP_DIR}/include
)

target_link_directories(prog PUBLIC
    ${ASSIMP_DIR}/bin
)

target_link_libraries(prog PUBLIC
    -lglfw
    -lGL
    -lX11
    -lpthread
    -lXrandr
    -lXi
    -ldl
    assimp
)

