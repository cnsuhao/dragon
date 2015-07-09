#include "stdafx.h"
#include "windowlessricheditex.h"
#include "3rd\markup\markup.h"
#include "Ole\Gif\gifoleobject.h"
#include "OLE\Composite\Picture\pictureole.h"
#include "OLE\reoleapi.h"
#include "Bubble\rebubblemgr.h"

using namespace UI;

WindowlessRichEditEx::WindowlessRichEditEx()
{
    m_bMsgHandled = FALSE;
    m_pBubbleMgr = NULL;
}
WindowlessRichEditEx::~WindowlessRichEditEx()
{
    EnableBubbleMode(false);
}

void  WindowlessRichEditEx::OnCreate()
{
    // ������Դ��ȡ�ӿ�
    IRichEdit* pRE = GetControlPtr();
    if (pRE)
    {
        SetREOleResUIApplication(pRE->GetUIApplication());
    }
}

void  WindowlessRichEditEx::EnableBubbleMode(bool b)
{
    if (b)
    {
        if (!m_pBubbleMgr)
        {
            m_pBubbleMgr = new BubbleManager(this);
            m_pBubbleMgr->Init();
        }
    }
    else
    {
        SAFE_DELETE(m_pBubbleMgr);
    }
}

bool  WindowlessRichEditEx::IsBubbleMode()
{
    return m_pBubbleMgr ? true:false;
}

REBubble*  WindowlessRichEditEx::CreateBubble(
	BubbleType eType, BubbleAlign eAlign, int nStartCP, int nEndCP)
{
    // ��Ҫ�ȵ���EnableBubbleMode
    if (!m_pBubbleMgr)
        return NULL;

    REBubble*  pBubble = m_pBubbleMgr->CreateBubble(eType, eAlign, nStartCP, nEndCP);
    return pBubble;
}

// ������������֮��ļ�϶
// ԭ������һ���հ��У������е�����������Ϊ���޴󣨼��޷�������
// ע����ǰλ��Ӧ���������һ��\r\n��
void  WindowlessRichEditEx::InsertBubbleSpace()
{
    CHARFORMAT2 cf;
    cf.cbSize = sizeof(cf);
    CopyDefaultCharFormat(&cf);

    PARAFORMAT2 pf;
    pf.cbSize = sizeof(pf);
	CopyDefaultParaFormat(&pf);
    pf.dxStartIndent = 200000;

	SetSelectionCharFormat(&cf);
	SetSelectionParaFormat(&pf);

	InsertText(TEXT("\r\n"));
	ResetSelectionParaFormat();
	ResetSelectionCharFormat();
}

void  WindowlessRichEditEx::OnOleSizeChanged(REOleBase* pItem)
{
    if (!pItem || !m_pBubbleMgr)
        return;

    int cp = GetOleCharPos(pItem);
    if (-1 == cp)
        return;

    REBubble* pBubble = m_pBubbleMgr->FindBubble(cp);
    if (!pBubble)
        return;

    m_pBubbleMgr->InvalidBubbleSize(pBubble);
}

bool  WindowlessRichEditEx::InsertGif(LPCTSTR szGifPath)
{
    GifOle* pGifOle = GifOle::CreateInstance(
        m_pIRichEdit->GetUIApplication(), 
        static_cast<IMessage*>(this->m_pIRichEdit));

    HRESULT hr = pGifOle->LoadGif(szGifPath);
    if (FAILED(hr))
    {
        SAFE_DELETE(pGifOle);
        return false;
    }

    return this->InsertOleObject(pGifOle);
}

bool WindowlessRichEditEx::InsertSkinGif(LPCTSTR szGifId)
{
    GifOle* pGifOle = GifOle::CreateInstance(
        m_pIRichEdit->GetUIApplication(), 
        static_cast<IMessage*>(m_pIRichEdit));

    HRESULT hr = pGifOle->LoadSkinGif(szGifId);
    if (FAILED(hr))
    {
        SAFE_DELETE(pGifOle);
        return false;
    }

    return this->InsertOleObject(pGifOle);
}

IPictureREOle*  WindowlessRichEditEx::InsertPicture(PictureREOleInfo* pInfo)
{
    if (!pInfo)
        return NULL;

    PictureOle* pOle = PictureOle::CreateInstance(pInfo);
    if (!pOle)
        return 0;

    this->InsertOleObject(pOle);
    return pOle->GetIPictureREOle();
}
// IPictureREOle*  WindowlessRichEditEx::FindPicture(LPCTSTR szId)
// {
//     if (!szId)
//         return NULL;
// 
//     REOleBase*  pOle = m_pOleMgr->FindOle(REOLE_PICTURE, szId);
//     if (!pOle)
//         return NULL;
// 
//     PictureOle* pPictureOle = static_cast<PictureOle*>(pOle);
//     return pPictureOle->GetIPictureREOle();
// }

bool  WindowlessRichEditEx::PasteEncodeText(LPWSTR szData)
{
    return ReplaceSelectionEncodeText(szData, wcslen(szData));
}
bool  WindowlessRichEditEx::ReplaceSelectionEncodeText(LPCTSTR szText, int nSize)
{
    if (NULL == szText)
        return false;

    CMarkup markup;
    if (!markup.SetDoc(szText))
        return false;

    if (!markup.FindElem(RE_CF_ROOT))
        return false;

    // ����������ʽ
    String  strColor = markup.GetAttrib(RE_CF_FONT_COLOR);
    String  strFace = markup.GetAttrib(RE_CF_FONT_FACE);
    String  strHeight = markup.GetAttrib(RE_CF_FONT_HEIGHT);
    String  strBold = markup.GetAttrib(RE_CF_FONT_BOLD);
    String  strItalic = markup.GetAttrib(RE_CF_FONT_ITALIC);
    String  strUnderline = markup.GetAttrib(RE_CF_FONT_UNDERLINE);

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    CopyDefaultCharFormat(&cf);

    cf.dwMask = CFM_ALL;
    cf.yHeight = _ttoi(strHeight.c_str());
    if (cf.yHeight == 0)
        cf.yHeight = m_cf.yHeight;
    cf.crTextColor = _ttoi(strColor.c_str());
    _tcscpy(cf.szFaceName, strFace.c_str());
    if (cf.szFaceName[0] == 0)
        _tcscpy(cf.szFaceName, m_cf.szFaceName);
    if (strBold == L"1")
        cf.dwEffects |= CFE_BOLD;
    if (strItalic == L"1")
        cf.dwEffects |= CFE_ITALIC;
    if (strUnderline == L"1")
        cf.dwEffects |= CFE_UNDERLINE;

    String  strAlign = markup.GetAttrib(RE_CF_PARA_ALIGN);
    String  strLineSpacing = markup.GetAttrib(RE_CF_LINESPACING);
    String  strLineBefore = markup.GetAttrib(RE_CF_LINEBEFORE);
    String  strLineAfter = markup.GetAttrib(RE_CF_LINEAFTER);

    PARAFORMAT2  pf;
    memset(&pf, 0, sizeof(pf));
    pf.cbSize = sizeof(pf);
    CopyDefaultParaFormat(&pf);

    if (!strAlign.empty())
    {
        pf.dwMask = PFM_ALIGNMENT;
        pf.wAlignment = _ttoi(strAlign.c_str());
        if (pf.wAlignment == 0)
            pf.wAlignment = PFA_LEFT;
    }

    if (!strLineSpacing.empty())
    {
        pf.dwMask |= PFM_LINESPACING;
        pf.dwMask |= PFM_LINESPACING;
        pf.dyLineSpacing = Util::DXtoHimetricX(_ttoi(strLineSpacing.c_str()));
        pf.bLineSpacingRule = tomLineSpaceExactly;
    }

    if (!strLineAfter.empty())
    {
        pf.dwMask |= PFM_SPACEAFTER;
        pf.dySpaceAfter = Util::DXtoHimetricX(_ttoi(strLineAfter.c_str()));
    }

    if (!strLineBefore.empty())
    {
        pf.dwMask |= PFM_SPACEBEFORE;
        pf.dySpaceBefore = Util::DXtoHimetricX(_ttoi(strLineBefore.c_str()));
    }

    SetSelectionCharFormat(&cf);
    SetSelectionParaFormat(&pf);

    // �����ı���ole����
    if (markup.IntoElem())
    {
        while (markup.FindElem())
        {
            String strTag = markup.GetTagName();
            if (strTag == RE_CF_OLE_TEXT)
            {
                ReplaceSelectionText(markup.GetData().c_str(), -1);
            }
            else if (strTag == RE_CF_OLE_COM)
            {
                String strCLSID = markup.GetAttrib(RE_CF_OLE_COM_CLSID);

                CLSID clsid = {0};
                CLSIDFromString((LPOLESTR)strCLSID.c_str(), &clsid);
                InsertComObject(clsid);
            }
            else if (strTag == RE_CF_OLE_EMOTION)
            {
                String strId = markup.GetAttrib(RE_CF_OLE_ID);
                InsertSkinGif(strId.c_str());
            }
            else if (strTag == RE_CF_OLE_GIF)
            {
                String strPath = markup.GetAttrib(RE_CF_OLE_PATH);
                InsertGif(strPath.c_str());
            }
            else if (strTag == RE_CF_OLE_PICTURE)
            {
                String strPath = markup.GetAttrib(RE_CF_OLE_PATH);
                PictureREOleInfo info = {0};
                info.eStatus = REOleLoadStatus_SUCCESS;
                info.szPath = strPath.c_str();
                InsertPicture(&info);
            }
        }
    }

    return true; 
}

struct InsertMessageContext
{
    enum {MAGIC = 141371003};
    InsertMessageContext()
    {
        nMagicNumber = MAGIC;
        nStartCP = 0;
        eAlign = BUBBLE_ALIGN_LEFT;
		eType = BUBBLE_NORMAL;
    }

    int  nMagicNumber;
    int  nStartCP;
	BubbleType  eType;
    BubbleAlign  eAlign;
};

// ��ʼ����һ����Ϣ������֧�ֶַ�β���һ����Ϣ������
// ����ֵ�����ڴ��ݸ�InsertMessageFinish
HANDLE  WindowlessRichEditEx::InsertMessageStart(BubbleType eType, BubbleAlign eAlign)
{
    if (!IsBubbleMode())
        return NULL;

    int nStartCP = GetSelectionStart();
    InsertMessageContext*  pContext = new InsertMessageContext;
    pContext->nStartCP = nStartCP;
    pContext->eAlign = eAlign;
	pContext->eType = eType;

    // �ж���һ��λ���Ƿ���һ�����ݡ�������������֮��Ҫ����һ���ָ���
    if (0 == nStartCP || m_pBubbleMgr->IsInBubble(nStartCP-1, NULL))
    {
        InsertBubbleSpace();
        pContext->nStartCP = GetSelectionStart();
    }

    return (HANDLE)pContext;
}

// ����һ����Ϣ�Ĳ���
IREBubble*  WindowlessRichEditEx::InsertMessageFinish(HANDLE hInsertStart)
{
	REBubble* pBubble = NULL;

    InsertMessageContext*  pContext = (InsertMessageContext*)hInsertStart;
    if (pContext && pContext->nMagicNumber != InsertMessageContext::MAGIC)
        pContext = NULL;

    if (pContext)
    {
        int nEndcp = GetSelectionStart();
        pBubble = CreateBubble(pContext->eType, pContext->eAlign, pContext->nStartCP, nEndcp);

        // ȫ��ˢ�£�Ҳ�ܽ�����ݱ����ݴ��µ�ˢ�²���������
        InvalidateRect(NULL);

        SAFE_DELETE(pContext);
    }

    InsertText(_T("\r\n")); 
    ResetSelectionCharFormat();
    ResetSelectionParaFormat();

    // ���ⲿȥscrollend
    // ScrollEnd();
// 	if (pBubble)
// 		return pBubble->GetIREBubble();
	return NULL;
}

bool  WindowlessRichEditEx::InsertEncodeText(LPCTSTR szText, int nSize)
{
    return ReplaceSelectionEncodeText(szText, nSize);
}
bool  WindowlessRichEditEx::InsertEncodeText(int nPos, LPCTSTR szText, int nSize)
{
    SetSel(nPos);
    return ReplaceSelectionEncodeText(szText, nSize);
}
bool  WindowlessRichEditEx::AppendEncodeText(LPCTSTR szText, int nSize)
{
    SelectEnd();
    return InsertEncodeText(szText, nSize);
}


LRESULT  WindowlessRichEditEx::OnGifFrameTick(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_pRichEdit)
        return 0;

    // 	if (false == DoGifOleUpdateRequst()) // �ú�����Ҫ�������ƶ���ˢ��֡��
    // 		return 0;

    GifOle*  pGifOleObj = (GifOle*)lParam;
    pGifOleObj->OnTick();

    return 0;
}

void  WindowlessRichEditEx::PreDraw(HDC hDC, RECT* prcDraw)
{
	__super::PreDraw(hDC, prcDraw);

    if (m_pBubbleMgr)
	{
		// �Ȼ���һ�飬�����ݲ��ֺã�������ȷ�������ݡ�
		// �����ݻ�����һ����ʱDC��
		IUIApplication* pUIApp = m_pIRichEdit->GetUIApplication();
		HDC hTempDC = pUIApp->GetCacheDC();
		HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
		SelectClipRgn(hTempDC, hRgn);
		{
			RealDraw(hTempDC, prcDraw);
		}
		SelectClipRgn(hTempDC, NULL);
		::DeleteObject(hRgn);
		pUIApp->ReleaseCacheDC(hTempDC);

        m_pBubbleMgr->OnDraw(hDC);
	}
}
void  WindowlessRichEditEx::PostDraw(HDC hDC, RECT* prcDraw)
{

}
void  WindowlessRichEditEx::OnSize(UINT nType, CSize size)
{
    __super::OnSize(nType, size);

    if (m_pBubbleMgr)
        m_pBubbleMgr->OnSize();
    
}

// �������ģʽ���棬�Ҷ�����ı������ʽ��������һ��
// ֱ������Ϊ��ͷ
void WindowlessRichEditEx::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	if (IsBubbleMode())
	{
		static HCURSOR _cursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(_cursor);
		return;
	}

	__super::TxSetCursor(hcur, fText);
}