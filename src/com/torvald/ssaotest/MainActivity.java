package com.torvald.ssaotest;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.content.res.AssetManager;
import android.content.Context;
import android.view.MotionEvent;

public class MainActivity extends Activity {
	private GLSurfaceView glView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		readResources(getResources().getAssets());
		glView = new GLView(this);
		setContentView(glView);
	}

	@Override
	protected void onPause() {
		super.onPause();
		glView.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		glView.onResume();
	}

	static {
		System.loadLibrary("main");
	}

	native public void readResources(AssetManager mng);
}

class GLView extends GLSurfaceView {
	public GLView(Context context) {
		super(context);
		setEGLContextClientVersion(3);
		renderer = new GLRenderer();
		setRenderer(renderer);
	}

	public boolean onTouchEvent(final MotionEvent event) {
		float x = event.getX();
		float y = event.getY();
		switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				nativeTouchDown(x, y);
				break;
			case MotionEvent.ACTION_MOVE:
				nativeTouchMove(x, y);
				break;
			case MotionEvent.ACTION_UP:
				nativeTouchUp(x, y);
				break;
		}
		return true;
	}

	GLRenderer renderer;
	native public void nativeTouchUp(float x, float y);
	native public void nativeTouchMove(float x, float y);
	native public void nativeTouchDown(float x, float y);
}

class GLRenderer implements GLSurfaceView.Renderer {
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		nativeInit();
	}

	public void onSurfaceChanged(GL10 gl, int w, int h) {
		nativeResize(w, h);
	}

	public void onDrawFrame(GL10 gl) {
		nativeRender();
	}

	private static native void nativeInit();
	private static native void nativeResize(int w, int h);
	private static native void nativeRender();
}
