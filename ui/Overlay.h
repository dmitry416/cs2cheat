#pragma once

#include <d3d11.h>
#include <atomic>

extern bool radarhack, bunnyhop, antiflash, aimbot, glow, trigger;
extern std::atomic<bool> shouldExit;

class Overlay {
public:
    static bool Initialize();

    static void RenderLoop();

    static void Cleanup();

private:
    static void HandleInput();

    static void DrawMenu();

    static void SetInputPassThrough(bool enabled);

    static ID3D11Device *device;
    static ID3D11DeviceContext *deviceContext;
    static IDXGISwapChain *swapChain;
    static ID3D11RenderTargetView *renderTargetView;
    static HWND window;
};