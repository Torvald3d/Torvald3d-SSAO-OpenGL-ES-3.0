#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "debug.h"
#include "reader.h"
#include "shader.h"
#include "matrix.h"
#include "model.h"

extern "C" {
	JNIEXPORT void JNICALL Java_com_torvald_ssaotest_MainActivity_readResources(JNIEnv* env, jobject obj, jobject assetManager);
	JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLRenderer_nativeInit(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLRenderer_nativeResize(JNIEnv* env, jobject obj, jint w, jint h);
	JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLRenderer_nativeRender(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLView_nativeTouchUp(JNIEnv* env, jobject obj, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLView_nativeTouchMove(JNIEnv* env, jobject obj, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLView_nativeTouchDown(JNIEnv* env, jobject obj, jfloat x, jfloat y);
}

Shader mainShader;
Shader ssaoShader;
int adrMatrixProj;
int adrMatrixView;
int adrCamPos;
int adrLightPos;
int diffuseAdr;
int normalAdr;
GLuint colorBuff;
GLuint normBuff;
GLuint depthBuff;
GLuint randMap;
int adrColorBuff;
int adrNormBuff;
int adrDepthBuff;
int adrRandMap;
int adrScreen;
int adrSsaoMask;
int adrRndTable;
int adrProj;
const int samples = 8;
const int samplesLow = 5;
vec3 rndTable[samples];
vec3 rndTableLow[samplesLow];
vec2 screen;
int randMapW = 4;
int randMapH = randMapW;
int adrAspect;
int adrLowDepthBuff;
int adrLowNormBuff;
int adrLowAspect;
int adrLowRandMap;
int adrLowScreen;
int adrLowRndTable;
int adrLowProj;

Shader quadSsaoColorShader;
Shader quadColorShader;
Shader quadSsaoShader;
Shader quadSsaoLowShader;
Shader blurShaderH;
Shader blurShaderW;
Shader blurLowShader;
Shader textShader;
GLuint ssaoFbo1;
GLuint ssaoFbo2;
GLuint ssaoBuff1;
GLuint ssaoBuff2;
int adrSsaoOnlyBuff;
int adrColorOnlyBuff;
int adrSsaoBuff;
int adrSsaoBuff1;
int adrSsaoBuff2;
int adrLowSsaoBuff1;
int adrLowSsaoBuff2;
int adrLowBlurSsaoBuff;
int adrLowBlurAspect;
int adrFontBuff;
float ssaoScaleW = 1.5;
float ssaoScaleH = ssaoScaleW;
GLuint ssaoLowFbo;
GLuint ssaoLowBuff;
float ssaoLowScaleW = 16;
float ssaoLowScaleH = ssaoLowScaleW;
GLuint blurLowFbo;
GLuint blurLowBuff;

GLuint fbo;
int width, height;
float aspect;
GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
GLfloat quadVert[8];
GLfloat quadTCoord[8];
GLushort quadInd[] = {0, 1, 2, 0, 2, 3};

float xTouchOld;
float yTouchOld;
float rotX, rotY, rotZ;
float posZ;
float posZOld;
bool rotateControll;
bool blurEnable;
int viewMode;
int modelNum;

Font font;
float fontScale = 1.0f/500.0f;
vec2 fpsPos = vec2(-0.95f, 0.75f);
vec2 *fpsVertex = NULL;
vec2 *fpsTCoord = NULL;
GLushort *fpsInd = NULL;
int fpsNumVertex = 0;
int fps = 0;
double fpsTime;
ModelData *currModel;
mat4 matrixView;
mat4 matrixProj;
mat4 iMatrixProj;
vec3 lightPos;
const int modelCount = 2;
Reader modelsBuff[modelCount];
ModelData models[modelCount];
// bridge: vert: 1900;  norm: 1900;  tang: 1900;  uv: 1900;  face: 822
// suzanne: vert: 4208;  norm: 4208;  tang: 4208;  uv: 4208;  face: 7569

int seed = 15;
vec2 threshold;

float random(float min, float max) {
	return (max-min)*rand()/RAND_MAX+min;
}

void updateMatrixView() {
	matrixView.identity();
	matrixView.translate(0, 0, posZ);
	matrixView.rotate(rotX, 1, 0, 0);
	matrixView.rotate(rotY, 0, 1, 0);
	matrixView.rotate(rotZ, 0, 0, 1);
}

void setFps(int fps) {
	delete[] fpsVertex;
	delete[] fpsTCoord;
	delete[] fpsInd;
	if (fps<10 && fps>=0)
		fpsNumVertex = 6;
	else if (fps>=10 && fps<100)
		fpsNumVertex = 12;
	else
		return;

	GLushort quadInd[] = {0, 2, 1, 0, 3, 2};
	vec2 scale = vec2(fontScale, fontScale*aspect);
	fpsVertex = new vec2[4*fpsNumVertex/6];
	fpsTCoord = new vec2[4*fpsNumVertex/6];
	fpsInd = new GLushort[fpsNumVertex];
	for (int i=0; i<fpsNumVertex; i++)
		fpsInd[i] = quadInd[i%6]+(i/6)*4;

	int id = fps%10;
	vec2 shift = vec2(font.symbols[0].vertex[3].v[0]*scale.v[0], 0);
	shift += fpsPos;
	for (int i=0; i<4; i++) {
		fpsVertex[i] = font.symbols[id].vertex[i]*scale+shift;
		fpsTCoord[i] = font.symbols[id].texCoord[i];
	}
	if (fps<10)
		return;
	id = fps/10;
	for (int i=4; i<8; i++) {
		fpsVertex[i] = font.symbols[id].vertex[i%4]*scale+fpsPos;
		fpsTCoord[i] = font.symbols[id].texCoord[i%4];
	}
}

JNIEXPORT void JNICALL Java_com_torvald_ssaotest_MainActivity_readResources(JNIEnv* env, jobject obj, jobject assetManager) {
	Reader::mgr = AAssetManager_fromJava(env, assetManager);
}

JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLView_nativeTouchUp(JNIEnv* env, jobject obj, jfloat x, jfloat y) {

}

JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLView_nativeTouchMove(JNIEnv* env, jobject obj, jfloat x, jfloat y) {
	vec2 pos = vec2(x, y);
	float dist = vec2::distance(threshold, pos);
	if (dist<15 && x>width*0.9)
		return;

	if (!rotateControll) {
		float dz = y-posZOld;
		posZ -= dz/20;
		if (posZ>-7)
			posZ = -7;
		if (posZ<-18)
			posZ = -18;
		posZOld = y;
	} else {
		float dx = x-xTouchOld;
		float dy = y-yTouchOld;
		rotY += dx/5;
		rotX += dy/5;
		yTouchOld = y;
		xTouchOld = x;
	}

	updateMatrixView();
}

JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLView_nativeTouchDown(JNIEnv* env, jobject obj, jfloat x, jfloat y) {
	threshold = vec2(x, y);

	if (x<width*0.1) {
		rotateControll = false;
		posZOld = y;
	} else {
		rotateControll = true;
		xTouchOld = x;
		yTouchOld = y;
	}

	if (x>width*0.9) {
		if (y<height*0.333) {
			viewMode++;
			if (viewMode>=4)
				viewMode = 0;
		} else if (y>=height*0.333 && y<height*0.666)
			blurEnable = !blurEnable;
		else {
			modelNum++;
			if (modelNum>=modelCount)
				modelNum = 0;
		}
	}
}

void activeTexture(int num, GLuint id, int adr) {
	glActiveTexture(GL_TEXTURE0+num);
	glBindTexture(GL_TEXTURE_2D, id);
	glUniform1i(adr, num);
}

void createTexture(GLuint &id, int inFormat, int w, int h, int format, int type, int filter, int wrap, void* pix=NULL) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, inFormat, w, h, 0, format, type, pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLRenderer_nativeInit(JNIEnv* env, jobject obj){
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	lightPos = vec3(-5, 5, 5);
	rotY = rotZ = 0;
	rotX = 30;
	posZ = -12;
	updateMatrixView();
	blurEnable = false;
	viewMode = 0;
	modelNum = 0;

	double t = getMs();
	Reader vertexShaderFile;
	Reader fragmentShaderFile;
	vertexShaderFile.readText("shaders/shader.vert");
	fragmentShaderFile.readText("shaders/shader.frag");
	if (!mainShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create mainShader");
	adrMatrixProj = glGetUniformLocation(mainShader.programId, "matrixProj");
	adrMatrixView = glGetUniformLocation(mainShader.programId, "matrixView");
	adrLightPos = glGetUniformLocation(mainShader.programId, "lightPos");
	diffuseAdr = glGetUniformLocation(mainShader.programId, "texDiff");
	normalAdr = glGetUniformLocation(mainShader.programId, "texNorm");

	vertexShaderFile.readText("shaders/ssao.vert");
	fragmentShaderFile.readText("shaders/ssao.frag");
	if (!ssaoShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create ssaoShader");
	adrNormBuff = glGetUniformLocation(ssaoShader.programId, "normBuff");
	adrDepthBuff = glGetUniformLocation(ssaoShader.programId, "depthBuff");
	adrRandMap = glGetUniformLocation(ssaoShader.programId, "randMap");
	adrScreen = glGetUniformLocation(ssaoShader.programId, "scr");
	adrAspect = glGetUniformLocation(ssaoShader.programId, "aspect");
	adrSsaoMask = glGetUniformLocation(ssaoShader.programId, "ssaoMask");
	adrRndTable = glGetUniformLocation(ssaoShader.programId, "rndTable");
	adrProj = glGetUniformLocation(ssaoShader.programId, "proj");

	fragmentShaderFile.readText("shaders/ssaoLow.frag");
	if (!quadSsaoLowShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create quadSsaoLowShader");
	adrLowDepthBuff = glGetUniformLocation(quadSsaoLowShader.programId, "depthBuff");
	adrLowNormBuff = glGetUniformLocation(quadSsaoLowShader.programId, "normBuff");
	adrLowAspect = glGetUniformLocation(quadSsaoLowShader.programId, "aspect");
	adrLowRandMap = glGetUniformLocation(quadSsaoLowShader.programId, "randMap");
	adrLowScreen = glGetUniformLocation(quadSsaoLowShader.programId, "scr");
	adrLowRndTable = glGetUniformLocation(quadSsaoLowShader.programId, "rndTable");
	adrLowProj = glGetUniformLocation(quadSsaoLowShader.programId, "proj");

	vertexShaderFile.readText("shaders/quad.vert");
	fragmentShaderFile.readText("shaders/blurH.frag");
	if (!blurShaderH.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create blurHShader");
	adrSsaoBuff1 = glGetUniformLocation(blurShaderH.programId, "ssaoBuff");
	adrLowSsaoBuff1 = glGetUniformLocation(blurShaderH.programId, "ssaoLowBuff");

	fragmentShaderFile.readText("shaders/blurW.frag");
	if (!blurShaderW.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create blurWShader");
	adrSsaoBuff2 = glGetUniformLocation(blurShaderW.programId, "ssaoBuff");
	adrLowSsaoBuff2 = glGetUniformLocation(blurShaderW.programId, "ssaoLowBuff");

	fragmentShaderFile.readText("shaders/blurLow.frag");
	if (!blurLowShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create blurLowShader");
	adrLowBlurAspect = glGetUniformLocation(blurLowShader.programId, "aspect");
	adrLowBlurSsaoBuff = glGetUniformLocation(blurLowShader.programId, "ssaoLowBuff");

	fragmentShaderFile.readText("shaders/quadSsaoColor.frag");
	if (!quadSsaoColorShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create quadSsaoColorShader");
	adrSsaoBuff = glGetUniformLocation(quadSsaoColorShader.programId, "ssaoBuff");
	adrColorBuff = glGetUniformLocation(quadSsaoColorShader.programId, "colorBuff");

	fragmentShaderFile.readText("shaders/quadSsao.frag");
	if (!quadSsaoShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create quadSsaoShader");
	adrSsaoOnlyBuff = glGetUniformLocation(quadSsaoShader.programId, "ssaoBuff");

	fragmentShaderFile.readText("shaders/quadColor.frag");
	if (!quadColorShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create quadColorShader");
	adrColorOnlyBuff = glGetUniformLocation(quadColorShader.programId, "colorBuff");

	fragmentShaderFile.readText("shaders/font.frag");
	if (!textShader.createProgram(vertexShaderFile.getBuf(), fragmentShaderFile.getBuf()))
		LOGE("Could not create textShader");
	adrFontBuff = glGetUniformLocation(textShader.programId, "fontBuff");

	double dt = getMs()-t;
	//LOGI("Load shaders: %f ms", dt);

	t = getMs();
	modelsBuff[0].readModel("model/bridge.model", &models[0]);
	Reader textureFile;
	Texture *tex = new Texture();
	textureFile.readTexture("textures/bridge/diffuse", tex);
	createTexture(models[0].diffuseId, GL_RGB8, tex->width, tex->height, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT, tex->data);
	delete tex;

	tex = new Texture();
	textureFile.readTexture("textures/bridge/normal", tex);
	createTexture(models[0].normalId, GL_RGB8, tex->width, tex->height, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT, tex->data);
	delete tex;

	modelsBuff[1].readModel("model/suzanne.model", &models[1]);
	tex = new Texture();
	textureFile.readTexture("textures/suzanne/diffuse", tex);
	createTexture(models[1].diffuseId, GL_RGB8, tex->width, tex->height, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT, tex->data);
	delete tex;

	tex = new Texture();
	textureFile.readTexture("textures/suzanne/normal", tex);
	createTexture(models[1].normalId, GL_RGB8, tex->width, tex->height, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT, tex->data);
	delete tex;

	Reader fontFile;
	fontFile.readSymbols("font/numbers", &font);

	glGenTextures(1, &font.textureId);
	glBindTexture(GL_TEXTURE_2D, font.textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	Texture fontTex;
	textureFile.readTexture("textures/numbers", &fontTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontTex.width, fontTex.height, 0, GL_RED, GL_UNSIGNED_BYTE, fontTex.data);

	dt = getMs()-t;
	//LOGI("Load models: %f ms", dt);

	t = getMs();
	srand(seed);
	for (int i=0; i<samples; i++) {
		rndTable[i] = vec3(random(-1, 1), random(-1, 1), random(-1, -0.1));
		rndTable[i].normalize();
		rndTable[i] *= (i+1.0f)/samples;
	}
	srand(seed);
	for (int i=0; i<samplesLow; i++) {
		float angle = DEG2RAD*360.0f*i/samplesLow;
		rndTableLow[i] = vec3(sinf(angle), cosf(angle), -0.1);
	}

	srand(seed+1000);
	unsigned char rnd[randMapW*randMapH*3];
	for (int i=0; i<randMapW*randMapH; i++) {
		vec3 v(random(-1, 1), random(-1, 1), 0);
		v.normalize();
		v *= 0.5;
		v += 0.5;
		rnd[i*3] = v.v[0]*255;
		rnd[i*3+1] = v.v[1]*255;
		rnd[i*3+2] = v.v[2]*255;
	}
	createTexture(randMap, GL_RGB8, randMapW, randMapH, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_REPEAT, rnd);

	dt = getMs()-t;
	//LOGI("Create rand texture: %f ms", dt);

	quadVert[0] = quadVert[2] = quadVert[3] = quadVert[5] = -1;
	quadVert[1] = quadVert[7] = quadVert[4] = quadVert[6] = 1;
	quadTCoord[0] = quadTCoord[2] = quadTCoord[3] = quadTCoord[5] = 0;
	quadTCoord[1] = quadTCoord[7] = quadTCoord[4] = quadTCoord[6] = 1;

	fps = 0;
	fpsTime = getMs();
}

JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLRenderer_nativeResize(JNIEnv* env, jobject obj, jint w, jint h){
	matrixProj.identity();
	width = w;
	height = h;
	aspect = (float)width/height;
	matrixProj.setFrustum(30, aspect, 0.1, 40);
	glViewport(0, 0, width, height);

	double t = getMs();
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	createTexture(colorBuff, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_MIRRORED_REPEAT);
	createTexture(normBuff, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_MIRRORED_REPEAT);
	createTexture(depthBuff, GL_DEPTH_COMPONENT24, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, GL_NEAREST, GL_MIRRORED_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuff, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normBuff, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuff, 0);
	int err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
		LOGE("Main framebuffer error: %i", err);
	glDrawBuffers(2, buffers);

	glGenFramebuffers(1, &ssaoFbo1);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFbo1);
	createTexture(ssaoBuff1, GL_R8, width/ssaoScaleW, height/ssaoScaleH, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, GL_MIRRORED_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBuff1, 0);
	err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
		LOGE("SSAO framebuffer 1 error: %i", err);

	glGenFramebuffers(1, &ssaoFbo2);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFbo2);
	createTexture(ssaoBuff2, GL_R8, width/ssaoScaleW, height/ssaoScaleH, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, GL_MIRRORED_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBuff2, 0);
	err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
		LOGE("SSAO framebuffer 2 error: %i", err);

	glGenFramebuffers(1, &ssaoLowFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoLowFbo);
	createTexture(ssaoLowBuff, GL_R8, width/ssaoLowScaleW, height/ssaoLowScaleH, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, GL_MIRRORED_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoLowBuff, 0);
	err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
		LOGE("ssaoLowFbo error: %i", err);

	glGenFramebuffers(1, &blurLowFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, blurLowFbo);
	createTexture(blurLowBuff, GL_R8, width/ssaoLowScaleW, height/ssaoLowScaleH, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, GL_MIRRORED_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurLowBuff, 0);
	err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
		LOGE("blurLowFbo error: %i", err);

	double dt = getMs()-t;
	//LOGI("Create buffers: %f ms", dt);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	setFps(fps);
}

JNIEXPORT void JNICALL Java_com_torvald_ssaotest_GLRenderer_nativeRender(JNIEnv* env, jobject obj){
	// model
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(mainShader.programId);
	glUniformMatrix4fv(adrMatrixProj, 1, GL_FALSE, matrixProj.m);
	glUniformMatrix4fv(adrMatrixView, 1, GL_FALSE, matrixView.m);
	glUniform3f(adrLightPos, lightPos.v[0], lightPos.v[1], lightPos.v[2]);
	activeTexture(0, models[modelNum].diffuseId, diffuseAdr);
	activeTexture(1, models[modelNum].normalId, normalAdr);

	for (int i=0; i<models[modelNum].modelsCount; i++) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, models[modelNum].models[i].vertex);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, models[modelNum].models[i].normals);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, models[modelNum].models[i].tangents);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, models[modelNum].models[i].texCoord);
		glDrawElements(GL_TRIANGLES, models[modelNum].models[i].facesNum*3, GL_UNSIGNED_SHORT, models[modelNum].models[i].faces);
	}

	glDisable(GL_DEPTH_TEST);

	if (viewMode<3) {
		// ssao low
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoLowFbo);
		glViewport(0, 0, width/ssaoLowScaleW, height/ssaoLowScaleH);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(quadSsaoLowShader.programId);
		glUniform2f(adrLowScreen, (float)(width/ssaoLowScaleW)/randMapW, (float)(height/ssaoLowScaleH)/randMapH);
		glUniform1f(adrLowAspect, aspect);
		glUniform3fv(adrLowRndTable, samplesLow, (float*)rndTableLow);
		glUniformMatrix4fv(adrLowProj, 1, GL_FALSE, matrixProj.m);
		activeTexture(0, depthBuff, adrLowDepthBuff);
		activeTexture(1, normBuff, adrLowNormBuff);
		activeTexture(2, randMap, adrLowRandMap);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, quadVert);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, quadTCoord);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadInd);

		// blur low
		glBindFramebuffer(GL_FRAMEBUFFER, blurLowFbo);
		glViewport(0, 0, width/ssaoLowScaleW, height/ssaoLowScaleH);
		glUseProgram(blurLowShader.programId);
		glUniform1f(adrLowBlurAspect, aspect);
		activeTexture(0, ssaoLowBuff, adrLowBlurSsaoBuff);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, quadVert);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, quadTCoord);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadInd);

		// ssao
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFbo1);
		glViewport(0, 0, width/ssaoScaleW, height/ssaoScaleH);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ssaoShader.programId);
		glUniform2f(adrScreen, (float)(width/ssaoScaleW)/randMapW, (float)(height/ssaoScaleH)/randMapH);
		glUniform1f(adrAspect, aspect);
		glUniform3fv(adrRndTable, samples, (float*)rndTable);
		glUniformMatrix4fv(adrProj, 1, GL_FALSE, matrixProj.m);
		activeTexture(0, normBuff, adrNormBuff);
		activeTexture(1, depthBuff, adrDepthBuff);
		activeTexture(2, randMap, adrRandMap);
		activeTexture(3, blurLowBuff, adrSsaoMask);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, quadVert);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, quadTCoord);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadInd);
	}

	if (blurEnable) {
		// blur H
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFbo2);
		glViewport(0, 0, width/ssaoScaleW, height/ssaoScaleH);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(blurShaderH.programId);
		activeTexture(0, ssaoBuff1, adrSsaoBuff1);
		activeTexture(1, blurLowBuff, adrLowSsaoBuff1);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, quadVert);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, quadTCoord);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadInd);

		// blur W
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFbo1);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, width/ssaoScaleW, height/ssaoScaleH);
		glUseProgram(blurShaderW.programId);
		activeTexture(0, ssaoBuff2, adrSsaoBuff2);
		activeTexture(1, blurLowBuff, adrLowSsaoBuff2);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, quadVert);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, quadTCoord);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadInd);
	}

	// result
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	switch (viewMode) {
		case 0:
			glUseProgram(quadSsaoShader.programId);
			activeTexture(0, ssaoBuff1, adrSsaoOnlyBuff);
			break;
		case 1:
			glUseProgram(quadSsaoShader.programId);
			activeTexture(0, blurLowBuff, adrSsaoOnlyBuff);
			break;
		case 2:
			glUseProgram(quadSsaoColorShader.programId);
			activeTexture(0, colorBuff, adrColorBuff);
			activeTexture(1, ssaoBuff1, adrSsaoBuff);
			break;
		case 3:
			glUseProgram(quadColorShader.programId);
			activeTexture(0, colorBuff, adrColorOnlyBuff);
			break;
	}

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, quadVert);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, quadTCoord);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadInd);

	// fps
	if (getMs()-fpsTime>=1000.0) {
		setFps(fps);
		fpsTime = getMs();
		fps = 0;
	}
	fps++;

	glUseProgram(textShader.programId);
	glEnable(GL_BLEND);
	activeTexture(0, font.textureId, adrFontBuff);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, fpsVertex);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, fpsTCoord);
	glDrawElements(GL_TRIANGLES, fpsNumVertex, GL_UNSIGNED_SHORT, fpsInd);
	glDisable(GL_BLEND);
}
