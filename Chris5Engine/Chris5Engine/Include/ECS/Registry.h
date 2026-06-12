#pragma once
#include <unordered_map>
#include <memory>
#include <algorithm>
#include "ComponentPool.h"
#include "View.h"

namespace ECS {

    /// <summary>
    /// El Registry es el gestor central del ECS. Controla la creación/destrucción de entidades,
    /// almacena los componentes en sus respectivas piscinas y genera vistas optimizadas para los sistemas.
    /// </summary>
    class Registry {
    public:
        Registry() = default;
        ~Registry() = default;

        /// <summary>
        /// Genera una nueva Entidad única. Reutiliza índices de entidades previamente destruidas 
        /// para evitar el crecimiento infinito de los vectores, incrementando su versión.
        /// </summary>
        /// <returns>Un EntityID de 64 bits listo para usar.</returns>
        EntityID CreateEntity() {
            EntityIndex index;
            EntityVersion version;

            // Si no hay índices reciclables, creamos una ranura completamente nueva
            if (m_freeIndices.empty()) {
                index = m_entityCount++;
                version = 1; // Primera generación/versión de esta ranura indexada
            }
            // Si hay ranuras libres en el "cementerio" de índices, las reutilizamos
            else {
                index = m_freeIndices.back();
                m_freeIndices.pop_back();

                // Recuperamos la versión actual de esa ranura (que ya fue incrementada al destruirse)
                version = m_entityVersions[index];
            }

            // Aseguramos que el vector de tracking de versiones sea lo bastante grande
            if (index >= m_entityVersions.size()) {
                m_entityVersions.resize(index + 1, 1);
            }

            // Empaquetamos el índice de 32 bits y la versión de 32 bits en el ID único de 64 bits
            EntityID id = MakeEntityID(index, version);
            m_livingEntities.push_back(id); // Registramos que está viva globalmente
            return id;
        }

        /// <summary>
        /// Destruye por completo una entidad, liberando todos sus componentes y preparando su ID para reciclaje.
        /// </summary>
        void DestroyEntity(EntityID entity) {
            // Si el ID ya expiró o no existe, ignoramos la petición para evitar corrupción
            if (!IsValid(entity)) return;

            // 1. Recorremos POLIMÓRFICAMENTE todas las piscinas de componentes existentes
            //    y eliminamos los datos que esta entidad poseía en cada una de ellas.
            for (auto&& [typeId, pool] : m_componentPools) {
                pool->Remove(entity);
            }

            EntityIndex index = GetEntityIndex(entity);

            // 2. Incrementamos la versión en el registro maestro. 
            //    Esto invalida INSTANTÁNEAMENTE cualquier copia de este EntityID que haya quedado guardada por ahí.
            m_entityVersions[index]++;

            // 3. Enviamos el índice al cementerio de disponibles para que CreateEntity() lo recicle después
            m_freeIndices.push_back(index);

            // 4. Eliminamos la entidad de la lista de entidades vivas usando el patrón Erase-Remove
            m_livingEntities.erase(
                std::remove(m_livingEntities.begin(), m_livingEntities.end(), entity),
                m_livingEntities.end()
            );
        }

        /// <summary>
        /// Comprueba si un EntityID sigue siendo válido o si corresponde a una entidad destruida en el pasado.
        /// </summary>
        [[nodiscard]] bool IsValid(EntityID entity) const noexcept {
            EntityIndex index = GetEntityIndex(entity);
            if (index >= m_entityVersions.size()) return false;

            // Es válida SÓLO si la versión grabada en el ID coincide exactamente con la del registro maestro
            return m_entityVersions[index] == GetEntityVersion(entity);
        }

        /// <summary>
        /// Instancia o asigna un componente del tipo T a la entidad mediante Perfect Forwarding.
        /// </summary>
        template<typename T, typename... Args>
        T& AddComponent(EntityID entity, Args&&... args) {
            assert(IsValid(entity) && "Intentando añadir componente a una entidad inválida o muerta");
            // Obtiene la piscina de T (la crea si no existe) y delega la inserción de datos
            return GetOrCreatePool<T>()->Emplace(entity, std::forward<Args>(args)...);
        }

        /// <summary>
        /// Elimina el componente del tipo T de la entidad especificada.
        /// </summary>
        template<typename T>
        void RemoveComponent(EntityID entity) {
            assert(IsValid(entity) && "Intentando eliminar componente de una entidad inválida o muerta");
            GetOrCreatePool<T>()->Remove(entity);
        }

        /// <summary>
        /// Obtiene una referencia mutable al componente T de la entidad.
        /// </summary>
        template<typename T>
        [[nodiscard]] T& GetComponent(EntityID entity) {
            assert(IsValid(entity) && "Intentando acceder a componentes de una entidad inválida o muerta");
            return GetOrCreatePool<T>()->Get(entity);
        }

        /// <summary>
        /// Consulta si la entidad posee o no un componente del tipo T.
        /// </summary>
        template<typename T>
        [[nodiscard]] bool HasComponent(EntityID entity) {
            assert(IsValid(entity) && "Intentando consultar componentes de una entidad inválida o muerta");
            return GetOrCreatePool<T>()->Contains(entity);
        }

        /// <summary>
        /// El corazón de las consultas de alto rendimiento del ECS.
        /// Devuelve un objeto View para iterar únicamente sobre las entidades que tengan el grupo exacto de componentes pedido.
        /// </summary>
        template<typename... Components>
        [[nodiscard]] View<Components...> GetView() {
            // 1. Recolectamos en una tupla de C++ los punteros a los pools de los componentes solicitados.
            std::tuple<ComponentPool<Components>*...> pools{ GetOrCreatePool<Components>()... };

            // 2. Estrategia de optimización (Smallest Pool Optimization):
            //    Para filtrar rápido, buscamos cuál de los pools solicitados tiene MENOS entidades registradas.
            //    Ese pool será nuestra base de iteración principal en la View, reduciendo drásticamente los descartes.
            SparseSet* smallestPool = nullptr;
            size_t minSize = std::numeric_limits<size_t>::max();

            // Lambda helper encargada de evaluar el tamaño de cada pool de forma genérica
            auto findSmallest = [&](auto* pool) {
                if (pool->Size() < minSize) {
                    minSize = pool->Size();
                    smallestPool = pool;
                }
                };

            // std::apply expande la tupla de pools y ejecuta la lambda para cada uno de ellos (Fold Expression)
            std::apply([&](auto*... args) { (findSmallest(args), ...); }, pools);

            // 3. Devolvemos la View configurada con el pool más pequeño como iterador maestro y el resto como validadores.
            return View<Components...>(smallestPool, std::get<ComponentPool<Components>*>(pools)...);
        }

    private:
        /// <summary>
        /// Helper perezoso (Lazy Initialization) que busca la piscina del tipo T.
        /// Si no existe en el mapa, la instancia dinámicamente en tiempo de ejecución.
        /// </summary>
        template<typename T>
        ComponentPool<T>* GetOrCreatePool() {
            // Obtenemos el ID único numérico entero asignado estáticamente a la clase T
            ComponentTypeID typeId = ComponentIDGenerator::GetComponentTypeID<T>();

            auto it = m_componentPools.find(typeId);
            if (it == m_componentPools.end()) {
                // Si es la primera vez que se pide este componente, creamos su piscina especializada
                auto pool = std::make_unique<ComponentPool<T>>();
                auto ptr = pool.get(); // Guardamos el puntero crudo antes de mover el ownership
                m_componentPools[typeId] = std::move(pool); // Almacenamiento polimórfico en el mapa
                return ptr;
            }

            // Si ya existía, hacemos un cast seguro desde la interfaz base IComponentPool* a su tipo real
            return static_cast<ComponentPool<T>*>(it->second.get());
        }

    private:
        EntityIndex m_entityCount = 0;              // Contador global de ranuras creadas históricamente
        std::vector<EntityVersion> m_entityVersions;// Versión actual de cada ranura (índice array = EntityIndex)
        std::vector<EntityIndex> m_freeIndices;     // Lista de ranuras muertas disponibles para ser reutilizadas (LIFO)
        std::vector<EntityID> m_livingEntities;     // Listado de control de todas las entidades activas en el mundo

        // El mapa maestro heterogéneo. Guarda de forma polimórfica CUALQUIER piscina de componente
        // utilizando la herencia de IComponentPool e indexándolo por su ComponentTypeID numérico.
        std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;
    };
}