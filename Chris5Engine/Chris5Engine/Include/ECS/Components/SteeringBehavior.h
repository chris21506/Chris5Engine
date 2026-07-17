// Archivo: ECS/Components/SteeringBehavior.h
#pragma once

#include <SFML/System/Vector2f.hpp>
#include <cstdint>
#include <imgui.h>

namespace ECS {

    // Los 6 comportamientos autónomos solicitados en la actividad + opción nula
    enum class SteeringType : uint8_t {
        NONE = 0,
        SEEK,
        FLEE,
        ARRIVE,
        WANDER,
        PURSUIT,
        OBSTACLE_AVOIDANCE
    };

    struct SteeringBehavior {
        // --- ESTADO ACTIVO ---
        SteeringType activeBehavior;

        // --- PARAMETROS GENERALES ---
        sf::Vector2f targetPosition; // Destino para Seek, Flee y Arrive
        uint32_t targetEntityID;     // Entidad a interceptar en Pursuit

        // --- PARAMETROS DE ARRIVE ---
        float slowingRadius;         // Distancia donde comienza la desaceleración

        // --- PARAMETROS DE WANDER ---
        float wanderRadius;          // Radio del círculo virtual proyectado
        float wanderDistance;        // Distancia hacia el frente para ubicar el círculo
        float wanderJitter;          // Desviación angular aleatoria máxima aplicada por segundo
        float wanderAngle;           // Ángulo actual en radianes (guarda el rumbo entre frames)

        // --- PARAMETROS DE OBSTACLE AVOIDANCE ---
        float boxLength;             // Largo de la caja de detección / "radar" frontal

        // --- CONSTRUCTOR ---
        SteeringBehavior()
            : activeBehavior(SteeringType::NONE)
            , targetPosition(0.0f, 0.0f)
            , targetEntityID(0)
            , slowingRadius(120.0f)
            , wanderRadius(30.0f)
            , wanderDistance(80.0f)
            , wanderJitter(40.0f)
            , wanderAngle(0.0f)
            , boxLength(100.0f)
        {
        }

        // --- RENDERIZADO EN EL INSPECTOR (Dear ImGui) ---
        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "Comportamiento Autonomo");

            // Selector del comportamiento activo (Combo Box)
            const char* items[] = { "None", "Seek", "Flee", "Arrive", "Wander", "Pursuit", "Obstacle Avoidance" };
            int currentItem = static_cast<int>(activeBehavior);

            if (ImGui::Combo("Comportamiento", &currentItem, items, 7)) {
                activeBehavior = static_cast<SteeringType>(currentItem);
            }

            ImGui::Separator();

            // Renderizar exclusivamente los controles del comportamiento seleccionado
            switch (activeBehavior) {
            case SteeringType::NONE:
                ImGui::TextWrapped("Agente pasivo. Elige un comportamiento para iniciar el movimiento autonomo.");
                break;

            case SteeringType::SEEK:
            case SteeringType::FLEE:
                ImGui::Text("Configurar Coordenadas:");
                ImGui::DragFloat2("Objetivo (X,Y)", &targetPosition.x, 1.0f);
                break;

            case SteeringType::ARRIVE:
                ImGui::Text("Configuracion Arrive:");
                ImGui::DragFloat2("Objetivo (X,Y)", &targetPosition.x, 1.0f);
                ImGui::SliderFloat("Radio Frenado", &slowingRadius, 10.0f, 500.0f, "%.1f px");
                break;

            case SteeringType::WANDER:
                ImGui::Text("Configuracion Wander (Circulo virtual):");
                ImGui::SliderFloat("Distancia Proyección", &wanderDistance, 10.0f, 300.0f, "%.1f px");
                ImGui::SliderFloat("Radio Circulo", &wanderRadius, 5.0f, 150.0f, "%.1f px");
                ImGui::SliderFloat("Ruido (Jitter)", &wanderJitter, 1.0f, 200.0f, "%.1f");
                break;

            case SteeringType::PURSUIT:
                ImGui::Text("Objetivo de Persecucion:");
                // Se ingresa el ID de la entidad objetivo registrada en tu ECS
                int targetID = static_cast<int>(targetEntityID);
                if (ImGui::InputInt("ID Entidad Presa", &targetID)) {
                    targetEntityID = targetID < 0 ? 0 : static_cast<uint32_t>(targetID);
                }
                break;

            case SteeringType::OBSTACLE_AVOIDANCE:
                ImGui::Text("Configuracion Radar de Evasion:");
                ImGui::SliderFloat("Largo del Radar", &boxLength, 20.0f, 400.0f, "%.1f px");
                break;
            }
        }
    };

} // namespace ECS
