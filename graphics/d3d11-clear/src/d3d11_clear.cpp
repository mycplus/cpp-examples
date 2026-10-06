// d3d11_clear.cpp - a window, a Direct3D 11 device and a flip-model swap chain.
// Each frame clears the back buffer to a slowly changing colour and presents it.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <cmath>
#include <cstdio>

using Microsoft::WRL::ComPtr;

namespace {
ComPtr<ID3D11Device>           g_device;
ComPtr<ID3D11DeviceContext>    g_context;
ComPtr<IDXGISwapChain1>        g_swapChain;
ComPtr<ID3D11RenderTargetView> g_rtv;

bool Check(HRESULT hr, const char* what)
{
    if (SUCCEEDED(hr)) return true;
    char buf[128];
    std::snprintf(buf, sizeof buf, "%s failed: HRESULT 0x%08lX\n",
                  what, static_cast<unsigned long>(hr));
    OutputDebugStringA(buf);
    return false;
}

bool CreateTargetView()
{
    ComPtr<ID3D11Texture2D> backBuffer;
    return Check(g_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), "GetBuffer") &&
           Check(g_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &g_rtv),
                 "CreateRenderTargetView");
}

bool InitD3D(HWND hwnd)
{
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;  // needs the Graphics Tools optional feature
#endif
    if (!Check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                                 nullptr, 0, D3D11_SDK_VERSION,
                                 &g_device, nullptr, &g_context), "D3D11CreateDevice"))
        return false;

    // The swap chain must come from the factory that created the device.
    ComPtr<IDXGIDevice>   dxgiDevice;
    ComPtr<IDXGIAdapter>  adapter;
    ComPtr<IDXGIFactory2> factory;
    if (!Check(g_device.As(&dxgiDevice), "QueryInterface(IDXGIDevice)") ||
        !Check(dxgiDevice->GetAdapter(&adapter), "GetAdapter") ||
        !Check(adapter->GetParent(IID_PPV_ARGS(&factory)), "GetParent(IDXGIFactory2)"))
        return false;

    DXGI_SWAP_CHAIN_DESC1 desc = {};      // Width/Height 0 = the window's client size
    desc.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;            // flip model does not allow MSAA back buffers
    desc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount      = 2;            // front + back: double buffering
    desc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;  // Windows 10 and later
    if (!Check(factory->CreateSwapChainForHwnd(g_device.Get(), hwnd, &desc,
                                               nullptr, nullptr, &g_swapChain),
               "CreateSwapChainForHwnd"))
        return false;
    factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
    return CreateTargetView();
}

void Resize(UINT width, UINT height)
{
    if (!g_swapChain || width == 0 || height == 0) return;   // not ready, or minimised
    g_context->OMSetRenderTargets(0, nullptr, nullptr);
    g_rtv.Reset();                        // every reference to a buffer must go first
    if (Check(g_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0),
              "ResizeBuffers"))
        CreateTargetView();
}

bool RenderFrame(unsigned frame)
{
    const float t = static_cast<float>(frame) * 0.02f;
    const float colour[4] = { 0.5f + 0.5f * std::sin(t),
                              0.5f + 0.5f * std::sin(t + 2.1f),
                              0.5f + 0.5f * std::sin(t + 4.2f), 1.0f };

    // Flip-model presentation unbinds the back buffer, so bind it every frame.
    g_context->OMSetRenderTargets(1, g_rtv.GetAddressOf(), nullptr);
    g_context->ClearRenderTargetView(g_rtv.Get(), colour);

    const HRESULT hr = g_swapChain->Present(1, 0);           // 1 = wait for vertical blank
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        Check(g_device->GetDeviceRemovedReason(), "Device removed");
        return false;                     // a real application recreates the device here
    }
    return Check(hr, "Present");
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_SIZE:    Resize(LOWORD(lp), HIWORD(lp)); return 0;
    case WM_DESTROY: PostQuitMessage(0);             return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
} // namespace

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int show)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = inst;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"D3D11Clear";
    if (!RegisterClassW(&wc)) return 1;

    HWND hwnd = CreateWindowW(wc.lpszClassName, L"Direct3D 11 clear", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
                              nullptr, nullptr, inst, nullptr);
    if (!hwnd || !InitD3D(hwnd)) return 1;
    ShowWindow(hwnd, show);

    MSG msg = {};
    unsigned frame = 0;
    while (msg.message != WM_QUIT) {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {  // drain input first
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else if (!RenderFrame(frame++)) {
            DestroyWindow(hwnd);
        }
    }
    return static_cast<int>(msg.wParam);
}
