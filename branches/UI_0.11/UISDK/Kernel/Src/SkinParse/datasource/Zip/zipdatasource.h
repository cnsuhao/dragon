#pragma once
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"
#include "3rd\zip\unzip.h"

namespace UI
{

class ZipDataSource : public ISkinDataSource
{
public:
    ZipDataSource();
    ~ZipDataSource();

	static ZipDataSource*  Create()
	{
		return new ZipDataSource();
	}
    void  Release()
    {
        delete this;
    }

    void  SetPath(LPCTSTR szPath);
	LPCTSTR  GetPath();

    SKIN_PACKET_TYPE  GetType();

    bool  Load_UIDocument(IUIDocument* pDocument, LPCTSTR szPath);
    bool  Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, bool bCreateAlphaChannel);
    bool  Load_Image(ImageWrap* pImage, LPCTSTR szPath);
    bool  Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage);
    bool  Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp);

private:
    void  TranslatePath(LPCTSTR szOrignPath, TCHAR* szLastPath);
	bool  Init();

private:
    String  m_strPath;

    HZIP  m_hZip;
};

}