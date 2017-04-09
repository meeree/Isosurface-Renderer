#version 450 core

out vec4 color;

uniform vec3 lightIntensities = vec3(0.5);
uniform vec3 ambient = vec3(0.3);

layout (location=3) uniform mat4 vMat;
layout (location=4) uniform mat4 mMat;

layout (location=5) uniform float val;
layout (location=6) uniform float maxVal;
layout (location=7) uniform vec3 scalarVec;
layout (location=8) uniform uint colorScheme;
layout (location=9) uniform vec3 camPos;
layout (location=10) uniform bool lineDraw;

layout (location=11) uniform float yMax;
layout (location=12) uniform float yMin;

in VS_OUT
{   vec3 position;
    vec3 normal;
} fs_in;

uniform vec3 pallete [16] = vec3[](
	vec3(0.25882352941176473, 0.11764705882352941, 0.058823529411764705),
	vec3(0.09803921568627451, 0.027450980392156862, 0.10196078431372549),
	vec3(0.03529411764705882, 0.00392156862745098, 0.1843137254901961),
	vec3(0.01568627450980392, 0.01568627450980392, 0.28627450980392155),
	vec3(0.0, 0.027450980392156862, 0.39215686274509803),
	vec3(0.047058823529411764, 0.17254901960784313, 0.5411764705882353),
	vec3(0.09411764705882353, 0.3215686274509804, 0.6941176470588235),
	vec3(0.2235294117647059, 0.49019607843137253, 0.8196078431372549),
	vec3(0.5254901960784314, 0.7098039215686275, 0.8980392156862745),
	vec3(0.8274509803921568, 0.9254901960784314, 0.9725490196078431),
	vec3(0.9450980392156862, 0.9137254901960784, 0.7490196078431373),
	vec3(0.9725490196078431, 0.788235294117647, 0.37254901960784315),
	vec3(1.0, 0.6666666666666666, 0.0),
	vec3(0.8, 0.5019607843137255, 0.0),
	vec3(0.6, 0.3411764705882353, 0.0),
	vec3(0.41568627450980394, 0.20392156862745098, 0.011764705882352941));

void main(void)
{

    if (lineDraw)
    {
        color = vec4(fs_in.normal,1.0);
        return;
    }
    float p = length(fs_in.position);
    vec4 col;
    if (colorScheme == 0)
    {
        col = vec4(1.0);
    }
    else if (colorScheme == 1)
    {
        float k = (fs_in.position.y-scalarVec.y*yMin)/(scalarVec.y*yMax-scalarVec.y*yMin);
        col = fs_in.position.y-scalarVec.y*0.1 <= scalarVec.y*yMin ? vec4(0.2, 0.2, 1.0, 1.0) : mix(mix(vec4(0.5451, 0.5137, 0.4706, 1.0), vec4(0.5176, 0.3294, 0.1255, 1.0), 2*min(k, 0.5)), vec4(0.3804, 0.8118, 0.0941, 1.0), 2*(max(k, 0.5)-0.5));
//        col = fs_in.position.y-scalarVec.y*0.1 <= scalarVec.y*yMin ? vec4(0.2, 0.2, 1.0, 1.0) : mix(mix(vec4(0.4941, 0.0471, 0.5530, 1.0), vec4(0.9490, 0.3412, 0.6745, 1.0), 2*min(k, 0.5)), vec4(0.3804, 0.8118, 0.0941, 1.0), 2*(max(k, 0.5)-0.5));
    }
    vec3 l = normalize(mat3(vMat*mMat)*(camPos*fs_in.position));
    float brightness = clamp(abs(dot(mat3(vMat*mMat)*fs_in.normal, l)), 0.0, 1.0);
    color = vec4((ambient + brightness * lightIntensities) * col.rgb, col.a);
}
