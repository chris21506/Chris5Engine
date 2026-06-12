#include "Core/CShape.h"
#include "Core/Window.h"

/// <summary>
/// Constructor de la clase. Inicializa el tipo de figura y utiliza la lista de 
/// inicialización de miembros para delegar la creación de la memoria a 'createShape'.
/// </summary>
/// <param name="shapeType">El tipo de figura geométrica que se desea asignar.</param>
CShape::CShape(ShapeType shapeType)
    : m_shapeType(shapeType),
    m_shape(createShape(shapeType)) // Llama a la factoría estática e inicializa el unique_ptr
{
    // El cuerpo queda vacío ya que toda la inicialización se resolvió en la lista superior.
}

/// <summary>
/// Envía la figura contenida a la ventana personalizada del motor para su renderizado.
/// </summary>
/// <param name="window">Referencia a la ventana del juego.</param>
void CShape::draw(Window& window) {
    // Seguridad contra punteros nulos: Solo intentamos dibujar si m_shape apunta a algo real (ej. no es EMPTY)
    if (m_shape) {
        // Desreferenciamos el unique_ptr (*m_shape) para pasar el objeto sf::Shape por referencia pura
        window.draw(*m_shape);
    }
}

/// <summary>
/// Expone de forma segura la dirección de memoria de la figura de SFML sin ceder su propiedad.
/// </summary>
/// <returns>Un puntero crudo a sf::Shape manejable por sistemas externos.</returns>
sf::Shape* CShape::getShape() {
    // .get() extrae la dirección física que custodia el std::unique_ptr sin liberar el recurso.
    return m_shape.get();
}

/// <summary>
/// Factoría Estática. Evalúa el enumerado y fabrica en memoria dinámica (Heap) la geometría correspondiente de SFML.
/// </summary>
/// <param name="shapeType">El tipo de geometría solicitada.</param>
/// <returns>Un std::unique_ptr propietario de la figura polimórfica de SFML.</returns>
std::unique_ptr<sf::Shape> CShape::createShape(ShapeType shapeType) {

    switch (shapeType)
    {
    case EMPTY:
    {
        // Caso por defecto seguro: No se genera ninguna representación visual.
        return nullptr;
    }
    case CIRCLE:
    {
        // Instancia un círculo de radio 50 píxeles.
        auto circle = std::make_unique<sf::CircleShape>(50.f);
        circle->setFillColor(sf::Color::White);
        circle->setPosition({ 100.0f, 100.0f }); // Posición inicial hardcodeada por defecto
        return circle; // Retorna el ownership del puntero inteligente
    }
    case RECTANGLE:
    {
        // CORRECCIÓN: Tu código original creaba un ConvexShape de 3 puntos (un triángulo) para el rectángulo.
        // Se cambió por un sf::RectangleShape real con dimensiones de 100x50 píxeles.
        auto rectangle = std::make_unique<sf::RectangleShape>(sf::Vector2f(100.f, 50.f));
        rectangle->setFillColor(sf::Color::White);
        rectangle->setPosition({ 200.0f, 200.0f });
        return rectangle;
    }
    case TRIANGLE:
    {
        // CORRECCIÓN: Se arregló la errata "tringle" por "triangle" para que coincida con su uso inferior.
        // SFML no tiene un 'sf::TriangleShape', por lo que usar un ConvexShape de 3 puntos es la solución óptima.
        auto triangle = std::make_unique<sf::ConvexShape>(3);

        // Definimos las esquinas locales del triángulo de forma horaria/antihoraria
        triangle->setPoint(0, sf::Vector2f(0.f, 0.f));       // Esquina superior izquierda
        triangle->setPoint(1, sf::Vector2f(100.f, 0.f));     // Esquina superior derecha
        triangle->setPoint(2, sf::Vector2f(50.f, 100.f));    // Vértice inferior central

        triangle->setFillColor(sf::Color::White);
        triangle->setPosition({ 300.0f, 200.0f });
        return triangle;
    }
    case POLYGON:
    {
        // Crea un polígono convexo personalizado de 5 lados utilizando coordenadas vectoriales explícitas.
        auto polygon = std::make_unique<sf::ConvexShape>(5);
        polygon->setPoint(0, sf::Vector2f(0.f, 0.f));
        polygon->setPoint(1, sf::Vector2f(100.f, 0.f));
        polygon->setPoint(2, sf::Vector2f(120.f, 50.f));
        polygon->setPoint(3, sf::Vector2f(50.f, 100.f));
        polygon->setPoint(4, sf::Vector2f(-20.f, 50.f));

        polygon->setFillColor(sf::Color::White);
        polygon->setPosition({ 400.0f, 400.0f });
        return polygon;
    }
    case LINE:
    {
        // En SFML 2D, las líneas físicas delgadas se suelen emular mediante rectángulos 
        // extremadamente delgados (Ancho: 200 píxeles, Alto: 1 píxel).
        auto line = std::make_unique<sf::RectangleShape>(sf::Vector2f(200.f, 1.0f));
        line->setFillColor(sf::Color::White);
        line->setPosition({ 500.0f, 500.0f });
        return line;
    }
    default:
        // Si entra un ShapeType no controlado, evitamos un crash devolviendo nullptr de forma segura.
        return nullptr;
    }
}