#version 300 es
precision mediump float;

uniform sampler2D ssaoBuff;
uniform sampler2D ssaoLowBuff;

layout(location = 0) out float outColor;

in vec2 texCoord;

const float blurSize = 2.5/1920.0;

void main() {
	if (texture(ssaoLowBuff, texCoord).r==1.0)
		discard;

	float sum = 0.0;
	sum += texture(ssaoBuff, vec2(texCoord.x, texCoord.y - 2.0*blurSize)).r * 0.0625;
	sum += texture(ssaoBuff, vec2(texCoord.x, texCoord.y -     blurSize)).r * 0.25;
	sum += texture(ssaoBuff, vec2(texCoord.x, texCoord.y               )).r * 0.375;
	sum += texture(ssaoBuff, vec2(texCoord.x, texCoord.y +     blurSize)).r * 0.25;
	sum += texture(ssaoBuff, vec2(texCoord.x, texCoord.y + 2.0*blurSize)).r * 0.0625;

	outColor = sum;
}
