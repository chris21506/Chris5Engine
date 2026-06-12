#pragma once
#include <tuple>
#include "ComponentPool.h"

namespace ECS {

    template<typename... Components>
    class View {
    public:
        View(SparseSet* smallestPool, ComponentPool<Components>*... pools)
            : m_smallestPool(smallestPool), m_pools(pools) {
        }

        // Iterador personalizado para bucles "for (EntityID entity : view)"
        class Iterator {
        public:
            Iterator(const SparseSet* pool, size_t index, std::tuple<ComponentPool<Components>*...> pools)
                : m_pool(pool), m_index(index), m_pools(pools) {
                // Si la primera entidad de m_dense no cumple los requisitos, avanza
                Validate();
            }

            EntityID operator*() const noexcept {
                return m_pool->GetEntities()[m_index];
            }

            Iterator& operator++() noexcept {
                ++m_index;
                Validate();
                return *this;
            }

            bool operator!=(const Iterator& other) const noexcept {
                return m_index != other.m_index;
            }

        private:
            void Validate() noexcept {
                const auto& entities = m_pool->GetEntities();
                while (m_index < entities.size()) {
                    EntityID entity = entities[m_index];
                    // Revisa si la entidad existe en todas las piscinas de la View
                    bool isValid = (std::get<ComponentPool<Components>*>(m_pools)->Contains(entity) && ...);
                    if (isValid) break;
                    ++m_index;
                }
            }

            const SparseSet* m_pool;
            size_t m_index;
            std::tuple<ComponentPool<Components>*...> m_pools;
        };

        Iterator begin() const noexcept { return Iterator(m_smallestPool, 0, m_pools); }
        Iterator end() const noexcept { return Iterator(m_smallestPool, m_smallestPool->Size(), m_pools); }

    private:
        SparseSet* m_smallestPool;
        std::tuple<ComponentPool<Components>*...> m_pools;
    };
}#pragma once
