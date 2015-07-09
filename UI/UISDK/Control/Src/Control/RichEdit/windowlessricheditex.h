#pragma once
#include "windowlessrichedit.h"


// ����ʵ�ָ���OLE�Ĳ��뼰����ҵ����

namespace UI
{
class WindowlessRichEditEx : public WindowlessRichEdit
{
public:
    WindowlessRichEditEx();
    ~WindowlessRichEditEx();

    BEGIN_MSG_MAP_EX(WindowlessRichEditEx)
        MSG_WM_SIZE( OnSize )
        MESSAGE_HANDLER_EX( UI_WM_GIFFRAME_TICK, OnGifFrameTick )
        CHAIN_MSG_MAP(WindowlessRichEdit)
    END_MSG_MAP()

public:
    bool  AppendEncodeText(LPCTSTR szText, int nSize);
    bool  InsertEncodeText(int nPos, LPCTSTR szText, int nSize);
    bool  InsertEncodeText(LPCTSTR szText, int nSize);
    bool  ReplaceSelectionEncodeText(LPCTSTR szText, int nSize);

    bool  InsertGif(LPCTSTR szGifPath);
    bool  InsertSkinGif(LPCTSTR szGifId);
    IPictureREOle*  InsertPicture(PictureREOleInfo* pInfo);

    void  EnableBubbleMode(bool b);
    bool  IsBubbleMode();
    REBubble*  CreateBubble(BubbleType eType, BubbleAlign eAlign, int nStartCP, int nEndCP);
    void  InsertBubbleSpace();

    HANDLE  InsertMessageStart(BubbleType eType, BubbleAlign eAlign);
    IREBubble*  InsertMessageFinish(HANDLE hInsertStart);

protected:
    void    OnSize(UINT nType, CSize size);
    LRESULT OnGifFrameTick(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    virtual void  OnCreate() override;
    virtual void  PreDraw(HDC hDC, RECT* prcDraw) override;
    virtual void  PostDraw(HDC hDC, RECT* prcDraw) override;
    virtual bool  PasteEncodeText(LPWSTR szData) override;
    virtual void  OnOleSizeChanged(REOleBase* pItem) override;

	void  TxSetCursor(HCURSOR hcur, BOOL fText) override;

protected:
    BubbleManager*  m_pBubbleMgr;
};
}