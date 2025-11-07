#ifndef CS2CHEAT_ENTITY_H
#define CS2CHEAT_ENTITY_H

#include "../headers/include.h"
#include "../headers/offsets.h"

class Entity {
private:
    uintptr_t address;
public:
    explicit Entity(uintptr_t address);

    int getHealth();

    int getTeamNum();

    bool getDormant();

    bool getSpotted();

    void setSpotted(bool value);

    bool isInit();
};


#endif
