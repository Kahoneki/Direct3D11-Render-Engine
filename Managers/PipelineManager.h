#pragma once
#include <d3d11.h>
#include <vector>

class PipelineManager
{
    friend class EngineManager;
    
public:
    PipelineManager() = default;
    ~PipelineManager() = default;
    
    //----View Methods----//
    static ID3D11DepthStencilView* GetCurrentDepthStencilView();
    static std::vector<ID3D11RenderTargetView*> GetCurrentRenderTargetViews();
    
    static void BindDepthStencilView(ID3D11DepthStencilView* depthStencilView);
    static void BindRenderTargetViews(const std::vector<ID3D11RenderTargetView*>& renderTargetViews);

    static void ClearRenderTargetView(ID3D11RenderTargetView* renderTargetView, float* clearColour);
    static void ClearDepthStencilView(ID3D11DepthStencilView* depthStencilView, float clearDepth, UINT8 clearStencil);

    
    //----Buffer Methods----//
    static void BindVertexBuffer(ID3D11Buffer* vertexBuffer);
    static void BindIndexBuffer(ID3D11Buffer* indexBuffer);
    static void BindConstantBuffer(ID3D11Buffer* constantBuffer);
    
private:
    static void Initialise();
    static void Shutdown();
};
