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
    constexpr ::std::ptrdiff_t m_fFlags = 0x3F8;
    constexpr ::std::ptrdiff_t m_flFlashDuration = 0x1610;
    constexpr ::std::ptrdiff_t m_vOldOrigin = 0x15A0;
    constexpr ::std::ptrdiff_t m_vecViewOffset = 0xD80;
    constexpr ::std::ptrdiff_t m_lifeState = 0x354;
    constexpr ::std::ptrdiff_t m_modelState = 0x190;
    constexpr ::std::ptrdiff_t m_pGameSceneNode = 0x330;


    inline uintptr_t dwEntityList = 0x1D0FBE8;
    inline uintptr_t dwLocalPlayerPawn = 0x1BEAEB8;
    constexpr ::std::ptrdiff_t dwViewAngles = 0x1E38670;
    constexpr ::std::ptrdiff_t dwViewMatrix = 0x1E2E0F0;
    constexpr ::std::ptrdiff_t dwForceJump = 0x1BE4830;
}
