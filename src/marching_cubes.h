#ifndef __MARCHING_CUBES_H__
#define __MARCHING_CUBES_H__

#include <cstdint>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>

//Define isolevel uint size 
typedef uint8_t iso_uint_t;
typedef std::vector<std::vector<std::vector<iso_uint_t>>> grid_t;
//Define vec3 type
typedef glm::vec3 vec3;

struct Grid
{
    grid_t mGrid;
    size_t mDim[3];
    double mSize[3];
    double mOrigin[3];
};

struct Voxel 
{
	iso_uint_t mVal;
};

struct Vertex 
{
    vec3 mPosition;
    vec3 mNormal;
};

void resizeGridData (grid_t&, size_t const (&)[3]);
bool writeFile (std::string const&, Grid const&, char const*);
bool readFile (char const*, Grid&);

vec3 interpolate(Grid const&, unsigned const&, unsigned const&, unsigned const&);
void polygonise(Grid const&, iso_uint_t const&, std::vector<Vertex>&);


#endif //MARCHING_CUBES_H
