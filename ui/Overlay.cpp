#include "Overlay.h"
#include "../utils/Player.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <dwmapi.h>
#include <vector>
#include <string>

ID3D11Device *Overlay::device = nullptr;
ID3D11DeviceContext *Overlay::deviceContext = nullptr;
IDXGISwapChain *Overlay::swapChain = nullptr;
ID3D11RenderTargetView *Overlay::renderTargetView = nullptr;
HWND Overlay::window = nullptr;

static bool isVisible = true;
static bool isMoveMode = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    if (msg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Overlay::SetInputPassThrough(bool enabled) {
    LONG exStyle = GetWindowLong(window, GWL_EXSTYLE);
    if (enabled) {
        SetWindowLong(window, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
    } else {
        SetWindowLong(window, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        SetForegroundWindow(window);
    }
}

bool Overlay::Initialize() {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
                     "Overlay", nullptr};
    RegisterClassEx(&wc);
    window = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
                            wc.lpszClassName, "Overlay", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                            GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);

    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(window, &margins);
    SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA);
    ShowWindow(window, SW_SHOWDEFAULT);

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = window;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd,
                                      &swapChain, &device, nullptr, &deviceContext) != S_OK)
        return false;

    ID3D11Texture2D *pBackBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    device->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);
    pBackBuffer->Release();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 5.0f;

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, deviceContext);

    SetInputPassThrough(false);

    return true;
}

void Overlay::HandleInput() {
    static bool toggleInsert = false;
    static bool toggleHome = false;

    if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
        if (!toggleInsert) {
            isVisible = !isVisible;
            toggleInsert = true;

            if (!isVisible) SetInputPassThrough(true);
            else {
                SetInputPassThrough(!isMoveMode);
            }
        }
    } else toggleInsert = false;

    if (isVisible) {
        if (GetAsyncKeyState(VK_HOME) & 0x8000) {
            if (!toggleHome) {
                isMoveMode = !isMoveMode;
                toggleHome = true;
                SetInputPassThrough(!isMoveMode);
            }
        } else toggleHome = false;
    }

    if (GetAsyncKeyState(VK_END) & 0x8000) shouldExit = true;

    struct Key {
        int k;
        bool *v;
    };
    static std::vector<Key> keys = {{VK_F4, &trigger},
                                    {VK_F5, &radarhack},
                                    {VK_F6, &bunnyhop},
                                    {VK_F7, &antiflash},
                                    {VK_F8, &aimbot},
                                    {VK_F9, &glow}};
    static bool keyState[256] = {0};
    for (auto &item: keys) {
        bool down = GetAsyncKeyState(item.k) & 0x8000;
        if (down && !keyState[item.k]) *item.v = !(*item.v);
        keyState[item.k] = down;
    }
}

void Overlay::RenderLoop() {
    while (!shouldExit) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) shouldExit = true;
        }

        HandleInput();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        bool needCursor = (isVisible && isMoveMode);
        ImGui::GetIO().MouseDrawCursor = needCursor;

        if (isVisible) DrawMenu();

        ImGui::Render();
        float clearColor[4] = {0, 0, 0, 0};
        deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
        deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swapChain->Present(1, 0);
    }
}

void Overlay::DrawMenu() {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 20, 20), ImGuiCond_FirstUseEver, ImVec2(1.0f, 0.0f));

    ImGuiWindowFlags flags =
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (!isMoveMode) {
        flags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 0.94f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 0.1f));

    if (ImGui::Begin("Watermark", nullptr, flags)) {
        if (isMoveMode) ImGui::TextColored(ImVec4(1, 1, 0, 1), "[DRAG MODE]");
        else ImGui::TextColored(ImVec4(0, 1, 1, 1), "CS2 PRIVATE");

        ImGui::Separator();

        auto Status = [](const char *name, bool active) {
            if (active) ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s [ON]", name);
            else ImGui::TextDisabled("%s [OFF]", name);
        };

        Status("[F4] Trigger", trigger);
        Status("[F5] Radar", radarhack);
        Status("[F6] Bhop", bunnyhop);
        Status("[F7] NoFlash", antiflash);
        Status("[F8] Aimbot", aimbot);
        Status("[F9] Glow", glow);

        ImGui::Separator();
        if (isMoveMode) ImGui::TextDisabled("[HOME] Save Position");
        else ImGui::TextDisabled("[HOME] Move Menu");

        ImGui::TextDisabled("[END] Exit Cheat");
    }
    ImGui::End();
    ImGui::PopStyleColor(2);
}

void Overlay::Cleanup() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (renderTargetView) renderTargetView->Release();
    if (swapChain) swapChain->Release();
    if (deviceContext) deviceContext->Release();
    if (device) device->Release();
    if (window) DestroyWindow(window);
}