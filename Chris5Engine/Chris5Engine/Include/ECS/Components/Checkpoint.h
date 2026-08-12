// Archivo: ECS/Components/Checkpoint.h
#pragma once

#include <imgui.h>

namespace ECS {

    struct CheckpointComponent {
        int checkpointIndex; // Orden numérico del checkpoint (0, 1, 2...)
        bool isFinishLine;   // Define si este punto marca la meta/vuelta
        float radius;        // Rango de activación

        CheckpointComponent(int index = 0, bool finish = false, float rad = 60.0f)
            : checkpointIndex(index)
            , isFinishLine(finish)
            , radius(rad)
        {
        }

        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(0.8f, 0.4f, 1.0f, 1.0f), "Punto de Control (Checkpoint)");

            ImGui::InputInt("Indice de Checkpoint", &checkpointIndex);
            ImGui::Checkbox("Es Linea de Meta", &isFinishLine);
            ImGui::SliderFloat("Radio Activacion", &radius, 10.0f, 300.0f, "%.1f px");
        }
    };

} // namespace ECS#pragma once
