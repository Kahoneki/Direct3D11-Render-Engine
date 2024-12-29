#include "PipelineManager.h"

#include "DeviceManager.h"
#include "EngineManager.h"

void PipelineManager::Initialise()
{
}

void PipelineManager::Shutdown()
{
}



//--------------------------------//
//----------View Methods----------//
//--------------------------------//
ID3D11DepthStencilView* PipelineManager::GetCurrentDepthStencilView()
{
    std::vector<ID3D11RenderTargetView*> rtv{ D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullptr };
    ID3D11DepthStencilView* dsv;
    DeviceManager::context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtv.data(), &dsv);

    return dsv;
}

std::vector<ID3D11RenderTargetView*> PipelineManager::GetCurrentRenderTargetViews()
{
    std::vector<ID3D11RenderTargetView*> rtv{ D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullptr };
    ID3D11DepthStencilView* dsv;
    DeviceManager::context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtv.data(), &dsv);

    return rtv;
}

void PipelineManager::BindDepthStencilView(ID3D11DepthStencilView* depthStencilView)
{
    std::vector<ID3D11RenderTargetView*> rtv{ GetCurrentRenderTargetViews() };
    DeviceManager::context->OMSetRenderTargets(rtv.size(), rtv.data(), depthStencilView);
}

void PipelineManager::BindRenderTargetViews(const std::vector<ID3D11RenderTargetView*>& renderTargetViews)
{
    ID3D11DepthStencilView* dsv{ GetCurrentDepthStencilView() };
    DeviceManager::context->OMSetRenderTargets(renderTargetViews.size(), renderTargetViews.data(), dsv);
}

void PipelineManager::ClearRenderTargetView(ID3D11RenderTargetView* renderTargetView, float* clearColour)
{
    DeviceManager::context->ClearRenderTargetView(renderTargetView, clearColour);
}

void PipelineManager::ClearDepthStencilView(ID3D11DepthStencilView* depthStencilView, float clearDepth, UINT8 clearStencil)
{
    DeviceManager::context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, clearDepth, clearStencil);
}

//---------------------------------//
//-------End of View Methods-------//
//---------------------------------//



//--------------------------------//
//---------Buffer Methods---------//
//--------------------------------//
void PipelineManager::BindVertexBuffer(ID3D11Buffer* vertexBuffer)
{
}

void PipelineManager::BindIndexBuffer(ID3D11Buffer* indexBuffer)
{
}

void PipelineManager::BindConstantBuffer(ID3D11Buffer* constantBuffer)
{
}
//---------------------------------//
//------End of Buffer Methods------//
//---------------------------------//