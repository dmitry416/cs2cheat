#ifndef SECURITY_H
#define SECURITY_H

#include <windows.h>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <winhttp.h>

class Security {
private:
    static inline bool isDebugged = false;
    static inline bool isTampered = false;

public:
    static bool Initialize() {
        CheckDebugger();
        IntegrityCheck();
        return !(isDebugged || isTampered);
    }

    static bool VerifyActivation(const std::string &key, const std::string &steamId) {
        if (key.length() != 32) return false;
        for (char c: key) {
            if (!isxdigit(c)) return false;
        }
        return ServerVerify(key, steamId);
    }

    static std::string GetSteamIDFromProcess() {
        DWORD steamPid = FindProcessId("steam.exe");
        if (steamPid == 0) return "";
        return "76561197960287930"; // Заглушка
    }

private:
    static DWORD FindProcessId(const std::string &processName) {
        DWORD pid = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 processEntry;
            processEntry.dwSize = sizeof(processEntry);
            if (Process32First(snapshot, &processEntry)) {
                do {
                    if (_stricmp(processEntry.szExeFile, processName.c_str()) == 0) {
                        pid = processEntry.th32ProcessID;
                        break;
                    }
                } while (Process32Next(snapshot, &processEntry));
            }
            CloseHandle(snapshot);
        }
        return pid;
    }

    static void CheckDebugger() {
        if (IsDebuggerPresent()) {
            isDebugged = true;
            return;
        }

        BOOL isRemoteDebuggerPresent = FALSE;
        if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &isRemoteDebuggerPresent)) {
            if (isRemoteDebuggerPresent) {
                isDebugged = true;
                return;
            }
        }

        auto start = std::chrono::high_resolution_clock::now();
        volatile int sum = 0;
        for (int i = 0; i < 500000; i++) {
            sum += i;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        if (duration.count() > 250000) {
            isDebugged = true;
            return;
        }

        const char *debuggers[] = {
                "x64dbg", "x32dbg", "idaq", "idaq64",
                "ollydbg", "windbg", "cheatengine", "cheat engine"
        };

        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 processEntry;
            processEntry.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hSnapshot, &processEntry)) {
                do {
                    std::string processName = processEntry.szExeFile;
                    std::transform(processName.begin(), processName.end(),
                                   processName.begin(), ::tolower);
                    for (const char *debugger: debuggers) {
                        std::string debuggerStr(debugger);
                        std::transform(debuggerStr.begin(), debuggerStr.end(),
                                       debuggerStr.begin(), ::tolower);
                        if (processName.find(debuggerStr) != std::string::npos) {
                            isDebugged = true;
                            CloseHandle(hSnapshot);
                            return;
                        }
                    }
                } while (Process32Next(hSnapshot, &processEntry));
            }
            CloseHandle(hSnapshot);
        }
    }

    static void IntegrityCheck() {
        char modulePath[MAX_PATH];
        GetModuleFileNameA(nullptr, modulePath, MAX_PATH);
        DWORD fileAttributes = GetFileAttributesA(modulePath);
        if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
            isTampered = true;
            return;
        }
    }

    static bool ServerVerify(const std::string &key, const std::string &steamId) {
        HINTERNET hSession = nullptr;
        HINTERNET hConnect = nullptr;
        HINTERNET hRequest = nullptr;

        try {
            hSession = WinHttpOpen(L"CS2Cheat/1.0",
                                   WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                   WINHTTP_NO_PROXY_NAME,
                                   WINHTTP_NO_PROXY_BYPASS, 0);
            if (!hSession) return false;

            WinHttpSetTimeouts(hSession, 5000, 5000, 5000, 5000);

            hConnect = WinHttpConnect(hSession, L"localhost", 8000, 0);
            if (!hConnect) {
                WinHttpCloseHandle(hSession);
                return false;
            }

            hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/verify",
                                          nullptr, WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
            if (!hRequest) {
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            std::string jsonBody = "{\"key\":\"" + key + "\",\"steamid\":\"" + steamId + "\"}";

            if (!WinHttpSendRequest(hRequest,
                                    L"Content-Type: application/json",
                                    -1,
                                    (LPVOID)jsonBody.c_str(),
                                    jsonBody.length(),
                                    jsonBody.length(), 0)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            if (!WinHttpReceiveResponse(hRequest, nullptr)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }

            std::string response;
            char buffer[4096];
            DWORD bytesRead = 0;

            if (WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead)) {
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    response = buffer;
                }
            }

            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);

            return response.find("\"status\": \"success\"") != std::string::npos;

        } catch (...) {
            if (hRequest) WinHttpCloseHandle(hRequest);
            if (hConnect) WinHttpCloseHandle(hConnect);
            if (hSession) WinHttpCloseHandle(hSession);
            return false;
        }
    }
};

#endif