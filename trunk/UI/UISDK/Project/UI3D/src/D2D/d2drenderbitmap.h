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
	// Direct2D 使用 Windows 图像处理组件 (WIC) 来加载位图。若要从文件加载位图，
	// 请首先使用 WIC 对象来加载图像，并将其转换为与 Direct2D 兼容的格式，然后
	// 使用 CreateBitmapFromWicBitmap 方法创建 ID2D1Bitmap。
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
		// 1.使用 IWICImagingFactory::CreateDecoderFromFileName 方法创建 IWICBitmapDecoder。

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
		// 2.从图像中检索某一帧并将该帧存储在 IWICBitmapFrameDecode 对象中。

		if (SUCCEEDED(hr))
		{
			// Create the initial frame.
			hr = pDecoder->GetFrame(0, &pSource);
		}

		//////////////////////////////////////////////////////////////////////////
		// 3.位图必须转换为 Direct2D 可以使用的格式，因此请将图像的像素格式转换为 32bppPBGRA。
		//  （有关支持的格式的列表，请参见像素格式和 Alpha 模式。）调用 IWICImagingFactory::
		//   CreateFormatConverter 方法创建 IWICFormatConverter 对象，然后调用 IWICFormatConverter 
		//   对象的 Initialize 方法执行转换。 

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
		// 4. 调用 CreateBitmapFromWicBitmap 方法创建 ID2D1Bitmap 对象，该对象可以
		//    通过呈现器目标绘制并与其他 Direct2D 对象一起使用。

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

    // 1. bitmap是一个设备依赖的资源
    // 2. bitmap是由相应的RenderTarget调用CreateBitmapFromWicBitmap创建的，可能有多个RT调用同一个BITMAP，
    //    这时也得生成相应多个BITPAM。  ??真的必须这样吗
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