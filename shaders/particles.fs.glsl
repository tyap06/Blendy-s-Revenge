#version 330 core

uniform vec3 startColor;
uniform vec3 endColor;

in float lifetime;

out vec4 fragColor;

void main()
{
   fragColor = vec4(startColor, 1.0);
}