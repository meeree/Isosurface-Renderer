#ifndef __DENSITY_SAMPLER_H__
#define __DENSITY_SAMPLER_H__

#include <glm/glm.hpp>
#include "../src/Simplex.h"

class DensitySampler 
{
public:
    float sample (glm::vec3 const&) const;
    float add (glm::vec3 const&, std::vector<float (*)(glm::vec3 const&)> const&) const;
    DensitySampler () = default;
};

#endif //DENSITY_SAMPLER_H
