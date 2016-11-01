
#include <android/log.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include "SDL_logger.h"
#include "AndriodAppMain.h"
#include <jni.h>


static JavaVM* mJavaVM;
static JNIEnv *menv;

JNIEnv* APP_JNI_GetEnv(void)
{
    /* From http://developer.android.com/guide/practices/jni.html
     * All threads are Linux threads, scheduled by the kernel.
     * They're usually started from managed code (using Thread.start), but they can also be created elsewhere and then
     * attached to the JavaVM. For example, a thread started with pthread_create can be attached with the
     * JNI AttachCurrentThread or AttachCurrentThreadAsDaemon functions. Until a thread is attached, it has no JNIEnv,
     * and cannot make JNI calls.
     * Attaching a natively-created thread causes a java.lang.Thread object to be constructed and added to the "main"
     * ThreadGroup, making it visible to the debugger. Calling AttachCurrentThread on an already-attached thread
     * is a no-op.
     * Note: You can call this function any number of times for the same thread, there's no harm in it
     */

    JNIEnv *env;
    int status = (*mJavaVM)->AttachCurrentThread(mJavaVM, &env, NULL);
    if(status < 0) {
        LOGE("failed to attach current thread");
        return 0;
    }

    return env;
}


/* Library init */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{

    mJavaVM = vm;
    LOGI("APP Main JNI_OnLoad called");
    if ((*mJavaVM)->GetEnv(mJavaVM, (void**) &menv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("Failed to get the environment using GetEnv()");
        return -1;
    }

    return JNI_VERSION_1_4;
}

char *configs[10]={0};
JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_SetIpConfig(JNIEnv* env, jclass cls,jobject array)
{

	APP_JNI_GetEnv();

	LOGV("SDL","Java_org_libsdl_app_SDLActivity_SetIpConfig() called!");

    int len = (*env)->GetArrayLength(env, array);
    int i;
	for (i = 0; i < len; ++i) {
	        const char* utf;
	        jstring string = (*env)->GetObjectArrayElement(env, array, i);

	        if (string) {
	            utf = (*env)->GetStringUTFChars(env, string, 0);
	            if (utf) {
	            	 configs[i] = SDL_strdup(utf);

	            	 LOGV("SDL",configs[i]);
	                (*env)->ReleaseStringUTFChars(env, string, utf);

	            }
	            (*env)->DeleteLocalRef(env, string);
	        }else
	        	configs[i] = SDL_strdup("");

	 }
}

char **getIpConfig()
{
	return &configs[0];
}
/*
JNIEXPORT int JNICALL Java_org_libsdl_app_SDLActivity_AppMainInit(JNIEnv* env, jclass cls)
{
	  mActivityClass = (jclass)((*mEnv)->NewGlobalRef(mEnv, cls));
	  midGetIpConfig = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass,
	                                "getIpConfig","()Landroid/view/Surface;");
}
*/

