#version 300 es

uniform mat4 matrixProj;
uniform mat4 matrixView;
uniform vec3 lightPos;
uniform	vec3 eyePos;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 nPos;
layout(location = 2) in vec3 tPos;
layout(location = 3) in vec2 tCoord;

out vec3 light;
out vec3 gNorm;
out float zPos;
out vec2 texCoord;

void main() {
	vec4 p = matrixProj*matrixView*vec4(vPos, 1.0);
	gl_Position = p;
	texCoord = tCoord;

	vec3 bitangent = cross(tPos, nPos);
	mat3 tbn = mat3(tPos, bitangent, nPos);
	light = normalize(lightPos-vPos)*tbn;

	zPos = p.z;
	vec4 n = (matrixView*vec4(nPos, 0.0));
	gNorm = normalize(n.xyz);
}
