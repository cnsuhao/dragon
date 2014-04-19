#include "StdAfx.h"
#include "clockwindow.h"
#include "../Other/resource.h"
#include <time.h>
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Kernel\Inc\Interface\iobject3dwrap.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include <math.h>

ClockWindow::ClockWindow()
{
    m_pHour = NULL;
    m_pMin = NULL;
    m_pSec = NULL;
    m_nMinRotate = m_nSecRotate = m_nHourRotate = 0;

	m_pHourLayer = NULL;
	m_pMinLayer = NULL;
	m_pSecLayer = NULL;
}

ClockWindow::~ClockWindow(void)
{
}

void ClockWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

    m_pHour = (UI::IPictureCtrl*)FindChildObject(_T("hour"));
    m_pMin  = (UI::IPictureCtrl*)FindChildObject(_T("min"));
    m_pSec  = (UI::IPictureCtrl*)FindChildObject(_T("sec"));

	if (m_pHour)
		m_pHourLayer = m_pHour->GetRenderLayer2();
	if (m_pMin)
		m_pMinLayer = m_pMin->GetRenderLayer2();
	if (m_pSec)
		m_pSecLayer = m_pSec->GetRenderLayer2();

    SetTimer(GetHWND(), 1, 100, NULL);
    CheckTime();
}

void  ClockWindow::CheckTime()
{
    time_t	t;
    tm      s_tm;

    time( &t );
    localtime_s( &s_tm, &t );

    // 由时间计算各各指针的旋转角度，满圈360度
    // 秒针:
    int nSecRotate = s_tm.tm_sec * 6;
    // 分针:
    int nMinRotate = s_tm.tm_min * 6;
    // 时针： 满圈是12*60分钟也就是720分钟1分钟走0.5度
    int nHourRotate = ((s_tm.tm_hour%12)*60 + s_tm.tm_min) >> 1;

    bool bNeedUpdate = false;
    if (nSecRotate != m_nSecRotate && m_pSec)
    {
        m_nSecRotate = nSecRotate;
#ifdef ROTATE_3D
        m_pSecLayer->Rotate3D(0, 0, nSecRotate);   // 3d旋转
#else
        m_pSecLayer->Rotate2D(nSecRotate);   // 2d旋转
#endif
        bNeedUpdate = true;
        //m_pSec->UpdateObject();
    }

    if (nMinRotate != m_nMinRotate && m_pMin)
    {   
        m_nMinRotate = nMinRotate;
#ifdef ROTATE_3D
        m_pMinLayer->Rotate3D(0, 0, m_nMinRotate);        
#else
		m_pMinLayer->Rotate2D(m_nMinRotate);        
#endif
        bNeedUpdate = true;
    }

    if (nHourRotate != m_nHourRotate && m_pHour)
    {
        m_nHourRotate = nHourRotate;
#ifdef ROTATE_3D
        m_pHourLayer->Rotate3D(0, 0, m_nHourRotate);
#else
		m_pHourLayer->Rotate2D(m_nHourRotate);
#endif
        bNeedUpdate = true;
    }

    if (bNeedUpdate)
        this->UpdateObject();
}
void  ClockWindow::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    CheckTime();
    return;
}
#define PI 3.141592653f
void  ClockWindow::OnPaint(UI::IRenderTarget* pRenderTarget)
{
#if 0
	HDC  hDC = pRenderTarget->GetHDC();
	SetViewportOrgEx(hDC, 285, 0, NULL);

	UI::IImage  image;
	image.Load(L"C:\\sechand.png", true);
	
	CRect  rc(0, 0, image.GetWidth(), image.GetHeight());
	POINT ptCenter = {rc.left+rc.Width()/2, rc.top+rc.Height()/2};  // 图片目标区域的中心，绕该点旋转，而不是默认是（0，0）坐标旋转

	float cosAngle = cos(1.0f*PI/180.0F);
	float sinAngle = sin(1.0f*PI/180.0F);

	XFORM m_xForm;
	m_xForm.eM11 = cosAngle;
	m_xForm.eM12 = sinAngle;
	m_xForm.eM21 = -sinAngle;
	m_xForm.eM22 = cosAngle;
	m_xForm.eDx = ptCenter.x - cosAngle*ptCenter.x + sinAngle*ptCenter.y;
	m_xForm.eDy = ptCenter.y - cosAngle*ptCenter.y - sinAngle*ptCenter.x;

	int nOldGraphicsMode = ::GetGraphicsMode(hDC);
	::SetGraphicsMode(hDC, GM_ADVANCED);
	::SetWorldTransform(hDC, &m_xForm);

	image.Draw(hDC, 0, 0);

	ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
	::SetGraphicsMode(hDC, nOldGraphicsMode);
	SetViewportOrgEx(hDC, 0, 0,NULL);
#endif
#if 0
    CRect  rc(0, 0, 126, 126);
    POINT ptCenter = {rc.left+rc.Width()/2, rc.top+rc.Height()/2};  // 图片目标区域的中心，绕该点旋转，而不是默认是（0，0）坐标旋转

    float cosAngle = cos(45.0f*PI/180.0F);
    float sinAngle = sin(45.0f*PI/180.0F);

    XFORM m_xForm;
    m_xForm.eM11 = cosAngle;
    m_xForm.eM12 = sinAngle;
    m_xForm.eM21 = -sinAngle;
    m_xForm.eM22 = cosAngle;
    m_xForm.eDx = ptCenter.x - cosAngle*ptCenter.x + sinAngle*ptCenter.y;
    m_xForm.eDy = ptCenter.y - cosAngle*ptCenter.y - sinAngle*ptCenter.x;

    HDC  hDC = pRenderTarget->GetHDC();
    Gdiplus::Graphics g(hDC);
    Gdiplus::Matrix  matrix;
//     matrix.SetElements(
//         m_xForm.eM11,
//         m_xForm.eM12,
//         m_xForm.eM21,
//         m_xForm.eM22, 
//         m_xForm.eDx, 
//         m_xForm.eDy);

    matrix.Translate(100, 0);
    Gdiplus::PointF ptCenter2(ptCenter.x, ptCenter.y);
    matrix.RotateAt(45, ptCenter2);
//     Gdiplus::REAL r[6];
//     matrix.GetElements(r);
    
    g.SetTransform(&matrix);
    Gdiplus::Bitmap*  pBitmap = Gdiplus::Bitmap::FromFile(L"C:\\test.png");
    Gdiplus::RectF rcDst1( (Gdiplus::REAL)0, (Gdiplus::REAL)0, 
        (Gdiplus::REAL)126, (Gdiplus::REAL)126);
    g.DrawImage(pBitmap, rcDst1, 
        (Gdiplus::REAL)0, 
        (Gdiplus::REAL)0,
        (Gdiplus::REAL)126,
        (Gdiplus::REAL)126,
        Gdiplus::UnitPixel, NULL
        );
    g.ResetTransform();
    SAFE_DELETE(pBitmap);
#endif
}