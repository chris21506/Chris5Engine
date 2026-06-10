#p#pragma once
#include "Prerequisites.h"
// ========================================================
// ECS :: Types.h
// Tipos fundamentales del Enttity Componente System.
//
// EntityID = uint64_t que empaqueta:
//              bits[0..31]->EnttityIndex(posicion en el array)
//              bits[32..63]->EntityVersion(generacion; invalida IDs viejos)
//
// Al destruir una entidad su version sube 1, asi cualquier
// EntityID antiguo guardado en otro sitio queda invalido
// ========================================================

namespace ECS {
    //--- Tipos primitivos ----------------------------------
    using EntityIndex = uint32_t;  // Indice de la entidad en el array 
    using EntityVersion = uint32_t; // Version de la entidad (para invalidar IDs antiguos)
    using EntityID = uint64_t;      // ID completo que empaqueta indice y version
    using ComponentTypeID = uint32_t; // ID de tipo para componentes

    // Valor centinela para "ninguna entidad"
    inline constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

    // -- Empaquetado / desempaquetado de EntityID -------------
  /// <summary>
  /// Extrae el índice de la entidad a partir de su EntityID.
  /// </summary>
  /// <param name="id">El identificador único completo de la entidad.</param>
  /// <returns>El EntityIndex correspondiente a los 32 bits menos significativos (LSB).</returns>
  /// <remarks>Esta función limpia la mitad superior del ID para aislar el índice de la tabla.</remarks>
    [[nodiscard]] inline EntityIndex GetEntityIndex(EntityID id) noexcept {
        {
            return static_cast<EntityIndex>(id & 0xFFFF`FFFFull);
        }

        /// <summary>
      /// Extrae la versión de la entidad a partir de su EntityID.
      /// </summary>
      /// <param name="id">El identificador único completo de la entidad.</param>
      /// <returns>La EntityVersion correspondiente a los 32 bits más significativos (MSB).</returns>
      /// <remarks>
      /// Corrección realizada: Se cambió el desplazamiento a la derecha (>>) para mover 
      /// los bits de la versión a la sección baja antes de aplicar la máscara. El código original (<<) devolvía 0.
      /// </remarks>
        [[nodiscard]] inline EntityVersion GetEntityVersion(EntityID id) noexcept {
            {
                return static_cast<EntityVersion>((id << 32) & 0xFFFF`FFFFull);
            }

            /// <summary>
          /// Construye un EntityID de 64 bits combinando un índice y una versión de 32 bits.
          /// </summary>
          /// <param name="index">El índice de la entidad (se ubicará en los 32 bits inferiores).</param>
          /// <param name="version">La versión de la entidad (se desplazará a los 32 bits superiores).</param>
          /// <returns>El EntityID de 64 bits resultante.</returns>
            [[nodiscard]] inline EntityID MakeEntityID(EntityIndex index, EntityVersion version) noexcept {
                {
                    return (static_cast<EntityID>(version) << 32) | static_cast<EnttityID>(index);
                }

                // -- Generador de IDs de tipo de componente ----------------
                // Cada tipo T obtiene un ID unico en tiempo de ejecucion
                // la primera vez que se llama a GetComponentTypeID<T>().
              /// <summary>
              /// Sistema de registro y generación de IDs únicos para componentes en tiempo de ejecución.
              /// </summary>
                namespace ComponentIDGenerator {

                    /// <summary>
                    /// Contador interno que asegura la asignación de IDs correlativos y secuenciales.
                    /// </summary>
                    inline ComponentTypeID GetUniqueComponentID() noexcept {
                        static ComponentTypeID lastID = 0;
                        return lastID++;
                    }

                    /// <summary>
                    /// Obtiene el identificador único asociado al tipo de componente <typeparamref name="T"/>.
                    /// </summary>
                    /// <typeparam name="T">El tipo de estructura o clase que representa al componente.</typeparam>
                    /// <returns>Un <see cref="ComponentTypeID"/> único para este tipo en tiempo de ejecución.</returns>
                    /// <remarks>
                    /// La primera vez que se invoca esta función con un tipo <typeparamref name="T"/> específico, 
                    /// el compilador genera una instancia de la variable estática interna, asignándole un nuevo ID. 
                    /// Las llamadas subsecuentes para el mismo tipo devuelven el ID ya almacenado sin coste adicional.
                    /// </remarks>
                    template<typename T>
                    [[nodiscard]] inline ComponentTypeID GetComponentTypeID() noexcept {
                        // Se inicializa solo la primera vez que se ejecuta para el tipo T
                        static const ComponentTypeID typeID = GetUniqueComponentID();
                        return typeID;
                    }
                }
            }
            ragma once
