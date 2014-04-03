#version 300 es

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 tCoord;

uniform float aspect;

out vec2 texCoord;
out vec3 viewRay;

void main() {
	gl_Position = vec4(vPos, 0.0, 1.0);
	texCoord = tCoord;
	viewRay = vec3(-vPos.x*aspect*0.57735, -vPos.y*0.57735, 1.0);
}
