#include "shader.h"
#include "debug.h"

Shader::Shader(){
}

int Shader::loadShader(GLenum shaderType, const char* pSource) {
	int shader = glCreateShader(shaderType);
	if (shader) {
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		int compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			int infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = new char[infoLen];
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
					delete[] buf;
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

bool Shader::createProgram(const char* pVertexSource, const char* pFragmentSource) {
	int vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader)
		return 0;
	int pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader)
		return 0;
	programId = glCreateProgram();
	if (programId) {
		glAttachShader(programId, vertexShader);
		glAttachShader(programId, pixelShader);
		glLinkProgram(programId);
		int linkStatus = GL_FALSE;
		glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			int bufLength = 0;
			glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = new char[bufLength];
				if (buf) {
					glGetProgramInfoLog(programId, bufLength, NULL, buf);
					LOGE("Could not link program:\n%s\n", buf);
					delete[] buf;
				}
			}
			glDeleteProgram(programId);
			programId = 0;
		}
	}
	return programId;
}
