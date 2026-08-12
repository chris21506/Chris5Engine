// Archivo: ECS/Components/LapCounter.h
#pragma once

#include <imgui.h>

namespace ECS {

    struct LapCounterComponent {
        // --- ESTADO DE LA CARRERA ---
        int currentLap;               // Vuelta actual (inicia en 1)
        int totalLaps;                 // Total de vueltas requeridas para terminar
        int nextRequiredCheckpoint;    // Índice del próximo checkpoint obligatorio a validar
        int totalCheckpointsInTrack;   // Cantidad total de checkpoints presentes en el circuito

        // --- BANDERAS Y ESTADOS ---
        bool isFinished;               // True si la entidad completó todas las vueltas
        bool wrongDirection;           // Detecta si el agente avanza en sentido contrario

        // --- CONSTRUCTOR ---
        LapCounterComponent(int maxLaps = 3, int checkpointsInTrack = 4)
            : currentLap(1)
            , totalLaps(maxLaps)
            , nextRequiredCheckpoint(0)
            , totalCheckpointsInTrack(checkpointsInTrack)
            , isFinished(false)
            , wrongDirection(false)
        {
        }

        // --- RENDERIZADO EN EL INSPECTOR (Dear ImGui) ---
        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Contador de Vueltas (Lap Counter)");

            if (isFinished) {
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "STATUS: CARRERA FINALIZADA");
            }
            else {
                ImGui::Text("Vuelta Activa: %d / %d", currentLap, totalLaps);
            }

            if (wrongDirection) {
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[!] ADVERTENCIA: Sentido Contrario");
            }

            ImGui::Separator();

            // Parámetros modificables en tiempo de ejecución
            ImGui::InputInt("Vueltas Totales", &totalLaps);
            ImGui::InputInt("Checkpoints en Pista", &totalCheckpointsInTrack);
            ImGui::InputInt("Siguiente Checkpoint", &nextRequiredCheckpoint);

            ImGui::Checkbox("Finalizo Carrera", &isFinished);
            ImGui::Checkbox("Sentido Contrario", &wrongDirection);
        }
    };

} // namespace ECS#pragma once
