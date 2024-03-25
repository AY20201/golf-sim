#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D overlay;

void main()
{
	vec4 col = texture(overlay, texCoord);

	FragColor = col;
}