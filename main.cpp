#include "utils/utils.h"
#include "utils/Entity.h"
#include "utils/Player.h"

using namespace std;
using namespace offsets;

bool radarhack = false;
bool bunnyhop = false;
bool glow = false;

const int STANDING = 65665;
const int CROUCHING = 65667;
const int JUMP_ON = 65537;
const int JUMP_OFF = 256;

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
        glow = !glow;
        cout << "[Toggle] Glow = " << (glow ? "ON" : "OFF") << endl;
    }
}

int main() {
    if (!init()) {
        cout << "Press ENTER to exit..." << endl;
        cin.get();
        return 1;
    }

    cout << "\n==========================" << endl;
    cout << "RadarHack [F5]" << endl;
    cout << "Bunnyhop  [F6]" << endl;
    cout << "Glow      [F7]" << endl;
    cout << "==========================" << endl;

    Player player;
    vector<Entity> entities;
    entities.reserve(64);

    bool wasGlowEnabled = false;

    const Color enemyGlowColor = {255, 0, 0, 255}; 

    for (;;) {
        checkKeys();

        if (!player.isInit()) {
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

        getEntities(entities);

        if (radarhack) {
            for (Entity &entity : entities) {
                if (entity.isInit() && !entity.getDormant() && entity.getTeamNum() != player.getTeamNum()) {
                    entity.setSpotted(true);
                }
            }
        }

        if (glow) {
            for (Entity &entity : entities) {
                if (entity.isInit() && !entity.getDormant() && entity.getTeamNum() != player.getTeamNum()) {
                    entity.setGlow(enemyGlowColor);
                }
            }
        } else if (wasGlowEnabled) {
            for (Entity &entity : entities) {
                if (entity.isInit()) {
                    entity.disableGlow();
                }
            }
        }
        wasGlowEnabled = glow;


        if (bunnyhop) {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                int fflag = player.getFFlag();
                if (fflag == STANDING || fflag == CROUCHING) {
                    this_thread::sleep_for(chrono::milliseconds(1));
                    player.setJump(JUMP_ON);
                } else {
                    player.setJump(JUMP_OFF);
                }
            }
        }

        this_thread::sleep_for(chrono::milliseconds(10));
    }

    if (hProcess) CloseHandle(hProcess);
    return 0;
}