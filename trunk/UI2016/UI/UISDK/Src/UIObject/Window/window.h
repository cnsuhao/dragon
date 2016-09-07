#pragma once

#include "Src\UIObject\Window\windowbase.h"

namespace UI
{

class Window : public WindowBase
{
public:
	Window(IWindow* p);
	~Window(void);

	UI_BEGIN_MSG_MAP()
// 		UIMSG_BN_CLICKED(XML_SYS_BTN_CLOSE,     OnSysClose )
// 		UIMSG_BN_CLICKED(XML_SYS_BTN_MINIMIZE,  OnSysMinimize )
// 		UIMSG_BN_CLICKED2(m_pBtnSystemMaximize, OnSysMaximize )
// 		UIMSG_BN_CLICKED2(m_pBtnSystemRestore,  OnSysRestore )
// 		UIMSG_SKINCHANGED(_OnSkinChanged)
		MSG_WM_LBUTTONDBLCLK(_OnLButtonDblClk)
		MSG_WM_CLOSE(OnClose)
        UIMSG_QUERYINTERFACE(Window)
	UI_END_MSG_MAP_CHAIN_PARENT(WindowBase)

    IWindow* GetIWindow() { return m_pIWindow; }

	// 内部方法
protected:
	virtual void  virtualInnerInitWindow() override;
    virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);

	void  _OnSkinChanged();
	void  _OnLButtonDblClk(UINT nFlags, POINT point);

	// 模拟系统标题栏按钮
	void  OnSysClose();
	void  OnSysMinimize();
	void  OnSysMaximize();
	void  OnSysRestore();
	void  HandleSysBtnStyle();
    void  OnClose();

protected:
    IWindow*   m_pIWindow;
	IObject*   m_pBtnSystemMaximize;
	IObject*   m_pBtnSystemRestore;
	IObject*   m_pBtnSystemMinimize;
	IObject*   m_pBtnSystemClose;
};



}