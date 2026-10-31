/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <jni.h>
void client_ps5_capture_enable(int);
void client_ps5_capture_probe(char*,unsigned);
JNIEXPORT void JNICALL Java_ProsperoMicrophone_nativeEnable(JNIEnv *env,jclass owner,jboolean enabled){(void)env;(void)owner;client_ps5_capture_enable(enabled);}
JNIEXPORT jstring JNICALL Java_ProsperoMicrophone_nativeProbe(JNIEnv *env,jclass owner){(void)owner;char result[512]={0};client_ps5_capture_probe(result,sizeof(result));return (*env)->NewStringUTF(env,result);}
