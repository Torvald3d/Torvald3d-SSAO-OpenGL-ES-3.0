#version 300 es
precision highp float;

uniform sampler2D texDiff;
uniform sampler2D texNorm;

layout(location = 0) out vec3 colorBuff;
layout(location = 1) out vec3 normBuff;

in vec3 light;
in vec3 gNorm;
in float zPos;
in vec2 texCoord;

const vec3 ambientColor = vec3(0.3);
const float zFar = 40.0;

void main() {
	vec3 n = normalize(texture(texNorm, texCoord).xyz*2.0-1.0);
	vec3 l = normalize(light);
	vec3 c = texture(texDiff, texCoord).rgb;
	float a = clamp(dot(n, l), 0.0, 1.0);
	colorBuff = c*(a+ambientColor);

	normBuff = normalize(gNorm)*0.5+0.5;
	gl_FragDepth = zPos/zFar;
}
