#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

#define PI 3.14159

out vec2 texCoord;
out vec3 normal;
out vec3 currentPos;
out vec3 tangent;

uniform mat4 view;
uniform mat4 projection;

uniform mat4 modelMatrix;

uniform float time;

vec3 GerstnerWave(vec4 wave, vec3 p, inout vec3 vTangent, inout vec3 binormal)
{
	float steepness = wave.z;
    float wavelength = wave.w;
    float k = 2 * PI / wavelength;
    float c = sqrt(9.81 / k);
    vec2 d = normalize(vec2(wave.x, wave.y));
    
    float f = k * (dot(d, p.xz) - c * time);
    float a = steepness / k;
    
    vTangent += vec3(-d.x * d.x * (steepness * sin(f)), d.x * (steepness * cos(f)), -d.x * d.y * (steepness * sin(f)));

    binormal += vec3(-d.x * d.y * (steepness * sin(f)), d.y * (steepness * cos(f)), -d.y * d.y * (steepness * sin(f)));

    return vec3(d.x * (a * cos(f)), a * sin(f), d.y * (a * cos(f)));
}

void main()
{
    vec3 vPos = aPos;
    vec3 vTangent = vec3(1.0, 0.0, 0.0);
    vec3 binormal = vec3(0.0, 0.0, 1.0);
    vPos += GerstnerWave(vec4(1.0, 0.75, 0.2, 15), aPos, vTangent, binormal);
    vPos += GerstnerWave(vec4(0.1, 0.5, 0.1, 10), aPos, vTangent, binormal);
    vPos += GerstnerWave(vec4(-0.5, 1.0, 0.1, 20), aPos, vTangent, binormal);
    normal = normalize(cross(binormal, vTangent));

	currentPos = vec3(modelMatrix * vec4(vPos, 1.0));
	gl_Position = projection * view * modelMatrix * vec4(vPos, 1.0);
	texCoord = aTex;
    tangent = aTangent;
}

/*
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec2 texCoord;
out vec3 normal;
out vec3 currentPos;
out vec4 currentPosLightSpace;
out vec3 tangent;
out vec3 viewSpacePosition;

uniform mat4 view;
uniform mat4 projection;

uniform mat4 modelMatrix;
uniform mat4 lightMatrix;

void main()
{
	currentPos = vec3(modelMatrix * vec4(aPos, 1.0f));
	currentPosLightSpace = lightMatrix * vec4(currentPos, 1.0);
	gl_Position = projection * view * modelMatrix * vec4(aPos, 1.0);
	texCoord = aTex;
	normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
	tangent = aTangent;
	viewSpacePosition = (view * vec4(currentPos, 1.0)).xyz;
}
*/