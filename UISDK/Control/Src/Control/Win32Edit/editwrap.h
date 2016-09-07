#pragma once
#ifdef COMPILE_Win32Edit
namespace UI
{
enum EditDestroyFlag
{
    EDF_NONE,
    EDF_ACCEPT,
    EDF_CANCEL,
    EDF_DISCARD,  // 外部调用的，立即停止编辑，并且不发送消息
};

class Win32Edit;
class EditWrap
{
public:
    EditWrap(Win32Edit* pControl);
    ~EditWrap();

    static EditWrap*  NewInstance(Win32Edit* pControl)
    {
        return new EditWrap(pControl);
    }

    bool  Create(RECT* prc, HWND hWndParent);
    void  Show();
    void  Hide();
    void  Destroy(EditDestroyFlag eFlag);

    void  SetFont(HFONT hFont);
    void  SetText(LPCTSTR szText, bool bSelAll);
    void  SetLimitText(int lLimitText, bool bByteLimit);

public:
    static LRESULT CALLBACK  ThunkWndProc( HWND hwnd, UINT uMsg, 
        WPARAM wParam, LPARAM lParam );
    LRESULT	WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:
    void  OnContextMenu();
    void  OnChar(UINT nChar, bool& bHandled);
	void  OnPaste(bool& bHandled);
    void  GetEditTextAndSelText(String& str, String& strSel);

public:
    HWND  m_hWnd;
    HWND  m_hWndParent;

protected:
    Win32Edit*  m_pWin32Edit;
    int         m_nFontHeight;  // 字体高度
    COLORREF    m_colorBkg;
    CRect       m_rcScreen;     // 窗口在屏幕上的坐标
    bool        m_bDestroying;  
    long        m_lLimitText;   // 0不限制
    bool        m_bByteLimit;   // 是否按字节进行限制

    CWndProcThunk  m_thunk;
    WNDPROC  m_oldWndProc; 
};

}
#endif