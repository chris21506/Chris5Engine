// Archivo: ECS/Components/RaceStats.h
#pragma once

#include <imgui.h>

namespace ECS {

    struct RaceStatsComponent {
        int currentLap;          // Vuelta actual
        int totalLaps;           // Vueltas totales de la carrera
        int lastCheckpointIndex; // Último checkpoint validado correctamente
        float currentLapTime;    // Tiempo actual en la vuelta (segundos)
        float bestLapTime;       // Mejor tiempo registrado (segundos)
        float totalRaceTime;     // Tiempo total acumulado (segundos)
        bool isFinished;         // Marca si la entidad ya terminó la carrera

        RaceStatsComponent(int maxLaps = 3)
            : currentLap(1)
            , totalLaps(maxLaps)
            , lastCheckpointIndex(-1)
            , currentLapTime(0.0f)
            , bestLapTime(0.0f)
            , totalRaceTime(0.0f)
            , isFinished(false)
        {
        }

        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.9f, 1.0f), "Estadisticas de Carrera");

            ImGui::LabelText("Estado", isFinished ? "CARRERA FINALIZADA" : "EN CARRERA");
            ImGui::LabelText("Vuelta", "%d / %d", currentLap, totalLaps);
            ImGui::LabelText("Ultimo Checkpoint", "%d", lastCheckpointIndex);
            ImGui::LabelText("Tiempo Vuelta", "%.2f s", currentLapTime);
            ImGui::LabelText("Mejor Vuelta", "%.2f s", bestLapTime);
            ImGui::LabelText("Tiempo Total", "%.2f s", totalRaceTime);

            ImGui::Separator();
            ImGui::InputInt("Vueltas Totales Meta", &totalLaps);
        }
    };

} // namespace ECS#pragma once
