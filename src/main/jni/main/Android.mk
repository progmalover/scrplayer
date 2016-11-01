LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)

SDL_PATH := $(LOCAL_PATH)/../SDL2-2.0.4
 
DEP_PATH := $(LOCAL_PATH)/../dep
LOCAL_LIBS := $(LOCAL_PATH)/../../libs

LOCAL_MODULE := mainlogic

#ndk_lib_sources := $(NDK_ROOT)/sources

LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
                                    Render2.cpp \
                                    StudentLogic.cpp \
                                    tcpclient.cpp  \
                                    multi.cpp\
                                    AudioCallback.cpp\
                                    AudioRender.cpp\
                                    mmcApp.cpp\
                                    AndroidAppMain.c\
                                    DataDispatch.cpp
		 

LOCAL_C_INCLUDES  :=$(SDL_PATH)/include\
		    $(DEP_PATH)/ffmpeg
		  
LOCAL_CFLAGS 	:=  $(LOCAL_CPPFLAGS) \
                   -std=c++11 -Wall -Wno-sign-compare -Wno-deprecated-declarations -Wl,--no-undefined \
                   -fPIC -DPIC -O3 -funroll-loops -ffast-math


LOCAL_SHARED_LIBRARIES :=libSDL2 libffmpeg
#LOCAL_STATIC_LIBRARIES := libavformat libavcodec libavutil libpostproc libswscale

 
LOCAL_PREBUILT_LIBS := $(DEP_PATH)/libs/libffmpeg.so

#$(info $(shell cp -rf ($(DEP_PATH)/libs/libffmpeg.so) $(LOCAL_LIBS)/))

LOCAL_LDLIBS +=  $(DEP_PATH)/libs/libffmpeg.so\
	-ldl -lGLESv1_CM -lGLESv2  -llog -ljnigraphics -landroid 

include $(BUILD_MULTI_PREBUILT)
include $(BUILD_SHARED_LIBRARY)

 
#include $(shell install -d  $(LOCAL_LIBS)/armeabi/)
#include $(shell install -p $(DEP_PATH)/libs/libffmpeg.so  $(LOCAL_LIBS)/armeabi)

#include $(shell cp -f $(DEP_PATH)/libs/libffmpeg.so  $(LOCAL_LIBS)/armeabi/libffmpeg.so)
 
