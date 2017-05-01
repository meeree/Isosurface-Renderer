#ifndef __POLYGONISER_H__
#define __POLYGONISER_H__

#include <glm/glm.hpp>
#include <vector>
#include <tuple>

class DensitySampler;

struct Vertex 
{
    glm::vec3 mPosition;
    glm::vec3 mNormal;
};

struct Grid
{
    size_t mDim[3];
    float mSize[3];
    float mOrigin[3];
};

class Polygoniser
{
public:
    virtual void polygonise (Grid const&, float const&, std::vector<Vertex>&, DensitySampler const&) = 0;
    virtual void polygonise (Grid const&, float const&, std::vector<Vertex>&, std::vector<unsigned>&, DensitySampler const&);
};

class MarchingCubes final : public Polygoniser
{
private:
    static uint16_t msEdgeTable[256];
    static int8_t msTriTable[256][16];
public:
    virtual void polygonise (Grid const&, float const&, std::vector<Vertex>&, DensitySampler const&) override;
    virtual void polygonise (Grid const&, float const&, std::vector<Vertex>&, std::vector<unsigned>&, DensitySampler const&) override;
};

class DualContour final : public Polygoniser
{
private:
    static uint16_t msEdgeTable[256];
    //This table is just a useful table to cut down on the size of the code
    static std::pair<uint8_t,uint8_t> msEdgeToVertTable[12];
public:
    virtual void polygonise (Grid const&, float const&, std::vector<Vertex>&, DensitySampler const&) override;
};

#endif //POLYGONISER_H
