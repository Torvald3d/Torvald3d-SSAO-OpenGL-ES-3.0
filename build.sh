#!/bin/sh

case "$1" in
	clean) ndk-build clean && ant clean
		;;
	deploy) adb install -r bin/MainActivity-debug.apk > /dev/null 2>&1 & # && adb logcat -c && adb logcat -s "SSAOTest"
		;;
	run) #adb shell am start -n com.torvald.ssaotest/com.torvald.ssaotest.MainActivity > /dev/null 2>&1 &
		;;
	*) #kill $(ps aux | grep "adb logcat" | grep -v "grep" | awk '{print $2}') > /dev/null 2>&1 &
		ndk-build NDK_DEBUG=0 -j9 && ant debug
		;;
esac
