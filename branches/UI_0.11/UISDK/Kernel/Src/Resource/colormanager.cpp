#include "StdAfx.h"
#include "colormanager.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Inc\Interface\iuires.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             颜色                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////



ColorManager::ColorManager(SkinRes* pSkinRes): m_resColor(pSkinRes)
{
    m_pIColorManager = NULL;
	m_pSkinRes = pSkinRes;
}

ColorManager::~ColorManager(void)
{
	this->Clear();
    SAFE_DELETE(m_pIColorManager);

	_MyListItem* pItem = m_listUIElement.GetFirst();
	while (pItem)
	{
		(**pItem).pXmlElement->Release();
		pItem = pItem->GetNext();
	}
	m_listUIElement.Clear();
}

IColorManager*  ColorManager::GetIColorManager()
{
    if (NULL == m_pIColorManager)
        m_pIColorManager = new IColorManager(this);

    return m_pIColorManager;
}
/*
**	清除所有col资源
*/
void ColorManager::Clear()
{
	m_resColor.Clear();
}

int ColorManager::GetColorCount()
{
	return m_resColor.GetColorCount();
}

IColorResItem* ColorManager::GetColorItemInfo(int nIndex)
{
	ColorResItem* pItem = this->m_resColor.GetColorItem(nIndex);
	if (NULL == pItem)
		return NULL;

    return pItem->GetIColorResItem();
}


bool ColorManager::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	return m_resColor.ChangeSkinHLS(h,l,s,nFlag);
}

ColorRes&  ColorManager::GetColorRes()
{
	return m_resColor;
}


//////////////////////////////////////////////////////////////////////////

HRESULT  ColorManager::UIParseColorTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IColorManager*  pColorMgr = pSkinRes->GetColorManager();
    if (NULL == pColorMgr)
        return E_FAIL;

    return pColorMgr->GetImpl()->ParseNewElement(pElem);
}

HRESULT  ColorManager::ParseNewElement(IUIElement* pElem)
{
	ColorTagElementInfo info;
	info.pXmlElement = pElem;
	pElem->AddRef();

	CComBSTR bstrId;
	if (pElem->GetAttrib(XML_ID, &bstrId))
	{
		info.strId = bstrId;
	}

	m_listUIElement.Add(info);

    // 遍历其子元素
    IUIChildNodeEnum* pEnum = pElem->EnumChild();
    if (pEnum)
    {
        IUIElement* pChildElement = NULL;
        while (pChildElement = pEnum->NextElement())
        {
            this->OnNewChild(pChildElement);
            SAFE_RELEASE(pChildElement);
        }

        SAFE_RELEASE(pEnum);
    }

    return S_OK;
}
void  ColorManager::OnNewChild(IUIElement* pElem)
{
    CComBSTR bstrTagName;
    pElem->GetTagName(&bstrTagName);

    //	加载所有属性
    IMapAttribute* pMapAttrib = NULL;
    pElem->GetAttribList(&pMapAttrib);

    CComBSTR  bstrValue;
    pElem->GetData(&bstrValue);

    if (false == m_resColor.LoadItem(pMapAttrib, bstrValue))
    {
        UI_LOG_WARN(_T("insert color failed."));
    }

    SAFE_RELEASE(pMapAttrib);
}