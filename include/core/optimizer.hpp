#pragma once

#include <string>
#include <cstdint>

// Estrutura que guarda o resultado de uma limpeza
struct CleanResult {
    uintmax_t bytesFreed = 0;   // Total de bytes liberados
    int filesDeleted = 0;       // Quantidade de arquivos removidos
    int filesFailed = 0;        // Arquivos ignorados (ex: em uso pelo Windows)
    bool success = false;       // Se a operação foi executada
};

// ========================================================
// DECLARAÇÃO DAS FUNÇÕES DE OTIMIZAÇÃO
// ========================================================

// 1. Esvazia a lixeira do Windows (sem exibir janelas de confirmação)
bool emptyRecycleBin();

// 2. Limpa o cache de resolução DNS do Windows (equivalente ao ipconfig /flushdns)
bool flushDns();

// 3. Limpa arquivos temporários da pasta %temp% do usuário
CleanResult cleanTempFiles();
