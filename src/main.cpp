#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include "graphics.h"
#include "density_sampler.h"

#include <fstream>
#include <iostream>
GLuint loadInShader(char const *fname, GLenum const &shaderType) { /* Called by shaders function */
    std::vector<char> buffer;
    std::ifstream in;
    in.open(fname, std::ios::binary);

    if(in.is_open()) {
        in.seekg(0, std::ios::end);
        size_t const &length = in.tellg();

        in.seekg(0, std::ios::beg);

        buffer.resize(length + 1);
        in.read(&buffer[0], length);
        in.close();
        buffer[length] = '\0';
    } else {
        std::cerr<<"Unable to open "<<fname<<std::endl;
        exit(-1);
    }

    GLchar const *src = &buffer[0];

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint test;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

    if(!test) {
        std::cerr<<"Shader compilation failed with this message:"<<std::endl;
        std::vector<char> compilationLog(512);
        glGetShaderInfoLog(shader, compilationLog.size(), NULL, &compilationLog[0]);
        std::cerr<<&compilationLog[0]<<std::endl;
        glfwTerminate();
        exit(-1);
    }

    return shader;
}

void mouseButtonCallback (GLFWwindow*, int, int, int)
{

}

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/volumeRenderer/src/Shaders/vert2.glsl", "/home/jhazelden/Cpp/OpenGL/volumeRenderer/src/Shaders/frag2.glsl", "Volume renderer v0.1"};

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
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g.toggleAxes();
    }
}

void getNormalsAndClean (std::vector<Vertex>& vertices, std::vector<unsigned>& indices, std::vector<glm::vec3>& faceNormals, bool const& normalize=true)
{
    for (unsigned i = 0; i < indices.size(); i += 3)
    {
        Vertex& v1{vertices[indices[i]]};
        Vertex& v2{vertices[indices[i+1]]};
        Vertex& v3{vertices[indices[i+2]]};
        glm::vec3 nm{glm::cross(v2.mPosition-v1.mPosition, v3.mPosition-v1.mPosition)};
        if (length(nm) <= 0.000001)
        {
            indices[i] = -1;
            indices[i+1] = -1;
            indices[i+2] = -1; 
            faceNormals[i/3].x = NAN;
            continue;
        }
        faceNormals[i/3] = normalize ? glm::normalize(nm) : nm;
    }
    
    indices.erase(std::remove(indices.begin(), indices.end(), -1), indices.end()); 
    faceNormals.erase(std::remove_if(faceNormals.begin(), faceNormals.end(), [](glm::vec3 const& v){return std::isnan(v.x);}), faceNormals.end()); 
}

void simpleVertexNormals (std::vector<Vertex>& vertices, bool const& normalize=true)
{
    for (unsigned i = 0; i < vertices.size(); i += 3)
    {
        Vertex& v1{vertices[i]};    
        Vertex& v2{vertices[i+1]};    
        Vertex& v3{vertices[i+2]};    

        glm::vec3 nm{glm::cross(v2.mPosition-v1.mPosition, v3.mPosition-v1.mPosition)};
        if (length(nm) < 0.00000001)
        {
            v1.mNormal.x = nanf("");
            v2.mNormal.x = nanf("");
            v3.mNormal.x = nanf("");
            continue;
        }
        if (normalize)
            nm = glm::normalize(nm);
        
        v1.mNormal = nm;
        v2.mNormal = nm;
        v3.mNormal = nm;
    }
    vertices.erase(std::remove_if(vertices.begin(), vertices.end(), [](Vertex const& v){return std::isnan(v.mPosition.x);}), vertices.end()); 
}

enum ePhongCalc
{
    PHONG_AVERAGE, 
    PHONG_AREA,
    PHONG_INV_AREA
};
void addPhongNormals (std::vector<Vertex>& vertices, std::vector<unsigned>& indices, std::vector<glm::vec3> const& faceNormals, ePhongCalc const& calc, bool const& normalize=true)
{
    for (auto& v: vertices)
    {
        v.mNormal = {0.0f,0.0f,0.0f};        
    }
    for (unsigned i = 0; i < indices.size(); i += 3)
    {
        for (uint8_t coord = 0; coord < 3; ++coord)
        {
            Vertex& v{vertices[indices[i+coord]]};
            switch (calc)
            {
                case PHONG_AVERAGE: v.mNormal += faceNormals[i/3]/length(faceNormals[i/3]); break;
                case PHONG_AREA: v.mNormal += faceNormals[i/3]; break;
                case PHONG_INV_AREA: v.mNormal += faceNormals[i/3]/pow(length(faceNormals[i/3]),2); break;
                default: std::cerr<<"Error: phong calculation undefined. Must be one of PHONG_AREA, PHONG_AREA, PHONG_INV_AREA."<<std::endl; return;
            }
        }
    }
    if (normalize)
    {
        for (auto& v: vertices)
        {
            v.mNormal = glm::normalize(v.mNormal);        
        }
    }
}

float stellaTransform (double const& x, double const& y, double const& z, unsigned const& iterMax, double const& capSqr, unsigned const& power)
{
    double x_{x}, y_{y}, z_{z};
    float p;
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
        else if (power == 2)
        {
            auto oldY = y_;
            x_ = (x_*x_-y_*y_)*(1-z_*z_/(x_*x_+y_*y_));
            y_ = 2*oldX*y_*(1-z_*z_/(oldX*oldX+y_*y_));
            z_ = -2*z_*sqrt(oldX*oldX+oldY*oldY);
            //x_ = x_*x_-2*y_*z_;
            //y_ = 2*oldX*y_+y_*y_;
            //z_ = 2*oldX*z_+z_*z_;
        }

        x_ += x;
        y_ += y;
        z_ += z;
        
        double distSqr{x_*x_+y_*y_+z_*z_};
        if (distSqr > capSqr)
            break;
    }
    return p;
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
//
//void stellaCorris (Grid& grid, unsigned const& iterMax, double const& capSqr, unsigned const& power)
//{
//    for (unsigned k = 0; k < grid.mDim[2]; ++k)
//    {
//        for (unsigned j = 0; j < grid.mDim[1]; ++j)
//        {
//            for (unsigned i = 0; i < grid.mDim[0]; ++i)
//            {
//                glm::vec3 pos{interpolate(grid, i, j, k)};
//                grid.mGrid[i][j][k] = stellaTransform(pos.x, pos.y, pos.z, iterMax, capSqr, power);
//            }
//        }
//    }
//}

void translateSurface (std::vector<Vertex>& surfaceVerts, glm::vec3 const& translation)
{
    std::for_each(surfaceVerts.begin(), surfaceVerts.end(), [&](Vertex& v){v.mPosition += translation;});
}
//
//template <typename Lambda>
//void constructGrid (Grid& grid, Lambda&& f)
//{
//    for (unsigned k = 0; k < grid.mDim[2]; ++k)
//    {
//        for (unsigned j = 0; j < grid.mDim[1]; ++j)
//        {
//            for (unsigned i = 0; i < grid.mDim[0]; ++i)
//            {
//                glm::vec3 pos{interpolate(grid, i, j, k)};
//                grid.mGrid[i][j][k] = f(pos);
//            }
//
//        }
//    }
//}

std::function<GLfloat(glm::vec3 const&)> nTorus (unsigned const& n, GLfloat const& r)
{
    return {[&] (glm::vec3 const& pos) 
    {
        GLfloat g = pos.x*(pos.x-n);
        for (unsigned i = 1; i < n; ++i)
        {
            g *= (pos.x-i)*(pos.x-i);
        }
        g += pos.y*pos.y;
        return g*g + pos.z*pos.z - r*r;
    }};
}

int main () 
{
    g.setCam(glm::vec3(0,5,10), glm::vec3(0,-0.5,-1));

    auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 200.0f); 
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glfwSetKeyCallback(g.getWindow(), keyCallback);
    glfwSetMouseButtonCallback(g.getWindow(), mouseButtonCallback);

    {
        size_t dim[3]{90, 90, 90};
        float size[3]{2.2, 2.2, 2.2};
        float origin[3]{-1.1, -1.1, -1.1};

        Grid grid{{dim[0],dim[1],dim[2]},{size[0],size[1],size[2]},{origin[0],origin[1],origin[2]}};
        std::vector<Vertex> surfaceVerts;

        DensitySampler sampler;

        MarchingCubes marcher;
        marcher.polygonise(grid, 0, surfaceVerts, sampler);
        simpleVertexNormals(surfaceVerts);
//        translateSurface(surfaceVerts, glm::vec3(0,-1.5,0));
        g.addSurface(1, surfaceVerts);

//        DualContour countourer;
//        surfaceVerts = {};
//        countourer.polygonise(grid, 0, surfaceVerts, sampler);
//        simpleVertexNormals(surfaceVerts);
////        translateSurface(surfaceVerts, glm::vec3(0,2.5,0));
//        g.addSurface(0, surfaceVerts);
    }

    g.mustUpdate();
    GLFWAttrs.scalar = 10;
    g.scale(GLFWAttrs.scalar);

    glUniform1f(6, 1);

    glfwSetInputMode(g.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g.getWindow(), GLFW_STICKY_KEYS, false);

    g.toggleAxes(50);
    g.loop(0.1, 0.0);
}
