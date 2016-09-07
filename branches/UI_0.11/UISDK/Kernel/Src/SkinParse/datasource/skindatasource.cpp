#include "stdafx.h"
#include "skindatasource.h"
#include "Zip/zipdatasource.h"
#include "File/filedatasource.h"
#include "Zip/bytebufferreader.h"
#include "File/filebufferreader.h"

namespace UI
{

void  CreateDataSourceInstance(SKIN_PACKET_TYPE e, ISkinDataSource** pp)
{
    if (NULL == pp)
        return;

    ISkinDataSource* p = NULL;
    switch (e)
    {
    case SKIN_PACKET_TYPE_DIR:
        {
			p = FileDataSource::Create();
        }
        break;;

    case SKIN_PACKET_TYPE_ZIP:
        {
			p = ZipDataSource::Create();
        }
        break;
    }
    
    *pp = p;
}

void  CreateStreamBuffer(STREAM_TYPE e, IStreamBufferReader** pp)
{
    if (NULL == pp)
        return;

    IStreamBufferReader* p = NULL;
    switch (e)
    {
    case STREAM_TYPE_FILE:
        {
			p = FileBufferReader::Create();
        }
        break;;

    case STREAM_TYPE_BYTE:
        {
			p = ByteBufferReader::Create();
        }
        break;
    }

    *pp = p;
}


GdiplusBitmapLoadWrap::GdiplusBitmapLoadWrap()
{
    m_pBitmap = NULL;
}
GdiplusBitmapLoadWrap::~GdiplusBitmapLoadWrap()
{
    Destroy();
}
Gdiplus::Bitmap*  GdiplusBitmapLoadWrap::GetBitmap()
{
    return m_pBitmap;
}

void  GdiplusBitmapLoadWrap::Destroy()
{
    SAFE_DELETE(m_pBitmap);
}
bool  GdiplusBitmapLoadWrap::LoadFromFile(LPCTSTR szPath)
{
    SAFE_DELETE(m_pBitmap);
    m_pBitmap = Gdiplus::Bitmap::FromFile(szPath);

    if (m_pBitmap->GetLastStatus() != Gdiplus::Ok)
    {
        SAFE_DELETE(m_pBitmap);
        return false;
    }
    return true;
}
bool  GdiplusBitmapLoadWrap::LoadFromByte(LPBYTE pByte, int nSize)
{
    SAFE_DELETE(m_pBitmap);

    IStream* pStream = NULL;
    HGLOBAL  hNew    = ::GlobalAlloc(GHND, nSize);
    LPBYTE   lpByte  = (LPBYTE)::GlobalLock(hNew);
    ::memcpy(lpByte, pByte, nSize);
    ::GlobalUnlock(hNew);

    HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
    if (ht != S_OK)
    {
        GlobalFree(hNew);
        return false;
    }
    m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
    pStream->Release();  
    hNew = NULL;

    if (m_pBitmap->GetLastStatus() != Gdiplus::Ok)
    {
        SAFE_DELETE(m_pBitmap);
        return false;
    }
    return true;
}

}