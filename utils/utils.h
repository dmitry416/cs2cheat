#ifndef CS2CHEAT_UTILS_H
#define CS2CHEAT_UTILS_H

#include "../headers/include.h"
#include "../headers/offsets.h"


class Entity;

extern uintptr_t BaseAddress;
extern HWND hwnd;
extern HANDLE hProcess;
extern DWORD processtap;
extern bool radarhack;

uintptr_t GetModuleBaseAddress(const char *modName);

template<typename T>
T RPM(uintptr_t address);

template<typename T>
void WPM(uintptr_t address, T buffer);

void checkKeys();

bool init();

void getEntities(std::vector<Entity> &entities);

float getDistance(glm::vec3 p1, glm::vec3 p2);

glm::vec3 AnglesToForward(glm::vec3 angles);

glm::vec3 CalculateViewAngles(glm::vec3 source, glm::vec3 destination);

#endif