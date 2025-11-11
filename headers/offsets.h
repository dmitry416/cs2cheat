#pragma once

#include <Windows.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <optional>

namespace offsets {
    constexpr ::std::ptrdiff_t m_bSpotted = 0x8;
    constexpr ::std::ptrdiff_t m_bDormant = 0x10B;
    constexpr ::std::ptrdiff_t m_iTeamNum = 0x3EB;
    constexpr ::std::ptrdiff_t m_iHealth = 0x34C;
    constexpr ::std::ptrdiff_t m_hPlayerPawn = 0x8FC;
    constexpr ::std::ptrdiff_t m_entitySpottedState = 0x2700;
    constexpr ::std::ptrdiff_t dwForceJump = 0x1BE4830;
    constexpr ::std::ptrdiff_t m_fFlags = 0x3F8;
    constexpr ::std::ptrdiff_t m_flFlashDuration = 0x1610;


    inline uintptr_t dwEntityList = 0x1D0FBE8;
    inline uintptr_t dwLocalPlayerPawn = 0x1BEAEB8;
}
