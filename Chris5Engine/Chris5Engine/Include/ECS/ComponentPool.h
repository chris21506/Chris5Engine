#pragma once
#include "SparseSet.h"

namespace ECS {

    // ============================================================================
    // IComponentPool (Interfaz Base Polimórfica)
    // ============================================================================
    // C++ no permite meter clases con templates diferentes (ej. ComponentPool<Pos>
    // y ComponentPool<Vel>) dentro de un mismo std::vector o std::unordered_map.
    // Al heredar de esta interfaz común sin templates, el Registry podrá guardar 
    // punteros `IComponentPool*` de cualquier tipo de componente en un solo mapa.
    class IComponentPool : public SparseSet {
    public:
        // Destructor virtual puro por defecto para asegurar que al borrar un 
        // IComponentPool* se llame al destructor de la clase hija correspondiente.
        virtual ~IComponentPool() = default;
    };

    // ============================================================================
    // ComponentPool<T> (Piscina de Componentes Tipo T)
    // ============================================================================
    // Mantiene los datos de los componentes del tipo T perfectamente empaquetados
    // y contiguos en memoria, alineados 1:1 con el array `m_dense` heredado de SparseSet.
    template<typename T>
    class ComponentPool final : public IComponentPool {
    public:
        ComponentPool() = default;
        ~ComponentPool() override = default;

        /// <summary>
        /// Construye e inserta un componente directamente en el pool.
        /// </summary>
        /// <typeparam name="...Args">Tipos de los argumentos del constructor de T.</typeparam>
        /// <param name="entity">ID de la entidad dueña del componente.</param>
        /// <param name="...args">Valores a pasar al constructor del componente.</param>
        /// <returns>Referencia al componente recién creado.</returns>
        template<typename... Args>
        T& Emplace(EntityID entity, Args&&... args) {
            // Seguridad: Una entidad no debería tener duplicados del mismo componente.
            assert(!Contains(entity) && "El componente ya existe en esta entidad");

            // 1. Registra la entidad en las tablas de control del SparseSet.
            //    Esto añade el ID a `m_dense` y mapea su posición en `m_sparse`.
            InsertEntity(entity);

            // 2. Construye el componente T al final del vector usando "Perfect Forwarding".
            //    Evita copias o movimientos innecesarios construyendo el objeto in-place.
            m_components.emplace_back(std::forward<Args>(args)...);

            // Devolvemos una referencia al elemento que acaba de entrar
            return m_components.back();
        }

        /// <summary>
        /// Obtiene el componente asociado a una entidad en tiempo constante O(1).
        /// </summary>
        [[nodiscard]] T& Get(EntityID entity) noexcept {
            assert(Contains(entity) && "Intentando obtener un componente inexistente");

            // Explicación del triple salto indexado O(1):
            // 1. GetEntityIndex(entity) saca el índice puro (ej. posición 4).
            // 2. m_sparse[4] nos da el índice real en el array denso (ej. posición 0).
            // 3. m_components[0] nos devuelve el componente de manera ultra rápida.
            return m_components[m_sparse[GetEntityIndex(entity)]];
        }

        /// <summary>
        /// Versión de solo lectura (const) de Get.
        /// </summary>
        [[nodiscard]] const T& Get(EntityID entity) const noexcept {
            assert(Contains(entity) && "Intentando obtener un componente inexistente");
            return m_components[m_sparse[GetEntityIndex(entity)]];
        }

        /// <summary>
        /// Elimina el componente de la entidad manteniendo el array denso sin huecos (O(1)).
        /// </summary>
        /// <remarks>
        /// Aplica la técnica "Swap-with-Last": Mueve el último componente del vector 
        /// al hueco del que vamos a borrar, y luego hace un pop_back.
        /// </remarks>
        void Remove(EntityID entity) override {
            // Si la entidad no tiene este componente, ignoramos la orden.
            if (!Contains(entity)) return;

            const EntityIndex sparseIdx = GetEntityIndex(entity); // Posición en tabla sparse
            const EntityIndex denseIdx = m_sparse[sparseIdx];     // Posición real en m_components

            // 1. Intercambio de Datos (Swap-with-Last):
            //    Movemos el componente del final del vector para tapar el hueco del eliminado.
            m_components[denseIdx] = std::move(m_components.back());

            // 2. Destruimos el duplicado sobrante del final. El tamaño del vector se reduce en 1.
            m_components.pop_back();

            // 3. Sincronización de índices:
            //    Llamamos al método del SparseSet base para que reacomode `m_dense` y `m_sparse`
            //    exactamente bajo el mismo orden en que acabamos de mover el componente.
            SparseSet::Remove(entity);
        }

        /// <summary>
        /// Limpia por completo la piscina de componentes destruyendo todos los datos.
        /// </summary>
        void Clear() override {
            SparseSet::Clear();    // Limpia los índices de control (sparse y dense)
            m_components.clear();  // Llama a los destructores de todos los componentes T alojados
        }

        // --- Acceso directo a los buffers de memoria contigua (Para optimizaciones o Render) ---
        [[nodiscard]] std::vector<T>& Data() noexcept { return m_components; }
        [[nodiscard]] const std::vector<T>& Data() const noexcept { return m_components; }

    private:
        // El Santo Grial del ECS: Memoria contigua.
        // Al estar todos los structs T pegados en RAM, cuando un Sistema los procesa en masa,
        // el procesador los sube en ráfagas a la memoria caché L1/L2/L3 de forma masiva, 
        // evitando los temidos "Cache Misses" de la programación orientada a objetos tradicional.
        std::vector<T> m_components;
    };
}
