#version 330 core

layout ( location = 0 ) in vec3 vertex_position;
layout ( location = 4 ) in vec4 position;

uniform mat3 M_v;
uniform mat3 M_p;
uniform float particleSize;

out float lifetime;

void main()
{
   vec3 position_viewspace = position.xyz;
   position_viewspace.xy += particleSize * (vertex_position.xy - vec2(0.5));
   gl_Position = vec4(position_viewspace, 1.0);
   lifetime = position.w;
};