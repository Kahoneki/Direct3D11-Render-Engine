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
    ID3D11Buffer* b;
    HRESULT hr{ DeviceManager::device->CreateBuffer(pDesc, pData, &b) };

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

    bool exit{ false };
    
    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_CONSTANT_BUFFER::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        exit = true;
    }
    if (exit) { return nullptr; }
    
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

ID3D11Buffer* ResourceManager::CreateStructuredBuffer(UINT count, UINT structSize, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{
    bool exit{ false };
    
    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_STRUCTURED_BUFFER::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        exit = true;
    }
    if (exit) { return nullptr; }

    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = count * structSize;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = structSize;

    if (CPUWriteable && !GPUWriteable)
    {
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.Usage = D3D11_USAGE_DYNAMIC;
    }
    else if (GPUWriteable && !CPUWriteable)
    {
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        bd.Usage = D3D11_USAGE_DEFAULT;
    }
    else
    {
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.Usage = D3D11_USAGE_IMMUTABLE;
    }

    bd.CPUAccessFlags = (CPUWriteable) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Buffer* b{ CreateBuffer(&bd, pData) };
    if (!b) { std::cerr << "RESOURCE_MANAGER::CREATE_STRUCTURED_BUFFER" << std::endl; } //Append error message from ResourceManager::CreateBuffer
    return b;
}

ID3D11Buffer* ResourceManager::CreateAppendConsumeBuffer(UINT count, UINT structSize, D3D11_SUBRESOURCE_DATA* pData)
{
    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = count * structSize;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = structSize;
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;
    ID3D11Buffer* b{ CreateBuffer(&bd, pData) };
    if (!b) { std::cerr << "RESOURCE_MANAGER::CREATE_APPEND_CONSUME_BUFFER" << std::endl; } //Append error message from ResourceManager::CreateBuffer
    return b;
}

ID3D11Buffer* ResourceManager::CreateRawBuffer(UINT size, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{
    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = size;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    bd.StructureByteStride = 0;
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bd.Usage = (GPUWriteable) ? (D3D11_USAGE_DEFAULT) : (D3D11_USAGE_IMMUTABLE);
    bd.CPUAccessFlags = 0;
    ID3D11Buffer* b{ CreateBuffer(&bd, pData) };
    if (!b) { std::cerr << "RESOURCE_MANAGER::CREATE_RAW_BUFFER" << std::endl; } //Append error message from ResourceManager::CreateBuffer
    return b;
}

ID3D11Buffer* ResourceManager::CreateIndirectArgsBuffer(UINT size, D3D11_SUBRESOURCE_DATA* pData)
{

    if (size % 4 != 0)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_INDIRECT_ARGS_BUFFER::SIZE_MUST_BE_MULTIPLE_OF_4" << std::endl;
    }
    
    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = size;
    bd.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    bd.StructureByteStride = 0;
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;
    ID3D11Buffer* b{ CreateBuffer(&bd, pData) };
    if (!b) { std::cerr << "RESOURCE_MANAGER::CREATE_INDIRECT_ARGS_BUFFER" << std::endl; } //Append error message from ResourceManager::CreateBuffer
    return b;
}

//----------------------------------------------//
//------------END OF BUFFER CREATION------------//
//----------------------------------------------//



//----------------------------------------------//
//---------------TEXTURE CREATION---------------//
//----------------------------------------------//
ID3D11Texture1D* ResourceManager::CreateTexture1D(UINT width, UINT mipLevels, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{
    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        return nullptr;
    }
    
    D3D11_TEXTURE1D_DESC td;
    td.Width = width;
    td.MipLevels = mipLevels;
    td.ArraySize = 1;
    td.Format = format;
    td.MiscFlags = (mipLevels == 1) ? (D3D11_RESOURCE_MISC_RESOURCE_CLAMP) : (D3D11_RESOURCE_MISC_RESOURCE_CLAMP | D3D11_RESOURCE_MISC_GENERATE_MIPS);

    if (CPUWriteable && !GPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_DYNAMIC;
    }
    else if (GPUWriteable && !CPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        td.Usage = D3D11_USAGE_DEFAULT;
    }
    else
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_IMMUTABLE;
    }
    td.CPUAccessFlags = (CPUWriteable) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Texture1D* t;
    DeviceManager::device->CreateTexture1D(&td, pData, &t);
    if (!t)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D::FAILED_TO_CREATE_TEXTURE_1D" << std::endl;
        return nullptr;
    }
    return t;
}

ID3D11Texture1D* ResourceManager::CreateTexture1DArray(UINT width, UINT mipLevels, UINT arraySize, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{
    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_ARRAY::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        return nullptr;
    }
    
    D3D11_TEXTURE1D_DESC td;
    td.Width = width;
    td.MipLevels = mipLevels;
    td.ArraySize = arraySize;
    td.Format = format;
    td.MiscFlags = (mipLevels == 1) ? (D3D11_RESOURCE_MISC_RESOURCE_CLAMP) : (D3D11_RESOURCE_MISC_RESOURCE_CLAMP | D3D11_RESOURCE_MISC_GENERATE_MIPS);

    if (CPUWriteable && !GPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_DYNAMIC;
    }
    else if (GPUWriteable && !CPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        td.Usage = D3D11_USAGE_DEFAULT;
    }
    else
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_IMMUTABLE;
    }
    td.CPUAccessFlags = (CPUWriteable) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Texture1D* t;
    DeviceManager::device->CreateTexture1D(&td, pData, &t);
    if (!t)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_ARRAY::FAILED_TO_CREATE_TEXTURE_1D" << std::endl;
        return nullptr;
    }
    return t;
}

ID3D11Texture2D* ResourceManager::CreateTexture2D(UINT width, UINT height, UINT mipLevels, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{
    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        return nullptr;
    }
    
    D3D11_TEXTURE2D_DESC td;
    td.Width = width;
    td.MipLevels = mipLevels;
    td.ArraySize = 1;
    td.Format = format;
    td.MiscFlags = (mipLevels == 1) ? (D3D11_RESOURCE_MISC_RESOURCE_CLAMP) : (D3D11_RESOURCE_MISC_RESOURCE_CLAMP | D3D11_RESOURCE_MISC_GENERATE_MIPS);

    if (CPUWriteable && !GPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_DYNAMIC;
    }
    else if (GPUWriteable && !CPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        td.Usage = D3D11_USAGE_DEFAULT;
    }
    else
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_IMMUTABLE;
    }
    td.CPUAccessFlags = (CPUWriteable) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Texture2D* t;
    DeviceManager::device->CreateTexture2D(&td, pData, &t);
    if (!t)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D::FAILED_TO_CREATE_TEXTURE_2D" << std::endl;
        return nullptr;
    }
    return t;
}

ID3D11Texture2D* ResourceManager::CreateTexture2DArray(UINT width, UINT height, UINT mipLevels, UINT arraySize, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{
    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_ARRAY::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        return nullptr;
    }
    
    D3D11_TEXTURE2D_DESC td;
    td.Width = width;
    td.MipLevels = mipLevels;
    td.ArraySize = arraySize;
    td.Format = format;
    td.MiscFlags = (mipLevels == 1) ? (D3D11_RESOURCE_MISC_RESOURCE_CLAMP) : (D3D11_RESOURCE_MISC_RESOURCE_CLAMP | D3D11_RESOURCE_MISC_GENERATE_MIPS);

    if (CPUWriteable && !GPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_DYNAMIC;
    }
    else if (GPUWriteable && !CPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        td.Usage = D3D11_USAGE_DEFAULT;
    }
    else
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_IMMUTABLE;
    }
    td.CPUAccessFlags = (CPUWriteable) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Texture2D* t;
    DeviceManager::device->CreateTexture2D(&td, pData, &t);
    if (!t)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_ARRAY::FAILED_TO_CREATE_TEXTURE_2D" << std::endl;
        return nullptr;
    }
    return t;
}

ID3D11Texture3D* ResourceManager::CreateTexture3D(UINT width, UINT height, UINT depth, UINT mipLevels, DXGI_FORMAT format, bool CPUWriteable, bool GPUWriteable, D3D11_SUBRESOURCE_DATA* pData)
{
    if (CPUWriteable && GPUWriteable)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_3D::CANNOT_HAVE_SIMULTANEOUS_CPU_AND_GPU_WRITES" << std::endl;
        return nullptr;
    }
    
    D3D11_TEXTURE3D_DESC td;
    td.Width = width;
    td.MipLevels = mipLevels;
    td.Format = format;
    td.MiscFlags = (mipLevels == 1) ? (D3D11_RESOURCE_MISC_RESOURCE_CLAMP) : (D3D11_RESOURCE_MISC_RESOURCE_CLAMP | D3D11_RESOURCE_MISC_GENERATE_MIPS);

    if (CPUWriteable && !GPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_DYNAMIC;
    }
    else if (GPUWriteable && !CPUWriteable)
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        td.Usage = D3D11_USAGE_DEFAULT;
    }
    else
    {
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Usage = D3D11_USAGE_IMMUTABLE;
    }
    td.CPUAccessFlags = (CPUWriteable) ? (D3D11_CPU_ACCESS_WRITE) : (0);

    ID3D11Texture3D* t;
    DeviceManager::device->CreateTexture3D(&td, pData, &t);
    if (!t)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_3D::FAILED_TO_CREATE_TEXTURE_2D" << std::endl;
        return nullptr;
    }
    return t;
}
//-----------------------------------------------//
//------------END OF TEXTURE CREATION------------//
//-----------------------------------------------//



//-----------------------------------------------//
//-----------------VIEW CREATION-----------------//
//-----------------------------------------------//
ID3D11DepthStencilView* ResourceManager::CreateDepthStencilView(ID3D11Texture2D* texture)
{
    ID3D11DepthStencilView* depthStencilView;
    HRESULT hr{ DeviceManager::device->CreateDepthStencilView(texture, NULL, &depthStencilView) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_DEPTH_STENCIL_VIEW::FAILED_TO_CREATE_DEPTH_STENCIL_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(depthStencilView);
    return depthStencilView;
}

ID3D11RenderTargetView* ResourceManager::CreateRenderTargetView(ID3D11Texture2D* texture)
{
    ID3D11RenderTargetView* renderTargetView;
    HRESULT hr{ DeviceManager::device->CreateRenderTargetView(texture, NULL, &renderTargetView) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_RENDER_TARGET_VIEW::FAILED_TO_CREATE_RENDER_TARGET_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(renderTargetView);
    return renderTargetView;
}

ID3D11ShaderResourceView* ResourceManager::CreateBufferShaderResourceView(ID3D11Buffer* pResource, UINT offset, UINT count, DXGI_FORMAT format, UINT flags)
{
    bool exit{ false };
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;

    D3D11_BUFFER_DESC bd;
    pResource->GetDesc(&bd);
    if ((bd.BindFlags & D3D11_BIND_SHADER_RESOURCE) == 0)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_SHADER_RESOURCE_VIEW::SHADER_RESOURCE_VIEWS_REQUIRE_D3D11_BIND_SHADER_RESOURCE_FLAG_ON_RESOURCE" << std::endl;
        exit = true;
    }
    if ((bd.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) != 0)
    {
        //Structured buffer
        if (format != DXGI_FORMAT_UNKNOWN)
        {
            std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_SHADER_RESOURCE_VIEW::VIEWS_ON_STRUCTURED_BUFFERS_MUST_USE_DXGI_FORMAT_UNKNOWN" << std::endl;
            exit = true;
        }
    }
    if ((flags & D3D11_BUFFEREX_SRV_FLAG_RAW) != 0)
    {
        //Raw view
        if (format != DXGI_FORMAT_R32_TYPELESS)
        {
            std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_SHADER_RESOURCE_VIEW::RESOURCE_VIEWS_WITH_D3D11_BUFFEREX_SRV_FLAG_RAW_MUST_USE_DXGI_FORMAT_R32_TYPELESS" << std::endl;
            exit = true;
        }
        if ((bd.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS) == 0)
        {
            std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_SHADER_RESOURCE_VIEW::RESOURCE_VIEWS_WITH_D3D11_BUFFEREX_SRV_FLAG_RAW_REQUIRE_D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS_FLAG_SET_ON_RESOURCE" << std::endl;
            exit = true;
        }
    }
    if (exit) { return nullptr; }
    srvd.Format = format;

    srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvd.BufferEx.FirstElement = offset;
    srvd.BufferEx.NumElements = count;
    srvd.BufferEx.Flags = flags;

    ID3D11ShaderResourceView* srv;
    HRESULT hr{ DeviceManager::device->CreateShaderResourceView(pResource, &srvd, &srv) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_SHADER_RESOURCE_VIEW::FAILED_TO_CREATE_SHADER_RESOURCE_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(srv);
    return srv;
}

ID3D11UnorderedAccessView* ResourceManager::CreateBufferUnorderedAccessView(ID3D11Buffer* pResource, UINT offset, UINT count, DXGI_FORMAT format, UINT flags)
{
    bool exit{ false };
    
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;

    D3D11_BUFFER_DESC bd;
    pResource->GetDesc(&bd);
    if ((bd.BindFlags & D3D11_BIND_UNORDERED_ACCESS) == 0)
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_UNORDERED_ACCESS_VIEW::UNORDERED_ACCESS_VIEWS_REQUIRE_D3D11_BIND_UNORDERED_ACCESS_FLAG_ON_RESOURCE" << std::endl;
        exit = true;
    }
    if ((bd.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) != 0)
    {
        //Structured buffer
        if (format != DXGI_FORMAT_UNKNOWN)
        {
            std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_UNORDERED_ACCESS_VIEW::STRUCTURED_BUFFERS_MUST_USE_DXGI_FORMAT_UNKNOWN" << std::endl;
            exit = true;
        }
    }
    if ((flags & D3D11_BUFFER_UAV_FLAG_RAW) != 0)
    {
        //Raw view
        if (format != DXGI_FORMAT_R32_TYPELESS)
        {
            std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_UNORDERED_ACCESS_VIEW::RAW_BUFFERS_MUST_USE_DXGI_FORMAT_R32_TYPELESS" << std::endl;
            exit = true;
        }
        if ((bd.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS) == 0)
        {
            std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_UNORDERED_ACCESS_VIEW::RESOURCE_VIEWS_WITH_D3D11_BUFFEREX_SRV_FLAG_RAW_REQUIRE_D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS_FLAG_SET_ON_RESOURCE" << std::endl;
            exit = true;
        }
    }
    if (exit) { return nullptr; }
    uavd.Format = format;

    uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavd.Buffer.FirstElement = offset;
    uavd.Buffer.NumElements = count;
    uavd.Buffer.Flags = flags;

    ID3D11UnorderedAccessView* uav;
    HRESULT hr{ DeviceManager::device->CreateUnorderedAccessView(pResource, &uavd, &uav) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_UNORDERED_ACCESS_VIEW::FAILED_TO_CREATE_UNORDERED_ACCESS_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(uav);
    return uav;
}



ID3D11RenderTargetView* ResourceManager::CreateTexture1DRenderTargetView(ID3D11Texture1D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    rtvd.Format = format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
    rtvd.Texture1D.MipSlice = mipSlice;
    
    ID3D11RenderTargetView* rtv;
    HRESULT hr{ DeviceManager::device->CreateRenderTargetView(texture, &rtvd, &rtv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_RENDER_TARGET_VIEW::FAILED_TO_CREATE_RENDER_TARGET_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(rtv);
    return rtv;
}

ID3D11DepthStencilView* ResourceManager::CreateTexture1DDepthStencilView(ID3D11Texture1D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Format = format;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
    dsvd.Texture1D.MipSlice = mipSlice;
    
    ID3D11DepthStencilView* dsv;
    HRESULT hr{ DeviceManager::device->CreateDepthStencilView(texture, &dsvd, &dsv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_DEPTH_STENCIL_VIEW::FAILED_TO_CREATE_DEPTH_STENCIL_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(dsv);
    return dsv;
}

ID3D11ShaderResourceView* ResourceManager::CreateTexture1DShaderResourceView(ID3D11Texture1D* texture, UINT mostDetailedMipSlice, UINT mipLevels, DXGI_FORMAT format)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    srvd.Texture1D.MostDetailedMip = mostDetailedMipSlice;
    srvd.Texture1D.MipLevels = mipLevels;

    ID3D11ShaderResourceView* srv;
    HRESULT hr{ DeviceManager::device->CreateShaderResourceView(texture, &srvd, &srv) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_SHADER_RESOURCE_VIEW::FAILED_TO_CREATE_SHADER_RESOURCE_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(srv);
    return srv;
}

ID3D11UnorderedAccessView* ResourceManager::CreateTexture1DUnorderedAccessView(ID3D11Texture1D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    uavd.Format = format;
    uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
    uavd.Texture1D.MipSlice = mipSlice;

    ID3D11UnorderedAccessView* uav;
    HRESULT hr{ DeviceManager::device->CreateUnorderedAccessView(texture, &uavd, &uav) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_UNORDERED_ACCESS_VIEW::FAILED_TO_CREATE_UNORDERED_ACCESS_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(uav);
    return uav;
}

ID3D11RenderTargetView* ResourceManager::CreateTexture1DArrayRenderTargetView(ID3D11Texture1D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    rtvd.Format = format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
    rtvd.Texture1DArray.MipSlice = mipSlice;
    rtvd.Texture1DArray.FirstArraySlice = firstArraySlice;
    rtvd.Texture1DArray.ArraySize = arraySlices;
    
    ID3D11RenderTargetView* rtv;
    HRESULT hr{ DeviceManager::device->CreateRenderTargetView(texture, &rtvd, &rtv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_ARRAY_RENDER_TARGET_VIEW::FAILED_TO_CREATE_RENDER_TARGET_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(rtv);
    return rtv;
}

ID3D11DepthStencilView* ResourceManager::CreateTexture1DArrayDepthStencilView(ID3D11Texture1D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Format = format;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
    dsvd.Texture1D.MipSlice = mipSlice;
    
    ID3D11DepthStencilView* dsv;
    HRESULT hr{ DeviceManager::device->CreateDepthStencilView(texture, &dsvd, &dsv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_ARRAY_DEPTH_STENCIL_VIEW::FAILED_TO_CREATE_DEPTH_STENCIL_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(dsv);
    return dsv;
}

ID3D11ShaderResourceView* ResourceManager::CreateTexture1DArrayShaderResourceView(ID3D11Texture1D* texture, UINT mostDetailedMipSlice, UINT mipLevels, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
    srvd.Texture1DArray.MostDetailedMip = mostDetailedMipSlice;
    srvd.Texture1DArray.MipLevels = mipLevels;
    srvd.Texture1DArray.FirstArraySlice = firstArraySlice;
    srvd.Texture1DArray.ArraySize = arraySlices;

    ID3D11ShaderResourceView* srv;
    HRESULT hr{ DeviceManager::device->CreateShaderResourceView(texture, &srvd, &srv) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_ARRAY_SHADER_RESOURCE_VIEW::FAILED_TO_CREATE_SHADER_RESOURCE_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(srv);
    return srv;
}

ID3D11UnorderedAccessView* ResourceManager::CreateTexture1DArrayUnorderedAccessView(ID3D11Texture1D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    uavd.Format = format;
    uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
    uavd.Texture1DArray.MipSlice = mipSlice;
    uavd.Texture1DArray.FirstArraySlice = firstArraySlice;
    uavd.Texture1DArray.ArraySize = arraySlices;

    ID3D11UnorderedAccessView* uav;
    HRESULT hr{ DeviceManager::device->CreateUnorderedAccessView(texture, &uavd, &uav) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_1D_ARRAY_UNORDERED_ACCESS_VIEW::FAILED_TO_CREATE_UNORDERED_ACCESS_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(uav);
    return uav;
}

ID3D11RenderTargetView* ResourceManager::CreateTexture2DRenderTargetView(ID3D11Texture2D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    rtvd.Format = format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvd.Texture2D.MipSlice = mipSlice;
    
    ID3D11RenderTargetView* rtv;
    HRESULT hr{ DeviceManager::device->CreateRenderTargetView(texture, &rtvd, &rtv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_RENDER_TARGET_VIEW::FAILED_TO_CREATE_RENDER_TARGET_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(rtv);
    return rtv;
}

ID3D11DepthStencilView* ResourceManager::CreateTexture2DDepthStencilView(ID3D11Texture2D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Format = format;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = mipSlice;
    
    ID3D11DepthStencilView* dsv;
    HRESULT hr{ DeviceManager::device->CreateDepthStencilView(texture, &dsvd, &dsv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_DEPTH_STENCIL_VIEW::FAILED_TO_CREATE_DEPTH_STENCIL_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(dsv);
    return dsv;
}

ID3D11ShaderResourceView* ResourceManager::CreateTexture2DShaderResourceView(ID3D11Texture2D* texture, UINT mostDetailedMipSlice, UINT mipLevels, DXGI_FORMAT format)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture1D.MostDetailedMip = mostDetailedMipSlice;
    srvd.Texture2D.MipLevels = mipLevels;

    ID3D11ShaderResourceView* srv;
    HRESULT hr{ DeviceManager::device->CreateShaderResourceView(texture, &srvd, &srv) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_SHADER_RESOURCE_VIEW::FAILED_TO_CREATE_SHADER_RESOURCE_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(srv);
    return srv;
}

ID3D11UnorderedAccessView* ResourceManager::CreateTexture2DUnorderedAccessView(ID3D11Texture2D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    uavd.Format = format;
    uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavd.Texture2D.MipSlice = mipSlice;

    ID3D11UnorderedAccessView* uav;
    HRESULT hr{ DeviceManager::device->CreateUnorderedAccessView(texture, &uavd, &uav) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_UNORDERED_ACCESS_VIEW::FAILED_TO_CREATE_UNORDERED_ACCESS_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(uav);
    return uav;
}

ID3D11RenderTargetView* ResourceManager::CreateTexture2DArrayRenderTargetView(ID3D11Texture2D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    rtvd.Format = format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvd.Texture2DArray.MipSlice = mipSlice;
    rtvd.Texture2DArray.FirstArraySlice = firstArraySlice;
    rtvd.Texture2DArray.ArraySize = arraySlices;
    
    ID3D11RenderTargetView* rtv;
    HRESULT hr{ DeviceManager::device->CreateRenderTargetView(texture, &rtvd, &rtv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_ARRAY_RENDER_TARGET_VIEW::FAILED_TO_CREATE_RENDER_TARGET_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(rtv);
    return rtv;
}

ID3D11DepthStencilView* ResourceManager::CreateTexture2DArrayDepthStencilView(ID3D11Texture2D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Format = format;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvd.Texture2D.MipSlice = mipSlice;
    
    ID3D11DepthStencilView* dsv;
    HRESULT hr{ DeviceManager::device->CreateDepthStencilView(texture, &dsvd, &dsv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_ARRAY_DEPTH_STENCIL_VIEW::FAILED_TO_CREATE_DEPTH_STENCIL_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(dsv);
    return dsv;
}

ID3D11ShaderResourceView* ResourceManager::CreateTexture2DArrayShaderResourceView(ID3D11Texture2D* texture, UINT mostDetailedMipSlice, UINT mipLevels, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvd.Texture2DArray.MostDetailedMip = mostDetailedMipSlice;
    srvd.Texture2DArray.MipLevels = mipLevels;
    srvd.Texture2DArray.FirstArraySlice = firstArraySlice;
    srvd.Texture2DArray.ArraySize = arraySlices;

    ID3D11ShaderResourceView* srv;
    HRESULT hr{ DeviceManager::device->CreateShaderResourceView(texture, &srvd, &srv) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_ARRAY_SHADER_RESOURCE_VIEW::FAILED_TO_CREATE_SHADER_RESOURCE_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(srv);
    return srv;
}

ID3D11UnorderedAccessView* ResourceManager::CreateTexture2DArrayUnorderedAccessView(ID3D11Texture2D* texture, UINT mipSlice, UINT firstArraySlice, UINT arraySlices, DXGI_FORMAT format)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    uavd.Format = format;
    uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
    uavd.Texture2DArray.MipSlice = mipSlice;
    uavd.Texture2DArray.FirstArraySlice = firstArraySlice;
    uavd.Texture2DArray.ArraySize = arraySlices;

    ID3D11UnorderedAccessView* uav;
    HRESULT hr{ DeviceManager::device->CreateUnorderedAccessView(texture, &uavd, &uav) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_2D_ARRAY_UNORDERED_ACCESS_VIEW::FAILED_TO_CREATE_UNORDERED_ACCESS_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(uav);
    return uav;
}

ID3D11RenderTargetView* ResourceManager::CreateTexture3DRenderTargetView(ID3D11Texture3D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    rtvd.Format = format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
    rtvd.Texture3D.MipSlice = mipSlice;
    
    ID3D11RenderTargetView* rtv;
    HRESULT hr{ DeviceManager::device->CreateRenderTargetView(texture, &rtvd, &rtv) };
    if (FAILED(hr)) {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_3D_RENDER_TARGET_VIEW::FAILED_TO_CREATE_RENDER_TARGET_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(rtv);
    return rtv;
}

ID3D11ShaderResourceView* ResourceManager::CreateTexture3DShaderResourceView(ID3D11Texture3D* texture, UINT mostDetailedMipSlice, UINT mipLevels, DXGI_FORMAT format)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    srvd.Texture3D.MostDetailedMip = mostDetailedMipSlice;
    srvd.Texture3D.MipLevels = mipLevels;

    ID3D11ShaderResourceView* srv;
    HRESULT hr{ DeviceManager::device->CreateShaderResourceView(texture, &srvd, &srv) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_3D_SHADER_RESOURCE_VIEW::FAILED_TO_CREATE_SHADER_RESOURCE_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(srv);
    return srv;
}

ID3D11UnorderedAccessView* ResourceManager::CreateTexture3DUnorderedAccessView(ID3D11Texture3D* texture, UINT mipSlice, DXGI_FORMAT format)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    uavd.Format = format;
    uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
    uavd.Texture3D.MipSlice = mipSlice;

    ID3D11UnorderedAccessView* uav;
    HRESULT hr{ DeviceManager::device->CreateUnorderedAccessView(texture, &uavd, &uav) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::RESOURCE_MANAGER::CREATE_TEXTURE_3D_UNORDERED_ACCESS_VIEW::FAILED_TO_CREATE_UNORDERED_ACCESS_VIEW" << std::endl;
        return nullptr;
    }
    resourceViews.push_back(uav);
    return uav;
}
//----------------------------------------------//
//-------------END OF VIEW CREATION-------------//
//----------------------------------------------//