#include "Player.h"
#include "utils.h"

void Player::updateAddress() {
    address = RPM<uintptr_t>(BaseAddress + dwLocalPlayerPawn);
}

int Player::getHealth() {
    updateAddress();
    if (!address) return 0;
    return RPM<int>(address + m_iHealth);
}

int Player::getTeamNum() {
    updateAddress();
    if (!address) return 0;
    return RPM<int>(address + m_iTeamNum);
}

bool Player::getDormant() {
    updateAddress();
    if (!address) return true;
    return RPM<bool>(address + m_bDormant);
}

bool Player::isInit() {
    updateAddress();
    if (!address) return false;

    int team = getTeamNum();
    if (team < 2 || team > 3) return false;

    int health = getHealth();
    if (health <= 0 || health > 100) return false;

    if (getDormant()) return false;

    return true;
}

int Player::getFFlag() {
    updateAddress();
    if (!address) return 0;
    return RPM<int>(address + m_fFlags);
}

void Player::setJump(int value) {
    WPM(BaseAddress + dwForceJump, value);
}
