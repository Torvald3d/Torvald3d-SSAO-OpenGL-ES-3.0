#ifndef DEBUG_H
#define DEBUG_H

#include <android/log.h>
#include <time.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SSAOTest", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "SSAOTest", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "SSAOTest", __VA_ARGS__))

static double getMs() {
	timespec res;
	clock_gettime(CLOCK_REALTIME, &res);
	return 1000.0*res.tv_sec + (double)res.tv_nsec/1e6;
}

#endif // DEBUG_H
