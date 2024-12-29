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
    
    static ID3D11Buffer* CreateVertexBuffer(UINT size, bool dynamic, bool streamout, D3D11_SUBRESOURCE_DATA* pData);
    static ID3D11Buffer* CreateIndexBuffer(UINT size, bool dynamic, D3D11_SUBRESOURCE_DATA* pData);
    static ID3D11Buffer* CreateConstantBuffer(UINT size, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData);

    static ID3D11DepthStencilView* CreateDepthStencilView(ID3D11Texture2D* depthStencilTexture);
    static ID3D11RenderTargetView* CreateRenderTargetView(ID3D11Texture2D* swapChainTexture);
    
private:
    static void Initialise();
    static void Shutdown();

    //For cleanup
    static std::vector<ID3D11Resource*> resources;
    static std::vector<ID3D11View*> resourceViews; 

    //Utility functions
    static ID3D11Buffer* CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData);
};