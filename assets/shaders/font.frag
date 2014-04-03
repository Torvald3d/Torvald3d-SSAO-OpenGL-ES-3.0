#version 300 es
precision mediump float;

uniform sampler2D fontBuff;

layout(location = 0) out vec4 outColor;

in vec2 texCoord;

void main() {
	float a = texture(fontBuff, texCoord).r;
	float black = smoothstep(0.35, 0.6, a);
	float white = smoothstep(0.1, 0.35, a);
	float color = 1.0-black;
	outColor = vec4(color, color, color, white);
}
