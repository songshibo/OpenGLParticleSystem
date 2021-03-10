#include "ForceField.hpp"

WindZone::WindZone(glm::vec3 _wind_scale, glm::vec3 freq)
{
    this->dirx = glm::vec3(1.0f, 0.0f, 0.0f);
    this->diry = glm::vec3(0.0f, 1.0f, 0.0f);
    this->dirz = glm::vec3(0.0f, 0.0f, 1.0f);
    this->wind_scale = _wind_scale;
    this->wind_freq = freq;
};


glm::vec3 WindZone::GenerateWind(float time)
{
    glm::vec3 dx = this->dirx * this->wind_scale.x * (float)(sin(time * this->wind_freq.x) * 0.5f + 0.3f);
    glm::vec3 dy = this->diry * this->wind_scale.y * (float)(sin(time * this->wind_freq.y) * 0.5f + 0.3f);
    glm::vec3 dz = this->dirz * this->wind_scale.z * (float)(sin(time * this->wind_freq.z) * 0.5f + 0.3f);

    return dx + dy + dz;
};