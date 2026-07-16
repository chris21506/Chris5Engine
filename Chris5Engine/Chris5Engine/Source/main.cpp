#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"

// --- NUEVOS INCLUDES PARA COMPORTAMIENTOS AUTÓNOMOS ---
#include "ECS/Components/SteeringAgentComponent.h"
#include "ECS/Components/SteeringBehaviorComponent.h"
#include "ECS/Systems/SteeringSystem.h"

#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/UISystem.h" 

Window g_window(Window(800, 600, "Labrid Engine"));
ECS::Registry registry;

void destroy()
{
    ImGui::SFML::Shutdown();
}

int main()
{
    // m_window es un puntero a sf::RenderWindow.
    if (!ImGui::SFML::Init(g_window.m_window)) {
        return -1;
    }

    // Registrar sistemas en el ECS.
    // 1. Agregamos el sistema de movimiento autónomo ANTES del RenderSystem para que calcule la física primero
    registry.AddSystem<ECS::SteeringSystem>();
    registry.AddSystem<ECS::RenderSystem>(g_window);
    registry.AddSystem<ECS::UISystem>();

    sf::Clock deltaClock;

    // --- ENTIDAD 1: Un círculo verde que actuará como Obstáculo / Presa estática ---
    ECS::EntityID circle = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
    registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50)));
    // Le añadimos un colisionador para poder esquivarlo con "Obstacle Avoidance" o seguirlo en "Pursuit"
    registry.AddComponent<ECS::ColliderComponent>(circle, 40.0f); // Radio de 40px

    // --- ENTIDAD 2: Un triángulo cian que será nuestro Agente Inteligente Autónomo ---
    ECS::EntityID tri = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f }, 45.f);
    registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));

    // Integramos físicamente al triángulo para que pueda moverse de forma autónoma
    registry.AddComponent<ECS::SteeringAgentComponent>(tri, 250.0f, 400.0f, 1.0f); // maxSpeed=250, maxForce=400, mass=1

    // Le agregamos el componente de comportamiento (por defecto inicia en NONE)
    // Ahora podrás seleccionarlo en el Inspector ImGui de tu UISystem, asignarle SEEK, WANDER, etc., y ver cómo reacciona en tiempo real.
    auto& behavior = registry.AddComponent<ECS::SteeringBehaviorComponent>(tri);
    behavior.activeBehavior = ECS::SteeringType::WANDER; // Lo inicializamos en Wander para que empiece a moverse solo inmediatamente

    while (g_window.m_window->isOpen()) {
        while (const std::optional event =
            g_window.m_window->pollEvent()) {
            // ImGui debe recibir todos los eventos de SFML 
            ImGui::SFML::ProcessEvent(*g_window.m_window, *event);

            if (event->is<sf::Event::Closed>()) {
                g_window.close();
            }
        }

        const sf::Time elapsedTime = deltaClock.restart();
        const float dt = elapsedTime.asSeconds();

        // Iniciar el frame de ImGui
        ImGui::SFML::Update(*g_window.m_window, elapsedTime);

        // Limpiar la ventana.
        g_window.clear(sf::Color::Black);

        // Renderizar los elementos de tu ECS (Actualiza SteeringSystem, RenderSystem y UISystem secuencialmente).
        registry.UpdateSystems(dt);

        // Renderizar ImGui despues de la escena.
        ImGui::SFML::Render(*g_window.m_window);

        // Presentar el frame.
        g_window.display();
    }

    destroy();
    return 0;
}