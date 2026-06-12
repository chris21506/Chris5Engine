#pragma once
#include "Prerequisites.h"
#include <string>
#include <memory>

// Nota: Asumimos que Prerequisites.h ya incluye los headers necesarios de SFML 
// como <SFML/Graphics.hpp> o <SFML/System.hpp>.

class Window {
public:
    // Constructor por defecto. Deja la ventana en un estado seguro y no inicializado (m_window = nullptr).
    Window() = default;

    /// <summary>
    /// Constructor parametrizado que inicializa, crea y abre la ventana en el sistema operativo.
    /// </summary>
    /// <param name="width">Ancho de la ventana en píxeles.</param>
    /// <param name="height">Alto de la ventana en píxeles.</param>
    /// <param name="title">Texto que se mostrará en la barra de título superior.</param>
    Window(int width, int height, const std::string& title);

    // Destructor de la clase. Se encarga de liberar la memoria de m_window automáticamente al salir de scope.
    ~Window();

    // void handleEvents(EngineGUI& engineGUI); // Comentado en el original: Procesará inputs/eventos de ventana e interfaz.

    /// <summary>
    /// Consulta si la ventana sigue abierta en el sistema y ejecutándose.
    /// </summary>
    /// <returns>True si la ventana está activa; False si el usuario la cerró.</returns>
    [[nodiscard]] bool isOpen() const;

    /// <summary>
    /// Borra todo el contenido previo de la pantalla y la pinta de un color uniforme. 
    /// Es el primer paso obligatorio al iniciar el renderizado de cada cuadro.
    /// </summary>
    /// <param name="color">El color base de fondo (por defecto es Negro Opaco: 0, 0, 0, 255).</param>
    void clear(const sf::Color& color = sf::Color(0, 0, 0, 255));

    /// <summary>
    /// Envía cualquier objeto gráfico bidimensional compatible con SFML al buffer interno de la ventana.
    /// </summary>
    /// <param name="drawable">Cualquier objeto que herede de sf::Drawable (ej. sf::Sprite, sf::Text, sf::Shape).</param>
    /// <param name="states">Configuraciones de renderizado (Blends, Shaders, Texturas, Transformaciones).</param>
    /// <remarks>CORRECCIÓN: Se cambió 'sf::Dramable' por 'sf::Drawable'.</remarks>
    void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);

    /// <summary>
    /// Intercambia el buffer oculto (donde estuviste dibujando con draw()) con el buffer visible de la pantalla.
    /// Evita el efecto de parpadeo (Screen Flickering) mediante la técnica de Double Buffering.
    /// </summary>
    void display();

    /// <summary>
    /// Cierra físicamente la ventana gráfica, pero mantiene viva la instancia de la clase en memoria.
    /// </summary>
    void close();

    /// <summary>
    /// Actualiza la lógica interna de la ventana y el control del tiempo (Mide el Delta Time).
    /// </summary>
    void update();

    /// <summary>
    /// Centraliza los pasos de dibujo del motor (Clear -> Draw de elementos -> Display).
    /// </summary>
    void render();

    /// <summary>
    /// Libera explícitamente los recursos y destruye el puntero de la ventana de SFML.
    /// </summary>
    void destroy();

public:
    // Puntero inteligente propietario de la ventana real de SFML. 
    // Al ser público, permite que sistemas externos de renderizado o GUI (como ImGui) 
    // accedan directamente al contexto gráfico de SFML si es necesario.
    std::unique_ptr<sf::RenderWindow> m_window = nullptr;

private:
    // sf::View controla la "cámara" del juego. Define qué porción del mundo 2D 
    // se renderiza en la pantalla, permitiendo hacer Zoom, Scroll y paneos de cámara.
    sf::View m_view;

    // Almacena el tiempo exacto que tardó en procesarse el último frame del juego.
    // Es vital pasárselo a los sistemas para lograr movimientos fluidos e independientes de los FPS.
    sf::Time deltaTime;

    // Cronómetro de alta precisión de SFML que cuenta el tiempo continuamente para poder calcular el deltaTime.
    sf::Clock clock;
};