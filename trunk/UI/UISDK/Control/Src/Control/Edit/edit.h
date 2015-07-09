#pragma once
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Src\Control\Caret\caret.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"

//////////////////////////////////////////////////////////////////////////
//
//                             �༭��ؼ�
//
//////////////////////////////////////////////////////////////////////////

//
//	2012.6.2
//		libo  ��д
//  2012.9.13
//      libo  �޸ķֲ㴰���¹���޷���ʾ������
//
//
//	TODO:
//		a. ��������ģʽ��������С����������
//		b. ��������ģʽ
//		c. ����CTRL+<- CTRL+-> ʶ�����Ĺ���
//		d. ˫��ѡ�еĹ���
//

#include <usp10.h>
#pragma comment(lib, "usp10.lib")

#ifndef _UNICODE 
	#error "usp10 requirs a unicode build."
#endif
namespace UI
{
class Edit;

//
//	��װ�༭�������
//
class EditData
{
public:
	EditData();
	~EditData();

public:
	void  BindToEdit(Edit* pEdit);

	void  SetText(LPCTSTR, bool bFilter, bool& bUpdate);
	void  ReplaceChar(const TCHAR& c, bool& bUpdate);
	void  ReplaceString(const String& str, bool& bUpdate);
	void  Delete(bool& bUpdate);
	void  BackSpace(bool& bUpdate);
    void  DeleteSelectionText(bool& bUpdate);
	void  SetCaret(int nCaret, bool bSetSelStart, bool& bUpdate);

	void  CutToClipboard();
	void  CopyToClipboard();
	void  PasteFromClipboard();
	void  GetPriorItemPos(int nCP, int* pPrior);
	void  GetNextItemPos(int nCP, int* pNext);

	void  SetMaxChar(long nMax);
    long  GetMaxChar();
	void  SetMaxChar2(int nMax, bool& bNeedUpdate);
	void  SetInsertMode(bool bInsertOrOverride);
	bool  GetInsertMode() { return m_bInsertMode; }

	void  GetText(String& str) { str = m_strText; }
	const String&  GetTextRef() { return m_strText; }
	LPCTSTR  GetText() { return m_strText.c_str(); }
	int   GetTextLength() { return (int)m_strText.length(); }
	int   GetCaretIndex() { return m_nCaret; }
	int   GetSelectionLength() { return abs(m_nCaret - m_nSelStart); }
	int   GetTextWidth()  { return m_nTextWidth; }
	void  GetSelectionInfo(int& nLeft, int& nRight) const;
	void  SetSelectionInfo(int nStart, int nEnd, bool& bUpdate);
	void  GetSelectionText(String& str);
	bool  IsSelectionExist();
    bool  Clear();

	bool  CP2X(int nCP, int* pX);
	bool  X2CP(int nX, int* pnCP, int* pbTrailOrLead);
	void  DestroyStringAnalysis();

	void  SetTextFilterDigit();
	void  ClearTextFilter();

protected:
	void  DeleteSelectionText();   // �ú����������ڲ�����,�������ݽ��д�����ɾ���ı�

	void  Fire_Text_Changed(bool bInvokeSetText);
	TextFilterResult  FilterString(LPCTSTR szSrc, String& strDest);
	TextFilterResult  FilterChar(const TCHAR& cSrc, TCHAR& cDest);

	bool  StringAnalysis();

private:
	String  m_strText;              // �༭�������
	long  m_nMaxChar;             // �������������ַ�����-1��ʾ������

	int   m_nSelStart;            // ѡ����ַ���㣬��û��ѡ��ʱ	m_nSelStart==m_nCaret	
	int   m_nCaret;               // ��ǰ����λ�ã�Ҳ��־��ѡ����End pos

	bool  m_bInsertMode;          // ����/��дģʽ

	// UniScribe��ر���
	SCRIPT_CONTROL	m_ScriptControl;		// For uniscribe
	SCRIPT_STATE	m_ScriptState;			// For uniscribe

	SCRIPT_STRING_ANALYSIS	m_Analysis;     // For cp2x, x2cp...
	int   m_nTextWidth;           // ��ǰ�ַ����ܳ���

	ITextFilter*  m_pTextFilter; // ���ֹ��ˡ�����ʵ��������С�������

	Edit*  m_pEdit;
};

class Edit : public MessageProxy
{
public:
	Edit();
	~Edit();

    UI_DECLARE_OBJECT(Edit, OBJ_CONTROL|CONTROL_EDIT)

	UI_BEGIN_MSG_MAP
 		UIMSG_WM_PAINT2(OnPaint)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
 		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_MOUSEMOVE(OnMouseMove)
 		UIMSG_WM_CHAR(OnChar)
		UIMSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)
		UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		UIMSG_WM_RBUTTONDOWN(OnRButtonDown)
        UIMSG_WM_RBUTTONUP(OnRButtonUp)
 		UIMSG_WM_SETFOCUS(OnSetFocus)
 		UIMSG_WM_KILLFOCUS(OnKillFocus)
		UIMSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
		UIMSG_WM_VISIBLE_CHANGED(OnVisibleChanged)
		UIMSG_WM_SHOWWINDOW(OnShowWindow)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMESSAGE_HANDLER_EX(WM_IME_STARTCOMPOSITION, OnStartComposition)
		UIMSG_WM_STATECHANGED2(OnStateChanged)
		UIMSG_CREATE_ACCESSIBLE(OnCreateAccessible)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Edit, IControl)

    void  SetIEdit(IEdit* p) { m_pIEdit = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IEdit*  GetIEdit() { return m_pIEdit; }

	// ��Ϣ����
protected:
    void  OnInitialize();
	void  OnPaint(IRenderTarget* hDC, RenderContext* pContext);
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void  OnSetFocus(IObject*);
	void  OnKillFocus(IObject*);
	void  OnWindowPosChanged(LPWINDOWPOS);
	void  OnShowWindow(BOOL bShow, UINT nStatus);
    UINT  OnGetDlgCode(LPMSG lpMsg);
	void  OnVisibleChanged(BOOL bVisible, IObject* pParent);
    LRESULT  OnStartComposition(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  OnStateChanged(UINT nMask);
	void  OnCreateAccessible(IUIAccessibleCreator*);

	void  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	void  OnMouseMove(UINT nFlags, POINT point);
	void  OnLButtonDblClk(UINT nFlags, POINT point);
	void  OnRButtonDown(UINT nFlags, CPoint point);
    void  OnRButtonUp(UINT nFlags, CPoint point);

	void  OnInputChar(UINT nChar);
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnKeyDown_Ctrl_A();
	void  OnKeyDown_Ctrl_X();
	void  OnKeyDown_Ctrl_C();
	void  OnKeyDown_Ctrl_V();
	void  OnKeyDown_Ctrl_Z();
	void  OnKeyDown_Ctrl_Y();
	void  OnKeyDown_Backspace(bool bCtrlDown);
	void  OnKeyDown_Delete(bool bCtrlDown);
	void  OnKeyDown_Left_Top(bool bCtrlDown);
	void  OnKeyDown_Right_Down(bool bCtrlDown);
	void  OnKeyDown_Home(bool bCtrlDown);
	void  OnKeyDown_End(bool bCtrlDown);
	void  OnKeyDown_Insert(bool bCtrlDown);
    void  OnKeyDown_Return();
    void  OnKeydown_Esc();
    void  OnKeydown_Tab();
   
    void  GetDesiredSize(SIZE* pSize); 
    void  OnSerialize(SERIALIZEDATA* pData);

public:
	int   GetTextLength();
    LPCTSTR  GetText();
	void  SetText(LPCTSTR szText, bool bUpdate);
	void  SetTextNoFilter(LPCTSTR szText, bool bUpdate);
	int   GetEditType();
	void  SetEditType(int n);

    void  DeleteSelectionText(bool bUpdate);
    bool  DoCut();
    bool  DoCopy();
    bool  DoPaste();
	void  SetSel(int nStartChar, int nEndChar);
	void  GetSel(int& nStartChar,int& nEndChar) const;
    void  Clear(bool bUpdate);

    bool  IsReadOnly();
    void  SetReadOnly(bool b, bool bUpdate);
	bool  IsRealFocus();
	
	void  SetTextFilterDigit();
	void  ClearTextFilter();

protected:
	void  EnterFocus();
	void  ExitFocus();
    long  GetTextHeight();
	
private:
	void  DrawNormal(
            IRenderTarget* pRenderTarget, 
            RenderContext* pContext);
	void  DrawFocus(IRenderTarget* pRenderTarget);

	void  CalcCaretPos(int nCaretIndex, bool& bUpdate);
	void  UpdateCaretByPos();
    IRenderFont*  GetRenderFont();
    long  GetCaretHeight();

	void  setText(LPCTSTR szText, bool bFilter, bool bUpdate);
    void  loadThemeEditBkgRender();

    void  LoadColor(LPCTSTR szColor) 
    {
        m_pIEdit->LoadColor(szColor, m_pColor); 
    }
    LPCTSTR  SaveColor() 
    {
        return m_pIEdit->SaveColor(m_pColor);
    }
    void  LoadSelectColor(LPCTSTR szColor)
    {
        m_pIEdit->LoadColor(szColor, m_pColorSelect); 
    }
    LPCTSTR  SaveSelectColor() 
    {
        return m_pIEdit->SaveColor(m_pColorSelect);
    }
    void  LoadSelectBkgColor(LPCTSTR szColor)
    {
        m_pIEdit->LoadColor(szColor, m_pColorSelectBk); 
    }
    LPCTSTR  SaveSelectBkgColor()
    {
        return m_pIEdit->SaveColor(m_pColorSelectBk);
    }

	// ����
protected:
	EditData  m_EditData;
    IEdit*  m_pIEdit;

	// ����
private:
    // �ؼ��н��㲻����edit���ܱ༭������instantedit��
	bool  m_bRealFocus;             

    // �Ƿ��������������϶�ѡ��
	bool  m_bMouseDrag;			

    // �Ƿ���Ҫ���´�ˢ�µ�ʱ����¹��λ��
	bool  m_bNeedUpdateCaretPos;    

	long  m_nXScroll;	            // �ַ������ĳ���
	long  m_nXCaretPos;				// ����λ�ã�����padding.left

    // ���߶Ȳ�Ӱ���ı�����λ�ã���Ӱ��ѡ�еı����߶�
    // ���־��л��ƣ�m_nTextHeightӰ���ı�����λ��
    long  m_nCaretHeight;			// ���ĸ߶�
    long  m_nTextHeight;            // �����Ӧ���ı��߶ȣ���'1'������

	friend class EditData;

	// ����
private:
	Color*   m_pColor;
	Color*   m_pColorSelect;
	Color*   m_pColorSelectBk;
	CCaret   m_caret;     // ����ʵ�ֲַ㴰���ϵĹ����ʾ

	long  m_lEditType;   // edit�ؼ����ͣ�����Ƕ����combo�е�edit
};
#if 0

#include <msctf.h>
class CUIElementSink;


// Text service framework���°汾��IMM������COM�ܹ���
// ���л���΢��ƴ�����뷨ʱ���õ�
class TsfHelper
{
public:
    TsfHelper();
    ~TsfHelper();

    void Init();
    void Release();

protected:
    bool Create();
    void Destroy();

private:
    long  m_lRef;
    ITfThreadMgrEx*  m_pThreadMgr;
    CUIElementSink*  m_pTsfSink;
    DWORD  m_dwUIElementSinkCookie;
};
TsfHelper*  GetTsf();

// Sink receives event notifications
class CUIElementSink : public ITfUIElementSink//, public ITfInputProcessorProfileActivationSink, public ITfCompartmentEventSink
{
public:
    CUIElementSink();
    ~CUIElementSink();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // ITfUIElementSink
    //   Notifications for Reading Window events. We could process candidate as well, but we'll use IMM for simplicity sake.
    STDMETHODIMP BeginUIElement(DWORD dwUIElementId, BOOL *pbShow);
    STDMETHODIMP UpdateUIElement(DWORD dwUIElementId);
    STDMETHODIMP EndUIElement(DWORD dwUIElementId);

//     // ITfInputProcessorProfileActivationSink
//     //   Notification for keyboard input locale change
//     STDMETHODIMP OnActivated(DWORD dwProfileType, LANGID langid, REFCLSID clsid, REFGUID catid,
//         REFGUID guidProfile, HKL hkl, DWORD dwFlags);
// 
//     // ITfCompartmentEventSink
//     //    Notification for open mode (toggle state) change
//     STDMETHODIMP OnChange(REFGUID rguid);

private:
    LONG _cRef;
};
#endif
}
