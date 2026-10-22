@echo off
chcp 65001 > nul
echo ========================================
echo       COMPILANDO OPTIHUB...
echo ========================================

if not exist build mkdir build

g++ -std=c++17 -O2 -Ilibs/imgui -Ilibs/imgui/backends src/main.cpp src/ui/interface.cpp src/core/hardware_reader.cpp src/core/optimizer.cpp libs/imgui/imgui.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui_tables.cpp libs/imgui/imgui_widgets.cpp libs/imgui/backends/imgui_impl_win32.cpp libs/imgui/backends/imgui_impl_dx11.cpp -ld3d11 -ld3dcompiler -lshell32 -lole32 -lgdi32 -ldwmapi -o build/optihub.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCESSO] build/optihub.exe gerado com sucesso!
    echo.
) else (
    echo.
    echo [ERRO] Falha na compilacao.
    echo.
)
