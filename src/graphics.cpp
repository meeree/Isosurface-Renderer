#include "graphics.h"

#include <vector>
#include <fstream>
#include <iostream>

GLuint Graphics::loadInShader(char const *fname, GLenum const &shaderType) 
{
    std::vector<char> buffer;
    std::ifstream in;
    in.open(fname, std::ios::binary);

    if(in.is_open()) 
	{
        in.seekg(0, std::ios::end);
        size_t const &length = in.tellg();

        in.seekg(0, std::ios::beg);

        buffer.resize(length + 1);
        in.read(&buffer[0], length);
        in.close();
        buffer[length] = '\0';
    } 
	else 
	{
        std::cerr<<"Unable to open "<<fname<<std::endl;
        exit(-1);
    }

    GLchar const *src = &buffer[0];

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint test;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

    if(!test) 
	{
        std::cerr<<"Shader compilation failed with this message:"<<std::endl;
        std::vector<char> compilationLog(512);
        glGetShaderInfoLog(shader, compilationLog.size(), NULL, &compilationLog[0]);
        std::cerr<<&compilationLog[0]<<std::endl;
        glfwTerminate();
        exit(-1);
    }

    return shader;
}

GLuint Graphics::msColorSchemeCount = 2;

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
    : mWidth{width_}, mHeight{height_}, mColorScheme{0}, mViewScalar{1.0f}, mModMat{1.0f}, mViewMat{1.0f}, mHoriAngle{3.14}, mVertAngle{0.0}, fMustUpdate{false}, fDrawAxes{false}
{
    if(!glfwInit()) 
    {
        std::cerr<<"failed to initialize glfw"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    mWindow = glfwCreateWindow(width_, height_, title_, NULL, NULL);

    if(!mWindow) 
    {
        std::cerr<<"failed to initialize window"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    glfwMakeContextCurrent(mWindow);

    glewExperimental = GL_TRUE;
    if(glewInit() != 0) 
    {
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
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    update();
    scale(1.0f);
}

void Graphics::addSurface (iso_type_t const& isolevel, std::vector<Vertex> const& surfaceVerts)
{
    mSurfaceIndexMap.push_back({isolevel, {mVertices.size(), surfaceVerts.size()}});
    mVertices.insert(mVertices.end(), surfaceVerts.begin(), surfaceVerts.end());
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
}

void Graphics::setCam (vec3 const& camPos, vec3 const& camDir)
{
	mCamPos = camPos;
    mCamDir = glm::normalize(camDir);
//	mHoriAngle = acos(dot(vec3(0.0f,0.0f,1.0f), mCamDir));
//	mVertAngle = acos(dot(vec3(0.0f,1.0f,0.0f), mCamDir));
}

void Graphics::moveCam (vec3 const& inc)
{
    mCamPos += inc;
}

void Graphics::scale (GLfloat const& scalar)
{
    glUniform1f(7, scalar);
}

void Graphics::update ()
{
    glUniform1ui(8, mColorScheme);
    fMustUpdate = false;
}

//NOTE: THIS IS A TERRIBLE WAY OF DOING THINGS! THIS SHOULD BE WORKED ON!
void Graphics::toggleAxes (GLfloat const& sz)
{
    if (!fDrawAxes)
    {
        mVertices.push_back({{0,0,0},{1,0,0}});
        mVertices.push_back({{sz,0,0},{1,0,0}});
        mVertices.push_back({{0,0,0},{0,1,0}});
        mVertices.push_back({{0,sz,0},{0,1,0}});
        mVertices.push_back({{0,0,0},{0,0,1}});
        mVertices.push_back({{0,0,sz},{0,0,1}});
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
    }
    else
        mVertices.erase(mVertices.end()-6, mVertices.end());
    fDrawAxes = !fDrawAxes;
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
    mHoriAngle += 6*(glfwGetTime()-t)*(mWidth/2-xpos);
    mVertAngle += 6*(glfwGetTime()-t)*(mHeight/2-ypos);

    mCamDir = vec3(
        cos(mVertAngle) * sin(mHoriAngle),
        sin(mVertAngle),
        cos(mVertAngle) * cos(mHoriAngle)
    );
    vec3 right{
        sin(mHoriAngle - 3.14f/2.0f),
        0,
        cos(mHoriAngle - 3.14f/2.0f)
    };
    vec3 up = glm::cross(right, mCamDir);
    vec3 moveVec{0.0f};
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveVec -= 0.3f * mCamDir;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveVec += 0.3f * mCamDir;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
    {
        moveVec -= 0.3f * up;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
    {
        moveVec += 0.3f * up;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveVec -= 0.3f * right;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveVec += 0.3f * right;
    }
    if (length(moveVec) > FLT_EPSILON)
    {
        moveCam(moveVec);
    }
    mViewMat = glm::lookAt(mCamPos, mCamPos+mCamDir, up); 
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(mViewMat));
    glUniform3f(9, mCamPos.x, mCamPos.y, mCamPos.z);
    
    if (fDrawAxes)
    {
        glUniform1i(10, 1);
        glDrawArrays(GL_LINES, mVertices.size()-6, 6);
        glUniform1i(10, 0);
    }
    for (auto const& isoIndPair: mSurfaceIndexMap)
    {
        glUniform1f(5, isoIndPair.first);
        glDrawArrays(GL_TRIANGLES, isoIndPair.second.first, isoIndPair.second.second);
    }
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
