#include "utils.h"
#include "Entity.h"

uintptr_t BaseAddress = 0;
HWND hwnd = nullptr;
HANDLE hProcess = nullptr;
DWORD processtap = 0;

uintptr_t GetModuleBaseAddress(const char *modName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processtap);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(modEntry);

    if (Module32First(hSnap, &modEntry)) {
        do {
            if (strcmp(modEntry.szModule, modName) == 0) {
                CloseHandle(hSnap);
                return (uintptr_t)modEntry.modBaseAddr;
            }
        } while (Module32Next(hSnap, &modEntry));
    }

    CloseHandle(hSnap);
    return 0;
}

template<typename T>
T RPM(uintptr_t address) {
    T buffer;
    if (!ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), nullptr)) {
        return T();
    }
    return buffer;
}

template<typename T>
void WPM(uintptr_t address, T buffer) {
    WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), nullptr);
}


template int RPM<int>(uintptr_t);
template bool RPM<bool>(uintptr_t);
template uintptr_t RPM<uintptr_t>(uintptr_t);
template void WPM<int>(uintptr_t, int);
template void WPM<bool>(uintptr_t, bool);

bool init() {
    hwnd = FindWindowA(nullptr, "Counter-Strike 2");
    if (!hwnd) {
        cout << "CS2 not found" << endl;
        return false;
    }
    cout << "CS2 FOUND" << endl;

    if (!GetWindowThreadProcessId(hwnd, &processtap)) {
        cout << "Failed to get process ID" << endl;
        return false;
    }
    cout << "ProcessID: " << processtap << endl;

    BaseAddress = GetModuleBaseAddress("client.dll");
    if (!BaseAddress) {
        cout << "Failed to get client.dll" << endl;
        return false;
    }
    cout << "BaseAddress: 0x" << hex << BaseAddress << dec << endl;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processtap);
    if (!hProcess || hProcess == INVALID_HANDLE_VALUE) {
        cout << "Failed to open process" << endl;
        return false;
    }

    cout << "\n==========================" << endl;
    cout << "RadarHack [F5]" << endl;
    cout << "Bunnyhop  [F6]" << endl;
    cout << "==========================" << endl;

    return true;
}

void getEntities(vector<Entity>& entities) {
    entities.clear();

    uintptr_t entityList = RPM<uintptr_t>(BaseAddress + dwEntityList);
    uintptr_t listEntry = RPM<uintptr_t>(entityList + 0x10);

    for (int i = 0; i < 64; ++i) {
        if (!listEntry) continue;
        uintptr_t curController = RPM<uintptr_t>(listEntry + i * 0x70);
        int pawnHandle = RPM<int>(curController + m_hPlayerPawn);
        if (!pawnHandle) continue;
        uintptr_t listEntry2 = RPM<uintptr_t>(entityList + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
        uintptr_t curPawn = RPM<uintptr_t>(listEntry2 + 0x70 * (pawnHandle & 0x1FF));
        if (!curPawn) continue;

        Entity entity(curPawn);
        if (entity.isInit()) entities.push_back(entity);
    }
}