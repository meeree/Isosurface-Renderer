#include "/home/jhazelden/Cpp/OpenGL/loadinshader.cpp"
#include <algorithm>
#include "graphics.h"

void constructNormals (std::vector<Vertex>& vertices)
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
        nm = glm::normalize(nm);
//        printf("%s,%s,%s\n",glm::to_string(vertices[i+1].mPosition).c_str(),glm::to_string(v2.mPosition).c_str(),glm::to_string(v3.mPosition).c_str());
//        std::cout<<glm::to_string(v2.mPosition)<<","<<glm::to_string(vertices[vertices.size()-2].mPosition)<<std::endl;
        
        v1.mNormal = nm;
        v2.mNormal = nm;
        v3.mNormal = nm;
    }
    vertices.erase(std::remove_if(vertices.begin(), vertices.end(), [](Vertex const& v){return std::isnan(v.mPosition.x);}), vertices.end()); 
}

float sampleHOST (glm::vec3 const& p)
{
    float density{
        -p.y
        +glm::perlin(glm::vec3(p.x,p.y,p.z)*6.96f)*0.15f
        +glm::simplex(glm::vec3(p.x,p.y,p.z)*9.91f)*0.10f
        +glm::perlin(glm::vec3(p.x,p.y,p.z)*0.91f)*3.00f};
    density -= glm::clamp((p.y-1.0)*3, 0.0, 1.0)*40;
    return density;
}

void gradientNormals (std::vector<Vertex>& vertices)
{
    for (auto& v: vertices)
    {
        auto& vert = v.mPosition;
        glm::vec3 grad;
        grad.x = sampleHOST(vert+glm::vec3(0.01f,0,0))-sampleHOST(vert+glm::vec3(-0.01f,0,0));
        grad.y = sampleHOST(vert+glm::vec3(0,0.01f,0))-sampleHOST(vert+glm::vec3(0,-0.01f,0));
        grad.z = sampleHOST(vert+glm::vec3(0,0,0.01f))-sampleHOST(vert+glm::vec3(0,0,-0.01f));
        v.mNormal = glm::normalize(grad);
    }
}

void mouseButtonCallback (GLFWwindow*, int, int, int)
{

}

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/volumeRendererCU/src/Shaders/vert2.glsl", "/home/jhazelden/Cpp/OpenGL/volumeRendererCU/src/Shaders/frag2.glsl", "Volume renderer v0.1"};

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

void translateSurface (std::vector<Vertex>& surfaceVerts, vec3 const& translation)
{
    std::for_each(surfaceVerts.begin(), surfaceVerts.end(), [&](Vertex& v){v.mPosition += translation;});
}

int main ()
{
    g.setCam(vec3(0,5,10), vec3(0,-0.5,-1));

    auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 200.0f); 
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

//    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glfwSetKeyCallback(g.getWindow(), keyCallback);
    glfwSetMouseButtonCallback(g.getWindow(), mouseButtonCallback);

    std::vector<Vertex> glVertices{};
    fillSpace(glVertices);
    constructNormals(glVertices);
//    gradientNormals(glVertices);
    g.addSurface(0, glVertices);

    g.mustUpdate();
    GLFWAttrs.scalar = glm::vec3(1.0f);
    g.scale(GLFWAttrs.scalar);

    glUniform1f(6, 1);

    glfwSetInputMode(g.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g.getWindow(), GLFW_STICKY_KEYS, false);

    g.toggleAxes(50);
    g.loop(0.1, 0.0);
}
