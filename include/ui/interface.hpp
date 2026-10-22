#pragma once

#include "../core/hardware_reader.hpp"
#include "../core/optimizer.hpp"
#include <string>
#include <vector>

// Abas de navegação (conforme o esboço)
enum class ActiveTab {
    Dashboard,      // Gráficos e monitoramento em tempo real
    Otimizacao,     // Botões de limpeza e ações do sistema
    Configuracoes   // Ajustes do app
};

// Estrutura que guarda o estado geral da interface
struct AppState {
    ActiveTab currentTab = ActiveTab::Dashboard;

    // Métricas atuais do computador
    CpuInfo cpu;
    RamInfo ram;
    DiskInfo diskC;

    // Histórico para alimentar os gráficos de linha (últimos 60 pontos)
    std::vector<float> cpuHistory = std::vector<float>(60, 0.0f);
    std::vector<float> ramHistory = std::vector<float>(60, 0.0f);

    // Mensagem de status da última otimização executada
    std::string statusMessage = "Sistema pronto.";
};

// ========================================================
// FUNÇÕES DE DESENHO DA INTERFACE (ESBOÇO)
// ========================================================

// 1. Atualiza as métricas lendo o hardware_reader
void updateMetrics(AppState& state);

// 2. Desenha a barra superior (Nome do App à esquerda + Abas à direita)
void renderTopBar(AppState& state);

// 3. Desenha a tela do Dashboard (Gráficos de CPU, RAM e Disco)
void renderDashboard(AppState& state);

// 4. Desenha a tela de Otimização (Botões de ação)
void renderOptimizer(AppState& state);

// 5. Desenha a tela de Configurações (Esboço para expansão futura)
void renderSettings(AppState& state);
