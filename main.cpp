#include "utils/utils.h"
#include "utils/Entity.h"
#include "utils/Player.h"

using namespace std;
using namespace offsets;

bool radarhack = false;
bool bunnyhop = false;
bool antiflash = false;
bool aimbot = false;
bool glow = false;

const int STANDING = 65665;
const int CROUCHING = 65667;
const int JUMP_ON = 65537;
const int JUMP_OFF = 256;

const float maxAimAngle = 5.0f;
const float maxDistance = 1000;

void checkKeys() {
    if (GetAsyncKeyState(VK_F5) & 1) {
        radarhack = !radarhack;
        cout << "[Toggle] RadarHack = " << (radarhack ? "ON" : "OFF") << endl;
    }

    if (GetAsyncKeyState(VK_F6) & 1) {
        bunnyhop = !bunnyhop;
        cout << "[Toggle] Bunnyhop = " << (bunnyhop ? "ON" : "OFF") << endl;
    }

    if (GetAsyncKeyState(VK_F7) & 1) {
        antiflash = !antiflash;
        cout << "[Toggle] antiflash = " << (antiflash ? "ON" : "OFF") << endl;
    }

    if (GetAsyncKeyState(VK_F8) & 1) {
        aimbot = !aimbot;
        cout << "[Toggle] aimbot = " << (aimbot ? "ON" : "OFF") << endl;
    }

    if (GetAsyncKeyState(VK_F9) & 1) {
        glow = !glow;
        cout << "[Toggle] glow = " << (glow ? "ON" : "OFF") << endl;
    }
}

int main() {
    if (!init()) {
        cout << "Press ENTER to exit..." << endl;
        cin.get();
        return 1;
    }

    Player player;
    vector<Entity> entities;
    entities.reserve(64);

    bool wasGlowEnabled = false;
    const Color enemyGlowColor = {255, 0, 0, 255}; 
    const Color teamGlowColor  = {0, 150, 255, 255};

    for (;;) {
        if (!player.isInit()) continue;
        checkKeys();

        if (!player.isInit()) {
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

        getEntities(entities);

        if (radarhack) {
            getEntities(entities);
            for (Entity &entity: entities) {
                if (!entity.isInit()) continue;
                if (entity.getDormant() || entity.getTeamNum() == player.getTeamNum()) continue;
                entity.setSpotted(true);
            }
        }

        if (glow) {
            getEntities(entities);
            uintptr_t localPlayerAddress = player.getAddress();

            for (Entity &entity : entities) {
                if (entity.isInit() && !entity.getDormant()) {
                    if (entity.getTeamNum() != player.getTeamNum()) {
                        entity.setGlow(enemyGlowColor);
                    } else if (entity.getAddress() != localPlayerAddress) {
                        entity.setGlow(teamGlowColor);
                    }
                }
            }
            wasGlowEnabled = true;
        } else if (wasGlowEnabled) {
            getEntities(entities);
            for (Entity &entity : entities) {
                if (entity.isInit()) {
                    entity.disableGlow();
                }
            }
            wasGlowEnabled = false;
        }


        if (bunnyhop) {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                int fflag = player.getFFlag();
                if (fflag == STANDING || fflag == CROUCHING) {
                    this_thread::sleep_for(chrono::milliseconds(1));
                    player.setJump(JUMP_ON);
                } else
                    player.setJump(JUMP_OFF);
            }
        }

        if (antiflash) {
            if (player.getFlashDuration() != 0)
                player.setFlashDuration(0);
        }

        if (aimbot && GetAsyncKeyState(VK_SHIFT) & 0x8000) {
            getEntities(entities);

            float smallestFOV = maxAimAngle;
            vec3 bestEnemyPos;
            int playerTeam = player.getTeamNum();
            vec3 playerEyePos = player.getLocalEyePos();
            vec3 currentViewAngles = RPM<vec3>(BaseAddress + dwViewAngles);

            for (Entity &entity: entities) {
                if (entity.getDormant() || entity.getTeamNum() == playerTeam) continue;

                vec3 entityEyePos = entity.getLocalEyePos();

                float distance = getDistance(playerEyePos, entityEyePos);
                if (distance > maxDistance) continue;

                vec3 delta = entityEyePos - playerEyePos;
                vec3 viewForward = AnglesToForward(currentViewAngles);

                float dotProduct = dot(normalize(delta), viewForward);
                float fov = acos(dotProduct) * (180.0f / pi<float>());

                if (fov < smallestFOV) {
                    smallestFOV = fov;
                    bestEnemyPos = entityEyePos;
                }
            }

            if (smallestFOV < maxAimAngle) {
                vec3 targetAngle = CalculateViewAngles(playerEyePos, bestEnemyPos);
                WPM<vec3>(BaseAddress + dwViewAngles, targetAngle);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(10));
    }

    if (hProcess) CloseHandle(hProcess);
    return 0;
}