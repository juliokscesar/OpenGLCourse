@echo off

:: Build GLFW
cd ext\glfw
if not exist build mkdir build
cd build
cmake ..
cmake --build . --target ALL_BUILD --config Release



:: Build assimp
cd ..\..\assimp
cmake CMakeLists.txt
cmake --build . --target ALL_BUILD --config Release
