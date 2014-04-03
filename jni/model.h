#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "matrix.h"

class Texture {
public:
	Texture();
	~Texture();
	int getDataSize();

	unsigned short width;
	unsigned short height;
	char *data;
};

class Model {
public:
	Model();
	~Model();

	int vertexNum;
	int normalsNum;
	int tangentsNum;
	int texCoordNum;
	int facesNum;
	vec3 *vertex;
	vec3 *normals;
	vec3 *tangents;
	vec2 *texCoord;
	vec3us *faces;
};

class ModelData {
public:
	ModelData();
	~ModelData();

	unsigned int diffuseId;
	unsigned int normalId;
	char *data;
	int modelsCount;
	Model *models;
};

struct Symbol {
	vec2 vertex[4];
	vec2 texCoord[4];
};

struct Font {
	Font();
	~Font();

	int symbolsNum;
	unsigned int textureId;
	Symbol *symbols;
};

#endif // MODEL_H
