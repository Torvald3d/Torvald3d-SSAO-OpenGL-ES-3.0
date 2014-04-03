#ifndef READER_H
#define READER_H

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "model.h"

class Reader {
private:
	int currIndex;
	char *buf;
	int size;
	void swap(char &a, char &b);

public:
	Reader();
	~Reader();
	void readText(const char *path);
	void readModel(const char *path, ModelData *data);
	void readTexture(const char *path, Texture *tex);
	void readSymbols(const char *path, Font *font);

	static AAssetManager *mgr;
	char *getBuf();
	int getSize();
	bool getLine(char *ptr, int n);
};

#endif
