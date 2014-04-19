#pragma once
#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace UI
{

template<class T>
class Direct2DRenderBitmapImpl : public T//IRenderResourceImpl<IRenderBitmap>
{
public:
	Direct2DRenderBitmapImpl()
	{
		m_nWidth = m_nHeight = 0;
		m_pConverter = NULL;
	}

public:
	virtual ~Direct2DRenderBitmapImpl()
	{
		//UI_LOG_DEBUG(_T("Direct2DRenderBitmap Delete. ptr=0x%08X"), this);
		SAFE_RELEASE(m_pConverter);
	}


	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D; }

	//
	// Direct2D ʹ�� Windows ͼ������� (WIC) ������λͼ����Ҫ���ļ�����λͼ��
	// ������ʹ�� WIC ����������ͼ�񣬲�����ת��Ϊ�� Direct2D ���ݵĸ�ʽ��Ȼ��
	// ʹ�� CreateBitmapFromWicBitmap �������� ID2D1Bitmap��
	//
	// An ID2D1Bitmap is a device-dependent resource: your application should 
	// create bitmaps after it initializes the render target with which the 
	// bitmap will be used, and recreate the bitmap whenever the render target 
	// needs recreated. (For more information about resources, see Resources 
	// Overview.)
	//
	virtual bool  LoadFromFile(const TCHAR* szPath, bool bCreateAlphaChannel)
	{
		//////////////////////////////////////////////////////////////////////////
		// 1.ʹ�� IWICImagingFactory::CreateDecoderFromFileName �������� IWICBitmapDecoder��

		IWICBitmapDecoder *pDecoder = NULL;
		IWICBitmapFrameDecode *pSource = NULL;
//		IWICFormatConverter *pConverter = NULL;

		HRESULT hr = g_pD2DApp->m_pWICFactory->CreateDecoderFromFilename(
			szPath,
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			);

		//////////////////////////////////////////////////////////////////////////
		// 2.��ͼ���м���ĳһ֡������֡�洢�� IWICBitmapFrameDecode �����С�

		if (SUCCEEDED(hr))
		{
			// Create the initial frame.
			hr = pDecoder->GetFrame(0, &pSource);
		}

		//////////////////////////////////////////////////////////////////////////
		// 3.λͼ����ת��Ϊ Direct2D ����ʹ�õĸ�ʽ������뽫ͼ������ظ�ʽת��Ϊ 32bppPBGRA��
		//  ���й�֧�ֵĸ�ʽ���б���μ����ظ�ʽ�� Alpha ģʽ�������� IWICImagingFactory::
		//   CreateFormatConverter �������� IWICFormatConverter ����Ȼ����� IWICFormatConverter 
		//   ����� Initialize ����ִ��ת���� 

		if (SUCCEEDED(hr))
		{

			// Convert the image format to 32bppPBGRA
			// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
			hr = g_pD2DApp->m_pWICFactory->CreateFormatConverter(&m_pConverter);

		}

		if (SUCCEEDED(hr))
		{
			hr = m_pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}

		//////////////////////////////////////////////////////////////////////////
		// 4. ���� CreateBitmapFromWicBitmap �������� ID2D1Bitmap ���󣬸ö������
		//    ͨ��������Ŀ����Ʋ������� Direct2D ����һ��ʹ�á�

// 		ID2D1Bitmap* pBitmap = NULL;
// 		if (SUCCEEDED(hr))
// 		{
// 			// Create a Direct2D bitmap from the WIC bitmap.
// 			hr = pRenderTarget->CreateBitmapFromWicBitmap(
// 				pConverter,
// 				NULL,
// 				&pBitmap
// 				);
// 		}

		pSource->GetSize(&m_nWidth, &m_nHeight);
//        pSource->GetPixelFormat(WICPixelFormatGUID)
		SAFE_RELEASE(pDecoder);
		SAFE_RELEASE(pSource);
//		SAFE_RELEASE(pConverter);

		if (SUCCEEDED(hr))
			return true;
		return false;
	}

    virtual bool  LoadFromData(byte* pData, int nSize, bool bCreateAlphaChannel)
    {
        UIASSERT(0); // TODO:
        return false;
    }

	virtual bool  Modify(const String& strFilePath, bool bCreateAlphaChannel)
	{
		UIASSERT(0); // TODO:
		return false;
	}
	virtual bool  Create( int nWidth, int nHeight )
	{
		UIASSERT(0); // TODO:
		return false;
	}

	virtual int   GetWidth()
	{
		return m_nWidth;
	}
	virtual int   GetHeight()
	{
		return m_nHeight;
	}

	virtual BYTE* LockBits() {return NULL;}
	virtual void  UnlockBits() {return;}

	virtual bool  SaveBits( ImageData* pImageData ) {return false;}
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l, short s, int nFlag ) {return false;}

	virtual void  SetAttribute( const ATTRMAP& mapAttrib ) {return;}

    virtual void  Destroy()
    {
        UIASSERT(0); // todo;
    }
    virtual int   GetBPP()
    {
        return 32;
    }
    virtual IMAGE_ITEM_TYPE  GetImageType()
    {
        return IMAGE_ITEM_TYPE_IMAGE;
    }
public:
	ID2D1Bitmap*  GetBitmap(ID2D1RenderTarget* pRenderTarget)
	{
		if (NULL == pRenderTarget || NULL == m_pConverter)
			return NULL;

		map<ID2D1RenderTarget*, ID2D1Bitmap*>::iterator iter = m_mapBitmap.find(pRenderTarget);
		if (iter != m_mapBitmap.end())
			return iter->second;

 		ID2D1Bitmap* pBitmap = NULL;
 		HRESULT	hr = pRenderTarget->CreateBitmapFromWicBitmap(m_pConverter, NULL, &pBitmap);
		if (SUCCEEDED(hr))
		{
			m_mapBitmap[pRenderTarget] = pBitmap;
			return pBitmap;
		}
		else
		{
			UI_LOG_ERROR(_T("%s CreateBitmapFromWicBitmap failed"), FUNC_NAME);
			return NULL;
		}
	}
private:
	IWICFormatConverter*    m_pConverter;
	UINT   m_nWidth;
	UINT   m_nHeight;

    // 1. bitmap��һ���豸��������Դ
    // 2. bitmap������Ӧ��RenderTarget����CreateBitmapFromWicBitmap�����ģ������ж��RT����ͬһ��BITMAP��
    //    ��ʱҲ��������Ӧ���BITPAM��  ??��ı���������
	map<ID2D1RenderTarget*, ID2D1Bitmap*>  m_mapBitmap;  
};


class Direct2DRenderBitmap : public Direct2DRenderBitmapImpl<IRenderResourceImpl<IRenderBitmap> >
{
public:
    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE; }
};

class Direct2DIconRenderBitmap : public Direct2DRenderBitmapImpl<IRenderResourceImpl<IImageIconRenderBitmap > >
{
public:
    virtual SIZE  GetDrawSize();
    virtual void  SetDrawSize(SIZE* ps);

    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_ICON; }

private:
    int    m_nIconWidth;
    int    m_nIconHeight;
};


class Direct2DImageListRenderBitmap : public Direct2DRenderBitmapImpl<IRenderResourceImpl<IImageListRenderBitmap> >
{
public:
    Direct2DImageListRenderBitmap();


    virtual int  GetItemWidth();
    virtual int  GetItemHeight();
    virtual IMAGELIST_LAYOUT_TYPE GetLayoutType();
    virtual bool GetIndexPos(int nIndex, POINT* pPoint);
    virtual int  GetItemCount() { return m_nCount; }
    virtual void  SetItemCount(int);
    virtual void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);

    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE_LIST; }
private:
    IMAGELIST_LAYOUT_TYPE   m_eLayout;
    int     m_nCount;
};

}