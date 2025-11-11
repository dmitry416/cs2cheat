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

void Entity::setGlow(const Color& color) {
    if (!address) return;
    uintptr_t glowProperty = address + offsets::m_Glow;
    
    WPM<bool>(glowProperty + offsets::m_bGlowing, true);
    WPM<Color>(glowProperty + offsets::m_glowColorOverride, color);
    WPM<int>(glowProperty + offsets::m_iGlowType, 3);
}

void Entity::disableGlow() {
    if (!address) return;
    uintptr_t glowProperty = address + offsets::m_Glow;
    WPM<bool>(glowProperty + offsets::m_bGlowing, false);
}

uintptr_t Entity::getAddress() const {
    return address;
}

bool Entity::isInit() {
    if (!address) return false;

    return true;
}

glm::vec3 Entity::getLocalEyePos() {
    return RPM<glm::vec3>(address + m_vOldOrigin) +
           RPM<vec3>(address + m_vecViewOffset);
}
