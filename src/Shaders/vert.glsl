#version 450 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

layout (location=2) uniform mat4 pMat;
layout (location=3) uniform mat4 vMat;
layout (location=4) uniform mat4 mMat;

layout (location=5) uniform uint val;
layout (location=6) uniform uint maxVal;
layout (location=7) uniform vec3 scalarVec;
layout (location=8) uniform uint colorScheme;
layout (location=9) uniform vec3 camPos;

layout (location=11) uniform float yMax;
layout (location=12) uniform float yMin;

uniform vec3 lightIntensities = vec3(0.7);
uniform vec3 ambient = vec3(0.2);

out VS_OUT
{   vec4 color;
} vs_out;

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
    float k = 16*float(val)/float(maxVal);
    int kInt = int(floor(k));
    float p = length(position);
    if (colorScheme == 0)     
    {
//        vs_out.color = vec4(abs(cos(p)), abs(sin(p)), 0.0, 1.0);
        vs_out.color = vec4(0.5);
     //   vs_out.color = vec4(mix(pallete[kInt-1], pallete[kInt], k-kInt), 1.0);
    }

    gl_Position = pMat*vMat*mMat*vec4(scalarVec*position, 1.0);

    vec3 pos = mat3(vMat*mMat)*scalarVec*position; 
    vec3 l = normalize(mat3(vMat*mMat)*camPos-pos);
    float brightness = clamp(dot(mat3(vMat*mMat)*normal, l), 0.0, 1.0);
    vs_out.color = vec4((ambient + brightness*lightIntensities)*vs_out.color.rgb, vs_out.color.a);
}
