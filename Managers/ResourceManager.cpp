#include "ResourceManager.h"

#include <iostream>

#include "DeviceManager.h"
#include "EngineManager.h"
#include "WindowManager.h"


std::vector<ID3D11Resource*> ResourceManager::resources{};
std::vector<ID3D11View*> ResourceManager::resourceViews{};


void ResourceManager::Initialise()
{
}

void ResourceManager::Shutdown()
{
    for (ID3D11Resource* r : resources)
    {
        r->Release();
    }
    for (ID3D11View* v : resourceViews)
    {
        v->Release();
    }
}



ID3D11Texture2D* ResourceManager::GetActiveSwapchainTexture()
{
    ID3D11Texture2D* swapChainTexture{};
    HRESULT hr{ WindowManager::swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainTexture)) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::GET_ACTIVE_SWAPCHAIN_TEXTURE::FAILED_TO_GET_ACTIVE_SWAP_CHAIN_TEXTURE" << std::endl;
        return nullptr;
    }
    resources.push_back(swapChainTexture);
    return swapChainTexture;
}

ID3D11Texture2D* ResourceManager::CreateDepthStencilTexture()
{
    D3D11_TEXTURE2D_DESC td;
    td.Width = WindowManager::width;
    td.Height = WindowManager::height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.SampleDesc = {1,0};
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    ID3D11Texture2D* depthStencilTexture{};
    HRESULT hr{ DeviceManager::device->CreateTexture2D(&td, NULL, &depthStencilTexture) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_DEPTH_STENCIL_TEXTURE::FAILED_TO_CREATE_DEPTH_STENCIL_TEXTURE" << std::endl;
        return nullptr;
    }
    resources.push_back(depthStencilTexture);
    return depthStencilTexture;
}


//-----------------------------------------------//
//----------------BUFFER CREATION----------------//
//-----------------------------------------------//
ID3D11Buffer* ResourceManager::CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData)
{
    ID3D11Device* device{ DeviceManager::device };
    ID3D11Buffer* b;
    HRESULT hr{ device->CreateBuffer(pDesc, pData, &b) };

    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_BUFFER::FAILED_TO_CREATE_BUFFER::CALLED_FROM::";
        return nullptr;
    }
    resources.push_back(b);
    return b;
}

ID3D11Buffer* ResourceManager::CreateVertexBuffer(UINT size, bool dynamic, bool streamout, D3D11_SUBRESOURCE_DATA* pData)
{
    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = size;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    bd.BindFlags = (streamout) ? (D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT) : (D3D11_BIND_VERTEX_BUFFER);
    bd.Usage = (dynamic) ? (D3D11_USAGE_DYNAMIC) : (D3D11_USAGE_IMMUTABLE);
    bd.CPUAccessFlags = (dynamic) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Buffer* b{ CreateBuffer(&bd, pData) };
    if (!b) { std::cerr << "RESOURCE_MANAGER::CREATE_VERTEX_BUFFER" << std::endl; } //Append error message from ResourceManager::CreateBuffer
    return b;
}

ID3D11Buffer* ResourceManager::CreateIndexBuffer(UINT size, bool dynamic, D3D11_SUBRESOURCE_DATA* pData)
{
    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = size;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    bd.Usage = (dynamic) ? (D3D11_USAGE_DYNAMIC) : (D3D11_USAGE_IMMUTABLE);
    bd.CPUAccessFlags = (dynamic) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Buffer* b{ CreateBuffer(&bd, pData) };
    if (!b) { std::cerr << "RESOURCE_MANAGER::CREATE_INDEX_BUFFER" << std::endl; } //Append error message from ResourceManager::CreateBuffer
    return b;
}

ID3D11Buffer* ResourceManager::CreateConstantBuffer(UINT size, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{

    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_CONSTANT_BUFFER::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        return nullptr;
    }
    
    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = size;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    if      (CPUWriteable && !GPUWriteable) { bd.Usage = D3D11_USAGE_DYNAMIC; }
    else if (GPUWriteable && !CPUWriteable) { bd.Usage = D3D11_USAGE_DEFAULT; }
    else                                    { bd.Usage = D3D11_USAGE_IMMUTABLE; }

    bd.CPUAccessFlags = (CPUWriteable) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Buffer* b{ CreateBuffer(&bd, pData) };
    if (!b) { std::cerr << "RESOURCE_MANAGER::CREATE_CONSTANT_BUFFER" << std::endl; } //Append error message from ResourceManager::CreateBuffer
    return b;
}
//----------------------------------------------//
//------------END OF BUFFER CREATION------------//
//----------------------------------------------//



//-----------------------------------------------//
//-----------------VIEW CREATION-----------------//
//-----------------------------------------------//
ID3D11DepthStencilView* ResourceManager::CreateDepthStencilView(ID3D11Texture2D* depthStencilTexture)
{
    ID3D11DepthStencilView* depthStencilView;
    HRESULT hr{ DeviceManager::device->CreateDepthStencilView(depthStencilTexture, NULL, &depthStencilView) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_DEPTH_STENCIL_VIEW::FAILED_TO_CREATE_DEPTH_STENCIL_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(depthStencilView);
    return depthStencilView;
}

ID3D11RenderTargetView* ResourceManager::CreateRenderTargetView(ID3D11Texture2D* swapChainTexture)
{
    ID3D11RenderTargetView* renderTargetView;
    HRESULT hr{ DeviceManager::device->CreateRenderTargetView(swapChainTexture, NULL, &renderTargetView) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_RENDER_TARGET_VIEW::FAILED_TO_CREATE_RENDER_TARGET_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(renderTargetView);
    return renderTargetView;
}
//----------------------------------------------//
//-------------END OF VIEW CREATION-------------//
//----------------------------------------------//