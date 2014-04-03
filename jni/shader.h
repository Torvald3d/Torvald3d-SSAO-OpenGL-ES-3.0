#ifndef SHADER_H
#define SHADER_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

class Shader{
public:
    Shader();
    int loadShader(GLenum shaderType, const char* pSource);
    bool createProgram(const char* pVertexSource, const char* pFragmentSource);
    int programId;
};

#endif // SHADER_H
