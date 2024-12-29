#include "Managers/EngineManager.h"
#include "Managers/PipelineManager.h"
#include "Managers/ResourceManager.h"

int main()
{

	float clearColour[4]{ 1.0f, 1.0f, 0.0f, 1.0f };
	EngineDescription ed
	{
		WindowDescription{800,800},
		RenderDescription{ clearColour }
	};
	
	EngineManager::Initialise(ed);

	ID3D11RenderTargetView* rtv{ ResourceManager::CreateRenderTargetView(ResourceManager::GetActiveSwapchainTexture()) };
	std::vector<ID3D11RenderTargetView*> rtvVec{ rtv };
	ID3D11Texture2D* dst{ ResourceManager::CreateDepthStencilTexture() };
	ID3D11DepthStencilView* dsv{ ResourceManager::CreateDepthStencilView(dst) };
	PipelineManager::BindRenderTargetViews(rtvVec);
	PipelineManager::BindDepthStencilView(dsv);
	
	while (EngineManager::applicationRunning)
	{
		EngineManager::Update();
	}

	EngineManager::Shutdown();
	
	return 0;
}
