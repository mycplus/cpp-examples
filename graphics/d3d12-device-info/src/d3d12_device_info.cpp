// Direct3D 12: pick an adapter, create a device, report its DirectX
// Raytracing (DXR) tier, and create the command queue every frame is sent to.
// Falls back to WARP, Microsoft's software rasterizer, when there is no GPU.
// Companion code for the MYCPLUS article on AAA game graphics in C++.
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <cstdio>
#include <cwchar>

using Microsoft::WRL::ComPtr;

static const char* dxr_tier(D3D12_RAYTRACING_TIER t) {
    switch (t) {
        case D3D12_RAYTRACING_TIER_NOT_SUPPORTED: return "not supported";
        case D3D12_RAYTRACING_TIER_1_0: return "1.0";
        default: return "1.1 or later";
    }
}

int main() {
    // 1. A DXGI factory enumerates the adapters (GPUs) in the system.
    ComPtr<IDXGIFactory6> factory;
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)))) {
        std::printf("CreateDXGIFactory2 failed\n");
        return 1;
    }

    // 2. Take the first high-performance hardware adapter that can create a device.
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device> device;
    for (UINT i = 0; factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                         IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
         ++i) {
        DXGI_ADAPTER_DESC1 desc{};
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;  // hardware first
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
            std::wprintf(L"Adapter: %ls\n", desc.Description);
            break;
        }
    }

    // 3. No usable GPU (for example on a CI machine): use WARP instead.
    if (!device) {
        ComPtr<IDXGIAdapter> warp;
        if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp))) ||
            FAILED(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
            std::printf("No Direct3D 12 device available\n");
            return 2;
        }
        std::printf("Adapter: WARP (software)\n");
    }

    // 4. Ask the driver which DirectX Raytracing tier it supports.
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 opts5{};
    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &opts5, sizeof(opts5))))
        std::printf("DXR tier: %s\n", dxr_tier(opts5.RaytracingTier));

    // 5. Every frame is recorded into command lists and submitted to a queue.
    D3D12_COMMAND_QUEUE_DESC qdesc{};
    qdesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ComPtr<ID3D12CommandQueue> queue;
    if (FAILED(device->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&queue)))) {
        std::printf("CreateCommandQueue failed\n");
        return 3;
    }
    std::printf("Direct command queue created\nPASS\n");
    return 0;
}
