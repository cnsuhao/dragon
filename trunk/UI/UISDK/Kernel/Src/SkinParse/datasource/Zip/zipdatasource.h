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

    void  SetDirection(LPCTSTR szDir);
    void  SetName(LPCTSTR szName);
    SKIN_PACKET_TYPE  GetType();

    LPCTSTR  GetDirection();
    LPCTSTR  GetName();

    bool  Load_UIDocument(IUIDocument* pDocument, LPCTSTR szPath);
    bool  Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, bool bCreateAlphaChannel);
    bool  Load_Image(ImageWrap* pImage, LPCTSTR szPath);
    bool  Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage);
    bool  Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp);

private:
    void  TranslatePath(LPCTSTR szOrignPath, TCHAR* szLastPath);
	bool  Init();

private:
    String  m_strDir;
    String  m_strName;

    HZIP  m_hZip;
};

}