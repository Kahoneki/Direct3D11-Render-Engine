#include <iostream>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND WindowSetup(UINT width, UINT height);

int main()
{

	HRESULT hr{};

	UINT WIN_WIDTH{ 800 };
	UINT WIN_HEIGHT{ 800 };
	HWND hwnd{ WindowSetup(WIN_WIDTH, WIN_HEIGHT) };
	if (hwnd == NULL) {
		return -1;
	}

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	DXGI_MODE_DESC bufferDesc{
		WIN_WIDTH,
		WIN_HEIGHT,
		DXGI_RATIONAL{60, 1},
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		DXGI_MODE_SCALING_UNSPECIFIED
	};
	DXGI_SWAP_CHAIN_DESC swapChainDesc{
		bufferDesc,
		DXGI_SAMPLE_DESC{1,0},
		DXGI_USAGE_RENDER_TARGET_OUTPUT,
		2,
		hwnd,
		true,
		DXGI_SWAP_EFFECT_DISCARD,
		NULL
	};
	IDXGISwapChain* swapChain{ nullptr };
	ID3D11Device* device{ nullptr };
	D3D_FEATURE_LEVEL featureLevel{};
	ID3D11DeviceContext* deviceContext{ nullptr };
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		&featureLevel,
		&deviceContext
	);
	if (FAILED(hr)) {
		std::cerr << "D3D11CreateDeviceAndSwapChain() has failed. HRESULT: " << std::hex << hr << std::endl;
	}
	if (featureLevel < D3D_FEATURE_LEVEL_11_0) {
		std::cerr << "Device does not support DirectX 11.0 or higher." << std::endl;
	}

	ID3D11Texture2D* swapChainBuffer{};
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&swapChainBuffer);
	if (FAILED(hr)) {
		std::cerr << "Failed to get the swap chain texture2D buffer. HRESULT: " << std::hex << hr << std::endl;
	}

	ID3D11Debug* debugger{};
	hr = device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugger);
	if (FAILED(hr)) {
		std::cerr << "Failed to get the device debugger. HRESULT: " << std::hex << hr << std::endl;
	}

	ID3D11RenderTargetView* renderTargetView{};
	hr = device->CreateRenderTargetView(swapChainBuffer, NULL, &renderTargetView);
	if (FAILED(hr)) {
		std::cerr << "Failed to get the swap chain render target view. HRESULT: " << std::hex << hr << std::endl;
	}


	D3D11_TEXTURE2D_DESC depthStencilBufferDesc
	{
		WIN_WIDTH,
		WIN_HEIGHT,
		1,
		1,
		DXGI_FORMAT_D32_FLOAT,
		{1,0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL,
		NULL,
		NULL
	};

	ID3D11Texture2D* depthStencilBuffer{};
	hr = device->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(hr)) {
		std::cerr << "Failed to create depth-stencil buffer. HRESULT: " << std::hex << hr << std::endl;
	}
	ID3D11DepthStencilView* depthStencilView{};
	hr = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	if (FAILED(hr)) {
		std::cerr << "Failed to create depth-stencil view. HRESULT: " << std::hex << hr << std::endl;
	}

	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	float clearColours[4]{ 0.8f, 1.0f, 1.0f, 1.0f }; //Light blue

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		deviceContext->ClearRenderTargetView(renderTargetView, clearColours);
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 0, 0);

		hr = swapChain->Present(0, NULL);
		if (FAILED(hr)) {
			std::cerr << "Failed to present swap chain buffer. HRESULT: " << std::hex << hr << std::endl;
		}
	}


	deviceContext->ClearState();

	swapChainBuffer->Release();
	debugger->Release();
	renderTargetView->Release();
	depthStencilBuffer->Release();
	depthStencilView->Release();

	swapChain->Release();
	device->Release();
	deviceContext->Release();

	return 0;
}



LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0); //Posts a quit message to the message queue
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam); //Default window procedure for this message
	}
	return 0;
}

HWND WindowSetup(UINT width, UINT height)
{
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProcedure;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"MyWindowClass";
	if (!RegisterClass(&wc)) {
		MessageBox(NULL, L"Window Class Registration Failed!", L"ERROR", MB_ICONERROR);
		return NULL;
	}

	HWND hwnd{ CreateWindow(wc.lpszClassName, L"Test", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL) };

	if (!hwnd) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONERROR);
		return NULL;
	}

	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}