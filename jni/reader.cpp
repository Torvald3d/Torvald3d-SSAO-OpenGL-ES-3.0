#include "reader.h"
#include "debug.h"

AAssetManager *Reader::mgr = NULL;

Reader::Reader() {
	buf = NULL;
	size = 0;
	currIndex = 0;
}

Reader::~Reader() {
	delete[] buf;
}

void Reader::swap(char &a, char &b) {
	char tmp = a;
	a = b;
	b = tmp;
}

void Reader::readText(const char *path) {
	delete[] buf;
	size = 0;
	currIndex = 0;
	if (mgr==NULL) {
		LOGE("AAssetManager is NULL");
		return;
	}

	AAsset *file = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
	if (file==NULL) {
		LOGE("Failed to read %s", path);
		return;
	}

	size = AAsset_getLength(file)+1;
	buf = new char[size];
	AAsset_read(file, buf, size-1);
	buf[size-1] = '\0';
	AAsset_close(file);
}

char *Reader::getBuf() {
	return buf;
}

int Reader::getSize() {
	return size;
}

bool Reader::getLine(char *ptr, int n) {
	if (currIndex>=size-1 || n<1)
		return false;

	for (int i=0; i<n; i++, currIndex++) {
		ptr[i] = buf[currIndex];
		if (ptr[i]=='\n' || currIndex==size-1) {
			ptr[i] = '\0';
			break;
		}
	}
	ptr[n-1] = '\0';
	while (currIndex<size) {
		if (buf[currIndex]=='\n') {
			currIndex++;
			break;
		}
		currIndex++;
	}
	return true;
}

void Reader::readModel(const char *path, ModelData *data) {
	if (mgr==NULL) {
		LOGE("AAssetManager is NULL");
		return;
	}

	AAsset *file = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
	if (file==NULL) {
		LOGE("Failed to read %s", path);
		return;
	}

	size = AAsset_getLength(file);
	data->data = new char[size];
	AAsset_read(file, data->data, size);
	AAsset_close(file);
	data->modelsCount = ((int*)data->data)[0];
	data->models = new Model[data->modelsCount];

	int param = 10;
	int shift;
	for (int i=0; i<data->modelsCount; i++) {
		data->models[i].vertexNum = ((int*)data->data)[1+i*param];
		shift = ((int*)data->data)[2+i*param];
		data->models[i].vertex = (vec3*)&data->data[shift];

		data->models[i].normalsNum = ((int*)data->data)[3+i*param];
		shift = ((int*)data->data)[4+i*param];
		data->models[i].normals = (vec3*)&data->data[shift];

		data->models[i].tangentsNum = ((int*)data->data)[5+i*param];
		shift = ((int*)data->data)[6+i*param];
		data->models[i].tangents = (vec3*)&data->data[shift];

		data->models[i].texCoordNum = ((int*)data->data)[7+i*param];
		shift = ((int*)data->data)[8+i*param];
		data->models[i].texCoord = (vec2*)&data->data[shift];

		data->models[i].facesNum = ((int*)data->data)[9+i*param];
		shift = ((int*)data->data)[10+i*param];
		data->models[i].faces = (vec3us*)&data->data[shift];
	}
}

void Reader::readTexture(const char *path, Texture *tex) {
	if (mgr==NULL) {
		LOGE("AAssetManager is NULL");
		tex = NULL;
	}

	AAsset *file = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
	if (file==NULL) {
		LOGE("Failed to read %s", path);
		tex = NULL;
	}

	size = AAsset_getLength(file);
	buf = new char[size];
	AAsset_read(file, buf, size);
	AAsset_close(file);
	tex->width = ((unsigned short*)buf)[0];
	tex->height = ((unsigned short*)buf)[1];
	tex->data = &buf[2*2];
}

void Reader::readSymbols(const char *path, Font *font) {
	if (mgr==NULL) {
		LOGE("AAssetManager is NULL");
		return;
	}

	AAsset *file = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
	if (file==NULL) {
		LOGE("Failed to read %s", path);
		return;
	}

	size = AAsset_getLength(file);
	buf = new char[size];
	AAsset_read(file, buf, size);
	AAsset_close(file);
	font->symbolsNum = ((int*)buf)[0];
	font->symbols = (Symbol*)&buf[4];
	buf = NULL;
}
