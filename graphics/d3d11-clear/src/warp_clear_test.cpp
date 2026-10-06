// warp_clear_test.cpp - headless check that runs without a GPU or a window.
// Clears a 4x4 BGRA texture to pure green on the WARP software rasterizer,
// copies it to CPU-readable memory and prints the first pixel's bytes.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdio>

using Microsoft::WRL::ComPtr;

int main()
{
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, nullptr, 0,
                                   D3D11_SDK_VERSION, &device, nullptr, &context);
    if (FAILED(hr)) { std::printf("D3D11CreateDevice: 0x%08lX\n", (unsigned long)hr); return 1; }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = desc.Height = 4;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;

    ComPtr<ID3D11Texture2D> target, staging;
    ComPtr<ID3D11RenderTargetView> rtv;
    if (FAILED(device->CreateTexture2D(&desc, nullptr, &target)) ||
        FAILED(device->CreateRenderTargetView(target.Get(), nullptr, &rtv))) return 1;

    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    if (FAILED(device->CreateTexture2D(&desc, nullptr, &staging))) return 1;

    const float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };   // R, G, B, A
    context->ClearRenderTargetView(rtv.Get(), green);
    context->CopyResource(staging.Get(), target.Get());

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped))) return 1;
    const unsigned char* px = static_cast<const unsigned char*>(mapped.pData);
    std::printf("row pitch: %u bytes for a 4-pixel row\n", mapped.RowPitch);
    std::printf("first pixel bytes: %02X %02X %02X %02X\n", px[0], px[1], px[2], px[3]);
    const bool ok = px[0] == 0x00 && px[1] == 0xFF && px[2] == 0x00 && px[3] == 0xFF;
    context->Unmap(staging.Get(), 0);
    std::printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
