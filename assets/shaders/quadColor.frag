#version 300 es
precision mediump float;

uniform sampler2D colorBuff;

layout(location = 0) out vec3 outColor;

in vec2 texCoord;

void main() {
	outColor = texture(colorBuff, texCoord).rgb;
}
