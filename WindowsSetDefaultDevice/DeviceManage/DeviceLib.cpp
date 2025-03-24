
#include "pch.h"
#include "DeviceLib.h"
#include <wincrypt.h>
#include <list>
#pragma comment(lib, "ole32.lib")
#include <vector>
// ��ȡ�豸��ID������
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
    deviceInfo.id = _wcsdup(deviceId);  // ����ID�ַ���

    PropVariantClear(&varName);
    pStore->Release();
    CoTaskMemFree(deviceId);  // �ͷŻ�ȡ�����豸ID�ڴ�

    return deviceInfo;
}
// ��ȡ��ǰĬ����Ƶ�����豸��ID������
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
// ��ȡ������Ƶ����豸
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
    *count = deviceCount; // �����豸����
    DeviceInfo* devices = new DeviceInfo[deviceCount]; // ��̬��������
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
// ����Ĭ����Ƶ�����豸
bool SetDefaultAudioPlaybackDevice(LPCWSTR devID,ERole role) {
    // ��ʼ�� COM ��
    HRESULT comHr = CoInitialize(nullptr);
    if (FAILED(comHr)) {
        std::cerr << "COM���ʼ��ʧ�ܣ��������: " << comHr << std::endl;
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
        free(devices[i].name); // �ͷ��豸�����ڴ�
        free(devices[i].id);   // �ͷ��豸ID�ڴ�
    }
    free(devices); // �ͷ��豸�����ڴ�
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
    FreeDeviceArray(devices, count); // �ͷ��豸�����ڴ�
}
void Free(wchar_t* vchat) {
    free(vchat);
}

