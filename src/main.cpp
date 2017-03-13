#include "/home/jhazelden/Cpp/OpenGL/loadinshader.cpp"

#include <ctime>
#include <cstdlib>
#include <algorithm>

#include "graphics.h"

void mouseButtonCallback (GLFWwindow*, int, int, int)
{

}

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/volumeRenderer/src/Shaders/vert.glsl", "/home/jhazelden/Cpp/OpenGL/volumeRenderer/src/Shaders/frag.glsl", "Volume renderer v0.1"};

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

void addNormalsAndClean (std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
{
    for (unsigned i = 0; i < indices.size(); i += 3)
    {
        Vertex& v1{vertices[indices[i]]};
        Vertex& v2{vertices[indices[i+1]]};
        Vertex& v3{vertices[indices[i+2]]};
        vec3 nm{glm::cross(v2.mPosition-v1.mPosition, v3.mPosition-v1.mPosition)};
        if (length(nm) <= DBL_EPSILON)
        {
            indices[i] = -1;
            indices[i+1] = -1;
            indices[i+2] = -1; 
            continue;
        }
        else 
            nm = glm::normalize(nm);

        v1.mNormal = nm;
        v2.mNormal = nm;
        v3.mNormal = nm;
    }
    std::cout<<indices.size()<<std::endl;
    indices.erase(std::remove(indices.begin(), indices.end(), -1), indices.end()); 
    std::cout<<indices.size()<<std::endl;
}

void addAveragedNormals (std::vector<Vertex>& vertices)
{
    for (unsigned i = 0; i < vertices.size(); i += 3)
    {
        Vertex& v1{vertices[i]};
        Vertex& v2{vertices[i+1]};
        Vertex& v3{vertices[i+2]};
        vec3 nm{glm::cross(v2.mPosition-v1.mPosition, v3.mPosition-v1.mPosition)};
        v1.mNormal = nm;
        v2.mNormal = nm;
        v3.mNormal = nm;
    }
    vec3 normals[vertices.size()];
    for (unsigned i = 0; i < vertices.size(); ++i)
    {
        auto vert = vertices[i];
        auto pos = vert.mPosition;
        vec3 sum{0.0f,0.0f,0.0f};
        unsigned count{0};
        for (unsigned j = 0; j < vertices.size(); ++j)
        {
            Vertex& v1{vertices[j]};
            if (length(v1.mPosition-pos) < 0.0001)
            {
                sum += v1.mNormal;
                ++count;
            }
        }
        normals[i] = glm::normalize(sum/count);
    }
    for (unsigned i = 0; i < vertices.size(); ++i)
    {
        vertices[i].mNormal = normals[i];
    }
}

iso_uint_t stellaTransform (double const& x, double const& y, double const& z, unsigned const& iterMax, double const& capSqr, unsigned const& power)
{
//    double x_{x}, y_{y}, z_{z};
//    iso_uint_t p;
//    for (p = 0; p < iterMax; ++p)
//    {
//        auto oldX = x_;
//        if (power == 3)
//        {
//            auto oldY = y_;
//            x_ = x_*x_*x_ - y_*y_*z_ - z_*z_*y_ - 5*x_*y_*z_;
//            y_ = y_*y_*y_ - y_*y_*z_ + 3*(y_*y_*oldX + oldX*oldX*y_);
//            z_ = z_*z_*z_ - z_*z_*oldY + 3*(z_*z_*oldX + oldX*oldX*z_);
//        }
//        else if (power == 2)
//        {
//            auto oldY = y_;
//            x_ = (x_*x_-y_*y_)*(1-z_*z_/(x_*x_+y_*y_));
//            y_ = 2*oldX*y_*(1-z_*z_/(oldX*oldX+y_*y_));
//            z_ = -2*z_*sqrt(oldX*oldX+oldY*oldY);
//            //x_ = x_*x_-2*y_*z_;
//            //y_ = 2*oldX*y_+y_*y_;
//            //z_ = 2*oldX*z_+z_*z_;
//        }
//
//        x_ += x;
//        y_ += y;
//        z_ += z;
//        
//        double distSqr{x_*x_+y_*y_+z_*z_};
//        if (distSqr > capSqr)
//            break;
//    }
//    return p;
    auto ellipsoid = 1.0-(pow(x/*-0.5*cos(sqrt(x*x+y*y))*/,2)/4+pow(y/*-0.5*sin(sqrt(x*x+y*y))*/,2));
    auto ellipsoidY = sqrt(1.0-z*z-x*x/4);
    return z*z<=ellipsoid
           || (y>=-ellipsoidY-0.5 && y<=-ellipsoidY-0.3 && 0.5>=x*x/4+z*z)
           || (x>=0.0 && x<=2+0.2 && 0.5/4>=z*z+y*y)

           || (y>=-1.05 && y<=0 && 0.001>=pow(x-sqrt(2.0)+sqrt(0.001)/2,2)+z*z)
           || (y>=-1.05 && y<=0 && 0.001>=pow(x+sqrt(2.0)-sqrt(0.001)/2,2)+z*z)
           || (y>=-1.05 && y<=0 && 0.001>=x*x+pow(z-sqrt(0.5)+sqrt(0.001),2))
           || (y>=-1.05 && y<=0 && 0.001>=x*x+pow(z+sqrt(0.5)-sqrt(0.001),2));
}

void stellaCorris (Grid& grid, unsigned const& iterMax, double const& capSqr, unsigned const& power)
{
    for (unsigned k = 0; k < grid.mDim[2]; ++k)
    {
        for (unsigned j = 0; j < grid.mDim[1]; ++j)
        {
            for (unsigned i = 0; i < grid.mDim[0]; ++i)
            {
                vec3 pos{interpolate(grid, i, j, k)};
                grid.mGrid[i][j][k] = stellaTransform(pos.x, pos.y, pos.z, iterMax, capSqr, power);
            }
        }
    }
}

void translateSurface (std::vector<Vertex>& surface, vec3 const& translation)
{
    for (auto& vert: surface)
    {
        vert.mPosition += translation;
    }
}

int main () 
{
    g.setCam(vec3(0,0,-10), vec3(0,0,1));

    auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 200.0f); 
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glfwSetKeyCallback(g.getWindow(), keyCallback);
    glfwSetMouseButtonCallback(g.getWindow(), mouseButtonCallback);

    unsigned iterMax = 15;
    GLfloat capSqr = 16;
    size_t dim[3]{300,300,300};
    double size[3]{5.0,4.0,4.0};
    double origin[3]{-2.0,-2.0,-2.0};

    grid_t gridData;
    resizeGridData(gridData, dim);
    Grid grid{gridData,{dim[0],dim[1],dim[2]},{size[0],size[1],size[2]},{origin[0],origin[1],origin[2]}};
    stellaCorris(grid, iterMax, capSqr, 2);
//    writeFile("", grid, "fl");

//    Grid grid;
//    readFile("fl", grid);

    std::vector<Vertex> surfaceVerts[2];
    std::vector<unsigned> surfaceInds[2];
    polygonise(grid, 1, surfaceVerts[0], surfaceInds[0]);
    addNormalsAndClean(surfaceVerts[0], surfaceInds[0]);
    g.addSurface(1, surfaceVerts[0], surfaceInds[0]);

//    polygonise(grid, 3, surfaces[1]);
//    constructNormals(surfaces[1]);
//    g.addSurface(3, surfaces[1]);
//    std::vector<Vertex> surface2 = surface;
//    translateSurface(surface2, {0,5,0});
//    addAveragedNormals(surface2);
//    g.addSurface(1, surface2);

    g.mustUpdate();
    GLFWAttrs.scalar = 10;
    g.scale(GLFWAttrs.scalar);

    glUniform1ui(6, iterMax);

    glfwSetInputMode(g.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g.getWindow(), GLFW_STICKY_KEYS, false);
    g.loop(0.1, 0.0);
}
