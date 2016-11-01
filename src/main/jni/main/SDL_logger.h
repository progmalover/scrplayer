//
// Created by liuchi on 2016/4/1.
//

#ifndef NDK_DEMO_SDL_LOGGER_H
#define NDK_DEMO_SDL_LOGGER_H

#include <android/log.h>

#define APPNAME     "multi system client"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, __VA_ARGS__)

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , APPNAME, __VA_ARGS__)

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO , APPNAME, __VA_ARGS__)

#define LOGW(...) __android_log_print(ANDROID_LOG_WARN , APPNAME, __VA_ARGS__)

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR , APPNAME, __VA_ARGS__)


#endif //NDK_DEMO_SDL_LOGGER_H
