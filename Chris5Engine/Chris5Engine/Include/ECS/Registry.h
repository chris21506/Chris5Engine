#pragma once
#include <unordered_map>
#include <memory>
#include <algorithm>
#include "ComponentPool.h"
#include "View.h"

namespace ECS {

    class Registry {
    public:
        Registry() = default;
        ~Registry() = default;

        // Genera una nueva Entidad (reutiliza indices de entidades destruidas subiendo la versión)
        EntityID CreateEntity() {
            EntityIndex index;
            EntityVersion version;

            if (m_freeIndices.empty()) {
                index = m_entityCount++;
                version = 1; // Generación base
            }
            else {
                index = m_freeIndices.back();
                m_freeIndices.pop_back();
                version = m_entityVersions[index]; // Recupera versión incrementada en la destrucción
            }

            if (index >= m_entityVersions.size()) {
                m_entityVersions.resize(index + 1, 1);
            }

            EntityID id = MakeEntityID(index, version);
            m_livingEntities.push_back(id);
            return id;
        }

        // Destruye una entidad y remueve todos sus componentes asociados
        void DestroyEntity(EntityID entity) {
            if (!IsValid(entity)) return;

            // Eliminar la entidad de todos los pools de componentes existentes
            for (auto&& [typeId, pool] : m_componentPools) {
                pool->Remove(entity);
            }

            EntityIndex index = GetEntityIndex(entity);
            m_entityVersions[index]++; // Incrementamos versión -> Invalida punteros/IDs remotos viejos
            m_freeIndices.push_back(index);

            m_livingEntities.erase(
                std::remove(m_livingEntities.begin(), m_livingEntities.end(), entity),
                m_livingEntities.end()
            );
        }

        [[nodiscard]] bool IsValid(EntityID entity) const noexcept {
            EntityIndex index = GetEntityIndex(entity);
            if (index >= m_entityVersions.size()) return false;
            return m_entityVersions[index] == GetEntityVersion(entity);
        }

        // Añadir componente
        template<typename T, typename... Args>
        T& AddComponent(EntityID entity, Args&&... args) {
            assert(IsValid(entity) && "Entidad invalida o muerta");
            return GetOrCreatePool<T>()->Emplace(entity, std::forward<Args>(args)...);
        }

        // Eliminar componente
        template<typename T>
        void RemoveComponent(EntityID entity) {
            assert(IsValid(entity) && "Entidad invalida o muerta");
            GetOrCreatePool<T>()->Remove(entity);
        }

        // Obtener componente
        template<typename T>
        [[nodiscard]] T& GetComponent(EntityID entity) {
            assert(IsValid(entity) && "Entidad invalida o muerta");
            return GetOrCreatePool<T>()->Get(entity);
        }

        template<typename T>
        [[nodiscard]] bool HasComponent(EntityID entity) {
            assert(IsValid(entity) && "Entidad invalida o muerta");
            return GetOrCreatePool<T>()->Contains(entity);
        }

        // Devuelve una estructura View para iterar eficientemente por tipos de componentes múltiples
        template<typename... Components>
        [[nodiscard]] View<Components...> GetView() {
            std::tuple<ComponentPool<Components>*...> pools{ GetOrCreatePool<Components>()... };

            // Encontramos el pool con menos entidades para optimizar la iteración
            SparseSet* smallestPool = nullptr;
            size_t minSize = std::numeric_limits<size_t>::max();

            auto findSmallest = [&](auto* pool) {
                if (pool->Size() < minSize) {
                    minSize = pool->Size();
                    smallestPool = pool;
                }
                };

            std::apply([&](auto*... args) { (findSmallest(args), ...); }, pools);

            return View<Components...>(smallestPool, std::get<ComponentPool<Components>*>(pools)...);
        }

    private:
        template<typename T>
        ComponentPool<T>* GetOrCreatePool() {
            ComponentTypeID typeId = ComponentIDGenerator::GetComponentTypeID<T>();
            auto it = m_componentPools.find(typeId);
            if (it == m_componentPools.end()) {
                auto pool = std::make_unique<ComponentPool<T>>();
                auto ptr = pool.get();
                m_componentPools[typeId] = std::move(pool);
                return ptr;
            }
            return static_cast<ComponentPool<T>*>(it->second.get());
        }

    private:
        EntityIndex m_entityCount = 0;
        std::vector<EntityVersion> m_entityVersions;
        std::vector<EntityIndex> m_freeIndices;
        std::vector<EntityID> m_livingEntities;

        // Contenedor heterogéneo polimórfico de piscinas indexadas por ID de tipo de componente
        std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;
    };
}#pragma once
