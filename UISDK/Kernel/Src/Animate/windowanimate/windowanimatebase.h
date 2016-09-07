#pragma once
#include "UISDK\Kernel\Src\ATL\image.h"
#include "UISDK\Kernel\Src\Animate\storyboard.h"

namespace UI
{
class LayeredAnimateWindow;
class AnimateManager;
class WindowBase;
interface ITimeline;
interface IWindowAnimateBase;


class WindowAnimateBase : public Storyboard
{
public:
    WindowAnimateBase();
    virtual ~WindowAnimateBase();

    virtual void  OnTick(int nCount, IStoryboard** ppTimerArray) {}
    
public:
    void  SetIWindowAnimateBase(IWindowAnimateBase* p) { m_pIWindowAnimateBase = p; }
    IWindowAnimateBase*  GetIWindowAnimateBase() { return m_pIWindowAnimateBase; }

    virtual void  Initialize();

    void  SetAnimateWindow(IWindowBase* pWindow);
    void  UpdateWindowSourceImage();

    void  GetSrcBitmap_Layered();
    void  GetSrcBitmap_Normal();

    int  GetWndTransparentType() { return m_nWndTranslateType; }

protected:
    IWindowAnimateBase*    m_pIWindowAnimateBase;
    WindowBase*            m_pWindow;
    LayeredAnimateWindow*  m_pLayeredWindow;

	CRect   m_rcWindowInBuffer;    //  ����������image�е�λ�ã�Ĭ��Ϊ0,0�����ڱ��߽���textmapping�Լ�ʵ���ˣ��ó�Ա���ٸ���
    Image   m_SrcImage;            //  ԭ���ڿ���
    BYTE*   m_pSrcBits;
    int     m_nSrcPitch;

    int     m_nSrcWndWidth;
    int     m_nSrcWndHeight;

    int     m_nWndTranslateType;   // ����͸�����ͣ�������layered��aero��

    Storyboard*  m_pStroyboard;
};

}