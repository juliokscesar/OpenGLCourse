@echo off

:: Build GLFW
cd ext\glfw
if not exist build mkdir build
cd build
cmake ..
cmake --build .



:: Build assimp
cd ..\..\assimp
if not exist build mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=0 ..
