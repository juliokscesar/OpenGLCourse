cd ext/glfw
if [[ ! -d build ]]; then
    mkdir build
fi

cd build
cmake ..
cmake --build .


cd ../../assimp
cmake CMakeLists.txt
cmake --build .
