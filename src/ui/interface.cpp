#include "../../include/ui/interface.hpp"
#include "../../libs/imgui/imgui.h"
#include <iomanip>
#include <sstream>

// ========================================================
// 1. ATUALIZAÇÃO PERIÓDICA DAS MÉTRICAS DE HARDWARE
// ========================================================
void updateMetrics(AppState& state) {
    // Lê os dados através do hardware_reader
    state.cpu.modelName = getCpuModelName();
    state.cpu.logicalCores = getCpuLogicalCores();
    state.cpu.currentUsagePercent = getCpuUsagePercent();

    state.ram = getRamInfo();
    state.diskC = getDiskInfo(L"C:\\");

    // Desloca o histórico para a esquerda e insere a nova leitura no final (fila)
    for (size_t i = 0; i < state.cpuHistory.size() - 1; ++i) {
        state.cpuHistory[i] = state.cpuHistory[i + 1];
        state.ramHistory[i] = state.ramHistory[i + 1];
    }
    state.cpuHistory.back() = static_cast<float>(state.cpu.currentUsagePercent);
    state.ramHistory.back() = static_cast<float>(state.ram.usagePercent);
}

// ========================================================
// 2. BARRA SUPERIOR (Nome do App + Abas de navegação)
// ========================================================
void renderTopBar(AppState& state) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));

    // Lado esquerdo: Nome do aplicativo
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "⚡ OPTIHUB");
    ImGui::SameLine(0, 30.0f);

    // Lado direito: Abas que alternam a tela
    if (ImGui::Button("  📊 Dashboard  ")) {
        state.currentTab = ActiveTab::Dashboard;
    }
    ImGui::SameLine();

    if (ImGui::Button("  🚀 Otimização  ")) {
        state.currentTab = ActiveTab::Otimizacao;
    }
    ImGui::SameLine();

    if (ImGui::Button("  ⚙️ Configurações  ")) {
        state.currentTab = ActiveTab::Configuracoes;
    }

    ImGui::PopStyleVar();
    ImGui::Separator();
    ImGui::Spacing();
}

// ========================================================
// 3. TELA DO DASHBOARD (Gráficos em Tempo Real)
// ========================================================
void renderDashboard(AppState& state) {
    ImGui::TextDisabled("MONITORAMENTO EM TEMPO REAL");
    ImGui::Spacing();

    // Divide em 2 colunas: CPU à esquerda, RAM à direita
    ImGui::Columns(2, "dashboard_cols", false);

    // --- COLUNA 1: PROCESSADOR (CPU) ---
    ImGui::BeginChild("CPU_Card", ImVec2(0, 180), true);
    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.4f, 1.0f), "🧠 PROCESSADOR (CPU)");
    ImGui::Text("Modelo: %s", state.cpu.modelName.c_str());
    ImGui::Text("Uso Atual: %.1f%% (%d nucleos)", state.cpu.currentUsagePercent, state.cpu.logicalCores);
    
    // Gráfico de linha de uso da CPU (0% a 100%)
    ImGui::PlotLines("##cpu_plot", state.cpuHistory.data(), (int)state.cpuHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(-1, 80));
    ImGui::EndChild();

    ImGui::NextColumn();

    // --- COLUNA 2: MEMÓRIA (RAM) ---
    ImGui::BeginChild("RAM_Card", ImVec2(0, 180), true);
    ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "💾 MEMÓRIA (RAM)");
    ImGui::Text("Uso: %.2f GB / %.2f GB (%d%%)", state.ram.usedGB, state.ram.totalGB, state.ram.usagePercent);
    ImGui::Text("Disponivel: %.2f GB livres", state.ram.freeGB);

    // Gráfico de linha de uso da RAM (0% a 100%)
    ImGui::PlotLines("##ram_plot", state.ramHistory.data(), (int)state.ramHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(-1, 80));
    ImGui::EndChild();

    ImGui::Columns(1); // Volta para coluna única
    ImGui::Spacing();

    // --- ARMAZENAMENTO (DISCO C:) ---
    if (state.diskC.success) {
        ImGui::BeginChild("Disk_Card", ImVec2(0, 100), true);
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "💽 ARMAZENAMENTO (C:)");
        
        float diskFraction = (float)state.diskC.usedGB / (float)state.diskC.totalGB;
        char diskOverlay[64];
        snprintf(diskOverlay, sizeof(diskOverlay), "%.1f GB livres de %.1f GB (%d%%)", 
                 state.diskC.freeGB, state.diskC.totalGB, state.diskC.usagePercent);
        
        ImGui::ProgressBar(diskFraction, ImVec2(-1, 24), diskOverlay);
        ImGui::EndChild();
    }
}

// ========================================================
// 4. TELA DE OTIMIZAÇÃO (Botões de Ação)
// ========================================================
void renderOptimizer(AppState& state) {
    ImGui::TextDisabled("CENTRAL DE OTIMIZACAO E LIMPEZA");
    ImGui::Spacing();

    // Botão 1: Limpeza de Arquivos Temporários
    if (ImGui::Button("  🧹 Limpar Arquivos Temporarios (%%temp%% e Windows Temp)  ", ImVec2(-1, 38))) {
        CleanResult res = cleanTempFiles();
        double mb = (double)res.bytesFreed / (1024.0 * 1024.0);
        state.statusMessage = "Limpeza concluida! " + std::to_string(res.filesDeleted) + 
                              " arquivos apagados (" + std::to_string((int)mb) + " MB liberados).";
    }

    ImGui::Spacing();

    // Botão 2: Esvaziar Lixeira
    if (ImGui::Button("  🗑️ Esvaziar Lixeira do Windows  ", ImVec2(-1, 38))) {
        if (emptyRecycleBin()) {
            state.statusMessage = "Lixeira do Windows esvaziada com sucesso!";
        } else {
            state.statusMessage = "A lixeira ja estava vazia.";
        }
    }

    ImGui::Spacing();

    // Botão 3: Flush DNS
    if (ImGui::Button("  🌐 Limpar Cache DNS (Flush DNS)  ", ImVec2(-1, 38))) {
        if (flushDns()) {
            state.statusMessage = "Cache DNS limpo e renovado com sucesso!";
        } else {
            state.statusMessage = "Falha ao limpar cache DNS.";
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Rodapé com o status da última ação
    ImGui::Text("Status: %s", state.statusMessage.c_str());
}

// ========================================================
// 5. TELA DE CONFIGURAÇÕES (Esboço inicial)
// ========================================================
void renderSettings(AppState& /*state*/) {
    ImGui::TextDisabled("AJUSTES DO APLICATIVO");
    ImGui::Spacing();

    static bool startWithWindows = false;
    ImGui::Checkbox("Iniciar com o Windows (minimizado)", &startWithWindows);

    static int intervalMs = 1000;
    ImGui::SliderInt("Frequencia de atualizacao (ms)", &intervalMs, 250, 2000);

    ImGui::Spacing();
    ImGui::TextDisabled("(Esboco: aqui podemos colocar opcoes de tema e preferencias)");
}
