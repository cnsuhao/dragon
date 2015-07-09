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
    IPictureREOle      m_oPictureREOle;  // ����ӿ�
    ImageOleElement*   m_pImage;         // ͼƬ���߼���ʧ��ͼƬ
    GifOleElement*     m_pLoadingGif;    // ���ڼ���
	ButtonOleElement*  m_pBtnMagnifier;  // �Ŵ�
    private:
    PictureLoadingFailureRes*  m_pRes;
    MagnifierRes*  m_pMagnifierRes;


protected:
    // ����Ӧ��С����RE��Сʱ����ͼƬ���а��������ţ���֤һҳ֮���ܹ���ʾ��ȫ
    // ͬʱ�����Ͻ�����һ���Ŵ󾵰�ť
    bool  m_bAdaptSize;  
    SIZE  m_sizeMax;
    int   m_nZoomRate;  // xx %

protected:
	String  m_strId;   // ͼƬmd5 id
    String  m_strDesc;
    String  m_strType;
    String  m_strName;
    String  m_strPath;
    REOleLoadStatus  m_eStatus;

    
};

}