#include "../../include/core/hardware_reader.hpp"
#include <iostream>
#include <iomanip>
#include <windows.h>

// esse aquivo é simplesmente a criação de funções para pegar informações do hardware do computador, 
// e a main somente exibe

// aqui ele converte para um inteiro de 64 bits, para poder fazer a conta de porcentagem do uso da CPU
static ULONGLONG fileTimeToUInt64(const FILETIME& ft) {
    return (((ULONGLONG)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}
// informações de ram
RamInfo getRamInfo() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    const double bytesToGB = 1024.0 * 1024.0 * 1024.0;
    
    RamInfo info;
    info.totalGB = (double)statex.ullTotalPhys / bytesToGB;
    info.freeGB = (double)statex.ullAvailPhys / bytesToGB;
    info.usedGB = info.totalGB - info.freeGB;
    info.usagePercent = static_cast<int>(statex.dwMemoryLoad);

    return info;
}
//informações de disco
DiskInfo getDiskInfo(const std::wstring& drive) {
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    DiskInfo info;
    info.drive = drive;
    info.totalGB = 0.0;
    info.freeGB = 0.0;
    info.usedGB = 0.0;
    info.usagePercent = 0;
    info.success = false;

    if (GetDiskFreeSpaceExW(drive.c_str(), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        const double bytesToGB = 1024.0 * 1024.0 * 1024.0;
        info.totalGB = (double)totalNumberOfBytes.QuadPart / bytesToGB;
        info.freeGB = (double)freeBytesAvailable.QuadPart / bytesToGB;
        info.usedGB = info.totalGB - info.freeGB;
        
        if (info.totalGB > 0) {
            info.usagePercent = static_cast<int>((info.usedGB / info.totalGB) * 100.0);
        }
        info.success = true;
    }

    return info;
}
//informações de CPU
std::string getCpuModelName() {
    HKEY hKey;
    char cpuBrand[256] = "Processador Desconhecido";
    DWORD bufferSize = sizeof(cpuBrand);

    // Le o nome oficial da CPU registrado no Windows
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)cpuBrand, &bufferSize);
        RegCloseKey(hKey);
    }
    return std::string(cpuBrand);
}
// retorna a quantidade de núcleos da CPU
int getCpuLogicalCores() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return static_cast<int>(sysInfo.dwNumberOfProcessors);
}

static ULONGLONG prevIdle = 0;
static ULONGLONG prevKernel = 0;
static ULONGLONG prevUser = 0;
static bool cpuInitialized = false;

// retorna a porcentagem de uso da CPU
double getCpuUsagePercent() {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return 0.0;
    }

    ULONGLONG idle = fileTimeToUInt64(idleTime);
    ULONGLONG kernel = fileTimeToUInt64(kernelTime);
    ULONGLONG user = fileTimeToUInt64(userTime);

    
    if (!cpuInitialized) {
        prevIdle = idle;
        prevKernel = kernel;
        prevUser = user;
        cpuInitialized = true;
        
        Sleep(100);
        GetSystemTimes(&idleTime, &kernelTime, &userTime);
        idle = fileTimeToUInt64(idleTime);
        kernel = fileTimeToUInt64(kernelTime);
        user = fileTimeToUInt64(userTime);
    }

    ULONGLONG diffIdle = idle - prevIdle;
    ULONGLONG diffKernel = kernel - prevKernel;
    ULONGLONG diffUser = user - prevUser;

    prevIdle = idle;
    prevKernel = kernel;
    prevUser = user;

    ULONGLONG totalSystem = diffKernel + diffUser;
    if (totalSystem == 0) return 0.0;

    double percent = (double)(totalSystem - diffIdle) * 100.0 / (double)totalSystem;
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;

    return percent;
}

#ifdef STANDALONE_HARDWARE_READER
int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "Exibição de Informações do Hardware\n\n";

    std::cout << "[CPU] " << getCpuModelName() << "\n";
    std::cout << "  - Núcleos Lógicos: " << getCpuLogicalCores() << "\n";
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "  - Uso Atual: " << getCpuUsagePercent() << "%\n\n";

    RamInfo ram = getRamInfo();
    std::cout << "[RAM] Memória Física\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  - Total: " << ram.totalGB << " GB\n";
    std::cout << "  - Em Uso: " << ram.usedGB << " GB (" << ram.usagePercent << "%)\n";
    std::cout << "  - Livre: " << ram.freeGB << " GB\n\n";

    DiskInfo diskC = getDiskInfo(L"C:\\");
    if (diskC.success) {
        std::cout << "[DISCO] C:\\\n";
        std::cout << "  - Total: " << diskC.totalGB << " GB\n";
        std::cout << "  - Em Uso: " << diskC.usedGB << " GB (" << diskC.usagePercent << "%)\n";
        std::cout << "  - Livre: " << diskC.freeGB << " GB\n\n";
    }

    DiskInfo diskD = getDiskInfo(L"D:\\");
    if (diskD.success) {
        std::cout << "[DISCO] D:\\\n";
        std::cout << "  - Total: " << diskD.totalGB << " GB\n";
        std::cout << "  - Em Uso: " << diskD.usedGB << " GB (" << diskD.usagePercent << "%)\n";
        std::cout << "  - Livre: " << diskD.freeGB << " GB\n\n";
    }

    std::cout << "========================================\n";
    return 0;
}
#endif