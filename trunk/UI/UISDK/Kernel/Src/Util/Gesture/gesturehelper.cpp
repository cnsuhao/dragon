#include "stdafx.h"
#include "gesturehelper.h"

namespace UI
{
GestureHelper*  GetGestureInstance()
{
    static GestureHelper dwm;
    return &dwm;
}

GestureHelper::GestureHelper()
{
    pSetGestureConfig = NULL;
    pGetGestureInfo = NULL;
    pCloseGestureInfoHandle = NULL;

    m_hModule = LoadLibrary(_T("User32.dll"));
    if (NULL == m_hModule)
        return;

    pSetGestureConfig = (pfnSetGestureConfig)GetProcAddress(m_hModule, "SetGestureConfig");
    pGetGestureInfo = (pfnGetGestureInfo)GetProcAddress(m_hModule, "GetGestureInfo");
    pCloseGestureInfoHandle = (pfnCloseGestureInfoHandle)GetProcAddress(m_hModule, "CloseGestureInfoHandle");
}

GestureHelper::~GestureHelper()
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
}

bool GestureHelper::IsValid()
{
    return pSetGestureConfig ? true:false;
}



}