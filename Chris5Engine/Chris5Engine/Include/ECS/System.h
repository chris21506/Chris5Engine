#pragma once
#include "Registry.h"

namespace ECS {

    /// <summary>
    /// Clase base abstracta para todos los Sistemas del ECS.
    /// Un Sistema no almacena datos ni estados de las entidades; su única función es 
    /// contener lógica pura y algoritmos que operan sobre los componentes a través del Registry.
    /// </summary>
    class System {
    public:
        // Constructor por defecto. Al ser una clase abstracta, no se puede instanciar directamente.
        System() = default;

        // Destructor virtual para asegurar que cuando el motor destruya un System* polimórfico,
        // se invoque correctamente el destructor de la clase derivada (el sistema real).
        virtual ~System() = default;

        /// <summary>
        /// Método clave que ejecutará la lógica del sistema en cada iteración del bucle principal (Game Loop).
        /// Al ser un método virtual puro (= 0), obliga a cualquier sistema hijo a implementarlo.
        /// </summary>
        /// <param name="registry">Referencia al gestor central para poder pedir Vistas (Views) de componentes.</param>
        /// <param name="deltaTime">El tiempo transcurrido en segundos desde el último cuadro (para movimiento independiente de los FPS).</param>
        virtual void Update(Registry& registry, float deltaTime) = 0;

        /// <summary>
        /// Consulta si el sistema está activo y debe ser procesado por el motor en este frame.
        /// </summary>
        [[nodiscard]] bool IsEnabled() const noexcept { return m_enabled; }

        /// <summary>
        /// Permite activar o desactivar dinámicamente el sistema en tiempo de ejecución.
        /// Útil para pausar sistemas específicos (ej. desactivar el PhysicsSystem en menús o cinemáticas).
        /// </summary>
        void SetEnabled(bool enabled) noexcept { m_enabled = enabled; }

    protected:
        // Estado de activación del sistema. Accesible por las clases hijas.
        bool m_enabled = true;
    };
}
