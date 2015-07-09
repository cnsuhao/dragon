#include "stdafx.h"
#include "zipdatasource.h"
#include "UISDK\Kernel\Src\SkinParse\xml\msxmlwrap\msxmlwrap.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\SkinParse\datasource\Zip\bytebufferreader.h"

namespace UI
{
ZipDataSource::ZipDataSource()
{
    m_hZip = NULL;
}
ZipDataSource::~ZipDataSource()
{
    if (m_hZip)
    {
        CloseZip(m_hZip);
        m_hZip = NULL;
    }
}
void  ZipDataSource::SetDirection(LPCTSTR szDir)
{
    if (szDir)
        m_strDir = szDir;
    else
        m_strDir.clear();
}
void  ZipDataSource::SetName(LPCTSTR szName)
{
    if (szName)
        m_strName = szName;
    else
        m_strName.clear();
}
SKIN_PACKET_TYPE  ZipDataSource::GetType()
{
    return SKIN_PACKET_TYPE_ZIP;
}

LPCTSTR  ZipDataSource::GetDirection()
{
    return m_strDir.c_str();
}
LPCTSTR  ZipDataSource::GetName()
{
    return m_strName.c_str();
}

bool  ZipDataSource::Init()
{
    if (NULL == m_hZip)
    {
        String  str = m_strDir;
        str.append(m_strName);
        str.append(_T("."));
        str.append(XML_SKIN_PACKET_EXT);

        m_hZip = OpenZip(str.c_str(), NULL);
        if (NULL == m_hZip)
        {
            UI_LOG_ERROR(_T("OpenZip Failed. file=%s"), str.c_str());
			return false;
        }
    }
	return true;
}

bool  ZipDataSource::Load_UIDocument(IUIDocument* pDocument, LPCTSTR szPath)
{
    if (!m_hZip || !Init())
	{
		return false;
	}

    ZIPENTRY ze;
    int index = 0; 

    TCHAR szPath2[MAX_PATH];
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

	int nSize = ze.unc_size;
    byte* szbuf = new byte[nSize];  // 最后一个用0填充，标明字符串结束
    memset(szbuf, 0, nSize);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    bool bRet = pDocument->LoadData(szbuf, nSize);    
    SAFE_ARRAY_DELETE(szbuf);
    return bRet;
}

void  ZipDataSource::TranslatePath(LPCTSTR szOrignPath, TCHAR* szLastPath)
{
    LPCTSTR p = szOrignPath;
    TCHAR* p2 = szLastPath;

    // 跳过 .\xxx 表示的当前目录
    if (_tcslen(szOrignPath) > 2 &&
        szOrignPath[0] == _T('.') && 
        (szOrignPath[1] == _T('/') || szOrignPath[1] == _T('\\')))
    {
        p += 2;
    }

    while (*p)
    {
        if (*p == '\\')
            *p2 = '/';
        else
            *p2 = *p;

        p++;
        p2++;
    }
    *p2 = _T('');
}

// 注：zip内部的路径符号是/，而不是\ 
bool  ZipDataSource::Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, bool bCreateAlphaChannel)
{
    if (!m_hZip || !Init())
        return false;

    if (NULL == pBitmap || NULL == szPath)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    TCHAR szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    bool bRet = pBitmap->LoadFromData((byte*)szbuf, ze.unc_size, bCreateAlphaChannel);
    delete[] szbuf;

    return bRet;
}

bool  ZipDataSource::Load_Image(ImageWrap* pImage, LPCTSTR szPath)
{
    if (!m_hZip || !Init())
        return false;

    if (NULL == pImage || NULL == szPath)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    TCHAR szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    pImage->GetImpl()->LoadFromData((byte*)szbuf, ze.unc_size);
    delete[] szbuf;

    return pImage->IsNull()? false:true;
}

bool  ZipDataSource::Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage)
{
    if (!m_hZip || !Init())
        return false;

    if (NULL == pImage || NULL == szPath)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    TCHAR szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    bool bRet = pImage->LoadFromByte((byte*)szbuf, ze.unc_size);
    delete[] szbuf;

    return bRet;
}

bool  ZipDataSource::Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp)
{
    if (!m_hZip || !Init())
        return false;

    if (NULL == szPath || NULL == pp)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    TCHAR szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    ByteBufferReader* pBuffer = new ByteBufferReader;
    pBuffer->load((const byte*)szbuf, ze.unc_size, true);
    *pp = pBuffer;

    return true;
}
}