#pragma once

#include <string>

struct RamInfo {
    double totalGB;
    double freeGB;
    double usedGB;
    int usagePercent;
};

struct DiskInfo {
    std::wstring drive;
    double totalGB;
    double freeGB;
    double usedGB;
    int usagePercent;
    bool success;
};

struct CpuInfo {
    std::string modelName;
    int logicalCores;
    double currentUsagePercent;
};

RamInfo getRamInfo();
DiskInfo getDiskInfo(const std::wstring& drive = L"C:\\");
double getCpuUsagePercent();
std::string getCpuModelName();
int getCpuLogicalCores();
