#include "density_sampler.h"
#include <algorithm>

#define OCT_CNT 4
static float frequencies []
{
    0.1f, 0.13f, 30.0f, 150.0f
};

static float amplitudes []
{
    4.0f, -2.0f, 0.025f, -0.004f
};

float f (glm::vec3 const& p) {return p.z*p.z-p.x*p.x-p.y*p.y;}
float umbrella (glm::vec3 const& p) {return p.y*p.y*p.z-p.x*p.x;}
float g (glm::vec3 const& p) {return 1.0-(p.x*p.x+p.y*p.y+p.z*p.z);}
float gstar (glm::vec3 const& p) {return 1.0-(pow(p.x,50)+pow(p.y,50)+pow(p.z,50));}
float h (glm::vec3 const& p) {return (2*p.y*(p.y*p.y-3*p.x*p.x)-(9*p.z*p.z-1))*(1-p.z*p.z)+pow(p.x*p.x+p.y*p.y,2);}
float crossCap (glm::vec3 const& p) {return 4*(pow(p.x,4)+p.x*p.x*p.y*p.y+p.x*p.x*p.z*p.z+p.x*p.x*p.z)+pow(p.y,4)+p.y*p.y*p.z*p.z-p.y*p.y;}

float DensitySampler::sample (glm::vec3 const& p) const
{
    return add(p, {&crossCap, &gstar, &f});
//    glm::vec3 pVec{p.x,p.y,p.z};
//        float rad = 3.0f;
//        glm::vec3 offset{pVec - glm::vec3(0.0f,-rad,0.0f)};
//        float density = rad-glm::length(offset);
////    float density = -p.y;
//    for (int i = 0; i < OCT_CNT; ++i)
//    {
//        density += 0.4*Simplex::noise((p*frequencies[i])*amplitudes[i]);
//    }
////    density -= glm::clamp((-1.0f+p.y)*3.0f, 0.0f, 1.0f)*40;
////    density += glm::clamp((float)(-1.0f-p.y+0.1f*cos(5.0f*(cos(p.x)-p.z))-Simplex::noise(p*0.7f)*0.2f)*2.0f, 0.0f, 1.0f)*40;
//    return density;
}

float DensitySampler::add (glm::vec3 const& p, std::vector<float (*)(glm::vec3 const&)> const& samplers) const
{
    std::vector<float> data(samplers.size());
    std::transform(samplers.begin(), samplers.end(), data.begin(), [&](float (*sampler)(glm::vec3 const&))
            {return sampler(p);});
    return *std::min_element(data.begin(), data.end());
//    return std::accumulate(data.begin(), data.end(), 0.0f)/data.size();
}
