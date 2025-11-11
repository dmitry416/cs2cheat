#ifndef CS2CHEAT_UTILS_H
#define CS2CHEAT_UTILS_H

#include "../headers/include.h"
#include "../headers/offsets.h"

using namespace std;
using namespace offsets;

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
float getFlashDuration();
void setFlashDuration();

#endif