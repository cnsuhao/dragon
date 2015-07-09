#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include <tom.h>

#include "UISDK\Control\Src\Control\Caret\caret.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"


//#pragma comment(lib, "Riched20.lib")  // -- vs2012�ϱ��벻��
typedef HRESULT (_stdcall *pfuncCreateTextServices)
(
    IUnknown *punkOuter,
    ITextHost *pITextHost, 
    IUnknown **ppUnk
);

//  RichEdit����Callback��ʵ�֣����Բο�MFC CRichEditView
//  RichEidt��ole view��ʵ�֣����Բο�ATL����һ���ؼ������ʵ�ִ���

// 
//
//	Q1. �޴���RE����ô�����ģ�
//		a. ����CreateTextSevices()
//      b. ??
//
//  Q2. �޴���RE����ô���Ƶ���������ģ�
//		a. �õ�һ���ڴ�DC?
//		b. ����itextservices->txdraw����������ṩһ��clientrect�����������ڸô�����������ʾһ����ɫ������
//		c. 
//
//	Q3. �޴���RE����ô�����Լ��ڴ����ϵķ�Χ�ģ�
//		a. Draw��ʱ�������clientrect������ʾ�ķ�Χ
//		b. Ȼ��������ʱ���жϵ�ǰ����Ƿ�λ�ڴ����ڻ��߷���TxGetClientRect
//	
//	Q4. ��ô�������֣�
//		a. ��WM_CHAR��Ϣ���ݸ�service text���ɡ���Ҫע�����ģ̬�Ի������ղ���WM_CHAR��Ϣ�ġ�
//
//	Q5. �������ֺ�����ôˢ����ʾ�ģ�
//		a. �ڲ�������TxInvalidateRect�ӿڣ��ڸýӿ��У������Լ�ȥʵ��Ϊ::InvalidateRect
//
//	Q6. ��ô����������ʽ��������б༭���ܣ�
//		a. TxGetPropertyBits ���ص�
//
//	Q7. ����ѡ������ʱ�����ֹ���λ����ʾ��̫׼ȷ����ô��������ѡ������ʱ����ʾ��ꣿ
//		a. ��ʵ���ʱ��TxCreateCaret(HBITMAP hbmp ���hbmp�ǲ�Ϊ�յģ�ʹ����������caret�Ͳ�����ʾ��why?
//
//  Q8. Ϊʲô����CCaretWindowʵ�ֹ��ʱ������������������ʱ��ϵͳ������أ�
//		
//
//	Q9. �������ô���ٵģ�
//		a. ��text service����kill focus��Ϣ�����һ��ô���set focus��Ϣ�������ٴεõ�����ʱ��겻��ʾ��
//      b. re�ڲ��յ�wm_killfoucsʱ�������::destroycaret
//
//  Q10.Ϊʲô����textservice����SETFOCUS��Ϣ��ȴ������ӦTxCreateCaret��Ϣ
//		a. ��Ϊû�е���OnTxInPlaceActivate
//
//	Q11.�ڿؼ�λ�øı����ôȥͬ������λ�ã�
//      a. ��txt service����һ��֪ͨ��m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
//
//  Q12.Ϊʲô��קʱ��ˢ�»�����쳣������Ӻ�ɾ�������Ƿֿ�ˢ�µ�
//      ��Ӧ��Ҳ��ˢ������δ�����ԭ��
//
//  Q13.�����ʾ��������
//      ��TxGetScrollBars�У��������m_dwStyle��ʽ�������Ҫ��m_dwStyle��ָ��
//      WM_HSCROLL,WM_VSCROLL...
//
//  Q14.OLE����������.
//      ��Ϊrichedit��undo���ܣ������һֱ�������ڴ��У�ֱ��richedit�رա�????
//
//  Q15.Rich Edit ��������� 
//
//		Rich Edit�ؼ�ע�����ּ������ʽ��"���ı���ʽ(RTF)"��һ�ֽ���"RichEdit �ı������"�ĸ�ʽ��Ӧ�ó���
//      ����ʹ��RegisterClipboardFormat������ע����Щ��ʽ����ȡֵΪCF_RTF��CF_RETEXTOBJ
//
//   Q16.IOleRichEditCallBack�е�QueryAcceptData��QueryInsertData��ʲô����?
//       
//      QueryInsertData��Ե�����richedit��InsertObject����һ��OLEʱ������
//      QueryAcceptData����ճ������קʱ���ᴥ����
//
//   Q17.������richedit�Ļ���Ч��
//       1. ע��txinvalidaterect�еĵ�һ������������������ʱ��prc��ֻ��һ�е����򣬶���������client rect.
//          �ڷֲ㴰���ϻ���ʱ���Ը���������м���ˢ��
//
//   Q18.�س�ʱ��������˸��ô���£�
//       ���WindowlessRichEdit::TxSetScrollPos����ע��
//
//   Q19. ��ô������ʾwindowless richedit
//      
//   Q20. ����WM_IME_COMPOSITION�����Ϣ�ᵼ�����뷨����һ���ְ��س����Ȼ��ʾ������
//        ����Ϊ�ڶԻ��򴰿��з�����һ��0�����¶Ի������ȥ����Ĭ�ϵ�dialog proc�����¶෢����һ��WM_CHAR��Ϣ
//   Q21. ΢��ƴ�����뷨������
//
//        . ����ʱ����ʾ���봰��
//        . ����ֻ��ʾ����ģʽ������������ģʽ
//        ������������WM_IME_COMPOSITION�����ԡ���һ��������û�������Ϣ���ݸ�Ĭ�ϵĴ��ڹ��̵��¡�
//        �ڶ���������û�н�WM_IME_COMPOSITION��Ϣ���ݸ�RE���£�
//        Ҳ�п�����û�е���OnTxInPlaceActivate����
//
//        . ��ʾ����ģʽ���������б�û�и���
//          REû�д���WM_IME_REQUEST����
//
//        . googleƴ���ĺ�ѡ�б�λ�ò���ȷ
//        . ��Ʒ��ʲ���ʾ����С���ڣ�ֻ��ʾ��ѡС����
//          ԭ����Щime��Ϣ��txsendmessage�᷵��S_FALSE����ʱ��Ҫ����Ϣ�������ݸ�DefWindowProc
//        . ��WM_IME_COMPOSITION��Ϣ�������ݸ�DefWindowProc֮�󣬷��ֳ���΢��ƴ�������������뷨���ظ������ˡ�
//          һ������WM_IME_COMPOSITION�У�һ������WM_CHAR��
//          ԭ��δ��WM_IME_CHAR��Ϣ���ݸ�RE����
//
//   Q22. RE����ʹ��ʲô��������ϵ�������
//        RE����������������ڴ��ڵġ����SetPos��RE�д��ݵ�Ҳ�Ǵ��ڣ�������ʱҪת��Ϊobject client
//
//   Q23. RE��С�ı�ʱ����������ݲ�����׶˵����⣬ʲôԭ��
//        RE��С�ı�ʱ�������е�ͼƬ�������У�����ͼƬ������˸��
//
//   Q24. ��ν�ֹrichedit������ק
//         ES_NOOLEDRAGDROP <--> TXTBIT_DISABLEDRAG 
//
namespace UI
{

class  RichEdit;
class  REOleBase;
class  REOleManager;
class  REBubble;
class  BubbleManager;

class ITextHostImpl : public ITextHost
{
public:
	ITextHostImpl();
	~ITextHostImpl();
protected:
	void Destroy();
public:

	// ITextHost Interface
	virtual HDC 		TxGetDC();
	virtual INT			TxReleaseDC(HDC hdc);
// 	virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);
// 	virtual BOOL 		TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
// 	virtual BOOL 		TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
// 	virtual BOOL 		TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
//	virtual void		TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual void 		TxViewChange(BOOL fUpdate);
//	virtual BOOL		TxShowCaret(BOOL fShow);
// 	virtual BOOL 		TxSetTimer(UINT idTimer, UINT uTimeout);
// 	virtual void 		TxKillTimer(UINT idTimer);
	virtual void		TxScrollWindowEx (INT dx,INT dy,LPCRECT lprcScroll,LPCRECT lprcClip,HRGN hrgnUpdate,LPRECT lprcUpdate,UINT fuScroll);
	virtual void		TxSetCapture(BOOL fCapture);
//	virtual void		TxSetFocus();
	virtual void 	    TxSetCursor(HCURSOR hcur, BOOL fText);
	virtual BOOL 		TxScreenToClient (LPPOINT lppt);
	virtual BOOL		TxClientToScreen (LPPOINT lppt);
	virtual HRESULT		TxActivate( LONG * plOldState );
	virtual HRESULT		TxDeactivate( LONG lNewState );
//	virtual HRESULT		TxGetClientRect(LPRECT prc);
//	virtual HRESULT		TxGetViewInset(LPRECT prc);
	virtual HRESULT 	TxGetCharFormat(const CHARFORMATW **ppCF );
	virtual HRESULT		TxGetParaFormat(const PARAFORMAT **ppPF);
	virtual COLORREF	TxGetSysColor(int nIndex);
//	virtual HRESULT		TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT		TxGetMaxLength(DWORD *plength);
	virtual HRESULT		TxGetScrollBars(DWORD *pdwScrollBar);
	virtual HRESULT		TxGetPasswordChar(TCHAR *pch);
	virtual HRESULT		TxGetAcceleratorPos(LONG *pcp);
	virtual HRESULT		TxGetExtent(LPSIZEL lpExtent);
	virtual HRESULT 	OnTxCharFormatChange (const CHARFORMATW * pcf);
	virtual HRESULT		OnTxParaFormatChange (const PARAFORMAT * ppf);
	virtual HRESULT		TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	// Far East Methods for getting the Input Context
	//#ifdef WIN95_IME
	virtual HIMC		TxImmGetContext();
	virtual void		TxImmReleaseContext( HIMC himc );
	//#endif
	virtual HRESULT		TxGetSelectionBarWidth (LONG *lSelBarWidth);

	// �ⲿ���÷��� �����ֲο�microsoft windowlessRE���̣�
	bool  IsPasswordMode() { return m_fPassword; }
	void  SetPasswordMode(bool b);
	WCHAR SetPasswordChar(WCHAR chPasswordChar);
	LONG  SetAccelPos(LONG l_accelpos);
    bool  SetCharFormatByLogFont(LOGFONT* plf, COLORREF color);
	bool  SetCharFormat(CHARFORMAT2* pcf);
	void  SetCharFormatEx(UINT nPart, CHARFORMAT2* pcf);
    void  SetSelectionCharFormat(CHARFORMAT2* pcf);
    void  SetSelectionParaFormat(PARAFORMAT2* ppf);
    void  GetDefaultCharFormat(CHARFORMAT2* pcf);
    void  CopyDefaultCharFormat(CHARFORMAT2* pcf);
    void  CopyDefaultParaFormat(PARAFORMAT2* ppf);
    void  ResetSelectionCharFormat();
    void  ResetSelectionParaFormat();

	void  InitDefaultParaFormat();
    void  SetDefaultParaAlign(long eAlign/* = PFA_LEFT*/);
	bool  IsWordWrap() { return m_fWordWrap; }
	void  SetWordWrap(bool fWordWrap);
	DWORD GetMaxLength() { return m_dwMaxLength; }
	void  SetMaxLegnth(DWORD dw);
	LONG  GetSelBarWidth();
	LONG  SetSelBarWidth(LONG l_SelBarWidth);
	bool  GetRichTextFlag() { return m_fRich; }
    void  SetMultiLine(bool b);
    bool  IsMultiLine();
    void  EnableUpdate(bool b);
    void  EnableDrag(bool b);

    LRESULT GetLangOptions();
    bool  IsDualFont();
    void  SetDualFont(bool useDualFont);

	IRichEditOle* GetRichEditOle() { return m_spOle; }
    LRESULT  RESendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0, LRESULT* pInvokeResult=NULL);
protected:

	// unknown attribute
// 	SIZE    m_sizeExtent;        // text service ����ʵ�����ŵĲ�����Each HIMETRIC unit corresponds to 0.01 millimeter.
	int     m_nxPerInch;
	int     m_nyPerInch;    
	LONG	m_laccelpos;         // Accelerator position

	// ��֪����
	DWORD   m_dwStyle;           // �༭����ʽ
	WCHAR	m_chPasswordChar;    // Password character, TODO: �ýӿ�δ���Թ�
	DWORD   m_dwMaxLength;       // ����������ݳ���
	LONG    m_lSelBarWidth;      // ������VS����ࣨ��ʾ�����֣���ר�����ڵ��ѡ��һ�еĶ���
	
	unsigned  m_fWordWrap:1; // Whether control should word wrap
	unsigned  m_fRich:1;     // Whether control is rich text
	unsigned  m_fRegisteredForDrop:1; // whether host has registered for drop
	unsigned  m_fPassword:1; // 
    unsigned  m_fShowSelectionInactive:1;
    unsigned  m_fDisableDrag:1;

	CHARFORMAT2W  m_cf;          // Default character format
	PARAFORMAT2   m_pf;          // Default paragraph format

	//  ������Դ������
	CComPtr<ITextServices>   m_spTextServices;
	CComPtr<IRichEditOle>    m_spOle;
    CComPtr<ITextDocument2>  m_spTextDoc;

	HWND   m_hParentWnd; // ���ڵĴ��ھ��
    HDC    m_hDCCache;   // ����ÿ�ζ�getdc
};

interface ITextEditControl
{
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;
};

 
// �����ж�
#define PRE_HANDLE_MSG() \
	{ \
		SetMsgHandled(TRUE); \
		lResult = OnPreHandleMsg(hWnd,uMsg,wParam,lParam); \
		if(IsMsgHandled()) \
			return FALSE; \
	}

// Ĭ�ϴ���
#define POST_HANDLE_MSG() \
	{ \
		SetMsgHandled(TRUE); \
		lResult = OnPostHandleMsg(hWnd,uMsg,wParam,lParam); \
		if(IsMsgHandled()) \
			return TRUE; \
	}

class  WindowlessRichEdit : public ITextHostImpl
	                           , public ITextEditControl
	                           , public IRichEditOleCallback
                               , public ICaretUpdateCallback
{
public:
	WindowlessRichEdit();
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// ����Ӹ�������ת����������Ϣ

	BEGIN_MSG_MAP_EX(WindowlessRichEdit)
		PRE_HANDLE_MSG()
		MSG_WM_SETCURSOR( OnSetCursor )
		MSG_WM_KILLFOCUS( OnKillFocus )
        MSG_WM_SETFOCUS( OnSetFocus )
        MSG_WM_SIZE( OnSize )

		MESSAGE_HANDLER_EX( WM_KEYDOWN,  OnKeyDown )
		MESSAGE_HANDLER_EX( WM_CHAR,     OnChar )
		MESSAGE_RANGE_HANDLER_EX( WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage )
        MESSAGE_HANDLER_EX( WM_MOUSELEAVE, OnMouseMessage )
		MESSAGE_HANDLER_EX( WM_VSCROLL,  OnScroll )
		MESSAGE_HANDLER_EX( WM_HSCROLL,  OnScroll )
        MESSAGE_HANDLER_EX( UI_WM_DROPTARGETEVENT, OnUIDropTargetEvent )
		MESSAGE_HANDLER_EX( WM_TIMER,    OnTimer )
        
        // ���뽫WM_IME_STARTCOMPOSITION���ݸ�RE������΢��ƴ�����뷨��������ʹ�ã�����ģʽ�����ã�
        MESSAGE_HANDLER_EX( WM_IME_STARTCOMPOSITION, OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_ENDCOMPOSITION, OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_COMPOSITION, OnDefaultHandle )
        UIMESSAGE_HANDLER_EX( WM_IME_REQUEST, OnDefaultHandle )
        UIMESSAGE_HANDLER_EX( WM_IME_CHAR, OnDefaultHandle )

//		POST_HANDLE_MSG()		
	END_MSG_MAP()

protected:
	LRESULT OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	BOOL    OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUIDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnKillFocus(HWND wndFocus);
    void    OnSetFocus(HWND wndOld);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void    OnSize(UINT nType, CSize size);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool    DoGifOleUpdateRequst();

protected:
	// *** IRichEditOleCallback methods ***
 	virtual HRESULT __stdcall GetNewStorage(LPSTORAGE FAR * lplpstg);
 	virtual HRESULT __stdcall GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame, 
        LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo) ;
 	virtual HRESULT __stdcall ShowContainerUI(BOOL fShow) ;
 	virtual HRESULT __stdcall QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) ;
 	virtual HRESULT __stdcall DeleteObject(LPOLEOBJECT lpoleobj) ;
 	virtual HRESULT __stdcall QueryAcceptData(LPDATAOBJECT lpdataobj, 
        CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict) ;
 	virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) ;
 	virtual HRESULT __stdcall GetClipboardData(CHARRANGE FAR * lpchrg,
        DWORD reco, LPDATAOBJECT FAR * lplpdataobj) ;
 	virtual HRESULT __stdcall GetDragDropEffect(BOOL fDrag, 
        DWORD grfKeyState, LPDWORD pdwEffect) ;
 	virtual HRESULT __stdcall GetContextMenu(WORD seltype,
        LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu) ;

    // ICaretUpdateCallback 
    virtual void  CaretInvalidateRect(RECT* prc, int nCount);
    virtual bool  NeedFixItalicCaretOffset() {return true; }

public:

	// Function
	void  BindControl(RichEdit* p);
	bool  Create(HWND hWndParent);
	void  Draw(HDC, bool bDrawShadow);
    void  DrawShadow(HDC hDC, CRect* prcClient);
	void  RealDraw(HDC hDC, RECT* prcClient);
    void  InvalidateRect(RECT* prcWnd);
	bool  HitTest(POINT pt);
    IRichEdit*   GetControlPtr();
    ITextDocument2*  GetDocPtr();
    void  SetTextFilterUint(UINT nMax);
    void  SetTextFilterDigit();
    void  SetTextFilterPhoneNumber();
    void  SetTextFilterAccount();

    // ��������
    bool  SetText(LPCTSTR szText);
    int   GetText(TCHAR* szBuf, uint nLen);
    void  GetText(BSTR* pbstr);
    void  GetTextW(IBuffer** ppBuffer);
    const char*  GetRtfBuffer(int nFlags=0);
    void   ReleaseRtfBuffer(const char*);
	int   GetTextRange(TCHAR*  szBuf, uint nStart, uint nSize);
    int   GetTextRange(BSTR* pbstr, uint nStart, uint nSize);
    int   GetTextLengthW();
    bool  GetEncodeTextW(IBuffer** ppBuffer);
    bool  GetEncodeTextW(LPCTSTR bstrContent, int ncpStart, IBuffer** ppBuffer);
    bool  AppendText(LPCTSTR szText, int nLength=-1);
    bool  InsertText(int nPos, LPCTSTR szText, int nLength=-1);
    bool  InsertText(LPCTSTR szText, int nLength=-1);
    bool  ReplaceSelectionText(LPCTSTR szText, int nSize);
    bool  IsReadOnly() { return (m_dwStyle & ES_READONLY) != 0; }
    void  SetReadOnly(bool fReadOnly);
    void  SetReadOnlyLight(bool fReadOnly);
    void  SetRichTextFlag(bool b);

    // message helper
    void  SetSel(int nPos, int nLen);
    void  SetSel(int nPos);
    void  SetSel2(int nStartChar, int nEndChar);
    void  SetSel(CHARRANGE &cr);
    void  SelectAll();
    void  SelectEnd();
    void  ScrollEnd();
    void  GetSel(int* pnPos, int* pnLen);
    int   GetSelectionStart();
    int   LineIndex(int nLineNum = -1);
    int   LineFromChar(int ncp);
    int   LineLength(int nLineCP = -1);
    int   GetLineCount();
    int   GetLine(int nLineIndex, TCHAR* szBuffer, int nBufferSize);
    int   GetFirstVisibleLine();
    void  GetVisibleCharRange(int* pnFirstCp, int* pnLastCp);
    void  GetCharRect(int ncp, RECT* prc);

    void  DisableUndoOp();
    int   CharFromPos(POINT pt);
    void  PosFromChar(UINT ncp, POINT* pPos);
    void  SetUndoLimit(int nUndoSize);
    void  SetLimitText(int nLimit);
	void  EnableWordBreak(bool b);

    void  KillFocus();
    void  SetFocus();
    bool  IsFocus();
    bool  IsScrollBarVisible();

    // event mask
    long  GetEventMask();
    void  SetEventMask(long n);
    void  ModifyEventMask(long nAdd, long nRemove);
    void  SetAutoResize(bool b, int nMaxSize = 0);
	
    // Ole����
    WORD  GetSelectionType();
    REOleBase*  GetSelectionOle();
    REOleBase*  GetOleByCharPos(uint ncp);
    int   GetOleCharPos(IOleObject* pOleObj);
    int   GetOleCharPos(REOleBase* pOle);
    IOleObject*  CreateOleObjectFromData(LPDATAOBJECT pDataObject, 
            bool bOleCreateFromDataOrOleCreateStaticFromData = true,
            OLERENDER render = OLERENDER_DRAW, 
            CLIPFORMAT cfFormat = 0, 
            LPFORMATETC lpFormatEtc = NULL);
//  int   GetObjectTypeByOleObject(LPOLEOBJECT pOleObject); 

    bool  InsertOleObject(REOleBase* pItem);
    bool  InsertComObject(REFCLSID clsid);

    HRESULT DoPaste(LPDATAOBJECT pDataObject, BOOL fReally, CLIPFORMAT* pOutClipFormat);
    void  PasteTextFilter(LPDATAOBJECT pDataObject, bool& bHandled);

    void  RevokeDragDrop(void);
    void  RegisterDragDrop(void);
    bool  CanDragDrop();

    // ������WindowlessRichEditEx���麯��
    virtual void  OnCreate(){};
    virtual void  PreDraw(HDC hDC, RECT* prcDraw);
    virtual void  PostDraw(HDC hDC, RECT* prcDraw) {};
    virtual void  OnOleSizeChanged(REOleBase* pItem) {};

    // TODO: ��Ϊ�˽���ڱ������޷����������ReplaceSelectionEncodeText
    virtual bool  PasteEncodeText(LPWSTR szData) {return false;};

public:
	// IUnknown  Interface
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

	// ITextHost Interface
	// ��Ҫ���ݿؼ����Խ��ж��ƵĽӿڷ�������ʵ�֣������ӿڽӿڷ���ITextHostImpl��ʵ��
	virtual BOOL    TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL    TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	virtual BOOL    TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
	virtual BOOL 	TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	virtual HRESULT TxGetClientRect(LPRECT prc);
	virtual void    TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT	TxGetViewInset(LPRECT prc);
    virtual BOOL 	TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void 	TxKillTimer(UINT idTimer);
    virtual void	TxSetFocus();
    virtual BOOL	TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL	TxSetCaretPos(INT x, INT y);
    virtual BOOL    TxShowCaret(BOOL fShow);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);

protected:
	void    ClearOleObjects();
    void    OnRequestResize(RECT* prc);
    void    SyncScrollPos();
    HRESULT DefTxNotify(DWORD iNotify, void* pv);

protected:
    RichEdit*    m_pRichEdit;
    IRichEdit*   m_pIRichEdit;
	REOleManager*  m_pOleMgr;

    ITextFilter*    m_pTextFilter; // ���ֹ��ˡ�����ʵ��������С�������

	int    m_nLastTimerKickCount; // ˢ��FPS���ơ����ﻹ��Ҫ�������Ż�����ʹֻ��һ��GIF��Ҳ��Ҫ����client rectˢ����
	
    bool   m_bDuringTxDraw;       // ��־��ǰ�Ƿ����ڵ���TxDraw��������ڼ�Ĺ�����������Ի����ͣ�����Ӧ����ȥˢ��RE�ˣ�������ѭ��
    bool   m_bFocus;
    bool   m_bScrollBarVisible;

    int    m_nAutoResizeMaxSize;  // ����Ӧ��Сʱ�����ֵ��singleline��Ӧwidth,multiline��Ӧheight��С��0��ʾ������

protected:
	// ��windowless richeditҪ���õĳ�ʼ������
	static LPCTSTR  GetLibraryName();
	void   InitRichEidtDll();
	void   ReleaseRichEidtDll();
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
	static pfuncCreateTextServices  s_funcCreateTextServices;
public:
	//RichEdit formats
	static UINT     s_cfRichTextFormat;      // CLIPFORMAT
	static UINT     s_cfRichTextAndObjects;  // CLIPFORMAT
};

}
