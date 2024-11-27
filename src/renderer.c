//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 16:25:51
//

#define COBJMACROS
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <stdio.h>
#include <stdlib.h>

// links
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "flock/renderer.h"

__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

typedef struct fl_renderer_internal {
    fl_window *Window;

    ID3D11Device* Device;
    ID3D11DeviceContext* Context;
    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    ID3D11Texture2D* Backbuffer;
    ID3D11RenderTargetView* RenderTarget;
    ID3D11RasterizerState* RasterizerState;

    IDXGIDevice* DXGIDevice;
    IDXGIFactory* Factory;
    IDXGIAdapter* Adapter;
    IDXGISwapChain* SwapChain;
} fl_renderer_internal;

b8 FlockRendererInit(fl_renderer *Renderer, fl_window *Window)
{
    Renderer->Internal = malloc(sizeof(fl_renderer_internal));
    Renderer->Surface = Window;
    fl_renderer_internal* Internal = (fl_renderer_internal*)Renderer->Internal;

    // Init device
    {
        const D3D_DRIVER_TYPE DriverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        const D3D_FEATURE_LEVEL Levels[] = { D3D_FEATURE_LEVEL_11_0 };
        D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

        for (i32 Driver = 0; Driver < 3; Driver++)
        {
            HRESULT Result = D3D11CreateDevice(NULL, DriverTypes[Driver], NULL, D3D11_CREATE_DEVICE_DEBUG, Levels, 1, D3D11_SDK_VERSION, &Internal->Device, &FeatureLevel, &Internal->Context);
            if (SUCCEEDED(Result))
                break;
        }

        ID3D11Device_QueryInterface(Internal->Device, &IID_IDXGIDevice, (void**)&Internal->DXGIDevice);
        IDXGIDevice_GetParent(Internal->DXGIDevice, &IID_IDXGIAdapter, (void**)&Internal->Adapter);
        IDXGIAdapter_GetParent(Internal->Adapter, &IID_IDXGIFactory, (void**)&Internal->Factory);
    }

    // Init swapchain
    {
        // Get window size
        RECT Rect;
        GetClientRect((HWND)Window->Handle, &Rect);
        u32 Width = Rect.right - Rect.left;
        u32 Height = Rect.bottom - Rect.top;

        // Create swapchain
        DXGI_SWAP_CHAIN_DESC Desc;
        memset(&Desc, 0, sizeof(Desc));
        Desc.BufferDesc.Width = Width;
        Desc.BufferDesc.Height = Height;
        Desc.BufferDesc.RefreshRate.Numerator = 120;
        Desc.BufferDesc.RefreshRate.Denominator = 1;
        Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        Desc.SampleDesc.Count = 1;
        Desc.SampleDesc.Quality = 0;
        Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        Desc.BufferCount = 1;
        Desc.OutputWindow = (HWND)Window->Handle;
        Desc.Flags = 0;
        Desc.Windowed = true;

        HRESULT Result = IDXGIFactory_CreateSwapChain(Internal->Factory, Internal->Device, &Desc, &Internal->SwapChain);
        if (FAILED(Result)) {
            printf("Failed to create D3D11 swap chain!\n");
            return false;
        }

        // Get backbuffer and render target
        IDXGISwapChain_GetBuffer(Internal->SwapChain, 0, &IID_ID3D11Texture2D, (void**)&Internal->Backbuffer);
        
        D3D11_RENDER_TARGET_VIEW_DESC View;
        memset(&View, 0, sizeof(View));
        View.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        View.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        ID3D11Device_CreateRenderTargetView(Internal->Device, Internal->Backbuffer, &View, &Internal->RenderTarget);
    }

    // Init shaders
    {
        // Read shader file
        FILE* File = fopen("shaders/shader.hlsl", "r");
        fseek(File, 0, SEEK_END);
        u32 Size = ftell(File);
        rewind(File);

        char* Contents = malloc(Size);
        memset(Contents, 0, Size);
        fread(Contents, Size, 1, File);
        fclose(File);

        // Compile vertex shader
        {
            ID3DBlob* ShaderBlob = NULL;
            ID3DBlob* ErrorBlob = NULL;
            HRESULT Status = D3DCompile(Contents, Size, NULL, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", 0, 0, &ShaderBlob, &ErrorBlob);
            if (FAILED(Status)) {
                printf("Vertex Shader Error: %s", ID3D10Blob_GetBufferPointer(ErrorBlob));
                return false;
            }

            ID3D11Device_CreateVertexShader(Internal->Device, ID3D10Blob_GetBufferPointer(ShaderBlob), ID3D10Blob_GetBufferSize(ShaderBlob), NULL, &Internal->VertexShader);
            ID3D10Blob_Release(ShaderBlob);
        }

        // Compile pixel shader
        {
            ID3DBlob* ShaderBlob = NULL;
            ID3DBlob* ErrorBlob = NULL;
            HRESULT Status = D3DCompile(Contents, Size, NULL, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", 0, 0, &ShaderBlob, &ErrorBlob);
            if (FAILED(Status)) {
                printf("Pixel Shader Error: %s", ID3D10Blob_GetBufferPointer(ErrorBlob));
                return false;
            }

            ID3D11Device_CreatePixelShader(Internal->Device, ID3D10Blob_GetBufferPointer(ShaderBlob), ID3D10Blob_GetBufferSize(ShaderBlob), NULL, &Internal->PixelShader);
            ID3D10Blob_Release(ShaderBlob);
        }
        free(Contents);
    }

    // Init rasterizer
    {
        D3D11_RASTERIZER_DESC Desc;
        memset(&Desc, 0, sizeof(Desc));
        Desc.CullMode = D3D11_CULL_NONE;
        Desc.FillMode = D3D11_FILL_SOLID;
        Desc.FrontCounterClockwise = false;

        ID3D11Device_CreateRasterizerState(Internal->Device, &Desc, &Internal->RasterizerState);
    }

    return true;
}

void FlockRendererPresent(fl_renderer *Renderer)
{
    fl_renderer_internal* Internal = (fl_renderer_internal*)Renderer->Internal;
    
    RECT Rect;
    GetClientRect((HWND)Renderer->Surface->Handle, &Rect);
    u32 Width = Rect.right - Rect.left;
    u32 Height = Rect.bottom - Rect.top;

    D3D11_VIEWPORT Viewport;
    Viewport.Width = Width;
    Viewport.Height = Height;
    Viewport.MinDepth = 0.0f;
    Viewport.MaxDepth = 1.0f;
    Viewport.TopLeftX = 0.0f;
    Viewport.TopLeftY = 1.0f;

    // Clear screen for funsies
    f32 Color[] = { 0.1, 0.1, 0.1, 1.0 };
    ID3D11DeviceContext_OMSetRenderTargets(Internal->Context, 1, &Internal->RenderTarget, NULL);
    ID3D11DeviceContext_ClearRenderTargetView(Internal->Context, Internal->RenderTarget, Color);
    ID3D11DeviceContext_VSSetShader(Internal->Context, Internal->VertexShader, NULL, 0);
    ID3D11DeviceContext_PSSetShader(Internal->Context, Internal->PixelShader, NULL, 0);
    ID3D11DeviceContext_RSSetViewports(Internal->Context, 1, &Viewport);
    ID3D11DeviceContext_RSSetState(Internal->Context, Internal->RasterizerState);
    ID3D11DeviceContext_IASetPrimitiveTopology(Internal->Context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D11DeviceContext_Draw(Internal->Context, 3, 0);

    IDXGISwapChain_Present(Internal->SwapChain, 0, 0);
}

void FlockRendererFree(fl_renderer *Renderer)
{
    fl_renderer_internal* Internal = (fl_renderer_internal*)Renderer->Internal;

    if (Internal->RasterizerState) ID3D11RasterizerState_Release(Internal->RasterizerState);
    if (Internal->RenderTarget) ID3D11RenderTargetView_Release(Internal->RenderTarget);
    if (Internal->SwapChain) IDXGISwapChain_Release(Internal->SwapChain);
    if (Internal->PixelShader) ID3D11PixelShader_Release(Internal->PixelShader);
    if (Internal->VertexShader) ID3D11VertexShader_Release(Internal->VertexShader);
    if (Internal->Context) ID3D11DeviceContext_Release(Internal->Context);
    if (Internal->Adapter) IDXGIAdapter_Release(Internal->Adapter);
    if (Internal->DXGIDevice) IDXGIDevice_Release(Internal->DXGIDevice);
    if (Internal->Device) ID3D11Device_Release(Internal->Device);
    if (Internal->Factory) IDXGIFactory_Release(Internal->Factory);
    free(Internal);
}
