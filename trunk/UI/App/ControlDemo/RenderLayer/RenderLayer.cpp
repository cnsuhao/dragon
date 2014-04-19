#include "StdAfx.h"
#include "RenderLayer.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\iflashwrap.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include <math.h>


CRenderLayerWindow::CRenderLayerWindow()
{
    srand(GetTickCount());

    m_pButtonJump = NULL;
}

CRenderLayerWindow::~CRenderLayerWindow(void)
{

}

void CRenderLayerWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    m_pButtonSwitchFlash = (UI::IButton*)FindChildObject(_T("switch_flash"));
    m_pFlash = (UI::IFlashWrap*)FindChildObject(_T("flash"));
	m_pPanel = (UI::IPanel*)FindChildObject(_T("panel"));
	m_pSlider = (UI::ISliderCtrl*)FindChildObject(_T("slider"));
    if (m_pPanel)
        m_pButtonJump = (UI::IButton*)m_pPanel->FindChildObject(_T("button_jump"));

    String strSkinPath = this->GetUIApplication()->GetActiveSkinRes()->GetPath();
    strSkinPath.append(_T("renderlayer\\"));
    String strUri;

    const TCHAR* szArry[] = 
    {
        _T("sunny.swf"),
        _T("cloudy.swf"),
        _T("overcast.swf"),
        _T("rain.swf"),
        _T("thunderstorm.swf"),
        _T("snow.swf"),
//         _T("lichun.swf"),
//         _T("qingming.swf")
    };
    int nSize = sizeof(szArry)/sizeof(TCHAR*);
    for (int i = 0; i < nSize; i++)
    {
        strUri = strSkinPath;
        strUri.append(szArry[i]);
        m_vecFlashUri.push_back(strUri);
    }

    SwitchFlash();
	if (m_pSlider)
	{
		m_pSlider->ModifyStyleEx(SLIDER_STYLE_AUTOTICKS|SLIDER_STYLE_SHOWTICKS, 0, true);
		m_pSlider->SetRange(0, 255, false);
		m_pSlider->SetPos(255, false);
	}
}


void  CRenderLayerWindow::OnBnClick()
{
    SwitchFlash();
}

void  CRenderLayerWindow::SwitchFlash()
{
    if (!m_pFlash)
        return;

    unsigned int nSize = m_vecFlashUri.size();
    if (nSize < 0)
        return;

    unsigned nIndex = rand()%nSize;
    m_pFlash->SetFlashUri((BSTR)m_vecFlashUri[nIndex].c_str());
    m_pFlash->Play();
    m_pFlash->UpdateLayout(true);
}

void  CRenderLayerWindow::OnTrbnPosChanged(int nPos, int nScrollType)
{
	if (m_pPanel)
	{
		UI::IRenderLayer2*  p = m_pPanel->GetRenderLayer2();
		if (p)
		{
			p->SetOpacity(nPos);
			m_pPanel->UpdateObject();
		}
	}
}

void  CRenderLayerWindow::OnBnClickJump()
{
    UI::IObject* pObj = FindChildObject(_T("edit"));
    if (!pObj)
        return;

#if 1
    static bool b = false;
    b = !b;
    if (b)
    {
        pObj->SetObjectPos(10, 140, 0, 0, SWP_NOSIZE);    
    }
    else
    {
        pObj->SetObjectPos(180, 140, 0, 0, SWP_NOSIZE);
    }

#elif 0
    static bool b = false;
    b = !b;
    pObj->SetVisible(b);

#elif 0
    HBITMAP h = pObj->TakeSnapshot();
    SAFE_DELETE_GDIOBJECT(h);
#endif


}