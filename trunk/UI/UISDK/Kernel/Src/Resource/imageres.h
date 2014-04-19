#pragma once

namespace UI
{

class ImageData;
class GDIRenderBitmap;
class GdiplusRenderBitmap;

//
//	image�е�һ����Ϣ����
//
class ImageResItem
{
public:
	ImageResItem();
	~ImageResItem();

	bool  ModifyImage(const TCHAR*  szPath);
    IMAGE_ITEM_TYPE  GetImageItemType();
    void  SetImageItemType(IMAGE_ITEM_TYPE e);

	DECLARE_STRING_SETGET(Id);
	DECLARE_STRING_SETGET(Path);
	DECLARE_bool_SETGET(UseSkinHLS);
	DECLARE_bool_SETGET(NeedAntiAliasing)

public:
	bool  ModifyHLS(short h, short l, short s, int nFlag);
	bool  ModifyHLS(IRenderBitmap* pBitmap, short h, short l, short s, int nFlag);
	bool  ModifyAlpha(int nAlphaPercent);
	IRenderBitmap*  GetImage(SkinRes* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI, bool* pbFirstTimeCreate=NULL);
    IRenderBitmap*  GetImage_gdi(SkinRes* pSkinRes, bool* pbFirstTimeCreate=NULL);
    IRenderBitmap*  GetImage_gdiplus(SkinRes* pSkinRes, bool* pbFirstTimeCreate=NULL);
    IRenderBitmap*  GetImage_d2d(SkinRes* pSkinRes, bool*  pbFirstTimeCreate=NULL);

    // ����Ixxx�ӿ�
    virtual IImageResItem*  GetIImageResItem(); 
    // ��ȡxml���ԣ���imagelist��layout��count
	virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    // �ڴ�����RenderBitmap֮���ɸ�������ȥΪ��������չ����
    virtual void  SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap);

protected:
	IImageResItem*  m_pIImageResItem;

    String    m_strId;      // image id
    String    m_strPath;    // image path

    bool  m_bUseSkinHLS;                   // ��ͼƬ�Ƿ����Ƥ��ɫ���ı� 
    bool  m_bNeedAntiAliasing;             // ��Ҫ֧�ֿ����(gdi��alphablend���Ų�֧��SetStretchBltMode��HALFTONE)
    bool  m_bMustHasAlphaChannel;          // ��ͼƬ�Ƿ���Ҫ����alpha channel��1. ��alpha channel��ͼƬһ�ʲ���gdi����������alphablend���ơ�2. û��alpha channel��gdiͼƬ���޷��ڷֲ㴰���������

    IMapAttribute*        m_pMapAttrib;    // Ϊ��ͼƬ���õ����ԣ�����imagelist��count��icon��width height
    IMAGE_ITEM_TYPE       m_eType;         // ͼƬ����

    ImageData*            m_pOriginImageData;    // ��ͼƬ��ԭʼ���ݣ��ı�ͼƬɫ��ʱʹ�ã�
    GDIRenderBitmap*      m_pGdiBitmap;          // �ⲿ���ã��ó�Ա���������ü���

    //  ע��Ϊ����߻�ͼЧ�ʣ�ͬʱҲ����һ��ͼƬ���ض�����ʽ������ʹ��GDI(AlphaBlend)����ͼ��
    //	    ��Ҫע����ǣ������Ҫʹ��alphaͨ��ʱ������ǿ�ƴ���һ��32λ��GDI BITMAP
    GdiplusRenderBitmap*  m_pGdiplusBitmap;      // �ⲿ����

    IRenderBitmap*        m_pDirect2DBitmap;   // �ⲿ����
};

class ImageListResItem : public ImageResItem
{
public:
    ImageListResItem();

    virtual IImageResItem*  GetIImageResItem(); 
    virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void  SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap);

    IMAGELIST_LAYOUT_TYPE  GetLayoutType();
    int  GetItemCount();
    void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);
    void  SetItemCount(int);

private:
    IMAGELIST_LAYOUT_TYPE   m_eLayoutType;
    int  m_nCount;
};

class ImageIconResItem : public ImageResItem
{
public:
    ImageIconResItem();

    virtual IImageResItem*  GetIImageResItem(); 
    virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void  SetRenderBitmapAttribute(IRenderBitmap* pRenderBitmap);

private:
    SIZE  m_sizeDraw;
};

//
//	image�б�
//
class ImageRes
{
public:
	ImageRes(SkinRes*  pSkinRes);
	~ImageRes();

	IImageRes*  GetIImageRes();

	long  GetImageCount();
	IImageResItem*  GetImageResItem(long lIndex);
	IImageResItem*  GetImageResItem(const TCHAR* szID);
	bool ModifyImage(const TCHAR* szId, const TCHAR* szPath);

	HBITMAP  LoadBitmap(const TCHAR* szId);
	HRESULT  GetBitmap(const TCHAR* szImageID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderBitmap** pRenderBitmap);

public:
	ImageResItem*  LoadItem(const TCHAR* szType, IMapAttribute* pMapAttrib, const TCHAR* szFullPath);

	ImageResItem*  GetImageItem2(int nIndex);
	ImageResItem*  GetImageItem2(const TCHAR* szId);

	ImageResItem*  InsertImage(IMAGE_ITEM_TYPE eType, const TCHAR* szId, const TCHAR* szPath);
	bool RemoveImage(const TCHAR* szId);
    bool RemoveImage(IImageResItem* pItem);
	bool Clear();
	bool ChangeSkinHLS(short h, short l, short s, int nFlag);
	bool ModifyImageItemAlpha(const String& strID, int nAlphaPercent);

private: 
	IImageRes*  m_pIImageRes;
	SkinRes*  m_pSkinRes;

    vector<ImageResItem*>   m_vImages;
};

}