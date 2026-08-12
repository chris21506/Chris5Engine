// Archivo: ECS/Components/PathFollower.h
#pragma once

#include <SFML/System/Vector2f.hpp>
#include <vector>
#include <imgui.h>

namespace ECS {

    struct PathFollowerComponent {
        std::vector<sf::Vector2f> waypoints; // Lista de nodos del circuito
        size_t currentWaypointIndex;         // Nodo actual al que se dirige
        float waypointRadius;                // Distancia para dar por alcanzado un punto
        bool isLooping;                      // Si es true, reinicia al terminar el circuito

        PathFollowerComponent(float radius = 40.0f, bool loop = true)
            : currentWaypointIndex(0)
            , waypointRadius(radius)
            , isLooping(loop)
        {
        }

        void AddWaypoint(const sf::Vector2f& point) {
            waypoints.push_back(point);
        }

        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.2f, 1.0f), "Seguidor de Ruta (Path Follower)");

            ImGui::LabelText("Puntos Registrados", "%zu", waypoints.size());

            int currentIndex = static_cast<int>(currentWaypointIndex);
            if (ImGui::InputInt("Nodo Actual Target", &currentIndex)) {
                if (currentIndex >= 0 && currentIndex < static_cast<int>(waypoints.size())) {
                    currentWaypointIndex = static_cast<size_t>(currentIndex);
                }
            }

            ImGui::SliderFloat("Radio Tolerancia Nodo", &waypointRadius, 5.0f, 200.0f, "%.1f px");
            ImGui::Checkbox("Ruta en Bucle (Circuito)", &isLooping);

            if (ImGui::Button("Limpiar Todos los Puntos")) {
                waypoints.clear();
                currentWaypointIndex = 0;
            }
        }
    };

} // namespace ECS#pragma once
