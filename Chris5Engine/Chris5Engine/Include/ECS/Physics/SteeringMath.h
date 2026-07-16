// Archivo: ECS/Physics/SteeringMath.h
#pragma once

#include <SFML/System/Vector2f.hpp>
#include <cmath>
#include <vector>
#include <random>
#include <limits>

namespace Steering {

    // Constante matemática para conversiones de ángulos
    constexpr float PI = 3.14159265f;

    // Estructura básica auxiliar para la evasión de obstáculos
    struct Obstacle {
        sf::Vector2f position;
        float radius;
    };

    // ====================================================
    // --- FUNCIONES AUXILIARES DE VECTORES ---
    // ====================================================

    inline float Length(const sf::Vector2f& v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    inline sf::Vector2f Normalize(const sf::Vector2f& v) {
        float len = Length(v);
        if (len > 0.0f) {
            return v / len;
        }
        return { 0.0f, 0.0f };
    }

    inline float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }

    inline sf::Vector2f Truncate(const sf::Vector2f& v, float max) {
        float len = Length(v);
        if (len > max && len > 0.0f) {
            return (v / len) * max;
        }
        return v;
    }

    // ====================================================
    // --- 1. SEEK (BUSCAR) ---
    // ====================================================
    inline sf::Vector2f Seek(
        const sf::Vector2f& currentPos,
        const sf::Vector2f& currentVelocity,
        const sf::Vector2f& targetPos,
        float maxSpeed,
        float maxForce)
    {
        sf::Vector2f desiredVelocity = targetPos - currentPos;
        desiredVelocity = Normalize(desiredVelocity) * maxSpeed;

        sf::Vector2f steer = desiredVelocity - currentVelocity;
        return Truncate(steer, maxForce);
    }

    // ====================================================
    // --- 2. FLEE (HUIR) ---
    // ====================================================
    inline sf::Vector2f Flee(
        const sf::Vector2f& currentPos,
        const sf::Vector2f& currentVelocity,
        const sf::Vector2f& dangerPos,
        float maxSpeed,
        float maxForce)
    {
        sf::Vector2f desiredVelocity = currentPos - dangerPos;
        desiredVelocity = Normalize(desiredVelocity) * maxSpeed;

        sf::Vector2f steer = desiredVelocity - currentVelocity;
        return Truncate(steer, maxForce);
    }

    // ====================================================
    // --- 3. ARRIVE (LLEGAR Y DESACELERAR SUAVEMENTE) ---
    // ====================================================
    inline sf::Vector2f Arrive(
        const sf::Vector2f& currentPos,
        const sf::Vector2f& currentVelocity,
        const sf::Vector2f& targetPos,
        float slowingRadius,
        float maxSpeed,
        float maxForce)
    {
        sf::Vector2f targetOffset = targetPos - currentPos;
        float distance = Length(targetOffset);

        if (distance <= 0.0f) return { 0.0f, 0.0f };

        // Calcular velocidad en función de la distancia al objetivo
        float speed = maxSpeed;
        if (distance < slowingRadius) {
            // Desaceleración suave y proporcional (mapeo lineal)
            speed = maxSpeed * (distance / slowingRadius);
        }

        sf::Vector2f desiredVelocity = targetOffset * (speed / distance);
        sf::Vector2f steer = desiredVelocity - currentVelocity;
        return Truncate(steer, maxForce);
    }

    // ====================================================
    // --- 4. WANDER (MERODEAR ORGÁNICAMENTE) ---
    // ====================================================
    inline sf::Vector2f Wander(
        const sf::Vector2f& currentPos,
        const sf::Vector2f& currentVelocity,
        float& wanderAngle, // ¡Debe ser una referencia para guardar estado!
        float wanderRadius,
        float wanderDistance,
        float wanderJitter,
        float deltaTime,
        float maxSpeed,
        float maxForce)
    {
        // Generador pseudoaleatorio estático para mantener buen rendimiento
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

        // 1. Modificar sutilmente el ángulo con un desvío aleatorio por segundo
        wanderAngle += dis(gen) * wanderJitter * deltaTime;

        // 2. Proyectar el círculo hacia el frente basándonos en la velocidad actual
        sf::Vector2f forward = Normalize(currentVelocity);
        if (Length(forward) == 0.0f) {
            forward = { 1.0f, 0.0f }; // Orientación por defecto si está estático
        }

        sf::Vector2f circleCenter = currentPos + forward * wanderDistance;

        // 3. Obtener el punto objetivo en el borde de la circunferencia proyectada
        sf::Vector2f displacement(
            std::cos(wanderAngle) * wanderRadius,
            std::sin(wanderAngle) * wanderRadius
        );
        sf::Vector2f targetWorld = circleCenter + displacement;

        // 4. Perseguir ese objetivo dinámico utilizando un comportamiento Seek clásico
        return Seek(currentPos, currentVelocity, targetWorld, maxSpeed, maxForce);
    }

    // ====================================================
    // --- 5. PURSUIT (PERSECUCIÓN PREDICTIVA) ---
    // ====================================================
    inline sf::Vector2f Pursuit(
        const sf::Vector2f& currentPos,
        const sf::Vector2f& currentVelocity,
        const sf::Vector2f& targetPos,
        const sf::Vector2f& targetVelocity,
        float maxSpeed,
        float maxForce)
    {
        sf::Vector2f toTarget = targetPos - currentPos;
        float distance = Length(toTarget);

        if (distance <= 0.0f) return { 0.0f, 0.0f };

        // Predecir el tiempo de intercepción en función de la distancia y nuestra velocidad
        float lookAheadTime = distance / maxSpeed;
        sf::Vector2f predictedTarget = targetPos + targetVelocity * lookAheadTime;

        // Perseguimos el punto donde estará la presa en el futuro
        return Seek(currentPos, currentVelocity, predictedTarget, maxSpeed, maxForce);
    }

    // ====================================================
    // --- 6. OBSTACLE AVOIDANCE (EVASIÓN DE OBSTÁCULOS) ---
    // ====================================================
    inline sf::Vector2f ObstacleAvoidance(
        const sf::Vector2f& currentPos,
        const sf::Vector2f& currentVelocity,
        float boxLength,
        const std::vector<Obstacle>& obstacles,
        float maxForce)
    {
        sf::Vector2f forward = Normalize(currentVelocity);
        if (Length(forward) == 0.0f) return { 0.0f, 0.0f };

        const Obstacle* closestObstacle = nullptr;
        float distToClosest = std::numeric_limits<float>::max();

        // 1. Escanear obstáculos colisionando con el campo visual (radar) del agente
        for (const auto& obs : obstacles) {
            sf::Vector2f toObstacle = obs.position - currentPos;
            float dist = Length(toObstacle);

            // Verificar si el obstáculo entra en el rango del radar
            if (dist < boxLength) {
                // Verificar que el obstáculo se encuentre frente al agente (ángulo menor a 90°)
                float projection = Dot(toObstacle, forward);
                if (projection > 0.0f) {
                    if (dist < distToClosest) {
                        distToClosest = dist;
                        closestObstacle = &obs;
                    }
                }
            }
        }

        // 2. Si detectamos peligro, calculamos la fuerza evasiva lateral
        if (closestObstacle != nullptr) {
            // Vector perpendicular a la dirección actual de avance
            sf::Vector2f avoidanceForce{ -forward.y, forward.x };
            sf::Vector2f toObstacle = closestObstacle->position - currentPos;

            // Decidir si esquivamos virando a la izquierda o a la derecha
            if (Dot(avoidanceForce, toObstacle) > 0.0f) {
                avoidanceForce = -avoidanceForce;
            }

            // Aplicar mayor fuerza entre más inminente sea la colisión
            float dangerFactor = 1.0f + (boxLength - distToClosest) / boxLength;
            avoidanceForce *= maxForce * dangerFactor;

            return avoidanceForce;
        }

        return { 0.0f, 0.0f }; // Trayectoria segura, no se requiere fuerza evasiva
    }
}
