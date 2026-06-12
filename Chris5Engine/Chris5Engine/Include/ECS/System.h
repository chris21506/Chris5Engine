#pragma once
#include "Registry.h"

namespace ECS {

    class System {
    public:
        System() = default;
        virtual ~System() = default;

        // Forzar paso por referencia del Registro del Mundo
        virtual void Update(Registry& registry, float deltaTime) = 0;

        bool IsEnabled() const noexcept { return m_enabled; }
        void SetEnabled(bool enabled) noexcept { m_enabled = enabled; }

    protected:
        bool m_enabled = true;
    };
}#pragma once
