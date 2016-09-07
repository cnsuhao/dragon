#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
    interface IButton;
}
class CToolBar : public IWindow
{
public:
    CToolBar();
    ~CToolBar();

    UI_BEGIN_MSG_MAP_Ixxx(CToolBar)
        UIMSG_BN_CLICKED2(m_pBtnSave, OnBtnSave)
        UIMSG_BN_CLICKED2(m_pBtnNewWindow, OnBtnNewWindow)
        UIMSG_BN_CLICKED2(m_pBtnUndo, OnBtnUndo)
        UIMSG_BN_CLICKED2(m_pBtnRedo, OnBtnRedo)
        UIMSG_BN_CLICKED2(m_pBtnPreview, OnBtnPreview)
        
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    bool  TestCommandIsValid(long ld);
    void  OnDirtyChanged(bool bDirty);
    void  OnProjectOpened();
    void  OnProjectClosed();
    void  OnUndoRedoCountChanged(long* pKey);
	void  UpdateBtnState();

    long*  GetCurViewKey();

protected:
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnInitialize();
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);

    void  OnBtnSave();
    void  OnBtnNewWindow();
    void  OnBtnUndo();
    void  OnBtnRedo();
    void  OnBtnPreview();


private:
    IButton*  m_pBtnSave;
    IButton*  m_pBtnNewWindow;
    IButton*  m_pBtnUndo;
    IButton*  m_pBtnRedo;

    IButton*  m_pBtnPreview;
};

