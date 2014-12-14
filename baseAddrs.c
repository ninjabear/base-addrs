#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

#include "dmode.h"

// create a snapshot of all process in memory and report back on the base module base address

int main(int argc, char **argv[]) {
    HANDLE hProcSnap;
    HANDLE hModSnap;
    PROCESSENTRY32 pe32;
    MODULEENTRY32 me32;

    if (DebugMode(TRUE) == FAILURE) {
        printf("Warning; could not activate debug mode. Continuing anyway.\n");
    }

    hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcSnap != INVALID_HANDLE_VALUE) {
        Process32First(hProcSnap, &pe32);
        do {
            if (pe32.th32ProcessID == GetCurrentProcessId()) {
                continue;
            }
            hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
            if (hModSnap != INVALID_HANDLE_VALUE) {
                Module32First(hModSnap, &me32);
                printf("%s has a base address of 0x%X\n", me32.szModule, me32.modBaseAddr);
                while (Module32Next(hModSnap, &me32)) {
                    printf("\t-> %s has a base address of 0x%X\n", me32.szExePath, me32.modBaseAddr); //remove if you only want the
                } // primary module
                CloseHandle(hModSnap);
            } else {
                printf("couldn't create module snapshot of %s\n", pe32.szExeFile);
            }
        } while (Process32Next(hProcSnap, &pe32));
        CloseHandle(hProcSnap);
    } else {
        printf("couldn't create process snapshot\n");
    }

    return 0;
}

