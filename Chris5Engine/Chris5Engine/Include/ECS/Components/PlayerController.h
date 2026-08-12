// Archivo: ECS/Components/PlayerController.h
#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <imgui.h>

namespace ECS {

    struct PlayerControllerComponent {
        float accelerationForce; // Fuerza aplicada al acelerar
        float turnSpeed;         // Velocidad de giro manual (grados/s)
        float brakeForce;        // Fuerza de frenado manual
        bool enableInput;        // Permite congelar/habilitar el control del jugador

        // Teclas por defecto (W, S, A, D)
        sf::Keyboard::Key keyAccelerate;
        sf::Keyboard::Key keyBrake;
        sf::Keyboard::Key keyTurnLeft;
        sf::Keyboard::Key keyTurnRight;

        PlayerControllerComponent(
            float accel = 400.0f,
            float turn = 180.0f,
            float brake = 600.0f)
            : accelerationForce(accel)
            , turnSpeed(turn)
            , brakeForce(brake)
            , enableInput(true)
            , keyAccelerate(sf::Keyboard::Key::W)
            , keyBrake(sf::Keyboard::Key::S)
            , keyTurnLeft(sf::Keyboard::Key::A)
            , keyTurnRight(sf::Keyboard::Key::D)
        {
        }

        void OnInspectorGUI() {
            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Control del Jugador");

            ImGui::Checkbox("Habilitar Entradas", &enableInput);
            ImGui::SliderFloat("Fuerza Aceleracion", &accelerationForce, 50.0f, 2000.0f, "%.1f");
            ImGui::SliderFloat("Velocidad Giro", &turnSpeed, 30.0f, 720.0f, "%.1f deg/s");
            ImGui::SliderFloat("Fuerza Freno", &brakeForce, 50.0f, 2000.0f, "%.1f");
        }
    };

} // namespace ECS#pragma once
