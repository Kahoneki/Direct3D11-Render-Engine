#pragma once

#include <d3d11.h>

//This class is only accessible to EngineManager
//EngineManager is responsible for injecting dependencies into any other classes' constructors
//e.g. ResourceManager::Initialise(ID3D11Device* device, ID3D11DeviceContext* context, D3D_FEATURE_LEVEL featureLevel)
class DeviceManager
{
    friend class EngineManager;
    friend class WindowManager;
    friend class ResourceManager;
    friend class PipelineManager;

public:
    DeviceManager() = default;
    ~DeviceManager() = default;

private:
    static void Initialise();
    static void Shutdown();
    
    static ID3D11Device* device;
    static ID3D11DeviceContext* context;
    static D3D_FEATURE_LEVEL featureLevel;
};