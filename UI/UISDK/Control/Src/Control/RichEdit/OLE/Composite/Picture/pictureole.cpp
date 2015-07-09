#include "stdafx.h"
#include "pictureole.h"
#include <shlobj.h>
#include "..\..\Element\image\imageoleelement.h"
#include "..\..\Element\gif\gifoleelement.h"
#include "..\..\Element\button\buttonoleelement.h"
#include "UISDK\Control\Src\Control\RichEdit\Resource\reresource.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Control\Src\Control\RichEdit\windowlessrichedit.h"

using namespace UI;

namespace UI
{
    extern UINT  g_cfRichEditUnicodeXmlFormat;
    extern UINT  CF_HTML;
}

PictureOle::PictureOle():
    m_oPictureREOle(this)
{
    m_pImage = NULL;
	m_pBtnMagnifier = NULL;
    m_pLoadingGif = NULL;
    m_bAdaptSize = false;
    m_sizeMax.cx = 0;
    m_sizeMax.cy = 0;
    m_eStatus = REOleLoadStatus_NOT_INITIALIZE;
    m_pRes = NULL;
    m_pMagnifierRes = NULL;
    m_nZoomRate = 100;
}
PictureOle::~PictureOle()
{
    if (m_pRes)
    {
        m_pRes->Release();
        m_pRes = NULL;
    }
    if (m_pMagnifierRes)
    {
        m_pMagnifierRes->Release();
        m_pMagnifierRes =  NULL;
    }
    if (m_eStatus == REOleLoadStatus_LOADING)
    {
        UnRegisterREOleResUpdate(
            m_strId.c_str(), 
            static_cast<IREOleResUpdateCallback*>(this));
    }
}

PictureOle*  PictureOle::CreateInstance(PictureREOleInfo* pInfo)
{
    if (!pInfo)
        return NULL;

    PictureOle* p = new PictureOle();
    p->SetId(pInfo->szId);
    p->SetDesc(pInfo->szDesc);
    p->SetType(pInfo->szType);
    p->SetName(pInfo->szName);
    p->UpdateStatus(pInfo->eStatus, pInfo->szPath);
    
	if (pInfo->eStatus == REOleLoadStatus_LOADING)
	{
		RegisterREOleResUpdate(
			pInfo->szId, 
			static_cast<IREOleResUpdateCallback*>(p),
			0);
	}
    return p;
}

IPictureREOle*  PictureOle::GetIPictureREOle()
{
    return &m_oPictureREOle;
}

void  PictureOle::OnPreInsert2RE()
{
    WindowlessRichEdit* pRE = GetWindowlessRE();
    if (pRE && pRE->IsReadOnly())
    {
        m_bAdaptSize = true;
    
        CRect  rc;
        pRE->TxGetClientRect(&rc);
		SetMaxSize(rc.Width(), rc.Height());

        // 刚创建时，还拿不到pRE指针，因此在这一步再尝试一次
        if (m_eStatus == REOleLoadStatus_SUCCESS)
        {
            CreateMagnifierBtnElement();
        }
    }
}

// 显示一张图片，有可能是显示失败的图片
bool  PictureOle::LoadImage(LPCTSTR szPath)
{
    if (!m_pImage)
        CreateImageElement();
    
    return m_pImage->LoadImage(szPath);
}
PictureLoadingFailureRes* PictureOle::GetRes()
{
    if (!m_pRes)
    {
        m_pRes = PictureLoadingFailureRes::Get();
        m_pRes->AddRef();
    }
    return m_pRes;
}
MagnifierRes*  PictureOle::GetMagnifierRes()
{
    if (!m_pMagnifierRes)
    {
        m_pMagnifierRes = MagnifierRes::Get();
        m_pMagnifierRes->AddRef();
    }
    return m_pMagnifierRes;
}
bool  PictureOle::LoadFailureImage()
{
    if (!m_pImage)
        CreateImageElement();

     m_pImage->LoadOuterImageRes(GetRes()->GetFailureImage());
     return true;
}
void  PictureOle::CreateImageElement()
{
    if (m_pImage)
        return;

    m_pImage = ImageOleElement::CreateInstance();
    m_pImage->m_layoutParam.StretchAll();

    __super::AddElement(m_pImage);

}
void  PictureOle::CreateLoadingGifElement()
{
    if (m_pLoadingGif)
        return;

    m_pLoadingGif = GifOleElement::CreateInstance();
    m_pLoadingGif->m_layoutParam.StretchAll();
    m_pLoadingGif->LoadOuterGifRes(GetRes()->GetLoadingGif());

    __super::AddElement(m_pLoadingGif);
}
void  PictureOle::DestroyLoadingGifElement()
{
    if (!m_pLoadingGif)
        return;

    __super::DestroyElement(m_pLoadingGif);
    m_pLoadingGif = NULL;
}

void  PictureOle::CreateMagnifierBtnElement()
{
    if (m_pBtnMagnifier)
        return;

    m_pBtnMagnifier = ButtonOleElement::CreateInstance();

	m_pBtnMagnifier->SetImage(
		GetMagnifierRes()->GetNormalImage(),
		GetMagnifierRes()->GetHoverImage(),
		GetMagnifierRes()->GetPressImage(),
		NULL);

    m_pBtnMagnifier->m_layoutParam.nRight = 0;
    m_pBtnMagnifier->m_layoutParam.nBottom = 0;
    m_pBtnMagnifier->m_layoutParam.nWidth = 20;
    m_pBtnMagnifier->m_layoutParam.nHeight = 20;

    ImageWrap* pImageTest = GetMagnifierRes()->GetNormalImage();
    if (pImageTest)
    {
        m_pBtnMagnifier->m_layoutParam.nWidth = pImageTest->GetWidth();
        m_pBtnMagnifier->m_layoutParam.nHeight = pImageTest->GetHeight();
    }

    m_pBtnMagnifier->SetTooltip(TEXT("点击查看原图"));
    m_pBtnMagnifier->SetVisible(false);

    __super::AddElement(m_pBtnMagnifier);
}	

HRESULT  PictureOle::GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData)
{
    if (m_eStatus != REOleLoadStatus_SUCCESS)
        return E_INVALIDARG;

    if (nClipFormat == g_cfRichEditUnicodeXmlFormat)
    {
        WCHAR  szText[1024] = {0};
        if (!m_strPath.empty())
        {
            wsprintf(szText, L"<%s %s=\"%s\" />", 
                RE_CF_OLE_PICTURE, 
                RE_CF_OLE_PATH, 
                m_strPath.c_str());
            *pbstrData = SysAllocString(szText);
            return S_OK;
        }

        return E_FAIL;
    }
    else if (nClipFormat == CF_HTML)
    {
        WCHAR  szText[1024] = {0};
        if (!m_strPath.empty())
        {
            wsprintf(szText, L"<IMG src=\"file:///%s\"/>", 
                m_strPath.c_str());
            *pbstrData = SysAllocString(szText);
            return S_OK;
        }

        return E_FAIL;
    }

    return E_NOTIMPL;
}

HRESULT  PictureOle::GetClipboardDataEx(IDataObject* pDataObject)
{
    if (NULL == pDataObject)
        return E_INVALIDARG;

    if (m_eStatus != REOleLoadStatus_SUCCESS)
        return E_INVALIDARG;

    if (!m_pImage || !m_pImage->GetImage())
        return E_FAIL;

    ImageWrap& image = *(m_pImage->GetImage());

#pragma region  // CF_DIB
    {
        FORMATETC  format = {0};
        format.dwAspect = DVASPECT_CONTENT;
        format.cfFormat = CF_DIB;
        format.tymed = TYMED_HGLOBAL;

        // CF_DIB的结构为: BITMAPINFOHEADER + BITS

        DIBSECTION  dibsec;
        ::GetObject(image.GetHBITMAP(), sizeof(dibsec), &dibsec);

        LPBYTE  lpImageBits = (LPBYTE)image.GetBits();
        int     nPitch = image.GetPitch();
        int     nAbsPitch = abs(nPitch);
        int     nImgHeight = image.GetHeight();
        int     nImgWidth = image.GetWidth();

        HGLOBAL hGlobal = GlobalAlloc(0, sizeof(BITMAPINFOHEADER) + nImgHeight*nAbsPitch);
        LPBYTE  lpbits = (LPBYTE)::GlobalLock(hGlobal);

        LPBYTE  pSrc = lpImageBits;
        LPBYTE  pDst = lpbits;

        memcpy(pDst, &dibsec.dsBmih, sizeof(BITMAPINFOHEADER));
        pDst += sizeof(BITMAPINFOHEADER);

        if (nPitch < 0)
        {
            pDst += nAbsPitch*(nImgHeight-1);
        }

        for (int i = 0; i < nImgHeight; i++)
        {
            memcpy(pDst, pSrc, abs(nPitch));
            pDst += nPitch;
            pSrc += nPitch;
        }

        ::GlobalUnlock(hGlobal);

        STGMEDIUM medium = {0};
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = hGlobal;

        if (FAILED(pDataObject->SetData(&format, &medium, TRUE)))
        {
            GlobalFree(hGlobal);
        }
    }
#pragma endregion

#pragma region // CF_BITMAP
    {
        FORMATETC  format = {0};
        format.dwAspect = DVASPECT_CONTENT;
        format.cfFormat = CF_BITMAP;
        format.tymed = TYMED_GDI;

        STGMEDIUM medium = {0};
        medium.tymed = TYMED_GDI;
        medium.hBitmap = (HBITMAP)OleDuplicateData((HANDLE)image.GetHBITMAP(), CF_BITMAP, 0);;
        pDataObject->SetData(&format, &medium, TRUE);
    } 
#pragma endregion

#pragma region // CF_HDROP
    {
        DROPFILES  dropfiles = {0};
        dropfiles.fWide = 1;
        dropfiles.pFiles = sizeof(dropfiles);

        FORMATETC  format = {0};
        format.dwAspect = DVASPECT_CONTENT;
        format.cfFormat = CF_HDROP;
        format.tymed = TYMED_HGLOBAL;

        // CF_HDROP的结构为: DROPFILES + FileArray (path1\0path2\0\0)

        LPCTSTR  szPath = m_strPath.c_str();
        int nPathSize = sizeof(TCHAR)*(_tcslen(szPath)+1);
        int nSize = sizeof(FORMATETC) + nPathSize + sizeof(TCHAR); // + \0

        HGLOBAL hGlobal = GlobalAlloc(0, nSize);
        LPBYTE  lpbits = (LPBYTE)::GlobalLock(hGlobal);
        memset(lpbits, 0, nSize);
        memcpy(lpbits, &dropfiles, sizeof(dropfiles));
        lpbits += sizeof(dropfiles);
        memcpy(lpbits, szPath, nPathSize);
        GlobalUnlock(hGlobal);

        STGMEDIUM medium = {0};
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = hGlobal;
        if (FAILED(pDataObject->SetData(&format, &medium, TRUE)))
        {
            GlobalFree(hGlobal);
        }
    }
#pragma endregion

    return S_OK;
}


void  PictureOle::GetSize(SIZE* pSize)
{
#ifdef _DEBUG
    pSize->cx = pSize->cy = 40;
#else
    pSize->cx = pSize->cy = 0;
#endif

    SIZE sDesired = {0};

    if (m_pLoadingGif)
        sDesired = m_pLoadingGif->GetGifSize();
    else if (m_pImage)
        sDesired = m_pImage->GetImageSize();
    else
        return;

    if (!m_bAdaptSize)
    {
        *pSize = sDesired;
        m_nZoomRate = 100;
        return;
    }

    if (sDesired.cx <= m_sizeMax.cx &&
        sDesired.cy <= m_sizeMax.cy)
    {
        pSize->cx = sDesired.cx;
        pSize->cy = sDesired.cy;
    }
    else
    {
        if (sDesired.cx && sDesired.cy)  // 非0
        {
            float fxPercent = (float)m_sizeMax.cx / (float)sDesired.cx;
            float fyPercent = (float)m_sizeMax.cy / (float)sDesired.cy;
            if (fxPercent > fyPercent) // y向更长
            {
                m_nZoomRate = (int)(fyPercent*100);
                pSize->cy = m_sizeMax.cy;
                pSize->cx = (int)(sDesired.cx * fyPercent);
            }
            else
            {
                m_nZoomRate = (int)(fxPercent*100);
                pSize->cx = m_sizeMax.cx;
                pSize->cy = (int)(sDesired.cy * fxPercent);
            }
        }
    }
}


LRESULT  PictureOle::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (msg == WM_SIZE)
    {
        if (m_bAdaptSize)
        {
            SetMaxSize(
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam));

            SelfSizeChanged();
        }
    }
    else if (msg == WM_MOUSEMOVE)
    {
        if (m_pBtnMagnifier && !m_pBtnMagnifier->IsVisible())
        {
            m_pBtnMagnifier->SetVisible(true);
            m_pBtnMagnifier->Invalidate();

            TCHAR szTips[64] = {0};
            _stprintf(szTips, TEXT("显示比例：%d%%，双击查看原图"), m_nZoomRate);
            ShowToolTip(szTips);
        }
    }
    else if (msg == WM_MOUSELEAVE)
    {
        if (m_pBtnMagnifier && m_pBtnMagnifier->IsVisible())
        {
            m_pBtnMagnifier->SetVisible(false);
            m_pBtnMagnifier->Invalidate();

            ShowToolTip(NULL);
        }
    }
    else if (msg == WM_NOTIFY)
    {
        // 点击了放大镜按钮
        if (wParam == BN_CLICKED && lParam == (LPARAM)m_pBtnMagnifier)
        {
            UIMSG  msg;
            msg.message = UI_WM_NOTIFY;
            msg.nCode = UI_REOLE_PICTURE_MAGNIFIER_BTN_CLICK;
            msg.wParam = (WPARAM)GetIPictureREOle();
            DoNotify(&msg);
        }
    }

    return __super::ProcessMessage(msg, wParam, lParam, bHandled);
}

// 减去气泡的缩进，确保图片能完整显示
void  PictureOle::SetMaxSize(int cx, int cy)
{
	m_sizeMax.cx = cx - 100;
	m_sizeMax.cy = cy - 20;
}

LPCTSTR  PictureOle::GetPath()
{
    return m_strPath.c_str();
}
void  PictureOle::SetDesc(LPCTSTR szDesc)
{
    if (szDesc)
        m_strDesc = szDesc;
    else
        m_strDesc.clear();
}
void  PictureOle::SetType(LPCTSTR szType)
{
    if (szType)
        m_strType = szType;
    else
        m_strType.clear();
}
void  PictureOle::SetName(LPCTSTR szName)
{
    if (szName)
        m_strName = szName;
    else
        m_strName.clear();
}

void  PictureOle::SetId(LPCTSTR  szId)
{
	if (szId)
		m_strId = szId;
	else
		m_strId.clear();
}
LPCTSTR  PictureOle::GetId()
{
	return m_strId.c_str();
}

void  PictureOle::OnREOleResUpdate(REOleLoadStatus e, LPCTSTR szPath, long lData)
{
	UpdateStatus(e, szPath);
}

void  PictureOle::UpdateStatus(REOleLoadStatus e, LPCTSTR szPath)
{
    REOleLoadStatus eOldStatus = m_eStatus;
    m_eStatus = e;
    m_strPath.clear();  // 只在成功的时候设置

    bool bSizeChange = false;

    if (m_eStatus == REOleLoadStatus_SUCCESS)
    {
        if (!szPath)
        {
            UpdateStatus(REOleLoadStatus_FAILURE);
            return;
        }

        if (!LoadImage(szPath))
        {
            UpdateStatus(REOleLoadStatus_FAILURE);
            return;
        }

        // 加载图片完成。显示放大镜按钮
        WindowlessRichEdit* pRE = GetWindowlessRE();
        if (pRE && pRE->IsReadOnly()) // pRE在刚创建时，还是NULL
        {
            CreateMagnifierBtnElement();
        }

        m_strPath = szPath;
        bSizeChange = true;

    }

    if (eOldStatus != e)
    {
        if (REOleLoadStatus_FAILURE == e || REOleLoadStatus_OVERDUE == e)
        {
            LoadFailureImage();
        }

        // 非加载模式下，需销毁gif资源
        if (REOleLoadStatus_LOADING == e)
            CreateLoadingGifElement(); 
        else
            DestroyLoadingGifElement();

        bSizeChange = true;
    }

    if (bSizeChange)
    {
        this->SelfSizeChanged();
    }
}