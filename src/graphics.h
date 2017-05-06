#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <map>

#include "polygoniser.h"

struct VboIndexer 
{
    GLint index;
    GLsizei size;
    bool fPureVertexDraw;
};

class Graphics 
{
private:
    std::vector<Vertex> mVertices, mLines;
    std::vector<unsigned> mIndices;
    std::map<float, VboIndexer> mSurfaceIndexMap;
    std::vector<VboIndexer> mLineIndexers;
	GLuint mVbo, mLineVbo, mEbo, mVao;
    GLfloat mWidth, mHeight;
    GLuint mShaderProgram;
    glm::mat3 mPmat, mVmat, mMmat;
    GLFWwindow* mWindow;
    glm::vec3 mCamPos, mCamDir;
    double mHoriAngle, mVertAngle;
    GLfloat mViewScalar;
    GLuint mColorScheme;
    static GLuint msColorSchemeCount;

    GLfloat mYMin, mYMax;
    GLfloat mMin, mMax;

    bool fMustUpdate;
    bool fDrawAxes;
    bool fMustSetMinMax;

    void setShaders (char const*, char const*);
public:
    glm::mat4 mModMat, mViewMat;

    Graphics (GLfloat const&, GLfloat const&, char const*, char const*, const char* title="Untitled Window");

    void setCam (glm::vec3 const&, glm::vec3 const&);
    void setMinMax ();
    void moveCam (glm::vec3 const&);
    void addSurface (float const&, std::vector<Vertex> const&, std::vector<unsigned> const&);
    void addSurface (float const&, std::vector<Vertex> const&);
    void scale (GLfloat const&);

    inline GLFWwindow* const& getWindow () const {return mWindow;};

    void mustUpdate ();
    void mustSetMinMax ();
    void toggleAxes (GLfloat const& sz=1.0f);

    void update ();
    void render (double const&, double const&);
    void loop (double const&, double);

    void incColorScheme ();
	static GLuint loadInShader (char const*, GLenum const&);
};

extern Graphics g;

struct Mesh
{
    std::vector<glm::vec3> mVertices;
    std::vector<unsigned> mIndices;
};

struct 
{
    GLfloat scalar=1.0f;
} GLFWAttrs;


#endif //GRAPHICS_H
