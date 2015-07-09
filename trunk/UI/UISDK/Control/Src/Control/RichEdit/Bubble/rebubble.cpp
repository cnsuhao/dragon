#include "stdafx.h"
#include "rebubble.h"
#include "rebubblemgr.h"
#include "..\windowlessrichedit.h"

// floating-point points��twips��1:20�Ĺ�ϵ??

// ���ݾ���ؼ���Ե��λ��
#define BUBBLE_MARGIN_TWIPS  500
#define BUBBLE_MARGIN_FLOAT  25

REBubble::REBubble()
{
	m_pBubbleMgr = NULL;
    m_ncpStart = m_ncpEnd = -1;
    m_nContentWidth = m_nContentHeight = 0;
	m_eAlignReal = m_eAlignDesired = BUBBLE_ALIGN_LEFT;
    m_pIREBubble = NULL;
    m_bNeedDeleteIREBubble = false;
}

REBubble::~REBubble()
{
    if (m_pIREBubble)
    {
        if (m_bNeedDeleteIREBubble)
            delete m_pIREBubble;
        m_pIREBubble = NULL;
    }
}

IREBubble*  REBubble::GetIREBubble()
{
    return m_pIREBubble;
}
void  REBubble::CreateIxxx(IREBubble* p)
{
    if (p)
    {
        m_pIREBubble = p;
        m_bNeedDeleteIREBubble = false;
    }
    else
    {
        m_pIREBubble = new IREBubble(this);
        m_bNeedDeleteIREBubble = true;
    }
}


void  REBubble::SetBubbleManager(BubbleManager* pMgr)
{
	m_pBubbleMgr = pMgr;
}

void  REBubble::GetDrawRegion(RECT* prc)
{
    if (!prc)
        return;

    // ptbubble���ص������Ͻ�λ�ã������OleObject::Draw���õ�������oleʵ��λ�ã�����׶���
    POINT  ptBubble;
    m_pBubbleMgr->GetRE()->PosFromChar(m_ncpStart, &ptBubble); 

    prc->left   = ptBubble.x;
    prc->top    = ptBubble.y;
    prc->right  = ptBubble.x + m_nContentWidth;
    prc->bottom = ptBubble.y + m_nContentHeight;


    CRegion4  region(0,0,0,0);
	GetContentPadding(&region);

	prc->left -= region.left;
	prc->top -= region.top;
	prc->right += region.right;
	prc->bottom += region.bottom;
}

void  REBubble::GetContentPadding(REGION4* pPadding)
{
	C9Region*  pRegion = NULL;
	if (m_eAlignReal == BUBBLE_ALIGN_RIGHT)
		pRegion = m_pBubbleMgr->Get9RegionLeftBubble();
	else
		pRegion = m_pBubbleMgr->Get9RegionLeftBubble();

	if (pRegion)
	{
		::SetRect(pPadding, pRegion->left, pRegion->top, pRegion->right, pRegion->bottom);
	}
	else
	{
		SetRect(pPadding, 12,12,12,12);
	}
}

// prcDraw����bubblemgr�м�����������ݻ������򣬰�����������
void  REBubble::Draw(HDC hDC, RECT* prcDraw)
{
    ImageWrap*  pImage = NULL;
    C9Region* p9Region = NULL;;

    if (m_eAlignReal == BUBBLE_ALIGN_LEFT)
    {
        pImage = m_pBubbleMgr->GetLeftBubbleImage();
        p9Region = m_pBubbleMgr->Get9RegionLeftBubble();
    }
    else
    {
        pImage = m_pBubbleMgr->GetRightBubbleImage();
        p9Region = m_pBubbleMgr->Get9RegionRightBubble();
    }

    pImage->Draw(hDC, 
        prcDraw->left, 
        prcDraw->top, 
        prcDraw->right-prcDraw->left, 
        prcDraw->bottom-prcDraw->top,
        0, 0, 
        pImage->GetWidth(),
        pImage->GetHeight(), 
        p9Region);
}

void  REBubble::SetContentCPInfo(int nStart, int nEnd)
{
    m_ncpStart = nStart;
    m_ncpEnd = nEnd;
}

void  REBubble::SetDesiredAlign(BubbleAlign e)
{
	m_eAlignDesired = e;
	m_eAlignReal = e;
}

// ע���ڵ��øú���ǰ�󣬱������SetReadOnlyLight���޸�ֻ�����ԣ����SetIndents����ʧ�ܵ�����
void  REBubble::CalcBubblePos()
{
    if (m_ncpStart == -1 || m_ncpEnd == -1)
    {
        // ������ôһ�����������ö�����ʱ��������������ʾ����ʱ��û������
        // cpstart/cpend���ȴ�����OnSize�����²����쳣
        return;
    }

    WindowlessRichEdit*  pRE = m_pBubbleMgr->GetRE();
    ITextDocument2*      pDoc = pRE->GetDocPtr();

    CComPtr<ITextPara>   spPara;
    CComPtr<ITextRange>  spRange;

    pDoc->Range(m_ncpStart, m_ncpEnd, &spRange);
    spRange->GetPara(&spPara);

    // �������������¼���
    spPara->SetIndents(0, BUBBLE_MARGIN_FLOAT, BUBBLE_MARGIN_FLOAT);

    int  nLineStart = pRE->LineFromChar(m_ncpStart);;
    int  nLineEnd = pRE->LineFromChar(m_ncpEnd);
    m_nContentHeight = m_nContentWidth = -1;

    // ��ȡ���ݿ��
    for (int nLine = nLineStart; nLine <= nLineEnd; nLine++)
    {
        POINT  ptEnd;
        POINT  ptStart;

        int ncpStart = pRE->LineIndex(nLine);
        int nLength  = pRE->LineLength(ncpStart);
        int ncpEnd = ncpStart + nLength;
        if (nLength > 0)
            ncpEnd--;  // zero base
        
        CComPtr<ITextRange>  spRangeLine;
        pDoc->Range(ncpStart, ncpEnd, &spRangeLine);
        if (!spRangeLine)
            return;

        // http://technet.microsoft.com/zh-cn/hh768766(v=vs.90) �����Ͷ���
#define _tomClientCoord     256  // Ĭ�ϻ�ȡ��������Ļ���꣬ Use client coordinates instead of screen coordinates.
#define _tomAllowOffClient  512  // Allow points outside of the client area.

        long lTypeTopLeft     = _tomAllowOffClient|_tomClientCoord|tomStart|TA_TOP|TA_LEFT;
        long lTypeRightBottom = _tomAllowOffClient|_tomClientCoord|tomEnd|TA_BOTTOM|TA_RIGHT;
        spRangeLine->GetPoint(lTypeTopLeft,     &ptStart.x, &ptStart.y);
        spRangeLine->GetPoint(lTypeRightBottom, &ptEnd.x,   &ptEnd.y);

        int nContentHeight = ptEnd.y - ptStart.y;
        int nContentWidht  = ptEnd.x - ptStart.x;

        if (m_nContentWidth < nContentWidht)
            m_nContentWidth = nContentWidht;
        m_nContentHeight += nContentHeight;
    }

	if (m_eAlignReal == BUBBLE_ALIGN_RIGHT)
	{
		this->UpdateRightBubbleIndents(spPara);
	}
	else if (m_eAlignReal == BUBBLE_ALIGN_CENTER)
	{
		this->UpdateCenterBubbleIndents(spPara);
	}
}

// �����Ҳ���������ֵ�������������ʱ�����ڴ�С�ı��
void  REBubble::UpdateRightBubbleIndents(ITextPara*  pPara)
{
    if (m_eAlignReal != BUBBLE_ALIGN_RIGHT)
        return;
    if (!pPara)
        return;

    CRect rcclient;
    WindowlessRichEdit*  pRE = m_pBubbleMgr->GetRE();
    pRE->GetControlPtr()->GetObjectClientRect(&rcclient);

    long lLeftIndent = Util::DXtoTwips(rcclient.Width()- m_nContentWidth - 1) - BUBBLE_MARGIN_TWIPS;
    if (lLeftIndent < BUBBLE_MARGIN_TWIPS)
        lLeftIndent = BUBBLE_MARGIN_TWIPS;

    // ReadOnly��RE����SetIndents�᷵��E_ACCESSDENIED Write access is denied. 
    HRESULT hr = pPara->SetIndents(0, (float)lLeftIndent/20, BUBBLE_MARGIN_FLOAT);
    if (hr == E_ACCESSDENIED)
    {
        UIASSERT(!m_pBubbleMgr->GetRE()->IsReadOnly());
    }
}
void  REBubble::UpdateCenterBubbleIndents(ITextPara*  pPara)
{
	if (m_eAlignReal != BUBBLE_ALIGN_CENTER)
		return;
	if (!pPara)
		return;

	CRect rcclient;
	WindowlessRichEdit*  pRE = m_pBubbleMgr->GetRE();
	pRE->GetControlPtr()->GetObjectClientRect(&rcclient);

	long lLeftIndent = Util::DXtoTwips((rcclient.Width()- m_nContentWidth)/2);
	if (lLeftIndent < BUBBLE_MARGIN_TWIPS)
		lLeftIndent = BUBBLE_MARGIN_TWIPS;

	// ReadOnly��RE����SetIndents�᷵��E_ACCESSDENIED Write access is denied. 
	HRESULT hr = pPara->SetIndents(0, (float)lLeftIndent/20, BUBBLE_MARGIN_FLOAT);
	if (hr == E_ACCESSDENIED)
	{
		UIASSERT(!m_pBubbleMgr->GetRE()->IsReadOnly());
	}
}

//////////////////////////////////////////////////////////////////////////

REPortraitBubble::REPortraitBubble()
{
	m_pImagePortrait = NULL;
    m_pIREPortraitBubble = NULL;
    m_bNeedDeleteIREPortraitBubble = false;
}

REPortraitBubble::~REPortraitBubble()
{
    if (m_pIREPortraitBubble)
    {
        if (m_bNeedDeleteIREPortraitBubble)
            delete m_pIREPortraitBubble;
        m_pIREPortraitBubble = NULL;
    }
}

IREPortraitBubble*  REPortraitBubble::GetIREPortraitBubble()
{
    return m_pIREPortraitBubble;
}
void  REPortraitBubble::CreateIxxx(IREPortraitBubble* p)
{
    if (p)
    {
        m_pIREPortraitBubble = p;
        m_bNeedDeleteIREPortraitBubble = false;
    }
    else
    {
        m_pIREPortraitBubble = new IREPortraitBubble(this);
        m_bNeedDeleteIREPortraitBubble = true;
    }

    REBubble::CreateIxxx(static_cast<IREBubble*>(m_pIREPortraitBubble));
}

void  REPortraitBubble::SetPortrait(ImageWrap* pPortrait)
{
	m_pImagePortrait = pPortrait;
}


void  REPortraitBubble::Draw(HDC hDC, RECT* prcDraw)
{
	__super::Draw(hDC, prcDraw);

	if (!m_pImagePortrait)
		return;

	POINT ptPortarit = {0};
	
	switch (m_eAlignReal)
	{
	case BUBBLE_ALIGN_LEFT:
		{
			ptPortarit.x = prcDraw->left - PORTRAIT_OFFSET_LEFT_X;
			ptPortarit.y = prcDraw->top - PORTRAIT_OFFSET_LEFT_T;
		}
		break;

	case BUBBLE_ALIGN_RIGHT:
		{
			ptPortarit.x = prcDraw->right + PORTRAIT_OFFSET_LEFT_X;
			ptPortarit.y = prcDraw->top - PORTRAIT_OFFSET_LEFT_T;
		}
		break;

	default:
		return;
	}

	m_pImagePortrait->Draw(hDC, ptPortarit.x, ptPortarit.y);
}
//////////////////////////////////////////////////////////////////////////
