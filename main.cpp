#include <thread>
#include <atomic>
#include <vector>
#include "utils/utils.h"
#include "utils/Player.h"
#include "utils/Entity.h"
#include "security/Security.h"
#include "ui/Overlay.h"

using namespace std;
using namespace offsets;
using namespace glm;

bool radarhack = false;
bool bunnyhop = false;
bool antiflash = false;
bool aimbot = false;
bool glow = false;
bool trigger = false;

atomic<bool> shouldExit = false;
atomic<bool> g_IsAuthenticated = false;

const int STANDING = 65665;
const int CROUCHING = 65667;
const int JUMP_ON = 65537;
const int JUMP_OFF = 256;

void SecurityLoop() {
    for (;!shouldExit;) {
        if (!Security::Initialize()) {
            shouldExit = true;
            ExitProcess(1);
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void CheatLoop() {
    Player player;
    vector<Entity> entities;
    entities.reserve(64);
    bool wasGlowEnabled = false;

    for (;!shouldExit;) {
        if (!g_IsAuthenticated) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        if (!player.isInit()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        getEntities(entities);

        if (radarhack) {
            for (Entity &entity: entities) {
                if (entity.isInit() && entity.getHealth() > 0 && entity.getTeamNum() != player.getTeamNum())
                    entity.setSpotted(true);
            }
        }

        if (glow) {
            uintptr_t localAddr = player.getAddress();
            for (Entity &entity: entities) {
                if (entity.isInit() && entity.getHealth() > 0) {
                    if (entity.getTeamNum() != player.getTeamNum())
                        entity.setGlow({255, 0, 0, 255});
                    else if (entity.getAddress() != localAddr)
                        entity.setGlow({0, 150, 255, 255});
                }
            }
            wasGlowEnabled = true;
        } else if (wasGlowEnabled) {
            for (Entity &entity: entities) if (entity.isInit()) entity.disableGlow();
            wasGlowEnabled = false;
        }

        if (bunnyhop && (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            int fflag = player.getFFlag();
            if (fflag == STANDING || fflag == CROUCHING) {
                player.setJump(JUMP_ON);
                this_thread::sleep_for(chrono::milliseconds(1));
                player.setJump(JUMP_OFF);
            }
        }

        if (antiflash && player.getFlashDuration() > 0.0f) {
            player.setFlashDuration(0.0f);
        }

        if (aimbot && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
            vec3 localPos = player.getLocalEyePos();
            vec3 viewAngles = RPM<vec3>(BaseAddress + dwViewAngles);
            float bestFov = 6.0f;
            vec3 bestPos = {0, 0, 0};
            bool found = false;

            for (Entity &ent: entities) {
                if (ent.getHealth() <= 0 || ent.getTeamNum() == player.getTeamNum()) continue;
                vec3 entPos = ent.getLocalEyePos();
                if (getDistance(localPos, entPos) > 1500) continue;

                vec3 delta = entPos - localPos;
                vec3 fwd = AnglesToForward(viewAngles);
                float fov = acos(dot(normalize(delta), fwd)) * (180.0f / 3.14159f);

                if (fov < bestFov) {
                    bestFov = fov;
                    bestPos = entPos;
                    found = true;
                }
            }
            if (found) {
                WPM<vec3>(BaseAddress + dwViewAngles, CalculateViewAngles(localPos, bestPos));
            }
        }

        if (trigger) {
            int xhair = player.getCrosshairID();
            if (xhair > 0) {
                uintptr_t entList = RPM<uintptr_t>(BaseAddress + dwEntityList);
                uintptr_t entry = RPM<uintptr_t>(entList + 0x8 * (xhair >> 9) + 0x10);
                uintptr_t pawn = RPM<uintptr_t>(entry + 0x70 * (xhair & 0x1FF));

                if (pawn) {
                    Entity target(pawn);
                    if (target.getTeamNum() != player.getTeamNum() && target.getHealth() > 0) {
                        this_thread::sleep_for(chrono::milliseconds(5));
                        player.setAttack(65537);
                        this_thread::sleep_for(chrono::milliseconds(10));
                        player.setAttack(256);
                    }
                }
            }
        }

        std::this_thread::sleep_for(chrono::milliseconds(2));
    }
}

int main() {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
    if (!init()) return 1;

    thread sec(SecurityLoop);
    sec.detach();
    thread cheat(CheatLoop);
    cheat.detach();

    if (Overlay::Initialize()) {
        Overlay::RenderLoop();
        Overlay::Cleanup();
    }

    shouldExit = true;
    if (hProcess) CloseHandle(hProcess);
    return 0;
}