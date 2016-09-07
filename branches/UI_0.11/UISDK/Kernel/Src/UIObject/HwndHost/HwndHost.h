#pragma  once
#include "UISDK\Kernel\Src\UIObject\Control\control.h"
#include "UISDK\Kernel\Inc\Interface\ihwndhost.h"

namespace UI
{

//
//  包装一个原始的HWND对象
//
// [2013.12.16] HwndHost也可能包装一个panel下的子对象，例如包装一个IEWrap
//
//
class HwndHost : public Control
{
public:
	UI_DECLARE_OBJECT(HwndHost, OBJ_HWNDHOST);

	HwndHost();
	~HwndHost();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_VISIBLE_CHANGED(OnVisibleChanged)
		UIMSG_WM_HITTEST(OnHitTest)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
		UIMSG_WM_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

    void  SetIHwndHost(IHwndHost* p) { m_pIHwndHost = p; };
	void  Attach(HWND hWnd);
	HWND  Detach();

protected:
	UINT  OnHitTest(POINT* pt, POINT*  ptInChild);
	void  OnVisibleChanged(BOOL bVisible, IObject* pObjChanged);
	void  OnSerialize(SERIALIZEDATA* pData);
    void  GetDesiredSize(SIZE* pSize);

	virtual void  SetObjectPos( int x, int y, int cx, int cy, int nFlag=0) override;
	virtual void  virtualSetVisibleEx(VISIBILITY_TYPE eType) override;
	virtual void  virtualSetEnable(bool b) override;
	virtual bool  IsVisible() override;
	virtual bool  IsEnable() override;

public:
	bool  SubclassWindow();
	bool  UnSubclassWindow();
	void  OnSubclassWindow();
	void  OnUnsubclassWindow();

protected:
	static LRESULT CALLBACK  _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT  WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT  DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    void  UpdateTabStopStyle();
public:
    IHwndHost*  m_pIHwndHost;
	HWND  m_hWnd;

protected:
	CWndProcThunk  m_thunk;   // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
	WNDPROC  m_oldWndProc;    // 该窗口的原始窗口过程
};

}