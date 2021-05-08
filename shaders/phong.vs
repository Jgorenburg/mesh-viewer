#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNor;

out vec3 Position;
out vec3 Normal;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 mvp;

void main() {   
	Normal = normalize(NormalMatrix * vNor);
	Position = vec3(ModelViewMatrix * vec4(vPos, 1.0));
	gl_Position = mvp * vec4(vPos, 1.0);
}