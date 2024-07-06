#include <iostream>

#include "Window.hpp"

int main()
{
    Window window("Hello World!", 800, 600);
    try
    {
        window.Init();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    window.MainLoop();

    return 0;
}
