#pragma once

#define BOOLIFY(x) !!(x)
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

typedef struct
{
    IMAGE_DOS_HEADER *pDosHeader;
    IMAGE_NT_HEADERS *pNtHeaders;
    IMAGE_SECTION_HEADER *pSectionHeader;
    std::unique_ptr<BYTE[]> pFileData;
    DWORD dwFileSize;
} NewProcessInfo;

const unsigned int READ_WRITE_SIZE = 640;
