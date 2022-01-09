//
// Created by David Obermann on 05.01.22.
//

#ifndef HELLO_CMAKE_PINGSOCKET_H
#define HELLO_CMAKE_PINGSOCKET_H

#include <jni.h>

#endif //HELLO_CMAKE_PINGSOCKET_H

JNIEXPORT jint JNICALL Java_de_kreativsoft_hello_1cmake_MainActivity_pingJNI(
        JNIEnv* env,
        jobject/*this*/,
        jstring /*host ip*/);