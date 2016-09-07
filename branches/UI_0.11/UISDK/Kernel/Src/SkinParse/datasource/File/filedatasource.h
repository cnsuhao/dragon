#pragma once
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"

namespace UI
{

class FileDataSource : public ISkinDataSource
{
public:
	static FileDataSource*  Create()
	{
		return new FileDataSource();
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
   // bool  FileDataSource::CalcFilePath(LPCTSTR szData, ISkinRes* pSkinRes, __out String& strLastPath);

private:
    String  m_strPath;
};

}