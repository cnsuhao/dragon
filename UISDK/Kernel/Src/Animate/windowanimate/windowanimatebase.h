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

	CRect   m_rcWindowInBuffer;    //  窗口数据在image中的位置（默认为0,0），哨兵边界由textmapping自己实现了，该成员不再负责
    Image   m_SrcImage;            //  原窗口快照
    BYTE*   m_pSrcBits;
    int     m_nSrcPitch;

    int     m_nSrcWndWidth;
    int     m_nSrcWndHeight;

    int     m_nWndTranslateType;   // 窗口透明类型（正常，layered，aero）

    Storyboard*  m_pStroyboard;
};

}