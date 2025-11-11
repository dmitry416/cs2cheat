#include "utils/utils.h"
#include "utils/Entity.h"
#include "utils/Player.h"

using namespace std;
using namespace offsets;

bool radarhack = false;
bool bunnyhop = false;
bool antiflash = false;

const int STANDING = 65665;
const int CROUCHING = 65667;
const int JUMP_ON = 65537;
const int JUMP_OFF = 256;

void checkKeys() {
    if (GetAsyncKeyState(VK_F5) & 1) {
        radarhack = !radarhack;
        cout << "[Toggle] radarhack = " << (radarhack ? "ON" : "OFF") << endl;
    }

    if (GetAsyncKeyState(VK_F6) & 1) {
        bunnyhop = !bunnyhop;
        cout << "[Toggle] bunnyhop = " << (bunnyhop ? "ON" : "OFF") << endl;
    }

    if (GetAsyncKeyState(VK_F7) & 1) {
        antiflash = !antiflash;
        cout << "[Toggle] antiflash = " << (antiflash ? "ON" : "OFF") << endl;
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

    for (;;) {
        checkKeys();

        if (radarhack) {
            if (!player.isInit()) continue;
            getEntities(entities);
            for (Entity &entity: entities) {
                if (!entity.isInit()) continue;
                if (entity.getDormant() || entity.getTeamNum() == player.getTeamNum()) continue;
                entity.setSpotted(true);
            }
        }

        if (bunnyhop) {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                int fflag = player.getFFlag();
                if (fflag == STANDING || fflag == CROUCHING) {
                    this_thread::sleep_for(chrono::milliseconds(1));
                    player.setJump(JUMP_ON);
                }
                else
                    player.setJump(JUMP_OFF);
            }
        }

        if (antiflash) {
            if (Player.getFlashDuration == 0.0f) continue;

            Player.setFlashDuration()
        }

        this_thread::sleep_for(chrono::milliseconds(10));
    }

    if (hProcess) CloseHandle(hProcess);
    return 0;
}
