#pragma once

#include <memory>
#include <vector>
#include <Windows.h>
#include "common.h"

NewProcessInfo GetTargetInfo(const LPCSTR lpPath)
{
    NewProcessInfo processInfo = { 0 };

    HANDLE hFile = CreateFileA(lpPath, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    DWORD dwFileSize = GetFileSize(hFile, nullptr);

    if (dwFileSize == -1)
    {
        fprintf(stderr, "Could not get size of file. Error = %X\n", GetLastError());
        exit(-1);
    }

    processInfo.pFileData = std::unique_ptr<BYTE[]>(new BYTE[dwFileSize]);
    processInfo.dwFileSize = dwFileSize;

    DWORD dwBytesRead = 0;
    bool bRet = BOOLIFY(ReadFile(hFile, processInfo.pFileData.get(), dwFileSize, &dwBytesRead, nullptr));
    if (!bRet)
    {
        fprintf(stderr, "Could not read file. Error = %X\n", GetLastError());
        exit(-1);
    }
    if (dwBytesRead != dwFileSize)
    {
        fprintf(stderr, "Could not complete entire read.\n"
            "Bytes read = %ul -- Bytes that should be read = %ul\n",
            dwBytesRead, dwFileSize);
        exit(-1);
    }

    processInfo.pDosHeader = (IMAGE_DOS_HEADER *)&(processInfo.pFileData.get()[0]);
    processInfo.pNtHeaders = (IMAGE_NT_HEADERS *)&(processInfo.pFileData.get()[processInfo.pDosHeader->e_lfanew]);

    CloseHandle(hFile);

    return processInfo;
}

std::vector<std::array<BYTE, READ_WRITE_SIZE>> SplitFile(const char * const pFilePath)
{
    std::vector<std::array<BYTE, READ_WRITE_SIZE>> splitFile;

    NewProcessInfo newProcessInfo = GetTargetInfo(pFilePath);
    for (DWORD i = 0; i < newProcessInfo.dwFileSize; i += READ_WRITE_SIZE)
    {
        std::array<BYTE, READ_WRITE_SIZE> splitArray = { 0 };
        memcpy(splitArray.data(), &newProcessInfo.pFileData.get()[i], READ_WRITE_SIZE);

        splitFile.push_back(splitArray);
    }

    return splitFile;
}

