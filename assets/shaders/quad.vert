#version 300 es

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 tCoord;

out vec2 texCoord;

void main() {
	gl_Position = vec4(vPos, 0.0, 1.0);
	texCoord = tCoord;
}
