#include "/home/jhazelden/Cpp/OpenGL/loadinshader.cpp"

#include <algorithm>
#include <ctime>
#include <cstdlib>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 mPosition;
    GLuint mVal;
    glm::vec3 mNormal;

    Vertex () = default;
    Vertex (glm::vec3 const&, GLuint const&, glm::vec3 const& normal_={0.0f,0.0f,0.0f});
};

Vertex::Vertex (glm::vec3 const& position_, GLuint const& val_, glm::vec3 const& normal_)
    : mPosition{position_}, mVal{val_}, mNormal{normal_} {}

struct FieldElement 
{
	glm::vec3 mPosition;
	unsigned val;
};

glm::vec3 VertexInterp(double const&, glm::vec3 const&, glm::vec3 const&, double const&, double const&);

glm::vec3 vertlist[12];

static int edgeTable[256]={
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };
static int triTable[256][16] =
    {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};


int Polygonise(FieldElement const (&grid) [8], unsigned const& isolevel, std::vector<Vertex>& vertices, glm::vec3 const& initPos)
{
    if (isolevel <= 20)
    {
        return 0;
    }
	int i,ntriang;
	int cubeindex;

	/*
	   Determine the index into the edge table which
	   tells us which vertices are inside of the surface
	   */
	cubeindex = 0;
	if (grid[0].val < isolevel) cubeindex |= 1;
	if (grid[1].val < isolevel) cubeindex |= 2;
	if (grid[2].val < isolevel) cubeindex |= 4;
	if (grid[3].val < isolevel) cubeindex |= 8;
	if (grid[4].val < isolevel) cubeindex |= 16;
	if (grid[5].val < isolevel) cubeindex |= 32;
	if (grid[6].val < isolevel) cubeindex |= 64;
	if (grid[7].val < isolevel) cubeindex |= 128;

	/* Cube is entirely in/out of the surface */
	if (edgeTable[cubeindex] == 0)
		return 0;

	/* Find the vertices where the surface intersects the cube */
	if (edgeTable[cubeindex] & 1)
		vertlist[0] =
			VertexInterp(isolevel,grid[0].mPosition,grid[1].mPosition,grid[0].val,grid[1].val);
//			(grid[0].mPosition+grid[1].mPosition)/2;
	if (edgeTable[cubeindex] & 2)
		vertlist[1] =
			VertexInterp(isolevel,grid[1].mPosition,grid[2].mPosition,grid[1].val,grid[2].val);
//			(grid[1].mPosition+grid[2].mPosition)/2;
	if (edgeTable[cubeindex] & 4)
		vertlist[2] =
			VertexInterp(isolevel,grid[2].mPosition,grid[3].mPosition,grid[2].val,grid[3].val);
//			(grid[2].mPosition+grid[3].mPosition)/2;
	if (edgeTable[cubeindex] & 8)
		vertlist[3] =
			VertexInterp(isolevel,grid[3].mPosition,grid[0].mPosition,grid[3].val,grid[0].val);
//			(grid[3].mPosition+grid[0].mPosition)/2;
	if (edgeTable[cubeindex] & 16)
		vertlist[4] =
			VertexInterp(isolevel,grid[4].mPosition,grid[5].mPosition,grid[4].val,grid[5].val);
//			(grid[4].mPosition+grid[5].mPosition)/2;
	if (edgeTable[cubeindex] & 32)
		vertlist[5] =
			VertexInterp(isolevel,grid[5].mPosition,grid[6].mPosition,grid[5].val,grid[6].val);
//			(grid[5].mPosition+grid[6].mPosition)/2;
	if (edgeTable[cubeindex] & 64)
		vertlist[6] =
			VertexInterp(isolevel,grid[6].mPosition,grid[7].mPosition,grid[6].val,grid[7].val);
//			(grid[6].mPosition+grid[7].mPosition)/2;
	if (edgeTable[cubeindex] & 128)
		vertlist[7] =
			VertexInterp(isolevel,grid[7].mPosition,grid[4].mPosition,grid[7].val,grid[4].val);
//			(grid[7].mPosition+grid[4].mPosition)/2;
	if (edgeTable[cubeindex] & 256)
		vertlist[8] =
			VertexInterp(isolevel,grid[0].mPosition,grid[4].mPosition,grid[0].val,grid[4].val);
//			(grid[0].mPosition+grid[4].mPosition)/2;
	if (edgeTable[cubeindex] & 512)
		vertlist[9] =
			VertexInterp(isolevel,grid[1].mPosition,grid[5].mPosition,grid[1].val,grid[5].val);
//			(grid[1].mPosition+grid[5].mPosition)/2;
	if (edgeTable[cubeindex] & 1024)
		vertlist[10] =
			VertexInterp(isolevel,grid[2].mPosition,grid[6].mPosition,grid[2].val,grid[6].val);
//			(grid[2].mPosition+grid[6].mPosition)/2;
	if (edgeTable[cubeindex] & 2048)
		vertlist[11] =
			VertexInterp(isolevel,grid[3].mPosition,grid[7].mPosition,grid[3].val,grid[7].val);
//			(grid[3].mPosition+grid[7].mPosition)/2;

	/* Create the triangle */
	ntriang = 0;
	for (i = 0; triTable[cubeindex][i] != -1; i += 3) {
		vertices.push_back({
			vertlist[triTable[cubeindex][i]]+initPos, 
            isolevel-1});
		vertices.push_back({
			vertlist[triTable[cubeindex][i+1]]+initPos, 
            isolevel-1});
		vertices.push_back({
			vertlist[triTable[cubeindex][i+2]]+initPos, 
            isolevel-1});
		++ntriang;
	}

	return ntriang;
}

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
   */
glm::vec3 VertexInterp(double const& isolevel, glm::vec3 const& p1, glm::vec3 const& p2, double const& valp1, double const& valp2)
{
	double mu;
	glm::vec3 p;

	if (fabs(isolevel-valp1) < 0.00001)
		return(p1);
	if (fabs(isolevel-valp2) < 0.00001)
		return(p2);
	if (fabs(valp1-valp2) < 0.00001)
		return(p1);
	mu = (isolevel - valp1) / (valp2 - valp1);
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	return(p);
}

void keyCallback(GLFWwindow*, int, int, int, int);

class Graphics 
{
private:
    std::vector<Vertex> mVertices;
	GLuint mVbo, mVao;
    GLfloat mWidth, mHeight;
    GLuint mShaderProgram;
    glm::mat3 mPmat, mVmat, mMmat;
    GLFWwindow* mWindow;
    glm::vec3 mCamPos, mCamDir;
    double mHoriAngle, mVertAngle;
    GLfloat mMin, mMax;
    GLfloat mViewScalar;
    GLuint mColorScheme;
    static GLuint msColorSchemeCount;

    bool fMustUpdate;
    bool fMustSetMinMax;

    void setShaders (char const*, char const*);

public:
    glm::mat4 mModMat, mViewMat;

    Graphics (GLfloat const&, GLfloat const&, char const*, char const*, const char* title="Untitled Window");

    void setCam (glm::vec3 const&, glm::vec3 const&);
    void moveCam (glm::vec3 const&);
    void setVertices (std::vector<Vertex> const&);
    void setMinMax ();
    void scale (GLfloat const&);

    inline GLFWwindow* const& getWindow () const {return mWindow;};

    void mustUpdate ();
    void mustSetMinMax ();

    void update ();
    void render (double const&, double const&);
    void loop (double const&, double);

    void incColorScheme ();
};

GLuint Graphics::msColorSchemeCount = 1;

void Graphics::setShaders (char const* vertLoc, char const* fragLoc)
{
	mShaderProgram = glCreateProgram();

    auto vertShader = loadInShader(vertLoc, GL_VERTEX_SHADER);
    auto fragShader = loadInShader(fragLoc, GL_FRAGMENT_SHADER);

    glAttachShader(mShaderProgram, vertShader);
    glAttachShader(mShaderProgram, fragShader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    glLinkProgram(mShaderProgram);
}

Graphics::Graphics (GLfloat const& width_, GLfloat const& height_, char const* vertLoc_, char const* fragLoc_, const char* title_)
    : mWidth{width_}, mHeight{height_}, mColorScheme{0}, mViewScalar{1.0f}, mModMat{1.0f}, mViewMat{1.0f}, mHoriAngle{3.14}, mVertAngle{0.0}
{
    if(!glfwInit()) {
        std::cerr<<"failed to initialize glfw"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    mWindow = glfwCreateWindow(width_, height_, title_, NULL, NULL);

    if(!mWindow) {
        std::cerr<<"failed to initialize window"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    glfwMakeContextCurrent(mWindow);

    glewExperimental = GL_TRUE;
    if(glewInit() != 0) {
        std::cerr<<"failed to initialize glew"<<std::endl;
        exit(EXIT_SUCCESS);
    }

    setShaders(vertLoc_, fragLoc_);
    glUseProgram(mShaderProgram);

    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glBindVertexArray(mVao);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, mPosition)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, mNormal)));
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, mVal)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    mustSetMinMax();
    update();
    scale(1.0f);
}

void Graphics::setVertices (std::vector<Vertex> const& vertices)
{
    mVertices = vertices;
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
}

void Graphics::setMinMax ()
{
    mMax = 0.0f;
    mMin = 0.0f;
    for (auto const& v: mVertices)
    {
        auto len = glm::length(v.mPosition);
        if (len > mMax)
        {
            mMax = len;
        }
        else if (len < mMin)
        {
            mMin = len;
        }
    }
}

void Graphics::setCam (glm::vec3 const& camPos, glm::vec3 const& camDir)
{
    mCamPos = camPos;
    mCamDir = camDir;
}

void Graphics::moveCam (glm::vec3 const& inc)
{
    mCamPos += inc;
}

void Graphics::scale (GLfloat const& scalar)
{
    glUniform1f(8, scalar);
}

void Graphics::update ()
{
    if (fMustSetMinMax)
    {
        setMinMax ();
        glUniform1f(5, mMax);
        glUniform1f(6, mMin);
        fMustSetMinMax = false;
    }
    glUniform1ui(9, mColorScheme);
    fMustUpdate = false;
}

void Graphics::render (double const& t, double const&)
{
    if (fMustUpdate)
    {
        update();
    }
    GLfloat const color [4] {0.0f, 0.0f, 0.0f, 1.0f};
    glClearBufferfv(GL_COLOR, 0.0f, color);
    glClear(GL_DEPTH_BUFFER_BIT);

    double xpos, ypos;
    glfwGetCursorPos(mWindow, &xpos, &ypos);
    glfwSetCursorPos(mWindow, mWidth/2, mHeight/2);
    mHoriAngle += 3*(glfwGetTime()-t)*(xpos-mWidth/2);
    mVertAngle += 3*(glfwGetTime()-t)*(ypos-mHeight/2);

    mCamDir = normalize(glm::vec3(
        cos(mVertAngle) * sin(mHoriAngle),
        sin(mVertAngle),
        cos(mVertAngle) * cos(mHoriAngle)
    ));
    glm::vec3 right{
        sin(mHoriAngle - 3.14f/2.0f),
        0,
        cos(mHoriAngle - 3.14f/2.0f)
    };
    glm::vec3 up = glm::cross(right, mCamDir);
    glm::vec3 moveVec{0.0f};
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveVec -= 0.1f * mCamDir;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveVec += 0.1f * mCamDir;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
    {
        moveVec -= 0.1f * up;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
    {
        moveVec += 0.1f * up;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveVec -= 0.1f * right;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveVec += 0.1f * right;
    }
    if (length(moveVec) > 0.00001f)
    {
        moveCam(moveVec);
    }
    mViewMat = glm::lookAt(mCamPos, mCamPos+mCamDir, up); 
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(mViewMat));

    glUniform3f(11, mCamPos.x, mCamPos.y, mCamPos.z);
    
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    
    glfwSwapBuffers(mWindow);
}

void Graphics::loop (double const& dt, double t=0)
{
    while (true)
    {
        render(glfwGetTime(), dt);
        glfwPollEvents();
        t += dt;
    }
}

void Graphics::incColorScheme ()
{
    mColorScheme = (mColorScheme+1)%msColorSchemeCount;
    mustUpdate();
}

void Graphics::mustUpdate ()
{
    fMustUpdate = true;
}

void Graphics::mustSetMinMax ()
{
    fMustSetMinMax = true;
}

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/volumeRenderer/Shaders/vert.glsl", "/home/jhazelden/Cpp/OpenGL/volumeRenderer/Shaders/frag.glsl", "Volume renderer v0.0"};

struct 
{
    GLfloat scalar=1.0f;
} GLFWAttrs;

void keyCallback(GLFWwindow*, int key, int, int action, int)
{   
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        g.incColorScheme();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
	    glfwTerminate(); 
        exit(EXIT_SUCCESS);
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        GLFWAttrs.scalar *= 11.0f/10;
        g.scale(GLFWAttrs.scalar);
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        GLFWAttrs.scalar *= 10.0f/11;
        g.scale(GLFWAttrs.scalar);
    }
}

void mouseButtonCallback (GLFWwindow*, int, int, int)
{

}

void addNormals (std::vector<Vertex>& vertices)
{
    for (unsigned i = 0; i < vertices.size(); i += 3)
    {
        Vertex& v1{vertices[i]};
        Vertex& v2{vertices[i+1]};
        Vertex& v3{vertices[i+2]};
        glm::vec3 nm{glm::normalize(glm::cross(v2.mPosition-v1.mPosition, v3.mPosition-v1.mPosition))};
        v1.mNormal = nm;
        v2.mNormal = nm;
        v3.mNormal = nm;
    }
}


unsigned stellaTransform (unsigned const& iterMax, GLfloat const& x, GLfloat const& y, GLfloat const& z, GLfloat const& capSqr, unsigned const& power)
{
    GLfloat x_{x}, y_{y}, z_{z};
    unsigned p;

    for (p = 0; p < iterMax; ++p)
    {
        auto oldX = x_;
        if (power == 3)
        {
            auto oldY = y_;
            x_ = x_*x_*x_ - y_*y_*z_ - z_*z_*y_ - 5*x_*y_*z_;
            y_ = y_*y_*y_ - y_*y_*z_ + 3*(y_*y_*oldX + oldX*oldX*y_);
            z_ = z_*z_*z_ - z_*z_*oldY + 3*(z_*z_*oldX + oldX*oldX*z_);
        }
//        GLfloat r = sqrt(distSqr);
//        GLfloat phi = atan(y_/x_);
//        GLfloat theta = acos(z_/r);
//        x_ = pow(r,power)*sin(power*theta)*cos(power*phi);
//        y_ = pow(r,power)*sin(power*theta)*sin(power*phi);
//        z_ = pow(r,power)*cos(power*theta);
        else if (power == 2)
        {
            x_ = x_*x_-2*y_*z_;
            y_ = 2*oldX*y_+y_*y_;
            z_ = 2*oldX*z_+z_*z_;
        }

        x_ += x;
        y_ += y;
        z_ += z;
        
        GLfloat distSqr{x_*x_+y_*y_+z_*z_};
        if (distSqr > capSqr)
        {
            break;
        }
    }
    
    return p;
}

std::vector<Vertex> stellaCorris (unsigned const (&dim) [3], unsigned const& iterMax, GLfloat const& capSqr, GLfloat const (&start) [3], GLfloat const (&end) [3], glm::vec3 const& initPos={0.0f,0.0f,0.0f}, unsigned const& power=1) 
{
    unsigned const& l{dim[0]}, w{dim[1]}, h{dim[2]};

    std::vector<FieldElement> elements (l*w*h);
    auto t = glfwGetTime();
    for (int i = 0; i < (int)l; ++i)
    {
        for (int j = 0; j < (int)w; ++j)
        {
            for (int k = 0; k < (int)h; ++k)
            {
                GLfloat x{start[0]+((GLfloat)j/w)*(end[0]-start[0])};
                GLfloat y{start[1]+((GLfloat)i/l)*(end[1]-start[1])};
                GLfloat z{start[2]+((GLfloat)k/h)*(end[2]-start[2])};
                
                unsigned p{stellaTransform(iterMax, x, y, z, capSqr, power)};
                elements[i*w*h+j*h+k] = {{x, y, z}, p};
            }
        }
    }
    std::vector<Vertex> tiling;
    for (int i = 0; i < (int)l-1; ++i)
    {
        for (int j = 0; j < (int)w-1; ++j)
        {
            for (int k = 0; k < (int)h-1; ++k)
            {
                int pos {i*(int)w*(int)h+j*(int)h+k};
                FieldElement box [8] {elements[pos], elements[pos+h], elements[pos+w*h+h], elements[pos+w*h],
                                      elements[pos+1], elements[pos+h+1], elements[pos+w*h+h+1], elements[pos+w*h+1]};
                GLfloat x{start[0]+((GLfloat)(j+0.5f)/w)*(end[0]-start[0])};
                GLfloat y{start[1]+((GLfloat)(i+0.5f)/l)*(end[1]-start[1])};
                GLfloat z{start[2]+((GLfloat)(k+0.5f)/h)*(end[2]-start[2])};
                unsigned p{stellaTransform(iterMax, x, y, z, capSqr, power)};
                Polygonise(box, p, tiling, initPos);
            }
        }
    }
    return tiling;
}

std::vector<Vertex> getState (std::fstream& fl)
{
    std::vector<Vertex> vertices;
    int i = 0;
    try
    {
        while (!fl.eof() && i < 134210000/3)
        {
            glm::vec3 pos; 
            glm::vec3 nm;
            GLuint val;

            fl>>pos.x; fl.ignore(); 
            fl>>pos.y; fl.ignore(); 
            fl>>pos.z; fl.ignore();

            fl>>nm.x; fl.ignore(); 
            fl>>nm.y; fl.ignore(); 
            fl>>nm.z; fl.ignore();

            fl>>val; fl.ignore(); 

            vertices.push_back({pos,val,nm});
            ++i;
        }
    }
    catch (std::bad_alloc& ba)
    {
        std::cout<<i<<std::endl;
        glfwTerminate();
        exit(0);
    }
    return vertices;
}

void print (std::vector<Vertex> const& tiling)
{
    for (auto const& v: tiling)
    {
        std::cout<<v.mPosition.x<<","<<v.mPosition.y<<","<<v.mPosition.z<<"|"
                 <<v.mNormal.x<<","<<v.mNormal.y<<","<<v.mNormal.z<<"|"
                 <<v.mVal<<";"
                 <<std::endl;
    }
}

int main () 
{
    g.setCam(glm::vec3(0,0,-10), glm::vec3(0,0,1));

    auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 100.0f); 
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glfwSetKeyCallback(g.getWindow(), keyCallback);
    glfwSetMouseButtonCallback(g.getWindow(), mouseButtonCallback);

    unsigned dim [3] {601, 601, 601};

    unsigned iterMax = 100;
    GLfloat capSqr = 64;

    std::vector<Vertex> tiling;

//    for (int i = 0; i < 10-1; ++i)
//    {
//        for (int j = 0; j < 10-1; ++j)
//        {
//            for (int k = 0; k < 10-1; ++k)
//            {
//                GLfloat start [3] {-4.0f+8.0f*float(j)/4, -3.0f+6.0f*float(k)/4, -3.0f+6.0f*float(i)/4};
//                GLfloat end [3] {-4.0f+8.0f*float(j+1)/4, -3.0f+6.0f*float(k+1)/4, -3.0f+6.0f*float(i+1)/4};
//                auto newTiling = stellaCorris(dim, iterMax, capSqr, start, end, {0.0f, 0.0f, 0.0f}, 2);
////                addNormals(newTiling);
//                print(newTiling);
//            }
//        }
//    }
//
//    {
//        auto newTiling = stellaCorris(dim, iterMax, capSqr, {-9.0f, -9.0f, -9.0f}, {9.0f, 9.0f, 9.0f}, {0.0f, 0.0f, 0.0f}, 2);
//        tiling.insert(tiling.end(), newTiling.begin(), newTiling.end());
//        addNormals(tiling);
//        print(tiling);
//    }
//    {
//        std::fstream fl;
//        fl.open("/home/jhazelden/Cpp/OpenGL/volumeRenderer/fl2", std::ios::in);
//        tiling = getState(fl);
//        fl.close();
//        addNormals(tiling);
//    }
    tiling.push_back({{0.0f, 0.0f, 0.0f}, 0});
    tiling.push_back({{1.0f, 0.0f, 0.0f}, 0});
    tiling.push_back({{0.0f, 1.0f, 0.0f}, 0});
    g.setVertices(tiling);
    g.mustUpdate();
    g.mustSetMinMax();
    GLFWAttrs.scalar = 10;
    g.scale(GLFWAttrs.scalar);

    glUniform1ui(10, iterMax);

    glfwSetInputMode(g.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g.getWindow(), GLFW_STICKY_KEYS, false);
    g.loop(0.1, 0.0);
}
