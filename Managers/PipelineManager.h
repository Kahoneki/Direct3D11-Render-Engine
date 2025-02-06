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
    COMPUTE_SHADER,
};


class PipelineManager
{
    friend class EngineManager;
    
public:
    PipelineManager() = default;
    ~PipelineManager() = default;
    
    //----View Methods----//
    [[nodiscard]] static ID3D11DepthStencilView* GetCurrentDepthStencilView();
    [[nodiscard]] static std::vector<ID3D11RenderTargetView*> GetCurrentRenderTargetViews();
    
    static void BindDepthStencilView(ID3D11DepthStencilView* depthStencilView);
    static void BindRenderTargetViews(const std::vector<ID3D11RenderTargetView*>& renderTargetViews);
    static void BindShaderResourceViews(ID3D11ShaderResourceView* shaderResourceViews, PIPELINE_STAGE stage, UINT startSlot, UINT numViews);
    static void BindUnorderedAccessViews(ID3D11UnorderedAccessView* unorderedAccessViews, PIPELINE_STAGE stage, UINT startSlot, UINT numViews, UINT* initialCounts=nullptr);

    static void ClearRenderTargetView(ID3D11RenderTargetView* renderTargetView, FLOAT* clearColour);
    static void ClearDepthStencilView(ID3D11DepthStencilView* depthStencilView, FLOAT clearDepth, UINT8 clearStencil);
    static void ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView* unorderedAccessView, FLOAT clearValue[4]);
    static void ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView* unorderedAccessView, UINT clearValue[4]);

    
    //----Buffer Methods----//
    static void BindVertexBuffers(ID3D11Buffer* vertexBuffers, UINT startSlot, UINT numBuffers, UINT stride, UINT offset=0);
    static void BindIndexBuffer(ID3D11Buffer* indexBuffer, DXGI_FORMAT format=DXGI_FORMAT_R32_UINT, UINT offset=0);
    static void BindConstantBuffers(PIPELINE_STAGE stage, UINT startSlot, UINT numBuffers, ID3D11Buffer* constantBuffer);
    

    //----Sampler Methods----//
    static void BindSamplerStates(ID3D11SamplerState* samplerState, PIPELINE_STAGE stage, UINT startSlot, UINT numSamplerStates);
    [[nodiscard]] static ID3D11SamplerState* GetSamplerStates(PIPELINE_STAGE stage, UINT startSlot, UINT numSamplerStates);

private:
    static void Initialise();
    static void Shutdown();
};
