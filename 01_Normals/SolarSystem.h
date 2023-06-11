// Path: 01_Normals\Header Files\SolarSystem.h
#pragma once

#include <vector>
#include "Planet.h"

class SolarSystem
{
public:
    SolarSystem();
    ~SolarSystem() = default;

    void Update(float delta_time);
    void AddPlanet(const Planet& planet);
    const std::vector<Planet>& GetPlanets() const;

private:
    std::vector<Planet> m_planets;
};
