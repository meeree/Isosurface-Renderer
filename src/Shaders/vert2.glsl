#version 450 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

layout (location=2) uniform mat4 pMat;
layout (location=3) uniform mat4 vMat;
layout (location=4) uniform mat4 mMat;

layout (location=5) uniform float val;
layout (location=6) uniform float maxVal;
layout (location=7) uniform float scalar;

out VS_OUT
{   vec3 position;
    vec3 normal;
} vs_out;

void main(void)
{
    float percent = float(val)/float(maxVal);
    float k = 16*percent;
    int kInt = int(floor(k));
    float p = length(position);

    gl_Position = pMat*vMat*mMat*vec4(scalar*position, 1.0);
    vs_out.position = scalar*position; 
    vs_out.normal = normal;
}
