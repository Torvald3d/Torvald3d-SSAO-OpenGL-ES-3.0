#version 300 es
precision highp float;

const int samples = 5;
const float radius = 0.8;
const float power = 2.0;
const float zFar = 40.0;

uniform sampler2D normBuff;
uniform sampler2D depthBuff;
uniform sampler2D randMap;
uniform sampler2D ssaoMask;
uniform vec2 scr;
uniform vec3 rndTable[samples];
uniform mat4 proj;

layout(location = 0) out float outColor;

in vec2 texCoord;
in vec3 viewRay;

float getSample(in int i, in mat3 rotate, in vec3 pos, in vec3 rnd) {
	vec3 samplePos = rotate*rnd;
	samplePos = samplePos*radius+pos;

	vec4 shift = proj*vec4(samplePos, 1.0);
	shift.xy /= shift.w;
	shift.xy = shift.xy*0.5+0.5;

	float sampleDepth = texture(depthBuff, shift.xy).x*zFar;
	float distanceCheck = smoothstep(0.0, 1.0, radius/abs(pos.z-sampleDepth));
	return step(sampleDepth, samplePos.z)*distanceCheck;
}

void main() {
	float depth = texture(depthBuff, texCoord).x;
	if (depth==1.0)
		discard;

	depth *= zFar;
	vec3 pos = viewRay*depth;
	vec3 normal = texture(normBuff, texCoord).xyz*2.0-1.0;
	vec3 rvec = texture(randMap, texCoord*scr).xyz*2.0-1.0;
	vec3 tangent = normalize(rvec-normal*dot(rvec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 rotate = mat3(tangent, bitangent, normal);

	float acc = 0.0;
	/*for (int i=0; i<samples; i++) { //на Adreno циклы глючат >___<
		vec3 samplePos = rotate*rndTable[0];
		samplePos = samplePos*radius+pos;

		vec4 shift = proj*vec4(samplePos, 1.0);
		shift.xy /= shift.w;
		shift.xy = shift.xy*0.5+0.5;

		float sampleDepth = texture(depthBuff, shift.xy).x*zFar;
		float distanceCheck = smoothstep(0.0, 1.0, radius/abs(pos.z-sampleDepth));
		acc += step(sampleDepth, samplePos.z)*distanceCheck;
	}*/
	acc += getSample(0, rotate, pos, rndTable[0]);
	acc += getSample(1, rotate, pos, rndTable[1]);
	acc += getSample(2, rotate, pos, rndTable[2]);
	acc += getSample(3, rotate, pos, rndTable[3]);
	acc += getSample(4, rotate, pos, rndTable[4]);

	outColor = step(254.0/255.0, 1.0-(acc/float(samples)));
}
