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

#include "marching_cubes.h"

class Graphics 
{
private:
    std::vector<Vertex> mVertices;
    std::vector<std::pair<iso_type_t, std::pair<size_t, size_t>>> mSurfaceIndexMap;
	GLuint mVbo, mVao;
    GLfloat mWidth, mHeight;
    GLuint mShaderProgram;
    glm::mat3 mPmat, mVmat, mMmat;
    GLFWwindow* mWindow;
    vec3 mCamPos, mCamDir;
    double mHoriAngle, mVertAngle;
    GLfloat mViewScalar;
    GLuint mColorScheme;
    static GLuint msColorSchemeCount;

    bool fMustUpdate;
    bool fDrawAxes;

    void setShaders (char const*, char const*);

public:
    glm::mat4 mModMat, mViewMat;

    Graphics (GLfloat const&, GLfloat const&, char const*, char const*, const char* title="Untitled Window");

    void setCam (vec3 const&, vec3 const&);
    void moveCam (vec3 const&);
    void addSurface (iso_type_t const& isolevel, std::vector<Vertex> const& surfaceVerts);
    void scale (GLfloat const&);

    inline GLFWwindow* const& getWindow () const {return mWindow;};

    void mustUpdate ();
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
    std::vector<vec3> mVertices;
    std::vector<unsigned> mIndices;
};

struct 
{
    GLfloat scalar=1.0f;
} GLFWAttrs;


#endif //GRAPHICS_H
