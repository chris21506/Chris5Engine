// Archivo: ECS/Systems/SteeringSystem.h
#pragma once

#include "../System.h"
#include "../Registry.h"
#include "../Components/Transform.h"
#include "../Components/SteeringAgent.h"
#include "../Components/SteeringBehavior.h"
#include "../Physics/SteeringMath.h"

namespace ECS {

    class SteeringSystem : public System {
    public:
        void Update(Registry& registry, float deltaTime) {
            // Obtenemos una vista con las entidades que tienen posicionamiento, físicas de agente y un comportamiento activo
            auto view = registry.GetView<TransformComponent, SteeringAgentComponent, SteeringBehaviorComponent>();

            for (EntityID entity : view) {
                auto& transform = registry.GetComponent<TransformComponent>(entity);
                auto& agent = registry.GetComponent<SteeringAgentComponent>(entity);
                auto& behavior = registry.GetComponent<SteeringBehaviorComponent>(entity);

                // Si no hay comportamiento activo, no aplicamos fuerzas físicas autónomas
                if (behavior.activeBehavior == SteeringType::NONE) {
                    continue;
                }

                sf::Vector2f steeringForce(0.0f, 0.0f);

                // --- PROCESAMIENTO DE COMPORTAMIENTOS ---
                switch (behavior.activeBehavior) {

                case SteeringType::SEEK:
                    steeringForce = Steering::Seek(
                        transform.position,
                        agent.velocity,
                        behavior.targetPosition,
                        agent.maxSpeed,
                        agent.maxForce
                    );
                    break;

                case SteeringType::FLEE:
                    steeringForce = Steering::Flee(
                        transform.position,
                        agent.velocity,
                        behavior.targetPosition,
                        agent.maxSpeed,
                        agent.maxForce
                    );
                    break;

                case SteeringType::ARRIVE:
                    steeringForce = Steering::Arrive(
                        transform.position,
                        agent.velocity,
                        behavior.targetPosition,
                        behavior.slowingRadius,
                        agent.maxSpeed,
                        agent.maxForce
                    );
                    break;

                case SteeringType::WANDER:
                    steeringForce = Steering::Wander(
                        transform.position,
                        agent.velocity,
                        behavior.wanderAngle, // Se pasa por referencia para persistir el ángulo
                        behavior.wanderRadius,
                        behavior.wanderDistance,
                        behavior.wanderJitter,
                        deltaTime,
                        agent.maxSpeed,
                        agent.maxForce
                    );
                    break;

                case SteeringType::PURSUIT: {
                    // Verificamos si la presa (entidad objetivo) existe en el registro
                    if (registry.IsValidEntity(behavior.targetEntityID)) {
                        sf::Vector2f targetPos = registry.GetComponent<TransformComponent>(behavior.targetEntityID).position;
                        sf::Vector2f targetVel(0.0f, 0.0f);

                        // Si la presa también es un agente físico, leemos su velocidad real
                        if (registry.HasComponent<SteeringAgentComponent>(behavior.targetEntityID)) {
                            targetVel = registry.GetComponent<SteeringAgentComponent>(behavior.targetEntityID).velocity;
                        }

                        steeringForce = Steering::Pursuit(
                            transform.position,
                            agent.velocity,
                            targetPos,
                            targetVel,
                            agent.maxSpeed,
                            agent.maxForce
                        );
                    }
                    break;
                }

                case SteeringType::OBSTACLE_AVOIDANCE: {
                    // Construimos la lista de obstáculos dinámicamente usando ColliderComponent
                    std::vector<Steering::Obstacle> obstacles;
                    auto obstacleView = registry.GetView<TransformComponent, ColliderComponent>();

                    for (EntityID obstacleEntity : obstacleView) {
                        // No colisionar ni esquivarse a uno mismo
                        if (obstacleEntity == entity) continue;

                        auto& obsTransform = registry.GetComponent<TransformComponent>(obstacleEntity);
                        auto& obsCollider = registry.GetComponent<ColliderComponent>(obstacleEntity);

                        obstacles.push_back({ obsTransform.position, obsCollider.radius });
                    }

                    steeringForce = Steering::ObstacleAvoidance(
                        transform.position,
                        agent.velocity,
                        behavior.boxLength,
                        obstacles,
                        agent.maxForce
                    );
                    break;
                }

                default:
                    break;
                }

                // --- INTEGRACIÓN FÍSICA (Euler de un paso) ---
                // F = m * a  =>  a = F / m
                sf::Vector2f acceleration = steeringForce / agent.mass;

                // v = v + a * dt
                agent.velocity += acceleration * deltaTime;

                // Limitamos la velocidad final para respetar el límite del agente
                agent.velocity = Steering::Truncate(agent.velocity, agent.maxSpeed);

                // s = s + v * dt
                transform.position += agent.velocity * deltaTime;

                // --- ORIENTACIÓN VISUAL ---
                // Hacemos que la rotación apunte hacia la dirección de movimiento si el agente avanza
                float currentSpeed = Steering::Length(agent.velocity);
                if (currentSpeed > 5.0f) {
                    float radians = std::atan2(agent.velocity.y, agent.velocity.x);
                    transform.rotation = radians * (180.0f / 3.14159265f); // Radianes a Grados
                }
            }
        }
    };

} // namespace ECS
