// Path: 01_Normals\Header Files\Planet.h
#pragma once

#include <glm/glm.hpp>
#include "Orbit.h"

class Planet
{
public:
    Planet(Orbit orbit, float radius);
    ~Planet() = default;

    void Update(float delta_time);

    const glm::vec3& GetPosition() const;

private:
    glm::vec3 m_position;
    float m_radius;
    Orbit m_orbit;

    float m_elapsedTime;
};