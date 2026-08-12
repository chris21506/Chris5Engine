#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"

// --- COMPONENTES Y SISTEMAS DE COMPORTAMIENTO AUTÓNOMO ---
#include "ECS/Components/SteeringAgentComponent.h"
#include "ECS/Components/SteeringBehaviorComponent.h"
#include "ECS/Systems/SteeringSystem.h"
#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"

// --- COMPONENTES BASE ---
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/ColliderComponent.h"

// --- COMPONENTES DE STEERING ---
#include "ECS/Components/SteeringAgentComponent.h"
#include "ECS/Components/SteeringBehaviorComponent.h"

// --- COMPONENTES DE CARRERAS Y JUGADOR ---
#include "ECS/Components/Obstacle.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/Components/PathFollower.h"
#include "ECS/Components/Checkpoint.h"
#include "ECS/Components/RaceStats.h"
#include "ECS/Components/LapCounter.h"

// --- SISTEMAS ---
#include "ECS/Systems/SteeringSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/UISystem.h"

Window g_window(Window(800, 600, "Labrid Engine"));
ECS::Registry registry;

void destroy()
{
    ImGui::SFML::Shutdown(*g_window.m_window);
}

int main()
{
    // Inicialización de SFML con ImGui
    if (!ImGui::SFML::Init(*g_window.m_window)) {
        return -1;
    }

    // Registrar sistemas en el registro ECS
    registry.AddSystem<ECS::SteeringSystem>();
    registry.AddSystem<ECS::RenderSystem>(g_window);
    registry.AddSystem<ECS::UISystem>();

    sf::Clock deltaClock;

    // 1. ENTIDAD OBSTÁCULO (ObstacleComponent + ColliderComponent)
    ECS::EntityID obstacle = registry.CreateEntity();
    registry.AddComponent<ECS::TransformComponent>(obstacle, sf::Vector2f{ 400.f, 300.f });
    registry.AddComponent<ECS::Render>(obstacle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50)));
    registry.AddComponent<ECS::ColliderComponent>(obstacle, 40.0f);
    registry.AddComponent<ECS::ObstacleComponent>(obstacle, 40.0f, true);

    // 2. ENTIDAD AGENTE IA (Steering + PathFollowerComponent)
    ECS::EntityID aiAgent = registry.CreateEntity();
    registry.AddComponent<ECS::TransformComponent>(aiAgent, sf::Vector2f{ 100.f, 100.f }, 0.f);
    registry.AddComponent<ECS::Render>(aiAgent, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));
    registry.AddComponent<ECS::SteeringAgentComponent>(aiAgent, 200.0f, 300.0f, 1.0f);

    auto& behavior = registry.AddComponent<ECS::SteeringBehaviorComponent>(aiAgent);
    behavior.activeBehavior = ECS::SteeringType::WANDER;

    auto& path = registry.AddComponent<ECS::PathFollowerComponent>(aiAgent, 30.0f, true);
    path.AddWaypoint({ 100.f, 100.f });
    path.AddWaypoint({ 700.f, 100.f });
    path.AddWaypoint({ 700.f, 500.f });
    path.AddWaypoint({ 100.f, 500.f });

    // 3. ENTIDAD JUGADOR (PlayerControllerComponent + RaceStatsComponent + LapCounterComponent)
    ECS::EntityID player = registry.CreateEntity();
    registry.AddComponent<ECS::TransformComponent>(player, sf::Vector2f{ 200.f, 200.f });
    registry.AddComponent<ECS::Render>(player, ECS::Render::Make(CIRCLE, sf::Color::Red));
    registry.AddComponent<ECS::PlayerControllerComponent>(player);
    registry.AddComponent<ECS::RaceStatsComponent>(player, 3);
    registry.AddComponent<ECS::LapCounterComponent>(player, 3, 4);

    // 4. ENTIDAD CHECKPOINT (CheckpointComponent)
    ECS::EntityID finishLine = registry.CreateEntity();
    registry.AddComponent<ECS::TransformComponent>(finishLine, sf::Vector2f{ 100.f, 100.f });
    registry.AddComponent<ECS::CheckpointComponent>(finishLine, 0, true, 60.0f);

    // Bucle principal del juego
    while (g_window.m_window->isOpen()) {
        while (const std::optional event = g_window.m_window->pollEvent()) {
            ImGui::SFML::ProcessEvent(*g_window.m_window, *event);

            if (event->is<sf::Event::Closed>()) {
                g_window.close();
            }
        }

        const sf::Time elapsedTime = deltaClock.restart();
        const float dt = elapsedTime.asSeconds();

        // Iniciar frame de ImGui
        ImGui::SFML::Update(*g_window.m_window, elapsedTime);

        // Limpieza de pantalla
        g_window.clear(sf::Color::Black);

        // Ejecutar lógica de los sistemas en el ECS
        registry.UpdateSystems(dt);

        // Renderizar interfaz ImGui sobre la escena
        ImGui::SFML::Render(*g_window.m_window);

        // Presentar el frame en pantalla
        g_window.display();
    }

    destroy();
    return 0;
}