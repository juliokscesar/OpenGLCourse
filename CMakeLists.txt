cmake_minimum_required(VERSION 3.28)
project(notanengine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


if ("$<CONFIG>:DEBUG")
    add_compile_definitions("_NE_DEBUG")
endif()


if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()



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

set(GLFW_INCLUDE ext/glfw/include)

if (WIN32)
    set(GLFW_LIBS ext/glfw/build/src/Release)
    set(ASSIMP_LIBS ext/assimp/lib/Release)
else()
    set(GLFW_LIBS ext/glfw/build)
    set(ASSIMP_LIBS ext/assimp/bin)
endif()

set(INCLUDE_DIRS
    ext
    ${GLFW_INCLUDE}
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${ASSIMP_DIR}/include
)

set(LIB_DIRS
    ${GLFW_LIBS}
    ${ASSIMP_LIBS}
)



    
if (WIN32)
    add_compile_definitions("_NE_WINDOWS")
    set(LIBS
        glfw3
        opengl32
        assimp-vc143-mt
    )
else()
    add_compile_definitions("_NE_LINUX")
    set(LIBS
        -lGL
        -lX11
        -lpthread
        -lXrandr
        -lXi
        -ldl
        assimp
        glfw
    )
endif()


set(SRC
    ${GLAD_SRC}
    ${IMGUI_SRC}
    ${PROJECT_NAME}/main.cpp
    ${PROJECT_NAME}/Shader.cpp
    ${PROJECT_NAME}/Window.cpp
    ${PROJECT_NAME}/Input.cpp
    ${PROJECT_NAME}/TransformComponent.cpp
    ${PROJECT_NAME}/Camera.cpp
    ${PROJECT_NAME}/UIHelper.cpp
    ${PROJECT_NAME}/Texture2D.cpp
    ${PROJECT_NAME}/ResourceManager.cpp
    ${PROJECT_NAME}/StaticMesh.cpp
    ${PROJECT_NAME}/Entity.cpp
    ${PROJECT_NAME}/Light.cpp
    ${PROJECT_NAME}/Render.cpp
)

include_directories(${INCLUDE_DIRS})


if(MSVC)
    set(HEADER_FILES
        ${PROJECT_NAME}/Shader.hpp
        ${PROJECT_NAME}/Window.hpp
        ${PROJECT_NAME}/Input.hpp
        ${PROJECT_NAME}/TransformComponent.hpp
        ${PROJECT_NAME}/Camera.hpp
        ${PROJECT_NAME}/UIHelper.hpp
        ${PROJECT_NAME}/Texture2D.hpp
        ${PROJECT_NAME}/ResourceManager.hpp
        ${PROJECT_NAME}/StaticMesh.hpp
        ${PROJECT_NAME}/Entity.hpp
        ${PROJECT_NAME}/Light.hpp
        ${PROJECT_NAME}/Render.hpp
    )

    add_executable(notanengine ${SRC} ${HEADER_FILES})


    target_link_directories(notanengine PUBLIC
        ${LIB_DIRS}
    )

    target_link_libraries(notanengine PUBLIC
        ${LIBS}
    )

else()
    add_executable(notanengine
        ${SRC}
    )

    target_link_directories(notanengine PUBLIC
        ${LIB_DIRS}
    )

    target_link_libraries(notanengine PUBLIC
        ${LIBS}
    )
endif()

set_target_properties(notanengine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "./Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "./Release"
)
