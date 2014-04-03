#include "model.h"

Texture::Texture() {
	data = NULL;
}

Texture::~Texture() {
	delete[] data;
}

int Texture::getDataSize() {
	return width*height*3;
}

Model::Model() {
	vertex = NULL;
	normals = NULL;
	tangents = NULL;
	texCoord = NULL;
	faces = NULL;
}

Model::~Model() {
	delete[] vertex;
	delete[] normals;
	delete[] tangents;
	delete[] texCoord;
	delete[] faces;
}

ModelData::ModelData() {
	data = NULL;
	models = NULL;
}

ModelData::~ModelData() {
	delete[] data;
	delete[] models;
}


Font::Font() {
	symbols = NULL;
}

Font::~Font() {
	delete[] symbols;
}
