#pragma once

#include <vector>

#include "Texture2D.hpp"

struct Material
{
    std::vector<Texture2D> DiffuseMaps;
    float TilingFactor = 1.0f;
    std::vector<Texture2D> SpecularMaps;
    float Shininess = 10.0f;
};

