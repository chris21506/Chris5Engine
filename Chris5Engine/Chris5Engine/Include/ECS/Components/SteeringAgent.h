// Archivo: ECS/Components/SteeringAgent.h
#pragma once

#include <SFML/System/Vector2f.hpp>
#include <imgui.h>

namespace ECS {

    struct SteeringAgent {
        // --- ESTADO DINÁMICO ---
        sf::Vector2f velocity;      // Velocidad actual del agente (px/s)

        // --- PARÁMETROS FÍSICOS (Ajustables desde el Inspector) ---
        float maxSpeed;             // Límite de velocidad máxima del agente
        float maxForce;             // Límite de la fuerza máxima de giro aplicable (maniobrabilidad)
        float mass;                 // Inercia. A mayor masa, giros más pesados y menor aceleración

        // --- CONSTRUCTOR ---
        SteeringAgent(
            float maxSpd = 200.0f,
            float maxFrc = 300.0f,
            float m = 1.0f)
            : velocity(0.0f, 0.0f)
            , maxSpeed(maxSpd)
            , maxForce(maxFrc)
            , mass(m > 0.01f ? m : 1.0f) // Protección básica contra división por cero (F/m)
        {
        }

        // --- RENDERIZADO EN EL INSPECTOR (Dear ImGui) ---
        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Fisicas de Movimiento");

            // Visualización de la velocidad actual (Solo lectura)
            ImGui::LabelText("Velocidad Actual", "%.1f, %.1f (px/s)", velocity.x, velocity.y);

            // Sliders interactivos para calibrar al agente en tiempo real
            ImGui::SliderFloat("Velocidad Maxima", &maxSpeed, 10.0f, 1000.0f, "%.1f px/s");
            ImGui::SliderFloat("Fuerza Maxima", &maxForce, 10.0f, 2000.0f, "%.1f N");
            ImGui::SliderFloat("Masa del Agente", &mass, 0.1f, 50.0f, "%.2f kg");
        }
    };

} // namespace ECS
