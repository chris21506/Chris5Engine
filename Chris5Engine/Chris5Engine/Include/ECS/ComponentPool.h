#pragma once
#include "SparseSet.h"

namespace ECS {

    // Interfaz puramente virtual para poder almacenar piscinas de tipos distintos en un solo contenedor
    class IComponentPool : public SparseSet {
    public:
        virtual ~IComponentPool() = default;
    };

    template<typename T>
    class ComponentPool final : public IComponentPool {
    public:
        ComponentPool() = default;
        ~ComponentPool() override = default;

        // Inserta o reemplaza un componente
        template<typename... Args>
        T& Emplace(EntityID entity, Args&&... args) {
            assert(!Contains(entity) && "El componente ya existe en esta entidad");

            // Registramos la entidad en el SparseSet y obtenemos su indice contiguo
            InsertEntity(entity);
            m_components.emplace_back(std::forward<Args>(args)...);

            return m_components.back();
        }

        // Obtiene la referencia al componente de la entidad
        [[nodiscard]] T& Get(EntityID entity) noexcept {
            assert(Contains(entity) && "Intentando obtener un componente inexistente");
            return m_components[m_sparse[GetEntityIndex(entity)]];
        }

        [[nodiscard]] const T& Get(EntityID entity) const noexcept {
            assert(Contains(entity) && "Intentando obtener un componente inexistente");
            return m_components[m_sparse[GetEntityIndex(entity)]];
        }

        // Sobrescritura de Remove con Swap-With-Last para mantener la contigüidad
        void Remove(EntityID entity) override {
            if (!Contains(entity)) return;

            const EntityIndex sparseIdx = GetEntityIndex(entity);
            const EntityIndex denseIdx = m_sparse[sparseIdx];

            // Swap-with-last en el array real de memoria contigua de datos
            m_components[denseIdx] = std::move(m_components.back());
            m_components.pop_back();

            // Llamamos a la base para actualizar m_dense y m_sparse
            SparseSet::Remove(entity);
        }

        void Clear() override {
            SparseSet::Clear();
            m_components.clear();
        }

        [[nodiscard]] std::vector<T>& Data() noexcept { return m_components; }
        [[nodiscard]] const std::vector<T>& Data() const noexcept { return m_components; }

    private:
        std::vector<T> m_components; // Memoria empaquetada cache-friendly
    };
}#pragma once
