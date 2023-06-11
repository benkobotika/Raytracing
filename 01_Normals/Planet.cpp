#include "Planet.h"

Planet::Planet(Orbit orbit, float radius)
    : m_orbit(orbit)
    , m_radius(radius)
    , m_elapsedTime(0.0f)
{
}

void Planet::Update(float delta_time)
{
    m_elapsedTime += delta_time;
    m_position = m_orbit.CalculatePosition(m_elapsedTime);
}

const glm::vec3& Planet::GetPosition() const
{
    return m_position;
}
