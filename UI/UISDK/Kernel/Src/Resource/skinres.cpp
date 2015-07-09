#include "StdAfx.h"
#include "skinres.h"
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Src\Resource\skinmanager.h"
#include "UISDK\Kernel\Src\SkinParse\datasource\skindatasource.h"
#include "UISDK\Kernel\Src\Util\Stopwatch\stopwatch.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"

SkinRes::SkinRes(SkinManager& o):
    m_mgrSkinRef(o),
    m_mgrLayout(this),
    m_mgrStyle(this),
    m_mgrFont(this),
    m_mgrColor(this),
    m_mgrImage(this, o.GetUIApplication())
    
{	
    m_pISkinRes = NULL;
    m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
    m_pDataSource = NULL;
    m_pHLSInfo = NULL;
}
SkinRes::~SkinRes()
{
    Unload();
    SAFE_DELETE(m_pISkinRes);
    SAFE_DELETE(m_pHLSInfo);
    SAFE_RELEASE(m_pDataSource);
}

ISkinRes*  SkinRes::GetISkinRes()
{
    if (NULL == m_pISkinRes)
    {
        m_pISkinRes = new ISkinRes(this);
    }
    return m_pISkinRes; 
}

void  SkinRes::SetParam(SKIN_PACKET_TYPE eType, LPCTSTR szName, LPCTSTR szPath)
{
    SAFE_DELETE(m_pDataSource);
    CreateDataSourceInstance(eType, &m_pDataSource);
    m_pDataSource->SetDirection(szPath);
    m_pDataSource->SetName(szName);
}


LPCTSTR  SkinRes::GetName()
{ 
    if (NULL == m_pDataSource)
        return NULL;

    return m_pDataSource->GetName(); 
}
LPCTSTR  SkinRes::GetPath() 
{ 
    if (NULL == m_pDataSource)
        return NULL;

    return m_pDataSource->GetDirection(); 
}

// 重新从Temp文件中读取数据

bool  SkinRes::Load()
{
    if (m_eLoadState == SKIN_RES_LOAD_STATE_LOADED)
        return true;

    // 加载速度测试
	StopWatch watch;
	watch.Start();

	SkinParseEngine parse(this);
    if (parse.Parse(m_pDataSource, XML_SKIN_XML))
    {
		m_eLoadState = SKIN_RES_LOAD_STATE_LOADED;
		
		__int64 lCost = watch.Now();
		TCHAR szText[128] = {0};
		_stprintf(szText, _T("Load Skin File Cost: %d ms\r\n"), lCost);
		::OutputDebugString(szText);

		UI_LOG_DEBUG(_T("Load Skin File Cost: %d ms"), lCost);
		return true;
    }   
	else
	{
		Unload();
		return false;
	}
}

bool  SkinRes::Unload()
{
    _DocList::iterator iter = m_listDoc.begin();
    for (; iter != m_listDoc.end(); iter++)
    {
        UISkinResDocInfo& o = *iter;
        SAFE_RELEASE(o.m_pDoc);
    }
    m_listDoc.clear();

    m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
    return true;
}

// parse.ProcessFile在解析配置文件过程中，如果发现一个新的文档，
// 则会回调该函数进行通知
void  SkinRes::OnNewUIDocument(IUIDocument* pDoc, LPCTSTR szPath)
{
	if (!pDoc || !szPath)
		return;

	pDoc->AddRef();

    UISkinResDocInfo info;
    info.m_pDoc = pDoc;
    info.m_strDocPath = szPath;

    m_listDoc.push_back(info);
}

bool SkinRes::Save()
{
    _DocList::iterator iter = m_listDoc.begin();
    for (; iter != m_listDoc.end(); iter++)
    {
        UISkinResDocInfo&  o = *iter;

        if (!o.m_pDoc->IsDirty())
            continue;

        if (!o.m_pDoc->Save())
        {
            UI_LOG_ERROR(_T("%s Save Xml Doc Failed. path=%d"), o.m_strDocPath.c_str());
        }
    }

	return true;
}


//
//	更新指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找原有对象的新属性和布局
//
bool SkinRes::ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild )
{
	return m_mgrLayout.ReLoadLayout(pRootObj, listAllChild);
}

IImageResItem* SkinRes::GetImageItemInfo(UINT nIndex)
{
	return m_mgrImage.GetImageItemInfo(nIndex);
}

IColorResItem* SkinRes::GetColorItemInfo(UINT nIndex)
{
	return m_mgrColor.GetColorItemInfo(nIndex);
}

IFontResItem* SkinRes::GetFontItemInfo(UINT nIndex)
{
	return m_mgrFont.GetFontItemInfo(nIndex);
}

bool SkinRes::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
    if (NULL == m_pHLSInfo)
    {
        m_pHLSInfo = new SKIN_HLS_INFO;
    }
    m_pHLSInfo->h = h;
    m_pHLSInfo->l = l;
    m_pHLSInfo->s = s;
    m_pHLSInfo->nFlag = nFlag;

	if (false == m_mgrImage.ChangeSkinHLS(h,l,s,nFlag))
		return false;
	if (false == m_mgrColor.ChangeSkinHLS(h,l,s,nFlag))
		return false;

#if 0 // -- 架构改造	
	// 保存该 HLS 数据
	SkinConfigRes* pSkinConfigRes = m_pSkinConfigMgr->GetSkinConfigRes();
	if (NULL == pSkinConfigRes)
		return false;

	pSkinConfigRes->SetHLS((char)h,(char)l,(char)s,nFlag);
#endif
	return true;
}


LPCTSTR  SkinRes::GetXmlDoc(UINT nIndex, IUIDocument** pp)
{
    if (!pp || nIndex >= GetXmlDocCount())
        return NULL;

	IUIDocument*  pDoc = m_listDoc[nIndex].m_pDoc;
	*pp = pDoc;
	pDoc->AddRef();

    return m_listDoc[nIndex].m_strDocPath.c_str();
}
bool  SkinRes::GetXmlDocByName(LPCTSTR szName, IUIDocument** pp)
{
	if (!szName || !pp)
		return false;

	_DocList::iterator iter = m_listDoc.begin();
	for (; iter != m_listDoc.end(); iter++)
	{
		UISkinResDocInfo& info = *iter;
		if (info.m_strDocPath == szName)
		{
			*pp = info.m_pDoc;
			info.m_pDoc->AddRef();
			return true;
		}
	}
	return false;
}

LPCTSTR  SkinRes::GetXmlDocName(IUIDocument* p)
{
    if (!p)
        return NULL;

    _DocList::iterator iter = m_listDoc.begin();
    for (; iter != m_listDoc.end(); iter++)
    {
        UISkinResDocInfo& info = *iter;
        if (info.m_pDoc == p)
        {
            return info.m_strDocPath.c_str();
        }
    }
    return NULL;
}

IUIApplication*  SkinRes::GetUIApplication()   { return m_mgrSkinRef.GetUIApplication(); }
ISkinManager*    SkinRes::GetISkinManager()    { return m_mgrSkinRef.GetISkinManager(); }