#include "PipelineManager.h"

#include <iostream>

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

void PipelineManager::BindShaderResourceViews(ID3D11ShaderResourceView* shaderResourceViews, PIPELINE_STAGE stage, UINT startSlot, UINT numViews)
{
    switch (stage)
    {
    case (PIPELINE_STAGE::VERTEX_SHADER):
    {
        DeviceManager::context->VSSetShaderResources(startSlot, numViews, &shaderResourceViews);
    }
    case (PIPELINE_STAGE::DOMAIN_SHADER):
    {
        DeviceManager::context->DSSetShaderResources(startSlot, numViews, &shaderResourceViews);
    }
    case (PIPELINE_STAGE::HULL_SHADER):
    {
        DeviceManager::context->HSSetShaderResources(startSlot, numViews, &shaderResourceViews);
    }
    case (PIPELINE_STAGE::GEOMETRY_SHADER):
    {
        DeviceManager::context->GSSetShaderResources(startSlot, numViews, &shaderResourceViews);
    }
    case (PIPELINE_STAGE::PIXEL_SHADER):
    {
        DeviceManager::context->PSSetShaderResources(startSlot, numViews, &shaderResourceViews);
    }
    case (PIPELINE_STAGE::COMPUTE_SHADER):
    {
        DeviceManager::context->CSSetShaderResources(startSlot, numViews, &shaderResourceViews);
    }
    default:
    {
        std::cerr << "ERROR::PIPELINE_MANAGER::BIND_SHADER_RESOURCE_VIEW::PROVIDED_STAGE_NOT_IN_ENUM" << std::endl;
    }
    }
}

void PipelineManager::BindUnorderedAccessViews(ID3D11UnorderedAccessView* unorderedAccessViews, PIPELINE_STAGE stage, UINT startSlot, UINT numViews, UINT* initialCounts)
{
    switch (stage)
    {
    case (PIPELINE_STAGE::PIXEL_SHADER):
    {
        std::vector<ID3D11RenderTargetView*> rtvs{ GetCurrentRenderTargetViews() };
        ID3D11DepthStencilView* dsv{ GetCurrentDepthStencilView() };
        DeviceManager::context->OMSetRenderTargetsAndUnorderedAccessViews(rtvs.size(), rtvs.data(), dsv, startSlot, numViews, &unorderedAccessViews, initialCounts);
    }
    case (PIPELINE_STAGE::COMPUTE_SHADER):
    {
        DeviceManager::context->CSSetUnorderedAccessViews(startSlot, numViews, &unorderedAccessViews, initialCounts);
    }
    default:
    {
        std::cerr << "ERROR::PIPELINE_MANAGER::BIND_UNORDERED_ACCESS_VIEW::PROVIDED_STAGE_MUST_BE_PIXEL_OR_COMPUTE" << std::endl;
    }
    }
}

void PipelineManager::ClearRenderTargetView(ID3D11RenderTargetView* renderTargetView, FLOAT* clearColour)
{
    DeviceManager::context->ClearRenderTargetView(renderTargetView, clearColour);
}

void PipelineManager::ClearDepthStencilView(ID3D11DepthStencilView* depthStencilView, FLOAT clearDepth, UINT8 clearStencil)
{
    DeviceManager::context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, clearDepth, clearStencil);
}

void PipelineManager::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView* unorderedAccessView, FLOAT clearValue[4])
{
    DeviceManager::context->ClearUnorderedAccessViewFloat(unorderedAccessView, clearValue);
}

void PipelineManager::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView* unorderedAccessView, UINT clearValue[4])
{
    DeviceManager::context->ClearUnorderedAccessViewUint(unorderedAccessView, clearValue);
}

//---------------------------------//
//-------End of View Methods-------//
//---------------------------------//



//--------------------------------//
//---------Buffer Methods---------//
//--------------------------------//
void PipelineManager::BindVertexBuffers(ID3D11Buffer* vertexBuffers, UINT startSlot, UINT numBuffers, UINT stride, UINT offset)
{
    DeviceManager::context->IASetVertexBuffers(startSlot, numBuffers, &vertexBuffers, &stride, &offset);
}

void PipelineManager::BindIndexBuffer(ID3D11Buffer* indexBuffer, DXGI_FORMAT format, UINT offset)
{
    DeviceManager::context->IASetIndexBuffer(indexBuffer, format, offset);
}

void PipelineManager::BindConstantBuffers(PIPELINE_STAGE stage, UINT startSlot, UINT numBuffers, ID3D11Buffer* constantBuffer)
{
    switch (stage)
    {
    case (PIPELINE_STAGE::VERTEX_SHADER):
    {
        DeviceManager::context->VSSetConstantBuffers(startSlot, numBuffers, &constantBuffer);
    }
    case (PIPELINE_STAGE::PIXEL_SHADER):
    {
        DeviceManager::context->PSSetConstantBuffers(startSlot, numBuffers, &constantBuffer);
    }
    default:
    {
        std::cerr << "ERROR::PIPELINE_MANAGER::BIND_CONSTANT_BUFFER::STAGE_MUST_BE_VERTEX_OR_PIXEL" << std::endl;
    }
    }
}
//---------------------------------//
//------End of Buffer Methods------//
//---------------------------------//



//---------------------------------//
//---------Sampler Methods---------//
//---------------------------------//
void PipelineManager::BindSamplerStates(ID3D11SamplerState* samplerState, PIPELINE_STAGE stage, UINT startSlot, UINT numSamplerStates)
{
    switch (stage)
    {
    case (PIPELINE_STAGE::VERTEX_SHADER):
    {
        DeviceManager::context->VSSetSamplers(startSlot, numSamplerStates, &samplerState);
    }
    case (PIPELINE_STAGE::DOMAIN_SHADER):
    {
        DeviceManager::context->DSSetSamplers(startSlot, numSamplerStates, &samplerState);
    }
    case (PIPELINE_STAGE::HULL_SHADER):
    {
        DeviceManager::context->HSSetSamplers(startSlot, numSamplerStates, &samplerState);
    }
    case (PIPELINE_STAGE::GEOMETRY_SHADER):
    {
        DeviceManager::context->GSSetSamplers(startSlot, numSamplerStates, &samplerState);
    }
    case (PIPELINE_STAGE::PIXEL_SHADER):
    {
        DeviceManager::context->PSSetSamplers(startSlot, numSamplerStates, &samplerState);
    }
    case (PIPELINE_STAGE::COMPUTE_SHADER):
    {
        DeviceManager::context->CSSetSamplers(startSlot, numSamplerStates, &samplerState);
    }
    default:
    {
        std::cerr << "ERROR::PIPELINE_MANAGER::BIND_SAMPLER_STATE::PROVIDED_STAGE_NOT_IN_ENUM" << std::endl;
    }
    }
}
ID3D11SamplerState* PipelineManager::GetSamplerStates(PIPELINE_STAGE stage, UINT startSlot, UINT numSamplerStates)
{
    ID3D11SamplerState* samplerStates;
    switch (stage)
    {
    case (PIPELINE_STAGE::VERTEX_SHADER):
    {
        DeviceManager::context->VSGetSamplers(startSlot, numSamplerStates, &samplerStates);
    }
    case (PIPELINE_STAGE::DOMAIN_SHADER):
    {
        DeviceManager::context->DSGetSamplers(startSlot, numSamplerStates, &samplerStates);
    }
    case (PIPELINE_STAGE::HULL_SHADER):
    {
        DeviceManager::context->HSGetSamplers(startSlot, numSamplerStates, &samplerStates);
    }
    case (PIPELINE_STAGE::GEOMETRY_SHADER):
    {
        DeviceManager::context->GSGetSamplers(startSlot, numSamplerStates, &samplerStates);
    }
    case (PIPELINE_STAGE::PIXEL_SHADER):
    {
        DeviceManager::context->PSGetSamplers(startSlot, numSamplerStates, &samplerStates);
    }
    case (PIPELINE_STAGE::COMPUTE_SHADER):
    {
        DeviceManager::context->CSGetSamplers(startSlot, numSamplerStates, &samplerStates);
    }
    default:
    {
        std::cerr << "ERROR::PIPELINE_MANAGER::GET_SAMPLER_STATE::PROVIDED_STAGE_NOT_IN_ENUM" << std::endl;
    }
    }
    return samplerStates;
}
//----------------------------------//
//------End of Sampler Methods------//
//----------------------------------//