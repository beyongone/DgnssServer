LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := comn

LOCAL_CPPFLAGS += -fexceptions -frtti

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
	$(LOCAL_PATH)/include \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

SRC_FILES := $(wildcard \
		$(LOCAL_PATH)/src/*.cpp \
		$(LOCAL_PATH)/*.cpp \
		)
		
SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES += $(SRC_FILES)

#LOCAL_CFLAGS += -D

include $(BUILD_STATIC_LIBRARY)

