#pragma once
#include "Prerequisites.h"

// Declaración hacia adelante (Forward Declaration) para evitar incluir todo 
// el header de 'Window' aquí, acelerando enormemente el tiempo de compilación.
class Window;

/// <summary>
/// Clase componente o contenedora que encapsula una figura geométrica de la biblioteca SFML (sf::Shape).
/// Actúa como un puente limpio entre los datos de tu motor y el renderizado físico de SFML.
/// </summary>
class CShape {
public:
    // Constructor por defecto. Inicializa el objeto en un estado seguro/vacío.
    CShape() = default;

    /// <summary>
    /// Constructor explícito que parametriza y crea la figura según el tipo solicitado.
    /// </summary>
    /// <param name="shapeType">Enumerado (ShapeType) que define si es un círculo, rectángulo, etc.</param>
    /// <remarks>
    /// Se usa 'explicit' para evitar que el compilador realice conversiones implícitas no deseadas
    /// (por ejemplo, pasar un entero sin querer y que intente transformarlo en un objeto CShape).
    /// </remarks>
    explicit CShape(ShapeType shapeType);

    // Destructor por defecto. Al destruir un objeto CShape, el ciclo de vida de
    // m_shape (std::unique_ptr) llegará a su fin y liberará la memoria asignada automáticamente.
    ~CShape() = default;

    /// <summary>
    /// Envía la figura geométrica al buffer de renderizado de la ventana del juego.
    /// </summary>
    /// <param name="window">Referencia a la ventana personalizada de tu motor encargada de dibujar en pantalla.</param>
    void draw(Window& window);

    /// <summary>
    /// Devuelve un puntero crudo (raw pointer) a la figura subyacente de SFML.
    /// </summary>
    /// <returns>Puntero no-propietario a sf::Shape. Permite modificar propiedades como el color, rotación o posición externa.</returns>
    sf::Shape* getShape();

private:
    /// <summary>
    /// Método de Factoría Estático (Static Factory Pattern).
    /// Centraliza la lógica de creación física en la memoria Heap (mediante new) de figuras específicas de SFML.
    /// </summary>
    /// <param name="shapeType">El tipo de geometría que se quiere fabricar.</param>
    /// <returns>Un puntero único propietario (std::unique_ptr) configurado con el subtipo de SFML.</returns>
    static std::unique_ptr<sf::Shape> createShape(ShapeType shapeType);

private:
    // El Santo Grial de la gestión de memoria moderna (RAII):
    // std::unique_ptr garantiza la PROPIEDAD EXCLUSIVA del recurso gráfico. 
    // Evita el uso peligroso de punteros crudos (*), borrados manuales (delete) y previene Memory Leaks.
    std::unique_ptr<sf::Shape> m_shape = nullptr;

    // Almacena el tipo de figura actual para consultas de lógica rápidas
    ShapeType m_shapeType;
};