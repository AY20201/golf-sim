#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform sampler2D renderedBlurScene;
uniform sampler2D gPosition;

float minDistance = 1.0;
float maxDistance = 10.0;

void main()
{
	vec4 focusColor = texture(renderedScene, texCoord);
	vec4 blurColor = texture(renderedBlurScene, texCoord);

	vec3 position = texture(gPosition, texCoord).xyz;

	vec3 focusPoint = texture(gPosition, vec2(0.5, 0.5)).xyz;

	float blur = smoothstep(minDistance, maxDistance, abs(position.y - focusPoint.y));

	FragColor = mix(focusColor, blurColor, blur);
}