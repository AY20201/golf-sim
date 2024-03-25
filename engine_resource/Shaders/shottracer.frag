#version 440 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 currentPos;
in vec4 currentPosLightSpace;
in vec3 tangent;
in vec3 viewSpacePosition;

//uniform float duration;
//float maxDuration = 6.0;
//float maxOpaque = 0.4;
//float power = 4.0;

void main()
{
	//float percentComplete = pow(clamp(duration / maxDuration, 0.0, 1.0), power);
	//float transparency = mix(0.0, maxOpaque, percentComplete);
	FragColor = vec4(1.0, 0.0, 0.0, 0.3);
}