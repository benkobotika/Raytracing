#include "SolarSystem.h"

void SolarSystem::AddPlanet(Planet planet)
{
    m_planets.push_back(planet);
}

void SolarSystem::Update(float delta_time)
{
    for (auto& planet : m_planets)
    {
        planet.Update(delta_time);
    }
}

std::vector<glm::vec3> SolarSystem::GetPlanetPositions() const
{
    std::vector<glm::vec3> positions;
    for (const auto& planet : m_planets)
    {
        positions.push_back(planet.GetPosition());
    }

    return positions;
}
