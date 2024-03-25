#version 430 core
out vec4 FragColor;

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

in vec2 texCoord;
in vec3 normal;
in vec3 currentPos;
in vec3 tangent;

float ambientFactor = 1.25;
float specularStrength = 2.0;
float specPower = 128;

float normalMapStrength = 0.05;
vec4 normalMapPanning = vec4(0.1, -0.15, -0.15, 0.05);

vec3 surfaceColor = vec3(0, 0.38, 0.69);

#define MAX_DIR_LIGHTS 1
uniform int numDirLights;
uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];

uniform vec3 camPos;
uniform samplerCube skybox;

uniform sampler2D albedo; //this is normal map one
uniform float albedoScale;
uniform vec3 albedoColor;
uniform sampler2D normalMap; //this is normal map two
uniform float normalMapScale;
uniform float time;

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 dir = light.direction;
	vec3 color = light.color;
	float inten = light.intensity;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(-dir);
	float diffuse = max(dot(norm, lightDirection), 0.0);

	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specAmount = pow(max(dot(viewDir, reflectionDirection), 0.0), specPower);
	vec3 specular = specAmount * vec3(1, 1, 1);

	vec3 eyeDir = normalize(fragPos - camPos);
	vec3 environmentReflectDir = reflect(eyeDir, norm);
	vec3 skyboxSample = vec3(texture(skybox, environmentReflectDir));

	//return (diffuse + specular) * skyboxSample * inten * color;
	return skyboxSample + specular;
}

vec3 CalculateTanSpace(vec3 normalMapSample, vec3 norm)
{
	vec3 normalMapS = (normalMapSample * 2.0 - 1.0) * normalMapStrength;
	vec3 tan = normalize(tangent);
	tan = normalize(tan - dot(tan, norm) * norm);
	vec3 bitangent = cross(tan, norm);
	mat3 TBN = mat3(tan, bitangent, norm);
	vec3 nm = normalize(TBN * normalMapS);

	return nm;
}

void main()
{
	vec3 norm = normalize(normal);
	vec3 normalMapSample1 = texture(albedo, (texCoord + normalMapPanning.xy * (time / 135)) * 135).rgb;
	vec3 normalMapSample2 = texture(normalMap, (texCoord + normalMapPanning.zw * (time / 135)) * 135).rgb;
	
	vec3 viewDir = normalize(camPos - currentPos);

	norm = CalculateTanSpace(normalMapSample1, norm) + CalculateTanSpace(normalMapSample2, norm);
	
	vec3 lightResult = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < numDirLights; i++) {
		lightResult += CalculateDirLight(dirLights[i], norm, currentPos, viewDir);
	}
	
	vec3 eyeDir = normalize(currentPos - camPos);
	vec3 environmentReflectDir = reflect(eyeDir, norm);
	vec3 skyboxSample = vec3(texture(skybox, environmentReflectDir));

	FragColor = vec4(surfaceColor * lightResult, 1.0);
}