# TODO: CMake

OUT=prog

GLAD_DIR="ext/glad"
GLAD_SRC=$GLAD_DIR/src/glad.c

IMGUI_DIR="ext/imgui"
IMGUI_SRC="$IMGUI_DIR/imgui.cpp $IMGUI_DIR/imgui_draw.cpp $IMGUI_DIR/imgui_widgets.cpp $IMGUI_DIR/imgui_tables.cpp $IMGUI_DIR/backends/imgui_impl_glfw.cpp $IMGUI_DIR/backends/imgui_impl_opengl3.cpp"

LIBS="-lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl"
SOURCES="main.cpp Shader.cpp Window.cpp Input.cpp StaticMesh.cpp Texture2D.cpp Entity.cpp TransformComponent.cpp Camera.cpp UIHelper.cpp"

g++ -Wall -Wextra  -o $OUT $GLAD_SRC $IMGUI_SRC $SOURCES -Iext/ -I$GLAD_DIR -I$GLAD_DIR/include -I$IMGUI_DIR -I$IMGUI_DIR/backends $LIBS
