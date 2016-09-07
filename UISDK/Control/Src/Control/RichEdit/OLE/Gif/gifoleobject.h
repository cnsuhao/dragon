#pragma 
#include "UISDK\Kernel\Inc\Util\igifimage.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "..\richeditolemgr.h"
#include "..\reoleimpl.h"


namespace UI
{
class GifImageItem;
class GifImageItemMgr;

class GifOle : public REOleImpl
{
protected:
	GifOle(IUIApplication* pUIApp, IMessage* pNotifyObj);
public:
	~GifOle();
    static GifOle* CreateInstance(IUIApplication* pUIApp, IMessage* pNotifyObj);

	HRESULT  LoadGif(LPCTSTR szPath);
    HRESULT  LoadSkinGif(LPCTSTR szId);
	HRESULT  Refresh();
    void     OnTick();

    virtual long  GetOleType() override { return REOLE_GIF; }
    virtual void  OnDraw(HDC hDC, RECT* prc) override;
	virtual void  GetSize(SIZE* pSize) override;
    virtual HRESULT  GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData);
    virtual HRESULT  GetClipboardDataEx(IDataObject* pDataObject);
    
protected:

    IUIApplication*   m_pUIApp;
    IGifImage*        m_pGifImage;   // ���ļ����ص�ͼƬ, ��skin����ʱ��m_pGifImageΪ�ղ�ʹ�ã�����Ҳ���ù��ͷ� ��
    IGifImageRender*  m_pGifRender;  // gif���ƽӿ�

	IMessage*         m_pNotifyMsg;
	String            m_strPath;
};


}