#include "stdafx.h"
#include "msxmlwrap.h"

namespace UI
{

UIMarkupChildNodeEnum::UIMarkupChildNodeEnum(IXMLDOMNodeList* p, IUIDocument* pDoc)
{
    m_pNodeList = p;
    m_pDocument = pDoc;
}
UIMarkupChildNodeEnum* UIMarkupChildNodeEnum::New(IXMLDOMNodeList* pNode, IUIDocument* pDoc)
{
    if (NULL == pNode)
        return NULL;

    return new UIMarkupChildNodeEnum(pNode, pDoc);
}
void  UIMarkupChildNodeEnum::Release()
{
    delete this;
}

// 查找元素，跳过其它类型
IUIElement*  UIMarkupChildNodeEnum::NextElement()
{
    CComPtr<IXMLDOMNode> pNode = NULL;
    if (FAILED(m_pNodeList->nextNode(&pNode)) || NULL == pNode)
        return NULL;

    DOMNodeType etype = NODE_INVALID;
    pNode->get_nodeType(&etype);
    if (NODE_ELEMENT != etype)
    {
        return this->NextElement();
    }

    return UIMarkupElement::New(pNode.p, m_pDocument);
}

UIMarkupElement::UIMarkupElement(IXMLDOMElement* pNode, IUIDocument* pDoc)
{
    m_lRef = 0;
    m_pElement = pNode;
    m_pDocument = pDoc;
}
UIMarkupElement* UIMarkupElement::New(IXMLDOMNode* pNode, IUIDocument* pDoc)
{
    if (NULL == pNode)
        return NULL;

    CComQIPtr<IXMLDOMElement> p = pNode;
    if (NULL == p)
        return NULL;

    UIMarkupElement* pNew = new UIMarkupElement(p, pDoc);
    pNew->AddRef();
    return pNew;
}

long  UIMarkupElement::AddRef()
{
    return ++m_lRef;
}
long  UIMarkupElement::Release()
{
    m_lRef--;

    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }

    return m_lRef;
}

void  UIMarkupElement::GetTagName(BSTR* pbstr)
{
    m_pElement->get_tagName(pbstr);
}

// <a> bb </a> 获取 bb
bool  UIMarkupElement::GetData(BSTR* pbstr)
{
    CComPtr<IXMLDOMNode> pChild = NULL;
    if (FAILED(m_pElement->get_firstChild(&pChild)))
        return false;

    if (NULL == pChild)
        return false;
    
    DOMNodeType type = NODE_INVALID;
    pChild->get_nodeType(&type);
    if (type != NODE_TEXT)
        return false;
    
    CComQIPtr<IXMLDOMText> pText = pChild;
    if (NULL == pText)
        return false;

    pText->get_text(pbstr);
    return true;
}

bool  UIMarkupElement::SetData(const TCHAR* szText)
{
    if (NULL == szText)
        return false;

    CComPtr<IXMLDOMNode> pChild = NULL;
    if (FAILED(m_pElement->get_firstChild(&pChild)))
        return false;

    if (pChild)
    {
        DOMNodeType type = NODE_INVALID;
        pChild->get_nodeType(&type);
        if (type != NODE_TEXT)
            return false;

        CComQIPtr<IXMLDOMText> pText = pChild;
        if (NULL == pText)
            return false;

        pText->put_text((BSTR)szText);
    }
    else
    {
        CComPtr<IXMLDOMDocument> pDoc = NULL;
        if (FAILED(m_pElement->get_ownerDocument(&pDoc)))
            return false;

        CComPtr<IXMLDOMText>  pText = NULL;
        if (FAILED(pDoc->createTextNode((BSTR)szText, &pText)))
            return false;

        CComPtr<IXMLDOMNode> pOutNewElem = NULL;
        if (FAILED(m_pElement->appendChild(pText, &pOutNewElem)))
            return false;
    }

    return true;
}

bool  UIMarkupElement::GetAttrib(const TCHAR* szKey, BSTR* pbstrValue)
{
    if (NULL == szKey || NULL == pbstrValue)
        return false;

    VARIANT var;
    VariantInit(&var);

    if (FAILED(m_pElement->getAttribute((BSTR)szKey, &var)))
    {
        VariantClear(&var);
        return false;
    }

    if (var.vt != VT_BSTR)
    {
        VariantClear(&var);
        return FALSE;
    }

    *pbstrValue = var.bstrVal;
    return true;
}

bool  UIMarkupElement::HasAttrib(const TCHAR* szKey)
{
    if (NULL == szKey)
        return false;

    if (0 == _tcslen(szKey))
        return false;

    VARIANT var;
    VariantInit(&var);

    bool bRet = false;
    if (SUCCEEDED(m_pElement->getAttribute((BSTR)szKey, &var)) && var.vt != VT_NULL)
        bRet = true;
    
    VariantClear(&var);

    return bRet;
}

bool  UIMarkupElement::AddAttrib(const TCHAR* szKey, const TCHAR* szValue)
{
    if (NULL == szKey || NULL == szValue)
        return false;

    CComVariant var(szValue);
    if (FAILED(m_pElement->setAttribute((BSTR)szKey, var)))
        return false;

    return true;
}

bool  UIMarkupElement::RemoveAttrib(const TCHAR* szKey)
{
    if (NULL == szKey)
        return false;

    if (FAILED(m_pElement->removeAttribute((BSTR)szKey)))
        return false;

    return true;
}

bool  UIMarkupElement::ModifyAttrib(const TCHAR* szKey, const TCHAR* szValue)
{
    return AddAttrib(szKey, szValue);
}

void  UIMarkupElement::GetAttribList(IMapAttribute** ppMapAttrib)
{
    CComPtr<IXMLDOMNamedNodeMap> pNodeMap; 
    if (FAILED(m_pElement->get_attributes(&pNodeMap)) || NULL == pNodeMap)
        return;

    UICreateIMapAttribute(ppMapAttrib);
    while (1)
    {
        CComPtr<IXMLDOMNode> pNode = NULL;

        // 这个是按照xml中的顺序遍历出来的
        if (FAILED(pNodeMap->nextNode(&pNode)) || NULL == pNode)
            break;

        CComBSTR bstrKey;
        CComVariant var;

        pNode->get_nodeName(&bstrKey);
        pNode->get_nodeValue(&var);

        if (var.vt != VT_BSTR)
        {
            UIASSERT(0); // 什么情况下会触发？
        }
        else
        {
#ifdef _UNICODE
            (*ppMapAttrib)->AddAttr((const TCHAR*)bstrKey, (const TCHAR*)var.bstrVal);
#else
            (*ppMapAttrib)->AddAttr(CW2T(bstrKey), CW2T(var.bstrVal));
#endif
        }
    }
}

void  UIMarkupElement::GetAttribList2(IListAttribute** ppListAttrib)
{
    CComPtr<IXMLDOMNamedNodeMap> pNodeMap; 
    if (FAILED(m_pElement->get_attributes(&pNodeMap)) || NULL == pNodeMap)
        return;

    UICreateIListAttribute(ppListAttrib);
    while (1)
    {
        CComPtr<IXMLDOMNode> pNode = NULL;

        // 这个是按照xml中的顺序遍历出来的
        if (FAILED(pNodeMap->nextNode(&pNode)) || NULL == pNode)
            break;

        CComBSTR bstrKey;
        CComVariant var;

        pNode->get_nodeName(&bstrKey);
        pNode->get_nodeValue(&var);

        if (var.vt != VT_BSTR)
        {
            UIASSERT(0); // 什么情况下会触发？
        }
        else
        {
#ifdef _UNICODE
            (*ppListAttrib)->AddAttr((const TCHAR*)bstrKey, (const TCHAR*)var.bstrVal);
#else
            (*ppListAttrib)->AddAttr(CW2T(bstrKey), CW2T(var.bstrVal));
#endif
        }
    }
}

IUIChildNodeEnum*  UIMarkupElement::EnumChild()
{
    CComPtr<IXMLDOMNodeList> pNodeList;
    if (FAILED(m_pElement->get_childNodes(&pNodeList)) || NULL == pNodeList)
        return NULL;

    return UIMarkupChildNodeEnum::New(pNodeList.p, m_pDocument);
}

bool  UIMarkupElement::FindChild(BSTR bstrChildName, IUIElement** ppElem)
{
	if (!ppElem)
		return false;

    IXMLDOMNode*  pChildNode = NULL;
    if (FAILED(m_pElement->selectSingleNode(bstrChildName, &pChildNode) || NULL == pChildNode))
        return NULL;

    IUIElement* p = UIMarkupElement::New(pChildNode, m_pDocument);
	SAFE_RELEASE(pChildNode);
	*ppElem = p;
	return true;
}

bool  UIMarkupElement::FirstChild(IUIElement** ppElem)
{
	if (!ppElem)
		return false;

    IXMLDOMNode*  pChildNode = NULL;
    if (FAILED(m_pElement->get_firstChild(&pChildNode) || NULL == pChildNode))
        return false;

    IUIElement* p = UIMarkupElement::New(pChildNode, m_pDocument);
	SAFE_RELEASE(pChildNode);
	*ppElem = p;
	return true;
}

HRESULT  UIMarkupElement::GetDocument(IUIDocument** pp)
{
    // 注：有一个get_ownerDocument接口

    if (NULL == m_pDocument)
        return NULL;

    if (NULL == pp)
        return E_INVALIDARG;

    *pp = m_pDocument;
    m_pDocument->AddRef();
    return S_OK;
}

void  UIMarkupElement::RemoveChild(IUIElement* p)
{
    if (NULL == p)
        return;

    UIMarkupElement* pElem = (UIMarkupElement*)p;
    m_pElement->removeChild(pElem->m_pElement, NULL);
}

bool  UIMarkupElement::AddChild(const TCHAR* szNodeName, IUIElement** pp)
{
    if (NULL == szNodeName)
        return NULL;

    CComPtr<IXMLDOMDocument> pDoc = NULL;
    if (FAILED(m_pElement->get_ownerDocument(&pDoc)))
        return false;

    CComPtr<IXMLDOMElement>  pNewElement = NULL;
    if (FAILED(pDoc->createElement((BSTR)szNodeName, &pNewElement)))
        return false;

    {
        // 添加换行符
        IXMLDOMText* pReturnNode = NULL;
        pDoc->createTextNode(L"\r\n\t\t", &pReturnNode);
        m_pElement->appendChild(pReturnNode, NULL);
        SAFE_RELEASE(pReturnNode);
    }

    CComPtr<IXMLDOMNode> pOutNewElem = NULL;
    if (FAILED(m_pElement->appendChild(pNewElement, &pOutNewElem)))
        return false;

    {
        // 添加换行符
        IXMLDOMText* pReturnNode = NULL;
        pDoc->createTextNode(L"\r\n\t", &pReturnNode);
        m_pElement->appendChild(pReturnNode, NULL);
        SAFE_RELEASE(pReturnNode);
    }

    if (pp)
    {
        UIMarkupElement* p = UIMarkupElement::New(pOutNewElem, m_pDocument);
        *pp = static_cast<IUIElement*>(p);
    }

    return true;
}

bool  UIMarkupElement::AddChild(IUIElement*  pElem)
{
	if (!pElem)
		return false;

	UIMarkupElement* pElement = static_cast<UIMarkupElement*>(pElem);

	CComPtr<IXMLDOMNode> pOutNewElem = NULL;
	HRESULT hr = m_pElement->appendChild(pElement->m_pElement, &pOutNewElem);
	return SUCCEEDED(hr)? true:false;
}

// 如果为空，则表示插在最后面
bool  UIMarkupElement::AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore)
{
    if (NULL == pInsertBefore)
    {
        return AddChild(pElem);
    }

    UIMarkupElement* pElement = static_cast<UIMarkupElement*>(pElem);
    UIMarkupElement* pElementBefore = static_cast<UIMarkupElement*>(pInsertBefore);

    CComPtr<IXMLDOMNode> pOutNewElem = NULL;
    IDispatch* pDispath = NULL;
    pElementBefore->m_pElement.QueryInterface(&pDispath);

    CComVariant  var(pDispath);
    HRESULT hr = m_pElement->insertBefore(pElement->m_pElement, var, &pOutNewElem);

    SAFE_RELEASE(pDispath);
    return SUCCEEDED(hr)? true:false;
}

// ms 没有提供insertAfter的
bool  UIMarkupElement::AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter)
{
    // 插在最前面
    if (!pInsertAfter)
    {
        IUIElement*  pFirstChild = NULL;
        FirstChild(&pFirstChild);
        bool bRet = this->AddChildBefore(pElem, pFirstChild);
        SAFE_RELEASE(pFirstChild);

        return bRet;
    }

    // 查看after的下一个，插在其前面
    UIMarkupElement* pElem2 = static_cast<UIMarkupElement*>(pElem);
    UIMarkupElement* pInsertAfter2 = static_cast<UIMarkupElement*>(pInsertAfter);

    CComPtr<IXMLDOMNode>  pNext;
    pInsertAfter2->m_pElement->get_nextSibling(&pNext);

    if (!pNext)
    {
        return this->AddChild(pElem);
    }

    CComPtr<IXMLDOMNode> pOutNewElem = NULL;
    IDispatch* pDispath = NULL;
    pNext.QueryInterface(&pDispath);

    CComVariant  var(pDispath);
    HRESULT hr = m_pElement->insertBefore(pElem2->m_pElement, var, &pOutNewElem);
    SAFE_RELEASE(pDispath);

    return SUCCEEDED(hr)? true:false;
}

bool  UIMarkupElement::ClearAttrib()
{
    CComPtr<IXMLDOMNamedNodeMap> pNodeMap; 
    if (FAILED(m_pElement->get_attributes(&pNodeMap)) || NULL == pNodeMap)
        return false;

    while (1)
    {
        CComPtr<IXMLDOMNode> pNode = NULL;

        // 这个是按照xml中的顺序遍历出来的
        if (FAILED(pNodeMap->nextNode(&pNode)) || NULL == pNode)
            break;

        CComQIPtr<IXMLDOMAttribute> pAttributeNode = pNode;
        if (pAttributeNode)
        {
            HRESULT hr = m_pElement->removeAttributeNode(pAttributeNode, NULL);
            if (FAILED(hr))
                return false;
        }
    }
    
    return true;
}

//////////////////////////////////////////////////////////////////////////

UIMarkup::UIMarkup()
{
    m_bDirty = false;
    m_lRef = 0;
    m_pDoc = NULL;
}

bool  UIMarkup::LoadContent(BSTR bstrContent)
{
    return Load(NULL, bstrContent, false);
}
bool  UIMarkup::LoadFile(const TCHAR* szFile)
{
    return Load(szFile, NULL, true);
}

// {F5078F1B-C551-11D3-89B9-0000F81FE221}
const GUID CLSID_MSXML2_DOMDocument26 = 
{ 0xF5078F1B, 0xC551, 0x11D3, { 0x89, 0xb9, 0x00, 0x00, 0xF8, 0x1F, 0xE2, 0x21 } };

bool  UIMarkup::Load(const TCHAR* szFile, BSTR bstrContent, bool bFileOrContent)
{
    HRESULT hr = S_OK;
    if (!m_pDoc)
    {
        if (FAILED(m_pDoc.CoCreateInstance(__uuidof(MSXML2::DOMDocument60))))
        {
            if (FAILED(m_pDoc.CoCreateInstance(CLSID_MSXML2_DOMDocument26/*__uuidof(MSXML2::DOMDocument26)*/)))  // xp下创建
            {
                MessageBox(NULL, _T("创建xml解析器失败"), _T("Error"), MB_OK);
                return false;
            }
            else
            {
                MessageBox(NULL, _T("创建xml解析器成功"), _T("Error"), MB_OK);
            }
        }
    }

    hr = m_pDoc->put_validateOnParse(VARIANT_TRUE);  // 对文档格式进行验证，否则当文档非法时,load会崩溃

    VARIANT_BOOL bRet = VARIANT_FALSE;

    if (bFileOrContent)
    {
        CComVariant var(szFile);
        hr = m_pDoc->load(var, &bRet);
    }
    else
    {
        hr = m_pDoc->loadXML(bstrContent, &bRet);
    }
    if (FAILED(hr) || VARIANT_FALSE == bRet)
    {
        IXMLDOMParseError* pErr = NULL;
        m_pDoc->get_parseError(&pErr);
        if (pErr)
        {
            long  lCode = 0;
            long  lLine = 0;
            long  lLinePos = 0;
            CComBSTR  bstrReason;

            pErr->get_errorCode(&lCode);
            pErr->get_reason(&bstrReason);
            pErr->get_line(&lLine);
            pErr->get_linepos(&lLinePos);

            UI_LOG_ERROR(_T("%s Parse Xml Failed. Path=%s, Error Code:%X, Line:%d(%d), Error Reason:%s"), 
                FUNC_NAME, bFileOrContent?szFile:_T(""), lCode, lLine, lLinePos, (BSTR)bstrReason);

            SAFE_RELEASE(pErr);
        }

        return false;
    }

    if (bFileOrContent)
        m_strPath = szFile;

    return true;
}

bool  UIMarkup::Save()
{
    CComVariant var(m_strPath.c_str());
    if (FAILED(m_pDoc->save(var)))
        return false;

    return true;
}
bool  UIMarkup::SaveAs(const TCHAR* szPath)
{
    if (NULL == szPath)
        return false;
    
    CComVariant var(szPath);
    if (FAILED(m_pDoc->save(var)))
        return false;

    return true;
}

// 多个路径之间用 / 进行分隔
bool  UIMarkup::FindElem(const TCHAR* szText, IUIElement** ppElem)
{
	if (!ppElem)
		return false;
		 
    CComBSTR bstr(szText);
    CComPtr<IXMLDOMNode>  pNode = NULL;
    HRESULT hr = m_pDoc->selectSingleNode(bstr, &pNode);
    if (FAILED(hr))
        return false;

    *ppElem = UIMarkupElement::New(pNode.p, this);
	return true;
}

UIMarkup*  UIMarkup::New()
{
    UIMarkup* p = new UIMarkup;
    p->AddRef();
    return p;
}
long  UIMarkup::Release()
{
    m_lRef --;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  UIMarkup::AddRef()
{
    return ++ m_lRef;
}

void  UIMarkup::SetDirty(bool b)
{
    m_bDirty = b;
}
bool  UIMarkup::IsDirty()
{
    return m_bDirty;
}

}