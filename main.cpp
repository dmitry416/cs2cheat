#include "utils/utils.h"
#include "utils/Entity.h"
#include "utils/Player.h"

using namespace std;
using namespace offsets;

bool radarhack = false;
bool bunnyhop = false;

void checkKeys() {
    if (GetAsyncKeyState(VK_F5) & 1) {
        radarhack = !radarhack;
        cout << "[Toggle] radarhack = " << (radarhack ? "ON" : "OFF") << endl;
    }

    if (GetAsyncKeyState(VK_F6) & 1) {
        bunnyhop = !bunnyhop;
        cout << "[Toggle] bunnyhop = " << (bunnyhop ? "ON" : "OFF") << endl;
    }
}

// после того, как прочли коментарии и поняли, как все работает, желательно удалить их.
int main() {
    if (!init()) {
        cout << "Press ENTER to exit..." << endl;
        cin.get();
        return 1;
    }

    Player player;
    vector<Entity> entities;
    entities.reserve(64);

    bool jump_flag = false;
    const int JUMP_ON = 65537;
    const int JUMP_OFF = 256;
    uintptr_t forceJumpAddress = BaseAddress + dwForceJump;

    for (;true;) { // фор просто чуть-чуть оптимизированней использовать, чем вайл. Так что это не ошибка
        checkKeys();

        if (radarhack) {
            if (!player.isInit()) continue; // проверяем, что у нас есть игрок. в противном случае, мы не находимся в игре и нам нет смысла делать что-то
            getEntities(entities); // получаем врагов (в их числе может быть игрок в теории). А вообще эта функция нуждается в доработке, так как не всегда и не всех она может найти.
            for (Entity &entity: entities) {
                if (!entity.isInit()) continue; // проверяем на то, вдруг мы нашли в памяти не врага, а мусор какой-то
                if (entity.getDormant() || entity.getTeamNum() == player.getTeamNum()) continue; // проверка на то, живой ли тип и находится ли он в противоположной команде
                entity.setSpotted(true); // делаем его видимым на карте
            }
        }

        if (bunnyhop) {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!jump_flag) {
                    WPM<int>(forceJumpAddress, JUMP_ON);
                    jump_flag = true;
                } 
                else {
                    WPM<int>(forceJumpAddress, JUMP_OFF);
                    jump_flag = false;
                }
            } 
            else 
                jump_flag = false;
        }

        this_thread::sleep_for(chrono::milliseconds(5));
        }

        this_thread::sleep_for(chrono::milliseconds(50));
    }

    if (hProcess) CloseHandle(hProcess);
    return 0;
}
