#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tlhelp32.h>

#define BUF_SIZE 1024 * 1024 


BOOL DumpProcessMemory(DWORD processID, const char* fileName);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <ProcessName or PID>\n", argv[0]);
        return 1;
    }

    DWORD processID = 0;

    // 判断输入是进程名称还是进程ID
    if (strchr(argv[1], '.')) {

        PROCESSENTRY32 pe;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) {
            printf("Error creating snapshot.\n");
            return 1;
        }

        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnap, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, L"ToDesk.exe") == 0) {
                    processID = pe.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }
    else {
        processID = atoi(argv[1]);
    }

    if (processID == 0) {
        printf("Cannot find process with ID or name '%s'.\n", argv[1]);
        return 1;
    }


    if (DumpProcessMemory(processID, "123.txt")) {
        printf("Memory dumped successfully to 123.txt\n");
    }
    else {
        printf("Failed to dump memory.\n");
    }

    return 0;
}


BOOL DumpProcessMemory(DWORD processID, const char* fileName) {
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processID);
    if (hProcess == NULL) {
        printf("Error opening process %ld\n", processID);
        return FALSE;
    }

    FILE* outputFile = NULL;
    if (fopen_s(&outputFile, fileName, "wb") != 0) {
        printf("Error opening output file.\n");
        CloseHandle(hProcess);
        return FALSE;
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    LPVOID lpBaseAddress = sysInfo.lpMinimumApplicationAddress;
    MEMORY_BASIC_INFORMATION mbi;

    while (lpBaseAddress < sysInfo.lpMaximumApplicationAddress) {
        if (VirtualQueryEx(hProcess, lpBaseAddress, &mbi, sizeof(mbi)) == sizeof(mbi)) {
            if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS) {
                BYTE* buffer = (BYTE*)malloc(mbi.RegionSize);
                if (buffer) {
                    SIZE_T bytesRead;
                    if (ReadProcessMemory(hProcess, lpBaseAddress, buffer, mbi.RegionSize, &bytesRead)) {
                        fwrite(buffer, 1, bytesRead, outputFile);
                    }
                    free(buffer);
                }
            }
            lpBaseAddress = (LPVOID)((BYTE*)lpBaseAddress + mbi.RegionSize);
        }
        else {
            break;
        }
    }

    fclose(outputFile);
    CloseHandle(hProcess);
    return TRUE;
}
