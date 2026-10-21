#pragma once

#include <string>
#include <cstdint>

struct CleanResult {
    uintmax_t bytesFreed = 0;
    int filesDeleted = 0;
    int filesFailed = 0;
    bool success = false;
};

bool emptyRecycleBin();
bool flushDns();
CleanResult cleanTempFiles();
