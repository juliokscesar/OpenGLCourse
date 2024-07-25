@echo off

:: Build GLFW
cd ext\glfw
if not exist build mkdir build
cd build
cmake ..
cmake --build .



:: Build assimp
cd ..\..\assimp
cmake -DBUILD_SHARED_LIBS=0 ..
cmake --build .
