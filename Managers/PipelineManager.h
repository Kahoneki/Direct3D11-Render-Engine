#pragma once
#include <d3d11.h>
#include <vector>


enum PIPELINE_STAGE
{
    VERTEX_SHADER,
    DOMAIN_SHADER,
    HULL_SHADER,
    GEOMETRY_SHADER,
    PIXEL_SHADER,
};


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
    static void BindVertexBuffer(ID3D11Buffer* vertexBuffer, UINT stride, UINT offset=0);
    static void BindIndexBuffer(ID3D11Buffer* indexBuffer, DXGI_FORMAT format=DXGI_FORMAT_R32_UINT, UINT offset=0);
    static void BindConstantBuffer(PIPELINE_STAGE stage, UINT slot, ID3D11Buffer* constantBuffer);
    
private:
    static void Initialise();
    static void Shutdown();
};
