#include "RenderManager.h"

#include <iostream>

#include "EngineManager.h"
#include "PipelineManager.h"
#include "WindowManager.h"

void RenderManager::Initialise()
{
}

void RenderManager::Shutdown()
{
}



void RenderManager::Render(float* clearColour)
{
    ID3D11RenderTargetView* rtv{ PipelineManager::GetCurrentRenderTargetViews()[0] };
    ID3D11DepthStencilView* dsv{ PipelineManager::GetCurrentDepthStencilView() };
    PipelineManager::ClearRenderTargetView(rtv, clearColour);
    PipelineManager::ClearDepthStencilView(dsv, 0, 0);

    HRESULT hr{ WindowManager::swapChain->Present(0, NULL) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RENDER_MANAGER::RENDER::FAILED_TO_PRESENT_SWAP_CHAIN" << std::endl;
        return;
    }
}
