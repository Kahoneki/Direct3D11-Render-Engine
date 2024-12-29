#pragma once
#include <d3d11.h>
#include <vector>

class RenderManager
{
    friend class EngineManager;
    
private:
    RenderManager() = default;
    static void Initialise();
    static void Shutdown();
    ~RenderManager() = default;

    static void Render(float* clearColour);
};