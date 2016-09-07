#ifndef IWINDOW_TRANSPARENT_MODE_H_BCA6EDC7_FAD0_4cce_95FA_0A1E00DCCC5C
#define IWINDOW_TRANSPARENT_MODE_H_BCA6EDC7_FAD0_4cce_95FA_0A1E00DCCC5C

namespace UI
{
interface ICustomWindow;

interface IWindowTransparent
{
    virtual BOOL  ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;
    virtual BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) = 0;

    virtual WINDOW_TRANSPARENT_TYPE  GetType() = 0;
    virtual bool  RequireAlphaChannel() = 0;
    virtual void  Init(ICustomWindow* pWnd) = 0;
    virtual void  Release() = 0;

    virtual void  Enable(bool b) = 0;
    virtual void  UpdateRgn() = 0;
    virtual bool  Commit() = 0;
};

class  AeroWindowWrap;
interface UISDKAPI IAeroWindowWrap
{
public:
    IAeroWindowWrap(AeroWindowWrap*  p);

    AERO_MODE  GetAeroMode();
    void  GetBlurRegion(CRegion4* pregion);
    void  UpdateRgn();
	void  SetTransparentMargins(RECT* prcMargin);

    AeroWindowWrap*  m_pImpl;
};


class  LayeredWindowWrap;
interface UISDKAPI ILayeredWindowWrap
{
public:
    ILayeredWindowWrap(LayeredWindowWrap*  p);
    byte  GetAlpha();
    void  SetAlpha(byte b, bool bUpdate);

    LayeredWindowWrap*  m_pImpl;
};



WINDOW_TRANSPARENT_TYPE  GetTransparentModeTypeFromAttr(IMapAttribute* p);
WINDOW_TRANSPARENT_TYPE  GetTransparentModeTypeFromAttr(const TCHAR* szText);
IWindowTransparent*  CreateTransparentModeByType(WINDOW_TRANSPARENT_TYPE eType);

}

#endif // IWINDOW_TRANSPARENT_MODE_H_BCA6EDC7_FAD0_4cce_95FA_0A1E00DCCC5C