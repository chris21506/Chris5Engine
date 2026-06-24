#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Systems/RenderSystem.h"

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
  registry.AddSystem<ECS::RenderSystem>(g_window);
  registry.AddSystem<ECS::UISystem>();

  sf::Clock deltaClock;

  ECS::EntityID circle = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
  registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50)));

  ECS::EntityID tri = registry.CreateEntity();
  registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f }, 45.f);
  registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));
  
  while (g_window->isOpen()) {
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

    // Renderizar los elementos de tu ECS.
    registry.UpdateSystems(dt);

    // Renderizar ImGui despues de la escena.
    ImGui::SFML::Render(*g_window.m_window);

    // Presentar el frame.
    g_window.display();
  }

  destroy();
  return 0;
}