#ifndef __MARCHING_CUBES_H__
#define __MARCHING_CUBES_H__

//#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
//inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
//{
//   if (code != cudaSuccess) 
//   {
//      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
//      if (abort) exit(code);
//   }
//}

#include <cstdint>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>

typedef float iso_type_t;

//Define vec3 type
typedef glm::vec3 vec3;

struct Grid
{
    size_t mDim[3];
    float mSize[3];
    float mOrigin[3];
};

struct Voxel 
{
	iso_type_t mVal;
};

struct Vertex 
{
    vec3 mPosition;
    vec3 mNormal;
};

void blabla (std::vector<Vertex>&);

//void resizeGridData (grid_t&, size_t const (&)[3]);
//bool writeFile (std::string const&, Grid const&, char const*);
//bool readFile (char const*, Grid&);
//
//vec3 interpolate(Grid const&, unsigned const&, unsigned const&, unsigned const&);
//void WRAPpolygonise (Grid*, float, unsigned, float*, unsigned[3], unsigned[3]);

#endif //MARCHING_CUBES_H
