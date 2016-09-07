#ifndef IRICHEDIT_H_7B01BC76_7BCB_4fe3_AE3B_5404D421D2D1
#define IRICHEDIT_H_7B01BC76_7BCB_4fe3_AE3B_5404D421D2D1
#include "UISDK\Kernel\Inc\Interface\icontrol.h"
#include "iricheditole.h"
#include "iricheditbubble.h"
#include <richedit.h>

namespace UI
{
    
// ���ⲿת��֪ͨ��Ϣ
// message: UI_WM_RICHEDIT_TXNOTIFY   
// wParam:  DWORD iNotify
// lParam:  void* pv
// return:  HRESULT
#define UI_WM_RICHEDIT_TXNOTIFY   137181715

// ֪ͨ�ⲿ����༭������Ӧ��С
// wParam: RECT*  pRequestSize
// lParam: int* nDefaultDesiredWidth/Height (�ⲿ���޸�)
// return: 1�Ѵ��� 0δ��������nDefaultDesiredWidth���޸Ĵ�С
#define UI_EN_REQUESTAUTOSIZE  149251035

    // ֪ͨ�ⲿ�������Ӧ��СĬ�ϴ���
#define UI_EN_AUTOSIZE_DONE  149251053


class RichEdit;
class WindowlessRichEditEx;
interface UICTRLAPI IRichEdit : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IRichEdit, RichEdit);
    void  SetWindowlessRichEdit(WindowlessRichEditEx* pRE);

    bool  SetText(LPCTSTR szText);
    int   GetText(TCHAR* szBuf, int nLen);
    bool  AppendText(LPCTSTR szText, int nLength=-1);
    bool  GetEncodeTextW(IBuffer** ppBuffer);
    const char*  GetRtfBuffer(int nFlags=0);
    void   ReleaseRtfBuffer(const char*);
    bool  AppendEncodeText(LPCTSTR szText, int nSize);
    bool  InsertEncodeText(int nPos, LPCTSTR szText, int nSize);
    bool  InsertEncodeText(LPCTSTR szText, int nSize);
    HANDLE  InsertMessageStart(BubbleType eType, BubbleAlign eAlign);
    void  InsertMessageFinish(HANDLE hInsertStart);

    LONG  GetSelBarWidth();
    LONG  SetSelBarWidth(LONG l_SelBarWidth);
    bool  GetRichTextFlag();
    void  SetRichTextFlag(bool b);
    void  SetMultiLine(bool b);
    bool  IsMultiLine();
    bool  IsReadOnly();
    void  SetReadOnly(bool fReadOnly);
    void  SetAutoResize(bool b, int nMaxSize);
    bool  SetCharFormatByLogFont(LOGFONT* plf, COLORREF color);
    bool  SetCharFormat(CHARFORMAT2* pcf);
    void  GetDefaultCharFormat(CHARFORMAT2* pcf);
	void  SetCharFormatEx(UINT nPart, CHARFORMAT2* pcf);
    void  GetDefaultParaFormat(PARAFORMAT2* ppf);
    void  SetDefaultParaFormat(PARAFORMAT2* ppf);
	void  SetSel(int nPos, int nLen);
    void  SelectEnd();
    void  GetSel(int* pnPos, int* pnLen);
    void  DisableUndoOp();
    void  EnableDragDrop(bool b);
    bool  CanDragDrop();
    void  ScrollEnd();
	void  EnableWordBreak(bool b);
    
    void  EnableBubbleMode(bool b);
    bool  IsBubbleMode();
	void  AppendBubbleSpace();

    bool  InsertGif(LPCTSTR szGifFile);
    bool  InsertSkinGif(LPCTSTR szGifId);

    IPictureREOle*  InsertPicture(PictureREOleInfo* pInfo);
private:
    WindowlessRichEditEx*  m_pWindowlessRE;
};
void  UICTRLAPI UpdateREOleRes(
        LPCTSTR  szMD5, 
        REOleLoadStatus e, 
        LPCTSTR szPath);

enum TextFilterResult
{
    TFR_NOT_HANDLED,  // δ����ʹ��Ĭ��RE����
    TFR_HANDLED,      // �Ѵ�������ҪRE����
    TFR_REPLACE_TEXT  // δ���������ı������滻���ٴ���
};
enum TextFilterType
{
    TFT_NONE,
    TFT_DIGIT,
    TFT_UINT,
    TFT_PHONENUMBER,
    TFT_ACCOUNT,
};
interface ITextFilter
{   
    // ��RE����ʱ������Ҫ�ٻ�ԭ״̬�ˣ�bRestoreState����Ϊfalse
    // ���滻textfilterʱ������Ҫ��bRestoreState����Ϊtrue
    virtual void  Release(bool bRestoreState) = 0;
    virtual TextFilterType  GetType() = 0;

    virtual void   KeyFilter(UINT nKey, TextFilterResult& bHandled) = 0;
    virtual TCHAR   CharFilter(const TCHAR szSrc, TextFilterResult& bHandled) = 0;
    virtual LPCTSTR   StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled) = 0;
};


// ������ʽ����
#define RE_CF_ROOT             _T("RE")            // �����
#define RE_CF_VERSION          _T("version")       // �汾��Ϣ
#define RE_CF_FONT_BOLD        _T("bold")          // ����(1/0)
#define RE_CF_FONT_ITALIC      _T("italic")        // ����(1/0)
#define RE_CF_FONT_UNDERLINE   _T("underline")     // ����(1/0)
#define RE_CF_FONT_COLOR       _T("color")         // ��ɫ
#define RE_CF_FONT_FACE        _T("face")          // ����
#define RE_CF_FONT_HEIGHT      _T("height")        // �����С
#define RE_CF_PARA_ALIGN       _T("align")         // ���뷽ʽ
#define RE_CF_LINESPACING      _T("linespacing")   // �и�
#define RE_CF_LINEBEFORE       _T("linebefore")    // 
#define RE_CF_LINEAFTER        _T("lineafter")     // 

#define RE_CF_OLE_TEXT         _T("text")
#define RE_CF_OLE_GIF          _T("gif")
#define RE_CF_OLE_EMOTION      _T("emotion")
#define RE_CF_OLE_COM          _T("com")
#define RE_CF_OLE_PICTURE      _T("picture")
enum RICHEDIT_OLE_TYPE
{
    RICHEDIT_OLE_GIF_TEXT = 1,
    RICHEDIT_OLE_GIF_FILE = 2,
    RICHEDIT_OLE_EMOTION  = 3,
    RICHEDIT_OLE_COM  = 4,
    RICHEDIT_OLE_PICTURE  = 5,
};

#define RE_CF_OLE_TEXT_LENGTH   _T("length")       // �ı��ڵ㣬���ݴ�С
#define RE_CF_OLE_COM_CLSID     _T("clsid")        // com���clsid
#define RE_CF_OLE_ID            _T("id")           // ����id
#define RE_CF_OLE_PATH          _T("path")         // ·��

}

#endif // IRICHEDIT_H_7B01BC76_7BCB_4fe3_AE3B_5404D421D2D1