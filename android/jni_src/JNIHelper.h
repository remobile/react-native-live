#ifndef __JNI_HELPER_H__
#define __JNI_HELPER_H__

#include <jni.h>
#include <string>

jstring str2jstr(JNIEnv* env, const std::string &str);
std::string jstr2str(JNIEnv* env, const jstring &jstr);

#define ATTACH_JVM(name, sign)  \
    bool isAttached = false; \
    JNIEnv *_env = NULL; \
    JavaVM *android_jvm = (JavaVM *)m_jvm; \
    jobject _camera = (jobject)m_camera; \
    jmethodID jmethod; \
    jclass _class; \
    do { \
        if (android_jvm->GetEnv((void **)&_env, JNI_VERSION_1_6) != JNI_OK) { \
            jint res = android_jvm->AttachCurrentThread(&_env,NULL); \
            ASSERT_ERROR(!((res<0)||!_env), "Could not attach thread to JVM"); \
            isAttached = true; \
        } \
        _class = _env->GetObjectClass(_camera); \
        ASSERT_ERROR(_class!=NULL, "GetObjectClass"); \
        _method = _env->GetMethodID(_class, name, sign); \
        ASSERT_ERROR(NULL!=_method, "GetMethodID "##name); \
    } while (0)

#define DETACH_JVM() \
    do { \
        if (isAttached) { \
            android_jvm->DetachCurrentThread(); \
        } \
    } while (0)


#define ASSERT_ERROR(trial, info) do {\
    if (!(trial)) { \
        LOGF("%s  <failed>", info); \
        goto on_error; \
    } else { \
        LOGV("%s  <success>", info); \
    } \
} while (0)


#endif //__JNI_HELPER_H__
