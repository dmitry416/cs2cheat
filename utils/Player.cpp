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

float Player::getFlashDuration() {
    updateAddress();
    if (!address) return 0;

    return RPM<float>(address + m_flFlashDuration);
}

void Player::setFlashDuration(float value) {
    updateAddress();
    if (!address) return;

    WPM<float>(address + m_flFlashDuration, value);
}

void Player::setJump(int value) {
    WPM(BaseAddress + dwForceJump, value);
}

glm::vec3 Player::getLocalEyePos() {
    return RPM<glm::vec3>(address + m_vOldOrigin) +
           RPM<vec3>(address + m_vecViewOffset);
}
