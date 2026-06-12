#include "Core/Window.h"

/// <summary>
/// Constructor parametrizado. Inicializa el contexto de SFML, abre la ventana y configura los FPS.
/// </summary>
Window::Window(int width, int height, const std::string& title) {
    // CORRECCIÓN: Se cambió 'conts' por 'const'.
    // CORRECCIÓN: No se puede usar '=' con 'new' en un unique_ptr. Usamos std::make_unique para inicializarlo de forma segura.
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({ static_cast<unsigned int>(width), static_cast<unsigned int>(height) }),
        title,
        sf::Style::Default
    );

    // CORRECCIÓN: Se arregló la lógica inversa de los mensajes de éxito/error.
    if (m_window) {
        // Si la ventana se inicializó correctamente, limitamos la tasa de refresco a 60 FPS estables.
        m_window->setFramerateLimit(60);

        // Enviamos un log confirmando que el recurso de la ventana está arriba.
        MESSAGE("Window", "Window", "SUCCESSFULLY OPENED");
    }
    else {
        // Si por alguna razón el contexto gráfico falla (ej. falta de drivers), detenemos el motor.
        ERROR_MSG("Window", "Window", "Failed to create window");
    }
}

/// <summary>
/// Destructor de la ventana. 
/// Al usar std::unique_ptr, no necesitas liberar la memoria manualmente aquí; se limpia sola.
/// </summary>
Window::~Window() {
    // El unique_ptr m_window destruirá la ventana de SFML automáticamente al salir de scope.
}

/// <summary>
/// Verifica si la ventana física sigue abierta por el sistema operativo.
/// </summary>
bool Window::isOpen() const {
    if (m_window) {
        // Retorna true solo si el puntero es válido Y la ventana interna está abierta.
        return m_window->isOpen();
    }
    else {
        ERROR_MSG("Window", "isOpen", "Window is null");
        return false;
    }
}

/// <summary>
/// Limpia el buffer de la pantalla rellenándolo con un color sólido.
/// </summary>
void Window::clear(const sf::Color& color) {
    if (m_window) {
        m_window->clear(color);
    }
    else {
        ERROR_MSG("Window", "clear", "Window is null");
    }
}

/// <summary>
/// Dibuja un objeto bidimensional compatible con SFML en el buffer trasero (Back Buffer).
/// </summary>
void Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
    if (m_window) {
        m_window->draw(drawable, states);
    }
    else {
        // CORRECCIÓN: Se unificó 'Error' a la macro global 'ERROR_MSG'.
        ERROR_MSG("Window", "draw", "Window is null");
    }
}

/// <summary>
/// Intercambia los buffers de video para mostrar en pantalla todo lo que se dibujó en este frame.
/// </summary>
void Window::display() {
    if (m_window) {
        m_window->display();
    }
    else {
        ERROR_MSG("Window", "display", "Window is null");
    }
}

/// <summary>
/// Envía la orden de cierre a la ventana de SFML (destruye la ventana física).
/// </summary>
void Window::close() {
    if (m_window) {
        m_window->close();
    }
    else {
        ERROR_MSG("Window", "close", "Window is null");
    }
}

/// <summary>
/// Actualiza el control del tiempo del juego. 
/// Mide cuántos milisegundos pasaron desde el frame anterior (Delta Time).
/// </summary>
void Window::update() {
    if (m_window) {
        // clock.restart() calcula el tiempo transcurrido y reinicia el cronómetro a cero 
        // para empezar a medir el siguiente cuadro del juego inmediatamente.
        deltaTime = clock.restart();
    }
}

/// <summary>
/// Espacio reservado para centralizar la canalización del renderizado.
/// </summary>
void Window::render() {
    // Actualmente vacío. Aquí coordinarás tus llamadas a clear(), draw() y display() en orden.
}

/// <summary>
/// Fuerza la destrucción inmediata del contexto de la ventana y libera su memoria asignada.
/// </summary>
void Window::destroy() {
    // .reset() destruye explícitamente el objeto sf::RenderWindow interno y pone el puntero en nullptr.
    m_window.reset();
}