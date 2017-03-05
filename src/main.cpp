#include "/home/jhazelden/Cpp/OpenGL/loadinshader.cpp"

#include <ctime>
#include <cstdlib>

#include "graphics.h"

void mouseButtonCallback (GLFWwindow*, int, int, int)
{

}

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

iso_uint_t stellaTransform (double const& x, double const& y, double const& z, unsigned const& iterMax, double const& capSqr, unsigned const& power)
{
    double x_{x}, y_{y}, z_{z};
    iso_uint_t p;
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
//        double r = sqrt(distSqr);
//        double phi = atan(y_/x_);
//        double theta = acos(z_/r);
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
        
        double distSqr{x_*x_+y_*y_+z_*z_};
        if (distSqr > capSqr)
            break;
    }
    return p;
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

int main () 
{
    g.setCam(glm::vec3(0,0,-10), glm::vec3(0,0,1));

    auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 100.0f); 
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glfwSetKeyCallback(g.getWindow(), keyCallback);
    glfwSetMouseButtonCallback(g.getWindow(), mouseButtonCallback);

    unsigned iterMax = 100;
    GLfloat capSqr = 64;
    size_t dim[3]{100,100,100};
    double size[3]{4.0,4.0,4.0};
    double origin[3]{-2.0,-2.0,-2.0};

//    std::vector<Vertex> tiling;

    grid_t gridData;
    resizeGridData(gridData, dim);
    Grid grid{gridData,{dim[0],dim[1],dim[2]},{size[0],size[1],size[2]},{origin[0],origin[1],origin[2]}};
    stellaCorris(grid, iterMax, capSqr, 2);
//    writeFile("pee pee", grid, "fl");
//
//    Grid grid;
//    readFile("fl", grid);
    for (unsigned i=0; i < 30; ++i)
    {
        std::vector<Vertex> surface;
        polygonise(grid, 10+i*3, surface);
        addNormals(surface);
        g.addSurface(10+i*3, surface);
    }
//    g.setVertices(tiling);
    g.mustUpdate();
    GLFWAttrs.scalar = 10;
    g.scale(GLFWAttrs.scalar);

    glUniform1ui(5, 30);
    glUniform1ui(6, iterMax);

    glfwSetInputMode(g.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g.getWindow(), GLFW_STICKY_KEYS, false);
    g.loop(0.1, 0.0);
}
