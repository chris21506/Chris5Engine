// Archivo: ECS/Components/Obstacle.h
#pragma once

#include <imgui.h>

namespace ECS {

    struct ObstacleComponent {
        float radius;       // Radio del obstáculo para detección de colisiones/evasión
        bool isStatic;      // Indica si es un obstáculo fijo o en movimiento
        float dangerWeight; // Prioridad/peso de amenaza para algoritmos de steering

        ObstacleComponent(float rad = 30.0f, bool stat = true, float weight = 1.0f)
            : radius(rad)
            , isStatic(stat)
            , dangerWeight(weight)
        {
        }

        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Obstaculo de Mapa");

            ImGui::SliderFloat("Radio Obstaculo", &radius, 5.0f, 300.0f, "%.1f px");
            ImGui::Checkbox("Es Estatico", &isStatic);
            ImGui::SliderFloat("Nivel de Amenaza", &dangerWeight, 0.1f, 10.0f, "%.1f");
        }
    };

} // namespace ECS#pragma once
