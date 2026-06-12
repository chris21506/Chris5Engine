#pragma once
#include <tuple>
#include "ComponentPool.h"

namespace ECS {

    /// <summary>
    /// Una View (Vista) es un contenedor virtual que permite iterar sobre un grupo específico de componentes.
    /// No copia datos; en su lugar, actúa como un filtro dinámico sobre las piscinas (pools) existentes.
    /// </summary>
    /// <typeparam name="...Components">Lista variable de componentes requeridos (ej. <Position, Velocity>).</typeparam>
    template<typename... Components>
    class View {
    public:
        /// <summary>
        /// Constructor de la vista. Recibe el pool más pequeño para optimizar la búsqueda y el resto de los pools.
        /// </summary>
        /// <param name="smallestPool">Piscina que tiene menor cantidad de entidades registradas (maestra de iteración).</param>
        /// <param name="pools">Packs de punteros a las piscinas de los componentes que queremos filtrar.</param>
        View(SparseSet* smallestPool, ComponentPool<Components>*... pools)
            : m_smallestPool(smallestPool), m_pools(pools) {
        }

        // ============================================================================
        // Iterator (Iterador personalizado compatible con bucles basados en rango)
        // ============================================================================
        // Permite escribir sintaxis como: for (EntityID entity : view) { ... }
        class Iterator {
        public:
            /// <summary>
            /// Inicializa el iterador en una posición específica del array denso.
            /// </summary>
            Iterator(const SparseSet* pool, size_t index, std::tuple<ComponentPool<Components>*...> pools)
                : m_pool(pool), m_index(index), m_pools(pools) {

                // Si la primera entidad apuntada por m_index no tiene TODOS los componentes, 
                // saltamos inmediatamente a la siguiente que sí los cumpla.
                Validate();
            }

            // Operador de desreferencia (*it): Devuelve el EntityID guardado en la posición actual
            EntityID operator*() const noexcept {
                return m_pool->GetEntities()[m_index];
            }

            // Operador de incremento (++it): Avanza al siguiente elemento de la lista y lo valida
            Iterator& operator++() noexcept {
                ++m_index;    // Moverse al siguiente índice del pool maestro
                Validate();   // Buscar la siguiente entidad válida
                return *this;
            }

            // Operador de comparación (!=): Permite al bucle saber cuándo ha llegado al final (view.end())
            bool operator!=(const Iterator& other) const noexcept {
                return m_index != other.m_index;
            }

        private:
            /// <summary>
            /// Filtro dinámico en runtime. Avanza m_index hasta encontrar una entidad que posea 
            /// absolutamente todos los componentes solicitados en la View, o hasta llegar al final del pool.
            /// </summary>
            void Validate() noexcept {
                const auto& entities = m_pool->GetEntities();

                while (m_index < entities.size()) {
                    EntityID entity = entities[m_index];

                    // Magia de C++17 (Fold Expression):
                    // Evalúa en paralelo: std::get<PoolA>(m_pools)->Contains(entity) && std::get<PoolB>(m_pools)->Contains(entity) && ...
                    // Si alguna piscina no contiene a la entidad, 'isValid' se vuelve false.
                    bool isValid = (std::get<ComponentPool<Components>*>(m_pools)->Contains(entity) && ...);

                    // Si la entidad actual cumple con todos los requisitos, detenemos el bucle while
                    // y el iterador se queda apuntando felizmente a este m_index.
                    if (isValid) break;

                    // Si no es válida, avanzamos el índice para inspeccionar la siguiente entidad del pool maestro
                    ++m_index;
                }
            }

            const SparseSet* m_pool;  // Puntero al pool maestro (el que estamos recorriendo físicamente)
            size_t m_index;           // Posición actual dentro del array m_dense del pool maestro
            std::tuple<ComponentPool<Components>*...> m_pools; // Tupla que almacena el resto de los pools para validar
        };

        // --- Funciones requeridas por C++ para habilitar los bucles 'for' automáticos ---

        // Devuelve un iterador apuntando al inicio (índice 0) de nuestra piscina optimizada
        Iterator begin() const noexcept { return Iterator(m_smallestPool, 0, m_pools); }

        // Devuelve un iterador apuntando al final (tamaño máximo) para saber cuándo detenerse
        Iterator end() const noexcept { return Iterator(m_smallestPool, m_smallestPool->Size(), m_pools); }

    private:
        SparseSet* m_smallestPool; // El pool con menos entidades; nuestra base para iterar rápido

        // Almacenamos los punteros de los pools en una tupla indexada por tipos en tiempo de compilación
        std::tuple<ComponentPool<Components>*...> m_pools;
    };
}