#ifndef CS2CHEAT_UTILS_H
#define CS2CHEAT_UTILS_H

#include "../headers/include.h"
#include "../headers/offsets.h"

using namespace std;
using namespace offsets;
using namespace glm;

class Entity;

extern uintptr_t BaseAddress;
extern HWND hwnd;
extern HANDLE hProcess;
extern DWORD processtap;
extern bool radarhack;

uintptr_t GetModuleBaseAddress(const char *modName);
template<typename T> T RPM(uintptr_t address);
template<typename T> void WPM(uintptr_t address, T buffer);
void checkKeys();
bool init();
void getEntities(vector<Entity>& entities);
float getDistance(vec3 p1, vec3 p2);
vec3 AnglesToForward(vec3 angles);
vec3 CalculateViewAngles(vec3 source, vec3 destination);

#endif