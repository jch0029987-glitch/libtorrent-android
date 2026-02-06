#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL
Java_com_tech_piratebay_NativeTorrent_nativeCreateSession(JNIEnv *, jobject);

JNIEXPORT void JNICALL
Java_com_tech_piratebay_NativeTorrent_nativeStartMagnet(
        JNIEnv *, jobject, jlong sessionPtr, jstring magnetUri, jstring savePath);

JNIEXPORT void JNICALL
Java_com_tech_piratebay_NativeTorrent_nativeStopSession(
        JNIEnv *, jobject, jlong sessionPtr);

#ifdef __cplusplus
}
#endif
