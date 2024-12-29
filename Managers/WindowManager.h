#pragma once

#include <dxgi.h>
#include <d3d11.h>

class WindowManager
{
    friend class EngineManager;
    friend class ResourceManager;
    friend class RenderManager;

public:
    WindowManager() = default;
    ~WindowManager() = default;
    
private:
    static void Initialise(const UINT _width, const UINT _height);
    static void Update();
    static void Shutdown();

    static UINT width;
    static UINT height;
    
    static HWND hwnd;
    static IDXGISwapChain* swapChain;
};