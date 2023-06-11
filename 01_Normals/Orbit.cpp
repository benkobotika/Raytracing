#include "Orbit.h"
#include <cmath>

Orbit::Orbit(float semiMajorAxis, float semiMinorAxis, float orbitalPeriod)
    : m_semiMajorAxis(semiMajorAxis)
    , m_semiMinorAxis(semiMinorAxis)
    , m_orbitalPeriod(orbitalPeriod)
{
}

glm::vec3 Orbit::CalculatePosition(float time) const
{
    float angle = 2.0f * M_PI * time / m_orbitalPeriod;
    float x = m_semiMajorAxis * cos(angle);
    float y = 0; //assuming orbits are on a plane for simplicity
    float z = m_semiMinorAxis * sin(angle);

    return glm::vec3(x, y, z);
}
