#version 430 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 currentPos;
in vec4 fullPos;
in vec3 tangent;

uniform samplerCube skybox;

uniform sampler2D colorSampler;
uniform vec3 camPos;

float transparency = 0.3;
vec3 tint = vec3(0.4, 0.4, 0.4);

void main()
{
	vec3 norm = normalize(normal);

	vec3 tan = normalize(tangent);
	tan = normalize(tan - dot(tan, norm) * norm);
	vec3 bitangent = cross(tan, norm);
	mat3 TBN = mat3(tan, bitangent, norm);
	norm = normalize(TBN * vec3(1.0, 1.0, 1.0));

	vec3 viewDir = normalize(camPos - currentPos);
	
	vec3 deviceCoord = (fullPos.xyz / fullPos.w) / 2.0 + 0.5;
	vec3 colorSample = texture(colorSampler, vec2(deviceCoord.x, deviceCoord.y)).rgb;

	float refractiveIndex = 1.2;
	vec3 eyeDir = normalize(currentPos - camPos);
	vec3 environmentReflectDir = refract(eyeDir, norm, 1.0 / refractiveIndex);
	vec3 skyboxSample = vec3(texture(skybox, environmentReflectDir));

	vec3 col = mix(colorSample, tint, transparency);

	FragColor = vec4(col * skyboxSample, 1.0);
}