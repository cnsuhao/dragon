#include "stdafx.h"
#include "reresource.h"
using namespace UI;

IUIApplication* g_pResUIApp = NULL;

namespace UI
{
void  SetREOleResUIApplication(IUIApplication* pUIApp)
{
    g_pResUIApp = pUIApp;
}
}

ResBase::ResBase()
{
    m_dwRef = 0;
}
long  ResBase::AddRef()
{
    if (0 == m_dwRef)
    {
        OnLoad();
    }
    m_dwRef++;

    return m_dwRef;
}
long  ResBase::Release()
{
    m_dwRef--;
    if (0 == m_dwRef)
    {
        OnUnload();
        return 0;
    }

    return m_dwRef;
}

bool  ResBase::LoadImage(LPCTSTR szId, ImageWrap* pImage)
{
    if (!g_pResUIApp || !szId || !pImage)
        return false;

    ISkinRes*  pSkinRes = g_pResUIApp->GetActiveSkinRes();
    if (!pSkinRes)
        return false;

    IImageRes*  pImageRes = pSkinRes->GetImageRes();
    if (!pImageRes)
        return false;

    ISkinDataSource*   pDataSource = pSkinRes->GetDataSource();
    if (!pDataSource)
        return false;

    IImageResItem*  pItem = pImageRes->GetImageResItem(szId);
    if (!pItem)
        return false;

    return pDataSource->Load_Image(pImage, pItem->GetPath());
}

bool  ResBase::LoadGdiplusBitmap(LPCTSTR szId, GdiplusBitmapLoadWrap* pBitmap)
{
    if (!g_pResUIApp || !szId || !pBitmap)
        return false;

    ISkinRes*  pSkinRes = g_pResUIApp->GetActiveSkinRes();
    if (!pSkinRes)
        return false;

    IImageRes*  pImageRes = pSkinRes->GetImageRes();
    if (!pImageRes)
        return false;

    ISkinDataSource*   pDataSource = pSkinRes->GetDataSource();
    if (!pDataSource)
        return false;

    IImageResItem*  pItem = pImageRes->GetImageResItem(szId);
    if (!pItem)
        return false;

    return pDataSource->Load_GdiplusImage(pItem->GetPath(), pBitmap);
}

bool  ResBase::LoadGdiplusBitmapFromRes(long lId, Gdiplus::Bitmap** ppBitmap)
{
    if (!ppBitmap)
        return false;

#if 0
    // ������Դ
    HRSRC hRsrc = ::FindResource(g_hInstance, MAKEINTRESOURCE(lId), TEXT("DATA"));
    if (hRsrc == NULL) 
        return false;

    // ������Դ
    HGLOBAL hImgData = ::LoadResource(g_hInstance, hRsrc);
    if (hImgData == NULL)
        return false;

    // �����ڴ��е�ָ����Դ
    LPVOID   lpVoid  = ::LockResource(hImgData);
    DWORD    nSize   = ::SizeofResource(g_hInstance, hRsrc);
    LPSTREAM pStream = NULL;
    HGLOBAL  hNew    = ::GlobalAlloc(GHND, nSize);
    LPBYTE   lpByte  = (LPBYTE)::GlobalLock(hNew);

    ::memcpy(lpByte, lpVoid, nSize);
    ::GlobalUnlock(hNew);

    // ��ָ���ڴ洴��������
    HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
    if (ht != S_OK)
    {
        GlobalFree(hNew);
        return false;
    }
    // ����ͼƬ
    Gdiplus::Bitmap* pBmp = Gdiplus::Bitmap::FromStream(pStream);

    //GlobalFree(hNew);  // GDI+��Ҫstream������ֱ������stream
    pStream->Release();  
    hNew = NULL;

    ::FreeResource(hImgData);

    *ppBitmap = pBmp;
#endif
    return true;
}

//////////////////////////////////////////////////////////////////////////

BubbleRes::BubbleRes()
{
	SetRectEmpty(&m_indentLeftBubble);
	SetRectEmpty(&m_indentRightBubble);
}

void  BubbleRes::OnLoad()
{
	LoadImage(
		XML_RICHEDIT_OLE_PREFIX 
		XML_REOLE_PICTURE_PREFIX 
		XML_REOLE_MAGNIFIER_PREFIX
		XML_REOLE_NORMAL,
		&m_imageLeftBubble);

	LoadImage(
		XML_RICHEDIT_OLE_PREFIX 
		XML_REOLE_PICTURE_PREFIX 
		XML_REOLE_MAGNIFIER_PREFIX
		XML_REOLE_HOVER,
		&m_imageRightBubble);
}
void  BubbleRes::OnUnload() 
{
	m_imageLeftBubble.Destroy();
	m_imageRightBubble.Destroy();
}

//////////////////////////////////////////////////////////////////////////

MagnifierRes*  MagnifierRes::Get()
{
    static MagnifierRes _this;
    return &_this;
}
void  MagnifierRes::OnLoad()
{
	LoadImage(
		XML_RICHEDIT_OLE_PREFIX 
		XML_REOLE_PICTURE_PREFIX 
		XML_REOLE_MAGNIFIER_PREFIX
		XML_REOLE_NORMAL,
		&m_imageNormal);

	LoadImage(
		XML_RICHEDIT_OLE_PREFIX 
		XML_REOLE_PICTURE_PREFIX 
		XML_REOLE_MAGNIFIER_PREFIX
		XML_REOLE_HOVER,
		&m_imageHover);

	LoadImage(
		XML_RICHEDIT_OLE_PREFIX 
		XML_REOLE_PICTURE_PREFIX 
		XML_REOLE_MAGNIFIER_PREFIX
		XML_REOLE_PRESS,
		&m_imagePress);
}

void  MagnifierRes::OnUnload()
{
	m_imageNormal.Destroy();   
	m_imageHover.Destroy();
	m_imagePress.Destroy();
}

//////////////////////////////////////////////////////////////////////////

PictureLoadingFailureRes::PictureLoadingFailureRes()
{
}

PictureLoadingFailureRes*  PictureLoadingFailureRes::Get()
{
    static PictureLoadingFailureRes _this;
    return &_this;
}
void  PictureLoadingFailureRes::OnLoad()
{
    LoadImage(
        XML_RICHEDIT_OLE_PREFIX 
        XML_REOLE_PICTURE_PREFIX 
        XML_REOLE_PICTURE_FAILURE_IMAGE,
        &m_imageFailure);

    LoadGdiplusBitmap(
        XML_RICHEDIT_OLE_PREFIX 
        XML_REOLE_PICTURE_PREFIX 
        XML_REOLE_PICTURE_LOADING_IMAGE,
        &m_imageLoaddingGif
        );
}

void  PictureLoadingFailureRes::OnUnload()
{
    m_imageFailure.Destroy();
    m_imageLoaddingGif.Destroy();
}
ImageWrap*  PictureLoadingFailureRes::GetFailureImage()
{
    return &m_imageFailure;
}
Gdiplus::Bitmap*  PictureLoadingFailureRes::GetLoadingGif()
{
    return m_imageLoaddingGif.GetBitmap();
}