#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform sampler2D transScreen;
uniform float threshold;

vec4 transitionColor = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{
    vec4 sceneColor = texture(renderedScene, texCoord);
    float transitionSample = texture(transScreen, texCoord).r;

    vec4 finalColor = smoothstep(0.1, 0.9, threshold) <= transitionSample ? sceneColor : transitionColor;

    FragColor = finalColor;
}