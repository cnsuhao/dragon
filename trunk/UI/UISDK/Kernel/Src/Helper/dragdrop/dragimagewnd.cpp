#include "stdafx.h"
#include <shlobj.h>

//
// ϵͳ�Դ�����קͼ��ȱ�ݽ϶࣬�Լ�ʵ��һ��
// 1. �Ƴ����������أ����봰��ʱ��ʾ�������˸
// 2. ͸��ɫ���ƣ�������һЩĪ�����������
// 3. ��С���ƣ�С��ĳ��ֵʱ������ʾ
// 4. ��win7����Ч
//

namespace UI
{
HHOOK  g_hHook = NULL;
HWND   g_hWnd = NULL;
POINT  g_ptOffset = {0};
HWND  CreateDragBitmapWnd(HBITMAP hBitmap, int x, int y);
BOOL  InstallDragMouseHook();
void  UninstallDragMouseHook();

// ע����Ҫ����SHDODragDrop������Ĭ���ṩһ�ſհ׵���קͼƬ
LRESULT  UIDoDragDrop(
			IDataObject* pDataObject, 
			IDropSource* pDropSource, 
			DWORD dwOKEffect,         
			DWORD* pdwEffect,        
			HBITMAP hbmpDragImage,
			POINT ptOffset)
{
	POINT pt = {0};
	GetCursorPos(&pt);
	g_ptOffset = ptOffset;
	pt.x -= ptOffset.x;
	pt.y -= ptOffset.y;

	g_hWnd = CreateDragBitmapWnd(hbmpDragImage, pt.x, pt.y);
	if (!g_hWnd)
		return DoDragDrop(pDataObject, pDropSource, dwOKEffect, pdwEffect);

	InstallDragMouseHook();
	
	LRESULT lRet = DoDragDrop(pDataObject, pDropSource, dwOKEffect, pdwEffect);

	UninstallDragMouseHook();
	DestroyWindow(g_hWnd);
	g_hWnd = NULL;

	return lRet;
}


HWND  CreateDragBitmapWnd(HBITMAP hBitmap, int x, int y)
{
	if (!hBitmap)
		return NULL;

	BITMAP bm = {sizeof(bm)};
	GetObject(hBitmap, sizeof(bm), &bm);

	POINT ptDst = {x, y};
	SIZE  sizeDst = {bm.bmWidth, bm.bmHeight};

	HWND hWnd = CreateWindowEx(
		WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOPMOST|WS_EX_TOOLWINDOW, 
		WND_DRAGBITMAPWND_CLASS_NAME,
		NULL, 
		WS_POPUP,
		ptDst.x, 
		ptDst.y,
		sizeDst.cx, 
		sizeDst.cy,
		NULL, NULL, NULL, NULL);
	if (!hWnd)
		return NULL;

	ShowWindow(hWnd, SW_SHOWNOACTIVATE);

	POINT ptSrc = {0, 0};
	HDC hDC = CreateCompatibleDC(NULL);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 180, AC_SRC_ALPHA};
	/*BOOL b = */UpdateLayeredWindow(hWnd, NULL, &ptDst, &sizeDst, hDC, &ptSrc, 0, &bf, ULW_ALPHA);
	SelectObject(hDC, hOldBitmap);
	DeleteDC(hDC);
	
	return hWnd;
}

LRESULT CALLBACK MouseProc(
	int nCode,
    WPARAM wParam,
    LPARAM lParam
)
{
	if (nCode < 0)
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE)
	{
		MOUSEHOOKSTRUCT* pStruct = (MOUSEHOOKSTRUCT*)lParam;

		if (g_hWnd)
		{
			SetWindowPos(g_hWnd, 
				0,
				pStruct->pt.x - g_ptOffset.x,
				pStruct->pt.y - g_ptOffset.y,
				0, 0, 
				SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		}
		
	}
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
BOOL  InstallDragMouseHook()
{
	if (g_hHook)
		return TRUE;

	g_hHook = SetWindowsHookEx(WH_MOUSE, MouseProc, NULL, GetCurrentThreadId());
	if (!g_hHook)
		return FALSE;

	return TRUE;
}

void  UninstallDragMouseHook()
{
	if (g_hHook)
	{
		UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
}

}