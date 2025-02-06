#pragma once
#include <d3d11.h>
#include <vector>

class ResourceManager
{
    friend class EngineManager;
    
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    static ID3D11Texture2D* GetActiveSwapchainTexture();
    static ID3D11Texture2D* CreateDepthStencilTexture();

    
    //----Buffers----//
    [[nodiscard]] static ID3D11Buffer* CreateVertexBuffer(UINT size, bool dynamic, bool streamout, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Buffer* CreateIndexBuffer(UINT size, bool dynamic, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Buffer* CreateConstantBuffer(UINT size, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Buffer* CreateStructuredBuffer(UINT count, UINT structSize, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Buffer* CreateAppendConsumeBuffer(UINT count, UINT structSize, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Buffer* CreateRawBuffer(UINT size, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Buffer* CreateIndirectArgsBuffer(UINT size, D3D11_SUBRESOURCE_DATA* pData);

    [[nodiscard]] static ID3D11ShaderResourceView* CreateBufferShaderResourceView(ID3D11Buffer* pResource, UINT offset, UINT count, DXGI_FORMAT format, UINT flags=0);
    [[nodiscard]] static ID3D11UnorderedAccessView* CreateBufferUnorderedAccessView(ID3D11Buffer* pResource, UINT offset, UINT count, DXGI_FORMAT format, UINT flags=0);


    //----Textures----//
    [[nodiscard]] static ID3D11Texture1D* CreateTexture1D(UINT width, UINT mipLevels, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Texture1D* CreateTexture1DArray(UINT width, UINT mipLevels, UINT arraySize, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Texture2D* CreateTexture2D(UINT width, UINT height, UINT mipLevels, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Texture2D* CreateTexture2DArray(UINT width, UINT height, UINT mipLevels, UINT arraySize, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    [[nodiscard]] static ID3D11Texture3D* CreateTexture3D(UINT width, UINT height, UINT depth, UINT mipLevels, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);
    
    [[nodiscard]] static ID3D11RenderTargetView* CreateTexture1DRenderTargetView(ID3D11Texture1D* texture, UINT mipSlice, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11DepthStencilView* CreateTexture1DDepthStencilView(ID3D11Texture1D* texture, UINT mipSlice, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11ShaderResourceView* CreateTexture1DShaderResourceView(ID3D11Texture1D* texture, UINT mostDetailedMipSlice, UINT mipLevels, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11UnorderedAccessView* CreateTexture1DUnorderedAccessView(ID3D11Texture1D* texture, UINT mipSlice, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11RenderTargetView* CreateTexture1DArrayRenderTargetView(ID3D11Texture1D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11DepthStencilView* CreateTexture1DArrayDepthStencilView(ID3D11Texture1D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11ShaderResourceView* CreateTexture1DArrayShaderResourceView(ID3D11Texture1D* texture, UINT mostDetailedMipSlice, UINT mipLevels, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11UnorderedAccessView* CreateTexture1DArrayUnorderedAccessView(ID3D11Texture1D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11RenderTargetView* CreateTexture2DRenderTargetView(ID3D11Texture2D* texture, UINT mipSlice, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11DepthStencilView* CreateTexture2DDepthStencilView(ID3D11Texture2D* texture, UINT mipSlice, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11ShaderResourceView* CreateTexture2DShaderResourceView(ID3D11Texture2D* texture, UINT mostDetailedMipSlice, UINT mipLevels, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11UnorderedAccessView* CreateTexture2DUnorderedAccessView(ID3D11Texture2D* texture, UINT mipSlice, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11RenderTargetView* CreateTexture2DArrayRenderTargetView(ID3D11Texture2D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11DepthStencilView* CreateTexture2DArrayDepthStencilView(ID3D11Texture2D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11ShaderResourceView* CreateTexture2DArrayShaderResourceView(ID3D11Texture2D* texture, UINT mostDetailedMipSlice, UINT mipLevels, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11UnorderedAccessView* CreateTexture2DArrayUnorderedAccessView(ID3D11Texture2D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11RenderTargetView* CreateTexture3DRenderTargetView(ID3D11Texture3D* texture, UINT mipSlice, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11ShaderResourceView* CreateTexture3DShaderResourceView(ID3D11Texture3D* texture, UINT mostDetailedMipSlice, UINT mipLevels, DXGI_FORMAT format);
    [[nodiscard]] static ID3D11UnorderedAccessView* CreateTexture3DUnorderedAccessView(ID3D11Texture3D* texture, UINT mipSlice, DXGI_FORMAT format);
    
    [[nodiscard]] static ID3D11DepthStencilView* CreateDepthStencilView(ID3D11Texture2D* texture);
    [[nodiscard]] static ID3D11RenderTargetView* CreateRenderTargetView(ID3D11Texture2D* texture);


    //----Samplers----//
    [[nodiscard]] static ID3D11SamplerState* CreateSamplerState(D3D11_SAMPLER_DESC samplerDesc);
    
private:
    static void Initialise();
    static void Shutdown();

    //For cleanup
    static std::vector<ID3D11Resource*> resources;
    static std::vector<ID3D11View*> resourceViews;
    static std::vector<ID3D11SamplerState*> samplerStates;
    

    //Utility functions
    [[nodiscard]] static ID3D11Buffer* CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData);
};