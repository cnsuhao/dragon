#pragma once
#ifdef COMPILE_Win32Edit
namespace UI
{
enum EditDestroyFlag
{
    EDF_NONE,
    EDF_ACCEPT,
    EDF_CANCEL,
    EDF_DISCARD,  // �ⲿ���õģ�����ֹͣ�༭�����Ҳ�������Ϣ
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
    int         m_nFontHeight;  // ����߶�
    COLORREF    m_colorBkg;
    CRect       m_rcScreen;     // ��������Ļ�ϵ�����
    bool        m_bDestroying;  
    long        m_lLimitText;   // 0������
    bool        m_bByteLimit;   // �Ƿ��ֽڽ�������

    CWndProcThunk  m_thunk;
    WNDPROC  m_oldWndProc; 
};

}
#endif