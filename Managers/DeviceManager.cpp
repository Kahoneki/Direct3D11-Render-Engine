#include "DeviceManager.h"

#include <iostream>

ID3D11Device* DeviceManager::device{};
ID3D11DeviceContext* DeviceManager::context{};
D3D_FEATURE_LEVEL DeviceManager::featureLevel{};

void DeviceManager::Initialise()
{

    if (device != nullptr)
    {
        std::cerr << "ERROR::DEVICE_MANAGER::INITIALISE::DEVICE_ALREADY_INITIALISED" << std::endl;
        return;
    }
    
    constexpr D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    const HRESULT hr{ D3D11CreateDevice(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_DEBUG,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &context
    ) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::DEVICE_MANAGER::INITIALISE::FAILED_TO_CREATE_DEVICE" << std::hex << hr << std::endl;
    }
    if (featureLevel < D3D_FEATURE_LEVEL_11_0) {
        std::cerr << "ERROR::DEVICE_MANAGER::INITIALISE::DEVICE_DOES_NOT_SUPPORT_DX11" << std::endl;
    }
}

void DeviceManager::Shutdown()
{
    context->ClearState();
    device->Release();
    context->Release();
}