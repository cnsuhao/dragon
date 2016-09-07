#include "stdafx.h"
#include "util.h"

void  MsgBox(const TCHAR* szText, const TCHAR* szTitle)
{
    ::MessageBox(
        g_pGlobalData->m_hWndMainFrame,
        szText, szTitle?szTitle:_T("UIEditor"), MB_OK|MB_ICONINFORMATION);
}

// 将exe中的模板保存到指定目录s
bool  SaveTemplateResFileTo(const TCHAR* szDstPath, UINT nId, const TCHAR* szResType)
{
    HRSRC hResource = FindResource(_Module.m_hInst, MAKEINTRESOURCE(nId), szResType);
    if (NULL == hResource)
    {
        MsgBox(_T("查找皮肤文件资源失败"));
        return false;
    }
    HGLOBAL hGlobal = LoadResource(_Module.m_hInst, hResource);
    {
        fstream  f;
        f.open(szDstPath, ios_base::out|ios_base::binary);
        if (f.fail())
        {
            FreeResource(hGlobal);
            return false;
        }

        int  nSize = SizeofResource(_Module.m_hInst, hResource);
        byte*  pData = (byte*)LockResource(hGlobal);
        f.write((char*)pData, nSize);
        UnlockResource(hGlobal);
        f.close();
    }
    FreeResource(hGlobal);
    return true;
}

// 获取UIEditor.xml所在路径
void  GetUIEditorConfigFilePath(TCHAR* pszPath)
{
    Util::GetAppPath_(pszPath);
    _tcscat(pszPath, _T("Editor\\"));
    _tcscat(pszPath, UIEDITOR_CONFIG_FILE_NAME);
}

// 解析命令行的中命令
void  ParsePropertyCommand(const TCHAR*  szCmdLine, String& strCommand, String& strArgs)
{
    if (NULL == szCmdLine)
        return;

    String strText(szCmdLine);
    TrimStringLeft(strText);

    const TCHAR* p = strText.c_str();
    while (*p)
    {
        if (*p == _T(' '))
            break;
        p++;
    }
    strCommand.insert(0, strText.c_str(), p-strText.c_str());

    // 过滤命令与参数之间的空格
    while (*p)
    {
        if (*p != _T(' '))
            break;
        p++;
    }

    strArgs.assign(p);
}

// 解析参数中的属性和值
void  ParsePropertyCommandArg(const TCHAR*  szArgs, String& strKey, String& strValue)
{
    if (!szArgs || _T('\0') == szArgs[0])
        return;

   
    TCHAR*  szSep = NULL; 
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            szSep = _T("=");   // 1. 尝试 = 分隔
        else 
            szSep = _T(" ");   // 2. 尝试 空格 分隔

        const TCHAR*  szPos = _tcsstr(szArgs, szSep);
        if (szPos)
        {
            strKey.clear();
            strKey.insert(0, szArgs, szPos-szArgs);

            strValue.assign(szPos+1);

            TrimString(strKey);
            TrimString(strValue);
            break;
        }
    }
}


void  TrimStringLeft(String&  str, const TCHAR c)
{
    int nLeft = 0;
    int nLength = str.length();
    for (int i = 0; i < nLength; i++)
    {
        if (str[i] != c)
            break;

        nLeft++;
    }
    str = str.substr(nLeft, nLength-nLeft);
}
void  TrimStringRight(String&  str, const TCHAR c)
{
    int nRight = 0;

    int nLength = str.length();
    for (int i = nLength-1; i >= 0; i--)
    {
        if (str[i] != c)
            break;

        nRight++;
    }

    str = str.substr(0, nLength-nRight);
}

void  TrimString(String&  str, const TCHAR c)
{
    int nLeft = 0;
    int nRight = 0;

    int nLength = str.length();
    for (int i = 0; i < nLength; i++)
    {
        if (str[i] != c)
            break;

        nLeft++;
    }

    for (int i = nLength-1; i >= 0; i--)
    {
        if (str[i] != c)
            break;

        nRight++;
    }

    if (nRight + nLeft >= nLength)
    {
        str.clear();
    }
    else
    {
        str = str.substr(nLeft, nLength-nLeft-nRight);
    }
}

// 等待用户按下回车进行确认
bool  WaitForEnter2Confirm(HWND hWnd)
{
    bool bOk = false;
    MSG  msg;
    while (GetMessage(&msg, hWnd, 0, 0))
    {
        if (msg.message == WM_KEYDOWN)
        {
            if (VK_RETURN == msg.wParam)
            {
                bOk = true;
            }
            break;
        }
        else if (msg.message > WM_MOUSEFIRST && msg.message < WM_MOUSELAST)
        {
            break;
        }
    }
    return bOk;
}

// 获取对象类型的显示图标
void  GetObjectTypeBitmap(OBJ_TYPE eType, UINT nControlType, IRenderBitmap** ppRenderBitma)
{
    IImageRes* pImgRes = g_pGlobalData->m_pMyUIApp->GetActiveSkinImageRes();
    if (!pImgRes)
        return;

    const TCHAR*  szImgId = NULL;
    if (eType == OBJ_WINDOW)
    {
        if (nControlType == WINDOW_TOOLTIP)
            szImgId = _T("control_tooltip");
        else
            szImgId = _T("control_AppWindow");
    }
    else if (eType == OBJ_PANEL)
    {
        szImgId = _T("control_panel");
    }
    else if (eType == OBJ_CONTROL)
    {
        switch (nControlType)
        {
        case CONTROL_BUTTON:
            szImgId = _T("control_button");
            break;
        case CONTROL_MENU:
            szImgId = _T("control_menu");
            break;
        case CONTROL_CHECKBUTTON:
            szImgId = _T("control_checkbox");
            break;
        case CONTROL_COMBOBOX:
            szImgId = _T("control_combobox");
            break;
        case CONTROL_GROUPBOX:
            szImgId = _T("control_groupbox");
            break;
        case CONTROL_HSCROLLBAR:
            szImgId = _T("control_hscrollbar");
            break;
        case CONTROL_VSCROLLBAR:
            szImgId = _T("control_vscrollbar");
            break;
        case CONTROL_HYPERLINK:
            szImgId = _T("control_hyperlink");
            break;
        case CONTROL_LABEL:
        case CONTROL_LEDCTRL:
            szImgId = _T("control_label");
            break;
        case CONTROL_LISTBOX:
            szImgId = _T("control_listbox");
            break;
        case CONTROL_PICTURE:
            szImgId = _T("control_picturebox");
            break;
        case CONTROL_RADIOBUTTON:
            szImgId = _T("control_radiobutton");
            break;
        case CONTROL_RICHEDIT:
            szImgId = _T("control_richtextbox");
            break;
        case CONTROL_TABCTRL:
            szImgId = _T("control_tabcontrol");
            break;
        case CONTROL_EDIT:
            szImgId = _T("control_textbox");
            break;
        case CONTROL_PROGRESSBAR:
            szImgId = _T("control_progress");
            break;
        case CONTROL_SLIDERCTRL:
            szImgId = _T("control_trackbar");
            break;
        case CONTROL_TREEVIEWCTRL:
            szImgId = _T("control_treeview");
            break;
        case CONTROL_IE:
            szImgId = _T("control_webbrowser");
            break;
        case CONTROL_LISTVIEWCTRL:
            szImgId = _T("control_listview");
            break;
        }
    }

    if (szImgId)
    {
        pImgRes->GetBitmap(szImgId, GRAPHICS_RENDER_LIBRARY_TYPE_GDI, ppRenderBitma);
    }
}