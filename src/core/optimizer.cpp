#include "../../include/core/optimizer.hpp"
#include <iostream>
#include <filesystem>
#include <windows.h>
#include <shellapi.h>

//nesse arquivo, basicamente, ele limpa a lixeira, limpa o cache DNS e limpa os arquivos temporários do Windows e do Usuário
//as bibliotecas utilizadas são a filesystem para manipular arquivos e diretórios, a windows.h para funções do Windows e a shellapi.h para esvaziar a lixeira

namespace fs = std::filesystem;

bool emptyRecycleBin() {
    HRESULT result = SHEmptyRecycleBinW(
        NULL, 
        NULL, 
        SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND
    );

    return (result == S_OK || result == S_FALSE);
}

typedef BOOL (WINAPI *pfnDnsFlushResolverCache)();

bool flushDns() {
    HMODULE hDnsApi = LoadLibraryA("dnsapi.dll");
    if (hDnsApi) {
        pfnDnsFlushResolverCache DnsFlush = 
            (pfnDnsFlushResolverCache)GetProcAddress(hDnsApi, "DnsFlushResolverCache");
        
        if (DnsFlush) {
            BOOL res = DnsFlush();
            FreeLibrary(hDnsApi);
            if (res) return true;
        } else {
            FreeLibrary(hDnsApi);
        }
    }

    int exitCode = system("ipconfig /flushdns > nul 2>&1");
    return (exitCode == 0);
}

static void cleanDirectory(const fs::path& dirPath, CleanResult& result) {
    std::error_code ec;
    if (!fs::exists(dirPath, ec) || !fs::is_directory(dirPath, ec)) {
        return;
    }

    for (const auto& entry : fs::directory_iterator(dirPath, fs::directory_options::skip_permission_denied, ec)) {
        std::error_code removeEc;
        uintmax_t fileSize = 0;

        if (entry.is_regular_file(ec)) {
            fileSize = entry.file_size(ec);
        }

        bool removed = fs::remove_all(entry.path(), removeEc);
        if (removed && !removeEc) {
            result.filesDeleted++;
            result.bytesFreed += fileSize;
        } else {
            result.filesFailed++;
        }
    }
}

CleanResult cleanTempFiles() {
    CleanResult result;
    std::error_code ec;

    //aqui limpa a pasta temporária do Usuário (%temp%)
    fs::path userTempPath = fs::temp_directory_path(ec);
    if (!ec) {
        cleanDirectory(userTempPath, result);
    }

    //e aqui limpa a pasta temporária do Windows temp
    wchar_t winDir[MAX_PATH];
    if (GetWindowsDirectoryW(winDir, MAX_PATH) > 0) {
        fs::path windowsTempPath = fs::path(winDir) / "Temp";
        cleanDirectory(windowsTempPath, result);
    }

    result.success = true;
    return result;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "TESTE\n\n";

    std::cout << "1. Esvaziar Lixeira\n";
    std::cout << "2. Limpar Cache DNS (Flush DNS)\n";
    std::cout << "3. Limpar Arquivos Temporários (%temp% e Windows Temp)\n";
    std::cout << "0. Sair\n";
    std::cout << "Escolha uma opcao: ";

    int choice;
    if (!(std::cin >> choice)) return 0;

    switch (choice) {
        case 1: {
            std::cout << "\nEsvaziando lixeira...\n";
            if (emptyRecycleBin()) {
                std::cout << ">> Sucesso: Lixeira esvaziada!\n";
            } else {
                std::cout << ">> Aviso: Lixeira já estava vazia ou falhou.\n";
            }
            break;
        }
        case 2: {
            std::cout << "\nExecutando Flush DNS...\n";
            if (flushDns()) {
                std::cout << ">> Sucesso: Cache DNS limpo com sucesso!\n";
            } else {
                std::cout << ">> Erro ao limpar cache DNS.\n";
            }
            break;
        }
        case 3: {
            std::cout << "\nLimpando arquivos temporários (%temp% e Windows Temp)...\n";
            CleanResult res = cleanTempFiles();
            
            double mbFreed = (double)res.bytesFreed / (1024.0 * 1024.0);
            std::cout << ">> Arquivos removidos: " << res.filesDeleted << "\n";
            std::cout << ">> Arquivos em uso (ignorados): " << res.filesFailed << "\n";
            std::cout << ">> Espaço liberado: " << mbFreed << " MB\n";
            break;
        }
        default:
            std::cout << "Saindo...\n";
            break;
    }

    std::cout << "========================================\n";
    return 0;
}
