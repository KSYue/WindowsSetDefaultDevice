#pragma once
#include "PolicyConfig.h"
#include <windows.h>
#include <mmdeviceapi.h>
#include <iostream>
#include <functiondiscoverykeys_devpkey.h>
#include <vector>
#include <Propidl.h>

#ifdef MATHLIBRARY_EXPORTS
#define MATHLIBRARY_API __declspec(dllexport)
#else
#define MATHLIBRARY_API __declspec(dllimport)
#endif
// 定义设备信息结构
struct DeviceInfo {
    wchar_t* name;
    wchar_t* id;
};

extern "C" MATHLIBRARY_API bool SetDefaultDevice(wchar_t* id, int type);
extern "C" MATHLIBRARY_API DeviceInfo GetDefaultDevice(int type);
extern "C" MATHLIBRARY_API DeviceInfo* GetAudioDevices(int* deviceCount, int type);
extern "C" MATHLIBRARY_API void FreeDevice(DeviceInfo* devices, int count);
extern "C" MATHLIBRARY_API void Free(wchar_t* vchat);
