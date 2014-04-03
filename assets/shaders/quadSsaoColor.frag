#version 300 es
precision mediump float;

uniform sampler2D colorBuff;
uniform sampler2D ssaoBuff;

layout(location = 0) out vec3 outColor;

in vec2 texCoord;

void main() {
	outColor = texture(colorBuff, texCoord).rgb*texture(ssaoBuff, texCoord).r;
}
