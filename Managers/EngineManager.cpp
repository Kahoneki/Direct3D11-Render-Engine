#include "EngineManager.h"

#include "DeviceManager.h"
#include "PipelineManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "WindowManager.h"

bool EngineManager::applicationRunning{};
EngineDescription EngineManager::ed{};

void EngineManager::Initialise(const EngineDescription& _ed)
{
    ed = _ed;
    applicationRunning = true;

    DeviceManager::Initialise();
    WindowManager::Initialise(ed.wd.winWidth, ed.wd.winHeight);
    ResourceManager::Initialise();
    PipelineManager::Initialise();
    RenderManager::Initialise();
}

void EngineManager::Update()
{
    WindowManager::Update();
    RenderManager::Render(ed.rd.clearColour);
}

void EngineManager::Shutdown()
{
    DeviceManager::Shutdown();
    WindowManager::Shutdown();
    ResourceManager::Shutdown();
    PipelineManager::Shutdown();
    RenderManager::Shutdown();
}