#include "Core/include/pme_app.h"


pme::App::App() : window{WIDTH, HEIGHT, "PhysicsModelingEngine"}, device{window}, swapChain{device, window.GetWindowExtent()}
{
    try
    {

    }
    catch(std::runtime_error ex)
    {

    }
}

pme::App::~App()
{

}

void pme::App::Run()
{
    while (!window.ShouldClose())
    {
        glfwPollEvents();
    }
}
