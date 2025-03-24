
#include "pch.h"
#include "DeviceLib.h"
#include <wincrypt.h>
#include <list>
#pragma comment(lib, "ole32.lib")
#include <vector>
// 获取设备的ID和名称
DeviceInfo GetDeviceInfo(IMMDevice* device) {
    DeviceInfo deviceInfo;
    deviceInfo.id = nullptr;
    deviceInfo.name = nullptr;

    LPWSTR deviceId = nullptr;
    device->GetId(&deviceId);

    IPropertyStore* pStore = nullptr;
    device->OpenPropertyStore(STGM_READ, &pStore);

    PROPVARIANT varName;
    PropVariantInit(&varName);
    pStore->GetValue(PKEY_Device_FriendlyName, &varName);
    deviceInfo.name = _wcsdup(varName.pwszVal);
    deviceInfo.id = _wcsdup(deviceId);  // 复制ID字符串

    PropVariantClear(&varName);
    pStore->Release();
    CoTaskMemFree(deviceId);  // 释放获取到的设备ID内存

    return deviceInfo;
}
// 获取当前默认音频播放设备的ID和名称
DeviceInfo GetCurrentDefaultAudioDevice(int type) {
    IMMDeviceEnumerator* pEnum = nullptr;
    CoInitialize(nullptr);
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    if (FAILED(hr)) return { nullptr, nullptr };

    IMMDevice* pDefDevice = nullptr;
    hr = pEnum->GetDefaultAudioEndpoint(type == 1 ? eRender : eCapture, eMultimedia, &pDefDevice);
    DeviceInfo deviceInfo = { nullptr, nullptr };
    if (SUCCEEDED(hr)) {
        deviceInfo = GetDeviceInfo(pDefDevice);
        pDefDevice->Release();
    }
    pEnum->Release();
    CoUninitialize();
    return deviceInfo;
}
// 获取所有音频输出设备
DeviceInfo* GetAudioOutputDevices(int* count, int type) {
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) return nullptr;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) {
        CoUninitialize();
        return nullptr;
    }
    IMMDeviceCollection* pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(type == 1 ? eRender : eCapture, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr)) {
        pEnumerator->Release();
        CoUninitialize();
        return nullptr;
    }
    UINT deviceCount;
    pCollection->GetCount(&deviceCount);
    *count = deviceCount; // 设置设备数量
    DeviceInfo* devices = new DeviceInfo[deviceCount]; // 动态分配数组
    for (UINT i = 0; i < deviceCount; ++i) {
        IMMDevice* pDevice = nullptr;
        hr = pCollection->Item(i, &pDevice);
        if (SUCCEEDED(hr)) {
            devices[i] = GetDeviceInfo(pDevice);
            pDevice->Release();
        }
    }
    pCollection->Release();
    pEnumerator->Release();
    CoUninitialize();
    return devices;
}
// 设置默认音频播放设备
bool SetDefaultAudioPlaybackDevice(LPCWSTR devID,ERole role) {
    // 初始化 COM 库
    HRESULT comHr = CoInitialize(nullptr);
    if (FAILED(comHr)) {
        std::cerr << "COM库初始化失败，错误代码: " << comHr << std::endl;
        return false;
    }
    IPolicyConfigVista* pPolicyConfig;
    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
    if (SUCCEEDED(hr)) {
        hr = pPolicyConfig->SetDefaultEndpoint(devID, role);
        pPolicyConfig->Release();
        if (FAILED(hr)) {
            return false;
        }
        return SUCCEEDED(hr);
    }
    else {
        return false;
    }
    CoUninitialize();
    return false;
}
void FreeDeviceArray(DeviceInfo* devices, int count) {
    for (int i = 0; i < count; i++) {
        free(devices[i].name); // 释放设备名称内存
        free(devices[i].id);   // 释放设备ID内存
    }
    free(devices); // 释放设备数组内存
}
bool Test(int* a, int b) {
    *a = 10;
    b = 20;
    return true;
}
bool SetDefaultDevice(wchar_t* id,int type)
{
    return SetDefaultAudioPlaybackDevice(id, static_cast<ERole>(type));
}
DeviceInfo GetDefaultDevice(int type) {
    return GetCurrentDefaultAudioDevice(type);
}
DeviceInfo* GetAudioDevices(int* deviceCount, int type)
{
    return GetAudioOutputDevices(deviceCount, type);
}
void FreeDevice(DeviceInfo* devices, int count) {
    FreeDeviceArray(devices, count); // 释放设备数组内存
}
void Free(wchar_t* vchat) {
    free(vchat);
}

