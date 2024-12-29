#include "WindowManager.h"

#include <iostream>

#include "DeviceManager.h"
#include "EngineManager.h"

//Forward declarations
static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static HWND WindowSetup(UINT width, UINT height);


UINT WindowManager::width{};
UINT WindowManager::height{};
    
HWND WindowManager::hwnd{};
IDXGISwapChain* WindowManager::swapChain{};


void WindowManager::Initialise(const UINT _width, const UINT _height)
{
    if (hwnd != NULL)
    {
        std::cerr << "ERROR::WINDOW_MANAGER::INITIALISE::WINDOW_ALREADY_INITIALISED" << std::endl;
        return;
    }
    
    width = _width;
    height = _height;
    
    ID3D11Device* device{ DeviceManager::device };
    
    //Setup Window
    UINT WIN_WIDTH{ 800 };
    UINT WIN_HEIGHT{ 800 };
    hwnd = WindowSetup(WIN_WIDTH, WIN_HEIGHT);
    if (hwnd == NULL) {
        std::cerr << "ERROR::WINDOW_MANAGER::INITIALISE::FAILED_TO_CREATE_WINDOW" << std::endl;
        return;
    }


    //Setup DXGI Factory
    IDXGIDevice* dxgiDevice;
    HRESULT hr{ device->QueryInterface(IID_PPV_ARGS(&dxgiDevice) ) };
    if (FAILED(hr))
    {
        std::cerr << "ERROR::WINDOW_MANAGER::INITIALISE::FAILED_TO_GET_DXGI_DEVICE" << std::endl;
        return;
    }
    IDXGIAdapter* dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    if (FAILED(hr))
    {
        std::cerr << "ERROR::WINDOW_MANAGER::INITIALISE::FAILED_TO_GET_DXGI_ADAPTER" << std::endl;
        dxgiDevice->Release();
        return;
    }
    IDXGIFactory* dxgiFactory;
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr))
    {
        std::cerr << "ERROR::WINDOW_MANAGER::INITIALISE::FAILED_TO_GET_DXGI_FACTORY" << std::endl;
        dxgiDevice->Release();
        dxgiAdapter->Release();
        return;
    }


    //Setup Swapchain
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
    hr = dxgiFactory->CreateSwapChain(dxgiDevice, &swapChainDesc, &swapChain);
    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();
    if (FAILED(hr))
    {
        std::cerr << "ERROR::WINDOW_MANAGER::INITIALISE::FAILED_TO_CREATE_SWAP_CHAIN" << std::endl;
        return;
    }
}

void WindowManager::Update()
{
    MSG msg;
    BOOL msgReceived{ GetMessage(&msg, NULL, 0, 0) };
    if (msgReceived)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    EngineManager::applicationRunning = msgReceived;
}

void WindowManager::Shutdown()
{
    swapChain->Release();
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

    HWND hwnd{ CreateWindow(wc.lpszClassName, L"Neki", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL) };

    if (!hwnd) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONERROR);
        return NULL;
    }

    ShowWindow(hwnd, SW_SHOW);

    return hwnd;
}