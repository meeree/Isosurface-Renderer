#version 450 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

layout (location=2) uniform mat4 pMat;
layout (location=3) uniform mat4 vMat;
layout (location=4) uniform mat4 mMat;

layout (location=5) uniform float val;
layout (location=6) uniform float maxVal;
layout (location=7) uniform vec3 scalarVec;

out VS_OUT
{   vec3 position;
    vec3 normal;
} vs_out;

void main(void)
{
    gl_Position = pMat*vMat*mMat*vec4(scalarVec*position, 1.0);
    vs_out.position = scalarVec*position; 
    vs_out.normal = normal;
}
