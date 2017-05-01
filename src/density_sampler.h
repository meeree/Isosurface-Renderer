#ifndef __DENSITY_SAMPLER_H__
#define __DENSITY_SAMPLER_H__

#include <glm/glm.hpp>
#include "../src/Simplex.h"

#define OCT_CNT 5
static float frequencies []
{
    1.0f, 0.5f, 0.1f, 0.13f, 5.7f
};

static float amplitudes []
{
    0.15f, 0.35f, 4.0f, -2.0f, 0.1f
};

class DensitySampler 
{
public:
    float sample (glm::vec3 const& p) const 
    {
    glm::vec3 pVec{p.x,p.y,p.z};
//    float rad = 7.5f;
//    glm::vec3 offset{pVec - glm::vec3(0.0f,-rad,0.0f)};
//    float density = rad-glm::length(offset);
    float density = -p.y;
    for (int i = 0; i < OCT_CNT; ++i)
    {
        density += Simplex::noise((p*frequencies[i])*amplitudes[i]);
    }
//    density -= glm::clamp((0.5f-p.y)*3.0f, 0.0f, 1.0f)*40;
    density += glm::clamp((float)(-1.0f-p.y+0.1f*cos(5.0f*(cos(p.x)-p.z))-Simplex::noise(p*0.4f)*0.2f)*2.0f, 0.0f, 1.0f)*40;
    density -= glm::clamp((1.0f+p.y)*3.0f, 0.0f, 1.0f)*40;
    return density;
    }
//        {return (2*pos.y*(pos.y*pos.y-3*pos.x*pos.x)-(9*pos.z*pos.z-1))*(1-pos.z*pos.z)+pow(pos.x*pos.x+pos.y*pos.y,2);}
//       {return pos.z*pos.z-pos.x*pos.x-pos.y*pos.y;}
//       {return 1.0-(pos.x*pos.x+pos.y*pos.y+pos.z*pos.z);}
    DensitySampler () = default;
};

#endif //DENSITY_SAMPLER_H
