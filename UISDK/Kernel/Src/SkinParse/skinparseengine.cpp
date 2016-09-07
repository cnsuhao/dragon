#include "stdafx.h"
#include "skinparseengine.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"

#include "xml\msxmlwrap/msxmlwrap.h"
#include "xml\pugixmlwrap\pugixmlwrap.h"

using namespace UI;

SkinParseEngine::SkinParseEngine(SkinRes* pSkinRes)
{   
	UIASSERT(pSkinRes);
    m_pSkinRes = pSkinRes;
	m_pUIApplication = m_pSkinRes->GetUIApplication();
}

SkinParseEngine::~SkinParseEngine()
{
}

bool  SkinParseEngine::Parse(ISkinDataSource* pDataSource, LPCTSTR szXmlFile)
{
	return Parse(pDataSource, szXmlFile, NULL);
}

bool  SkinParseEngine::Parse(ISkinDataSource* pDataSource, LPCTSTR szXmlFile, __out IUIDocument** ppDoc)
{
#if 0
	1. 查找 <skin> root element，如果没有找到则直接返回，表示这不是一个合法文件
		2. 遍历 <skin> 的child element

		2.1 获取tagName，例如image
		2.2 获取image标签对应的解析器 IImageParse
		2.3 调用IImageParse的NewElement
		2.4 IImageParse自己去负责遍历子结点
#endif
	if (!pDataSource || !szXmlFile)
		return false;

	if (ppDoc)
		*ppDoc = NULL;

    IUIDocument*  pUIDocument = NULL;
    CreateXmlDocument(XML_ENGINE_DEFAULT, &pUIDocument);
	if (!pDataSource->Load_UIDocument(pUIDocument, szXmlFile))
	{
		SAFE_RELEASE(pUIDocument);
		return false;
	}

	IUIElement* pRootElem = NULL;
	IUIChildNodeEnum* pEnum = NULL;

	do 
	{
		pUIDocument->FindElem(XML_SKIN, &pRootElem);
		if (NULL == pRootElem)
		{
			UI_LOG_ERROR(_T("Cannot find root element: %s"), XML_SKIN);
			break;
		}

		m_pSkinRes->OnNewUIDocument(pUIDocument, szXmlFile);

		pEnum = pRootElem->EnumChild();
		if (NULL == pEnum)
			break;

		IUIElement* pChildElement = NULL;
		while (pChildElement = pEnum->NextElement())
		{
			this->NewChild(pChildElement);
			SAFE_RELEASE(pChildElement);
		}
	}
	while (0);

	SAFE_RELEASE(pRootElem);
	SAFE_RELEASE(pEnum);

	if (ppDoc)
		*ppDoc = pUIDocument;
	else
		SAFE_RELEASE(pUIDocument);
	
	return true;
}

void  SkinParseEngine::NewChild(IUIElement* pElement)
{
    CComBSTR  bstrTagName;
    pElement->GetTagName(&bstrTagName);

    pfnParseSkinTag func;
    if (!m_pUIApplication->GetSkinTagParseFunc((LPCWSTR)bstrTagName, &func))
        return;

    func(pElement, m_pSkinRes->GetISkinRes());
}


HRESULT  SkinParseEngine::UIParseIncludeTagCallback(IUIElement* pElement, ISkinRes* pSkinRes)
{
    if (NULL == pElement || NULL == pSkinRes)
        return E_FAIL;

    CComBSTR  bstrFilePath;
    if (FAILED(pElement->GetData(&bstrFilePath)))
        return E_FAIL;

    SkinParseEngine  parse(pSkinRes->GetImpl());
	ISkinDataSource*  pDataSource = pSkinRes->GetDataSource();

#ifdef _UNICODE
    if (!parse.Parse(pDataSource, bstrFilePath))
#else
    if (!parse.Parse(pDataSource, CW2A((BSTR)bstrFilePath)))
#endif
        return E_FAIL;

    return S_OK;
}

namespace UI
{
bool  CreateXmlDocument(XML_ENGINE e, IUIDocument** pp)
{
    switch (e)
    {
    case MSXML:
        {
            return MsXmlDocument::CreateInstance(pp);
        }
        break;

    case PUGIXML:
    case XML_ENGINE_DEFAULT:
        {
            return PugiXmlDocument::CreateInstance(pp);
        }
        break;
    }

    if (pp)
        *pp = NULL;
    return false;
}
}