#pragma once
#include <memory>

#include <d3d11.h>

class DeviceManager;
class WindowManager;
class ResourceManager;
class PipelineManager;
class RenderManager;


struct WindowDescription
{
    UINT winWidth;
    UINT winHeight;
};

struct RenderDescription
{
    float* clearColour;
};

struct EngineDescription
{
    WindowDescription wd;
    RenderDescription rd;
};


class EngineManager
{
    friend class DeviceManager;
    friend class WindowManager;
    friend class ResourceManager;
    friend class PipelineManager;
    friend class RenderManager;

public:
    static void Initialise(const EngineDescription& _ed);
    static void Update();
    static void Shutdown();

    static bool applicationRunning;
    
private:
    static EngineDescription ed;
};