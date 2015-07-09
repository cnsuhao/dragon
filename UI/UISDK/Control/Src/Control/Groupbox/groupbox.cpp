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

		// �����Ű�
		s.AddLong(XML_GROUPBOX_TEXTINDENT, m_nTextIndent);
		s.AddRect(XML_GROUPBOX_TEXTMARGIN, m_rcTextMargin);

		// ����
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

		// �߿�
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

	// ����
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
	// ����������ü�����
	//
	HRGN hRgnOld = pRenderTarget->GetClipRgn();

	CRect rc = m_rcTextClipInWindow;
	POINT pt = m_pIGroupBox->GetRealPosInWindow();  // PS: ������תʱ�����λ��ȡ�Ĳ���ȷ���������������������ؼ�
	rc.OffsetRect(pt.x, pt.y);

	HRGN hRgnClip = ::CreateRectRgnIndirect(&rc);  // ���������ǻ��ڴ��ڵģ������ǻ���ƫ��
	::CombineRgn(hRgnClip, hRgnOld, hRgnClip, RGN_DIFF);
	pRenderTarget->SelectClipRgn(hRgnClip);
	::DeleteObject(hRgnClip);

	//
	//	���Ʊ߿�
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
	//	��ԭ��������
	//
	pRenderTarget->SelectClipRgn(hRgnOld);
	::DeleteObject(hRgnOld);

	//
	//	�������֣�����onerasebkgnd�п���ֱ�ӻ���ncclient���򣬲�������
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

    // ����ֻ��������ֵĲ��������ڳ����ַ����в�֧��
    if (m_nTextAlign & ALIGN_VCENTER)
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str(), 1);
    else
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str());
}


// ע��CalcBorderRect���ܽ�����OnEraseBkgnd�е��ã���ΪCalcBorderRect�ᵼ���Լ���client size�ı䣬
//     �����ڲ��ؼ�λ����Ҫ���е������������ڲ�һ��EDIT layout.left/right/top/bottom=10��
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

            if (m_nTextAlign & ALIGN_CENTER)   // �Ҳ����
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // �Ҳ࿿��
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // �Ҳ࿿��
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
        else
        {
            m_rcBorderForImage.left = (m_sizeText.cx + m_rcTextMargin.left + m_rcTextMargin.right)/2;
            m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
            m_rcText.OffsetRect(m_rcTextMargin.left, 0);

            if (m_nTextAlign & ALIGN_CENTER)   // ������
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // ��࿿��
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // ��࿿��
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
    }
    else if (m_nTextAlign & ALIGN_BOTTOM)      // �²�
    {
        m_rcBorderForImage.bottom = rcGroupBox.bottom - ((m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2);
        m_rcText.SetRect(0, rcGroupBox.bottom-m_sizeText.cy, m_sizeText.cx, rcGroupBox.bottom);
        m_rcTextMargin.OffsetRect(0, -m_rcTextMargin.bottom);

        if (m_nTextAlign & ALIGN_CENTER)       // �²����
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)   // �²㿿��
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                   // �²㿿��
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }
    else // if (m_nTextAlign & ALIGN_TOP)      // �ϲ�
    {
        m_rcBorderForImage.top = (m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2;
        m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
        m_rcText.OffsetRect(0, m_rcTextMargin.top);

        if (m_nTextAlign & ALIGN_CENTER)        // �ϲ����
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)    // �ϲ㿿��
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                    // �ϲ㿿��
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }

    // ������Ҫ�Ĳü�����
    m_rcTextClipInWindow.CopyRect(&m_rcText);
    m_rcTextClipInWindow.InflateRect(m_rcTextMargin.left, m_rcTextMargin.top, m_rcTextMargin.right, m_rcTextMargin.bottom);

    // ���ÿؼ��߿�
    CRegion4 rBorder(2,2,2,2);  // ���ÿؼ���border����
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