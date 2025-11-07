#include "utils/utils.h"
#include "utils/Entity.h"
#include "utils/Player.h"

using namespace std;
using namespace offsets;

bool radarhack = false;

void checkKeys() {
    if (GetAsyncKeyState(VK_F5) & 1) {
        radarhack = !radarhack;
        cout << "[Toggle] radarhack = " << (radarhack ? "ON" : "OFF") << endl;
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

        this_thread::sleep_for(chrono::milliseconds(50));
    }

    if (hProcess) CloseHandle(hProcess);
    return 0;
}
