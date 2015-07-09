#pragma once
#include "..\compositereole.h"
#include "UISDK\Control\Src\Control\RichEdit\Resource\oleresource_update.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"

namespace UI
{
class ImageOleElement;
class ButtonOleElement;
class GifOleElement;
class PictureLoadingFailureRes;
class MagnifierRes;

class PictureOle : 
		public CompositeREOle,
		public IREOleResUpdateCallback
{
public:
    PictureOle();
    ~PictureOle();

    static PictureOle* CreateInstance(PictureREOleInfo* pInfo);
    IPictureREOle*  GetIPictureREOle();

public:
	LPCTSTR  GetPath();
	LPCTSTR  GetId();

    void  SetDesc(LPCTSTR szDesc);
    void  SetType(LPCTSTR szType);
    void  SetName(LPCTSTR szName);
    void  UpdateStatus(REOleLoadStatus e, LPCTSTR szPath=NULL);
	void  SetId(LPCTSTR  szId);

protected:
    virtual long  GetOleType() { return REOLE_PICTURE; }
    virtual void  OnPreInsert2RE();
	virtual void  GetSize(SIZE* pSize);
    virtual HRESULT  GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData);
    virtual HRESULT  GetClipboardDataEx(IDataObject* pDataObject);
    virtual LRESULT  ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void  OnREOleResUpdate(REOleLoadStatus e, LPCTSTR szPath, long lData) override;

private:
	void  SetMaxSize(int cx, int cy);

    PictureLoadingFailureRes* GetRes();
    MagnifierRes*  GetMagnifierRes();
    bool  LoadImage(LPCTSTR szPath);
    bool  LoadFailureImage();

    void  CreateMagnifierBtnElement();
    void  CreateImageElement();
    void  CreateLoadingGifElement();
    void  DestroyLoadingGifElement();

protected:
    IPictureREOle      m_oPictureREOle;  // 对外接口
    ImageOleElement*   m_pImage;         // 图片或者加载失败图片
    GifOleElement*     m_pLoadingGif;    // 正在加载
	ButtonOleElement*  m_pBtnMagnifier;  // 放大镜
    private:
    PictureLoadingFailureRes*  m_pRes;
    MagnifierRes*  m_pMagnifierRes;


protected:
    // 自适应大小。当RE变小时，对图片进行按比例缩放，保证一页之内能够显示完全
    // 同时在右上角增加一个放大镜按钮
    bool  m_bAdaptSize;  
    SIZE  m_sizeMax;
    int   m_nZoomRate;  // xx %

protected:
	String  m_strId;   // 图片md5 id
    String  m_strDesc;
    String  m_strType;
    String  m_strName;
    String  m_strPath;
    REOleLoadStatus  m_eStatus;

    
};

}