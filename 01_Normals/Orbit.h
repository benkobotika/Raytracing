// Path: 01_Normals\Header Files\Orbit.h
#pragma once

#include <glm/glm.hpp>

class Orbit
{
public:
    Orbit(float semiMajorAxis, float semiMinorAxis, float orbitalPeriod);
    ~Orbit() = default;

    glm::vec3 CalculatePosition(float time) const;

private:
    float m_semiMajorAxis;
    float m_semiMinorAxis;
    float m_orbitalPeriod;
};
