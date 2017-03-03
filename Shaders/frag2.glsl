#version 450 core

out vec4 color;

uniform vec3 lightIntensities = vec3(0.8);
uniform vec3 ambient = vec3(0.8);

layout (location=4) uniform mat4 vMat;
layout (location=5) uniform mat4 mMat;
layout (location=11) uniform vec3 camPos;

in VS_OUT
{   vec4 color;
    vec3 position;
    vec3 normal;
} fs_in;

void main(void)
{
    vec3 l = normalize(mat3(vMat*mMat)*camPos-fs_in.position);
    float brightness = clamp(abs(dot(mat3(vMat*mMat)*fs_in.normal, l)), 0.0, 1.0);
    color = vec4((ambient + brightness * lightIntensities) * fs_in.color.rgb, fs_in.color.a);
}
