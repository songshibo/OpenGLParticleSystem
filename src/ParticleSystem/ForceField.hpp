#ifndef FORCEFIELD_HPP
#define FORCEFIELD_HPP

#include <glm/glm.hpp>
#include <algorithm>

class WindZone
{
    public:
        glm::vec3 dirx, diry, dirz;
        glm::vec3 wind_scale;
        glm::vec3 wind_freq;
    
    public:
        WindZone(glm::vec3 _wind_scale, glm::vec3 freq);
        glm::vec3 GenerateWind(float time);
};

#endif