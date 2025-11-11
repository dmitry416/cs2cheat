#ifndef CS2CHEAT_PLAYER_H
#define CS2CHEAT_PLAYER_H

#include "../headers/include.h"
#include "../headers/offsets.h"


class Player {
private:
    uintptr_t address;

    void updateAddress();

public:
    int getHealth();

    int getTeamNum();

    bool getDormant();

    int getFFlag();

    void setJump(int value);

    uintptr_t getAddress();

    bool isInit();
};


#endif
