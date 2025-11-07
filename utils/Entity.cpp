#include "Entity.h"
#include "utils.h"

Entity::Entity(uintptr_t address) {
    this->address = address;
}

int Entity::getHealth() {
    if (!address) return 0;
    return RPM<int>(address + m_iHealth);
}

int Entity::getTeamNum() {
    if (!address) return 0;
    return RPM<int>(address + m_iTeamNum);
}

bool Entity::getDormant() {
    if (!address) return true;
    return RPM<bool>(address + m_bDormant);
}

bool Entity::getSpotted() {
    if (!address) return false;
    return RPM<bool>(address + m_entitySpottedState + m_bSpotted);
}

void Entity::setSpotted(bool value) {
    if (!address) return;
    WPM<bool>(address + m_entitySpottedState + m_bSpotted, value);
}

bool Entity::isInit() {
    if (!address) return false;

    int team = getTeamNum();
    if (team < 2 || team > 3) return false;

    int health = getHealth();
    if (health < 0 || health > 100) return false;

    return true;
}