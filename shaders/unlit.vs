#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNor;

out vec3 color;
uniform mat4 mvp;

void main()
{
   color = 0.5 * (vNor + vec3(1.0));
   gl_Position = mvp * vec4(vPos, 1.0);
}
