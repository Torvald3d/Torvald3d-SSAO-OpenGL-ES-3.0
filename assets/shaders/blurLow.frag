#version 300 es
precision mediump float;

uniform sampler2D ssaoLowBuff;
uniform float aspect;

layout(location = 0) out float outColor;

in vec2 texCoord;

const float blurSize = 0.01;

void main() {
	float sum = 0.0;
	sum += texture(ssaoLowBuff, vec2(texCoord.x - blurSize, texCoord.y - blurSize*aspect)).r;
	sum += texture(ssaoLowBuff, vec2(texCoord.x - blurSize, texCoord.y + blurSize*aspect)).r;
	sum += texture(ssaoLowBuff, vec2(texCoord.x,            texCoord.y                  )).r;
	sum += texture(ssaoLowBuff, vec2(texCoord.x + blurSize, texCoord.y - blurSize*aspect)).r;
	sum += texture(ssaoLowBuff, vec2(texCoord.x + blurSize, texCoord.y + blurSize*aspect)).r;

	outColor = step(254.0/255.0, sum/5.0);
}
