#include "stdafx.h"
#include "groupbox.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iattribute.h"

namespace UI
{

GroupBox::GroupBox()
{
    m_pIGroupBox = NULL;
    m_rcBorderForImage.SetRectEmpty();
    m_rcTextClipInWindow.SetRectEmpty();
    m_rcText.SetRectEmpty();
    m_sizeText.cx = m_sizeText.cy = 0;
    m_bNeedMeasureText = false;

	m_rcTextMargin.SetRectEmpty();
	m_nTextIndent = 10;
	m_nTextAlign = 0;

	m_bNeedCalcRect = true;
}
GroupBox::~GroupBox()
{
}

HRESULT  GroupBox::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IGroupBox, IPanel);

	ObjStyle s = {0};
	s.default_transparent = 1;
	m_pIGroupBox->ModifyObjectStyle(&s, 0);
	return S_OK;
}

void  GroupBox::OnSerialize(SERIALIZEDATA* pData)
{
    DO_PARENT_PROCESS(IGroupBox, IPanel);

	{
		AttributeSerializerWrap s(pData, TEXT("GroupBox"));

		// 文字排版
		s.AddLong(XML_GROUPBOX_TEXTINDENT, m_nTextIndent);
		s.AddRect(XML_GROUPBOX_TEXTMARGIN, m_rcTextMargin);

		// 内容
		s.AddString(XML_TEXT, this, 
			memfun_cast<pfnStringSetter>(&GroupBox::SetText),
			memfun_cast<pfnStringGetter>(&GroupBox::GetText))
			->AsData();

		s.AddFlags(XML_GROUPBOX_TEXTALIGN, m_nTextAlign)
			->AddFlag(ALIGN_LEFT, XML_ALIGN_LEFT)
			->AddFlag(ALIGN_TOP, XML_ALIGN_TOP)
			->AddFlag(ALIGN_RIGHT, XML_ALIGN_RIGHT)
			->AddFlag(ALIGN_BOTTOM, XML_ALIGN_BOTTOM)
			->AddFlag(ALIGN_CENTER, XML_ALIGN_CENTER)
			->AddFlag(ALIGN_VCENTER, XML_ALIGN_VCENTER);
	}

	if (pData->IsLoad())
	{
		IUIApplication* pUIApp = m_pIGroupBox->GetUIApplication();
		if (!m_pIGroupBox->GetTextRender())
		{
			ITextRenderBase* pTextRender = NULL;
			pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_SIMPLE, m_pIGroupBox, &pTextRender);
			if (pTextRender)
			{
				pTextRender->SetTextAlignment(DT_EDITCONTROL|DT_WORDBREAK);
				pTextRender->Serialize(pData);

				m_pIGroupBox->SetTextRender(pTextRender);
				SAFE_RELEASE(pTextRender);
			}
		}

		// 边框
		if (!m_pIGroupBox->GetBkRender())
		{
			IRenderBase* p = NULL;
			pUIApp->CreateRenderBase(RENDER_TYPE_THEME_GROUPBOX_BKGND, m_pIGroupBox, &p);
			if (p)
			{
				p->Serialize(pData);

				m_pIGroupBox->SetBkgndRender(p);
				SAFE_RELEASE(p);
			}
		}
	}

	// 字体
	if (m_pIGroupBox->GetTextRender())
	{
		SERIALIZEDATA data(*pData);
		data.szParentKey = XML_TEXTRENDER_TYPE;
		m_pIGroupBox->GetTextRender()->Serialize(&data);
	}
}

void GroupBox::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    if (m_bNeedMeasureText)
        this->MeasureText();
	if (m_bNeedCalcRect)
		this->CalcBorderRect();

    //
	// 将文字区域裁剪出来
	//
	HRGN hRgnOld = pRenderTarget->GetClipRgn();

	CRect rc = m_rcTextClipInWindow;
	POINT pt = m_pIGroupBox->GetRealPosInWindow();  // PS: 当在旋转时，这个位置取的不正确，这种情况下先另用这个控件
	rc.OffsetRect(pt.x, pt.y);

	HRGN hRgnClip = ::CreateRectRgnIndirect(&rc);  // 剪裁区域是基于窗口的，而不是基于偏移
	::CombineRgn(hRgnClip, hRgnOld, hRgnClip, RGN_DIFF);
	pRenderTarget->SelectClipRgn(hRgnClip);
	::DeleteObject(hRgnClip);

	//
	//	绘制边框
	//
    IRenderBase* pBkgndRender = m_pIGroupBox->GetBkRender();
	if (pBkgndRender)
	{
		if (m_pIGroupBox->IsEnable())
			pBkgndRender->DrawState(pRenderTarget, &m_rcBorderForImage, GROUPBOX_BKGND_RENDER_STATE_NORMAL);
		else
			pBkgndRender->DrawState(pRenderTarget, &m_rcBorderForImage, GROUPBOX_BKGND_RENDER_STATE_DISABLE);
	}

	//
	//	还原剪裁区域
	//
	pRenderTarget->SelectClipRgn(hRgnOld);
	::DeleteObject(hRgnOld);

	//
	//	绘制文字，放在onerasebkgnd中可以直接画在ncclient区域，不受限制
	//
    ITextRenderBase* pTextRender = m_pIGroupBox->GetTextRender();
	if (pTextRender)
		pTextRender->DrawState(pRenderTarget, &m_rcText, 0, m_strText.c_str(), -1);
}

void GroupBox::OnSize(UINT nType, int cx, int cy)
{
	m_bNeedCalcRect = true;
    SetMsgHandled(FALSE);
}

void  GroupBox::SetText(LPCTSTR szText)
{
    if (NULL == szText)
        m_strText.clear();
    else
        m_strText = szText;

    m_bNeedMeasureText = true;
    m_bNeedCalcRect = true;
}

LPCTSTR  GroupBox::GetText()
{
	return m_strText.c_str();
}

void  GroupBox::MeasureText()
{
    m_bNeedMeasureText = false;
    ITextRenderBase* pTextRender = m_pIGroupBox->GetTextRender();
    if (NULL == pTextRender)
        return;

    // 这里只作简短文字的测量，对于长文字分两行不支持
    if (m_nTextAlign & ALIGN_VCENTER)
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str(), 1);
    else
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str());
}


// 注：CalcBorderRect不能仅放在OnEraseBkgnd中调用，因为CalcBorderRect会导致自己的client size改变，
//     导致内部控件位置需要进行调整。（例如内部一个EDIT layout.left/right/top/bottom=10）
void  GroupBox::CalcBorderRect()
{
    m_bNeedCalcRect = false;

    RECT rcGroupBox = { 0, 0, m_pIGroupBox->GetWidth(), m_pIGroupBox->GetHeight() };
    CopyRect(m_rcBorderForImage, &rcGroupBox);

    if (m_nTextAlign & ALIGN_VCENTER)
    {
        if (m_nTextAlign & ALIGN_RIGHT)
        {
            m_rcBorderForImage.right = rcGroupBox.right - ((m_sizeText.cx + m_rcTextMargin.left + m_rcTextMargin.right)/2);
            m_rcText.SetRect(rcGroupBox.right-m_sizeText.cx, 0, rcGroupBox.right, m_sizeText.cy);
            m_rcText.OffsetRect(-m_rcTextMargin.right, 0);

            if (m_nTextAlign & ALIGN_CENTER)   // 右侧居中
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // 右侧靠下
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // 右侧靠上
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
        else
        {
            m_rcBorderForImage.left = (m_sizeText.cx + m_rcTextMargin.left + m_rcTextMargin.right)/2;
            m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
            m_rcText.OffsetRect(m_rcTextMargin.left, 0);

            if (m_nTextAlign & ALIGN_CENTER)   // 左侧居中
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // 左侧靠下
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // 左侧靠上
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
    }
    else if (m_nTextAlign & ALIGN_BOTTOM)      // 下层
    {
        m_rcBorderForImage.bottom = rcGroupBox.bottom - ((m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2);
        m_rcText.SetRect(0, rcGroupBox.bottom-m_sizeText.cy, m_sizeText.cx, rcGroupBox.bottom);
        m_rcTextMargin.OffsetRect(0, -m_rcTextMargin.bottom);

        if (m_nTextAlign & ALIGN_CENTER)       // 下层居中
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)   // 下层靠右
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                   // 下层靠左
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }
    else // if (m_nTextAlign & ALIGN_TOP)      // 上层
    {
        m_rcBorderForImage.top = (m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2;
        m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
        m_rcText.OffsetRect(0, m_rcTextMargin.top);

        if (m_nTextAlign & ALIGN_CENTER)        // 上层居中
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)    // 上层靠右
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                    // 上层靠左
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }

    // 计算需要的裁剪区域
    m_rcTextClipInWindow.CopyRect(&m_rcText);
    m_rcTextClipInWindow.InflateRect(m_rcTextMargin.left, m_rcTextMargin.top, m_rcTextMargin.right, m_rcTextMargin.bottom);

    // 设置控件边框
    CRegion4 rBorder(2,2,2,2);  // 设置控件的border区域
    if (m_nTextAlign & ALIGN_VCENTER)
    {
        if (m_nTextAlign & ALIGN_RIGHT)
            rBorder.right = m_rcTextClipInWindow.Width();
        else
            rBorder.left = m_rcTextClipInWindow.Width();
    }
    else if (m_nTextAlign & ALIGN_BOTTOM)
    {
        rBorder.bottom = m_rcTextClipInWindow.Height();
    }
    else
    {
        rBorder.top = m_rcTextClipInWindow.Height();
    }
    m_pIGroupBox->SetBorderRegion(&rBorder);
}

}