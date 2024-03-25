#version 430 core

in vec2 texCoord;

uniform sampler2D albedo;

void main()
{
	if(texture(albedo, texCoord).a == 0.0)
	{
		discard;
	}
	gl_FragDepth = gl_FragCoord.z;
}