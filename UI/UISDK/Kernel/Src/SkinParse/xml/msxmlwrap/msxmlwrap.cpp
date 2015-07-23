#include "stdafx.h"
#include "msxmlwrap.h"

namespace UI
{

MsXmlChildNodeEnum::MsXmlChildNodeEnum(IXMLDOMNodeList* p, IUIDocument* pDoc)
{
    m_pNodeList = p;
    m_pDocument = pDoc;
}

bool MsXmlChildNodeEnum::CreateInstance(IXMLDOMNodeList* pNode, IUIDocument* pDoc, MsXmlChildNodeEnum** pp)
{
    if (pNode || !pp) 
        return false;

    *pp = new MsXmlChildNodeEnum(pNode, pDoc);
    return true;
}

void  MsXmlChildNodeEnum::Release()
{
    delete this;
}

// 查找元素，跳过其它类型
IUIElement*  MsXmlChildNodeEnum::NextElement()
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

    MsXmlElement* p = NULL;
    MsXmlElement::CreateInstance(pNode.p, m_pDocument, &p);
    return p;
}

MsXmlElement::MsXmlElement(IXMLDOMElement* pNode, IUIDocument* pDoc)
{
    m_lRef = 0;
    m_pElement = pNode;
    m_pDocument = pDoc;
}
void MsXmlElement::CreateInstance(IXMLDOMNode* pNode, IUIDocument* pDoc, MsXmlElement** pp)
{
    if (NULL == pNode || !pp)
        return;

    CComQIPtr<IXMLDOMElement> p = pNode;
    if (NULL == p)
        return;

    MsXmlElement* pNew = new MsXmlElement(p, pDoc);
    pNew->AddRef();
    *pp = pNew;
}

void  MsXmlElement::CreateInstance(IXMLDOMNode* pNode, IUIDocument* pDoc, IUIElement** pp)
{
    if (!pp)
        return;

    MsXmlElement* p = NULL;
    CreateInstance(pNode, pDoc, &p);

    *pp = static_cast<IUIElement*>(p);
}

long  MsXmlElement::AddRef()
{
    return ++m_lRef;
}
long  MsXmlElement::Release()
{
    m_lRef--;

    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }

    return m_lRef;
}

void  MsXmlElement::GetTagName(BSTR* pbstr)
{
    m_pElement->get_tagName(pbstr);
}

void  MsXmlElement::SetTagName(LPCTSTR szTag)
{
	CComQIPtr<IXMLElement> spElem = m_pElement;
	if (spElem)
		spElem->put_tagName(CComBSTR(szTag));
}

// <a> bb </a> 获取 bb
bool  MsXmlElement::GetData(BSTR* pbstr)
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

bool  MsXmlElement::SetData(LPCTSTR szText)
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

bool  MsXmlElement::GetAttrib(LPCTSTR szKey, BSTR* pbstrValue)
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
bool  MsXmlElement::GetAttribInt(LPCTSTR szKey, int* pInt)
{
    if (NULL == szKey || NULL == pInt)
        return false;

    CComBSTR  bstrValue;
    if (!GetAttrib(szKey, &bstrValue))
        return false;

    *pInt = _wtoi(bstrValue);
    return true;
}
bool  MsXmlElement::HasAttrib(LPCTSTR szKey)
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

bool  MsXmlElement::AddAttrib(LPCTSTR szKey, LPCTSTR szValue)
{
    if (NULL == szKey || NULL == szValue)
        return false;

    CComVariant var(szValue);
    if (FAILED(m_pElement->setAttribute((BSTR)szKey, var)))
        return false;

    return true;
}

bool  MsXmlElement::SetAttrib(LPCTSTR szKey, LPCTSTR szValue)
{
	if (NULL == szKey || NULL == szValue)
		return false;

	CComVariant var(szValue);
	if (FAILED(m_pElement->setAttribute((BSTR)szKey, var)))
		return false;

	return true;
}

bool  MsXmlElement::AddAttribInt(LPCTSTR szKey, int nInt)
{
    TCHAR szValue[16] = {0};
    _stprintf(szValue, TEXT("%d"), nInt);
    
    return AddAttrib(szKey, szValue);
}

bool  MsXmlElement::RemoveAttrib(LPCTSTR szKey)
{
    if (NULL == szKey)
        return false;

    if (FAILED(m_pElement->removeAttribute((BSTR)szKey)))
        return false;

    return true;
}

bool  MsXmlElement::ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue)
{
    return AddAttrib(szKey, szValue);
}

void  MsXmlElement::GetAttribList(IMapAttribute** ppMapAttrib)
{
    CComPtr<IXMLDOMNamedNodeMap> pNodeMap; 
    if (FAILED(m_pElement->get_attributes(&pNodeMap)) || NULL == pNodeMap)
        return;

    UICreateIMapAttribute(ppMapAttrib);

	CComBSTR  bstrTag;
	m_pElement->get_tagName(&bstrTag);
	(*ppMapAttrib)->SetTag(CW2T(bstrTag));

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

void  MsXmlElement::GetAttribList2(IListAttribute** ppListAttrib)
{
    CComPtr<IXMLDOMNamedNodeMap> pNodeMap; 
    if (FAILED(m_pElement->get_attributes(&pNodeMap)) || NULL == pNodeMap)
        return;

    UICreateIListAttribute(ppListAttrib);

	CComBSTR  bstrTag;
	m_pElement->get_tagName(&bstrTag);
	(*ppListAttrib)->SetTag(CW2T(bstrTag));

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

bool  MsXmlElement::SetAttribList2(IListAttribute* pListAttrib)
{
    if (!pListAttrib)
        return false;

    ClearAttrib();
    pListAttrib->BeginEnum();

    LPCTSTR szKey = NULL;
    LPCTSTR szValue = NULL;
    while (pListAttrib->EnumNext(&szKey, &szValue))
    {
        AddAttrib(szKey, szValue);
    }
    pListAttrib->EndEnum();

    return true;
}

IUIChildNodeEnum*  MsXmlElement::EnumChild()
{
    CComPtr<IXMLDOMNodeList> pNodeList;
    if (FAILED(m_pElement->get_childNodes(&pNodeList)) || NULL == pNodeList)
        return NULL;

    MsXmlChildNodeEnum* p = NULL;
    if (!MsXmlChildNodeEnum::CreateInstance(pNodeList.p, m_pDocument, &p))
        return NULL;

    return static_cast<IUIChildNodeEnum*>(p);
}

bool  MsXmlElement::FindChild(LPCTSTR szChildName, IUIElement** ppElem)
{
	if (!ppElem || !szChildName)
		return false;

    IXMLDOMNode*  pChildNode = NULL;
    if (FAILED(m_pElement->selectSingleNode((BSTR)szChildName, &pChildNode) || NULL == pChildNode))
        return NULL;

    MsXmlElement::CreateInstance(pChildNode, m_pDocument, ppElem);
	SAFE_RELEASE(pChildNode);
	return true;
}

bool  MsXmlElement::FirstChild(IUIElement** ppElem)
{
	if (!ppElem)
		return false;
    *ppElem = NULL;

    IXMLDOMNode*  pChildNode = NULL;
    if (FAILED(m_pElement->get_firstChild(&pChildNode) || NULL == pChildNode))
        return false;

    while (pChildNode)
    {
        IUIElement* p = NULL;
        MsXmlElement::CreateInstance(pChildNode, m_pDocument, &p);
        if (p)
        {
            SAFE_RELEASE(pChildNode);
            *ppElem = p;
            return true;
        }

        // 有可能不是一个element结点，继续
        IXMLDOMNode* pNext2 = NULL;
        if (FAILED(pChildNode->get_nextSibling(&pNext2)))
        {
            pChildNode->Release();
            return false;
        }
        pChildNode->Release();
        pChildNode = pNext2;
    }

    return false;
}


bool  MsXmlElement::NextSiblingElem(IUIElement** ppElem)
{
    if (!ppElem)
        return false;
    *ppElem = NULL;

    IXMLDOMNode* pNext = NULL;
    if (FAILED(m_pElement->get_nextSibling(&pNext)))
        return false;

    while (pNext)
    {
        IUIElement* p = NULL;
        MsXmlElement::CreateInstance(pNext, m_pDocument, &p);
        if (p)
        {
            SAFE_RELEASE(pNext);
            *ppElem = p;
            return true;
        }

        // 有可能不是一个element结点，继续
        IXMLDOMNode* pNext2 = NULL;
        if (FAILED(pNext->get_nextSibling(&pNext2)))
        {
            pNext->Release();
            return NULL;
        }
        pNext->Release();
        pNext = pNext2;
    }

    return false;
}

bool  MsXmlElement::GetDocument(IUIDocument** pp)
{
    // 注：有一个get_ownerDocument接口

    if (NULL == m_pDocument)
        return false;

    if (NULL == pp)
        return false;

    *pp = m_pDocument;
    m_pDocument->AddRef();
    return true;
}

bool  MsXmlElement::RemoveChild(IUIElement* p)
{
    if (!p)
        return false;

    MsXmlElement* pElem = (MsXmlElement*)p;
    m_pElement->removeChild(pElem->m_pElement, NULL);
	return true;
}

long  MsXmlElement::GetDepth()
{
	long  lDepth = 0;

	CComPtr<IXMLDOMNode> pChild = m_pElement;
	CComPtr<IXMLDOMNode> pParent = NULL;

	while (pChild && SUCCEEDED(pChild->get_parentNode(&pParent)) && pParent)
	{
		lDepth++;

		pChild = pParent;
		pParent.Release();
	}

	if (lDepth > 0)
	{
		lDepth -= 1; // 减掉get_parentNode枚举到的最顶层的 #document#
	}

	return lDepth;
}

// 添加子结点时，需要自己负责添加缩进。包括前面的空白和后面的空白。
//		如果当前行已经包含空白行，则判断中否需要增加缩进（废弃）
// 如果是文本，则替换为新的换行和缩进
void  MsXmlElement::AddNewLine(int nTabs)
{
	if (nTabs < 0)
		nTabs = 0;

	// 取出最后一个子结点，如果是text，
	CComQIPtr<IXMLDOMText> pLastTextNode;
	{
		CComPtr<IXMLDOMNode> pLastNode;
		m_pElement->get_lastChild(&pLastNode);
		DOMNodeType eType = NODE_INVALID;
		if (pLastNode && SUCCEEDED(pLastNode->get_nodeType(&eType)))
		{
			if (eType == NODE_TEXT)
			{
				pLastTextNode = pLastNode;
// 				if (pText)
// 				{
// 					CComBSTR bstrLastText;
// 					pText->get_data(&bstrLastText);
// 				}
			}
		}
	}

	// 判断当前需要添加多个
// 	bool bAdd_R_N = true;
// 	if (bstrLastText)
// 	{
// 		wchar_t* p = wcsrchr(bstrLastText, TEXT('\n'));
// 		if (p)
// 		{
// 			bAdd_R_N = false;
// 			p+=1;
// 			while (*p && *p==TEXT('\t'))
// 			{
// 				nTabs--;
// 				p++;
// 			}
// 
// 			if (nTabs <= 0)
// 				return;
// 		}
// 	}
	
	CComBSTR  appendText;
	CComPtr<IXMLDOMText> pNewLineNode = NULL;

	//if (bAdd_R_N)
		appendText.Append(TEXT("\r\n"));
	for (int i = 0; i < nTabs; i++)
		appendText.Append(TEXT("\t"));

	if (pLastTextNode)
	{
		pLastTextNode->put_data(appendText);
	}
	else
	{
		CComPtr<IXMLDOMDocument> pDoc = NULL;
		if (FAILED(m_pElement->get_ownerDocument(&pDoc)))
			return;

		pDoc->createTextNode(appendText, &pNewLineNode);
		m_pElement->appendChild(pNewLineNode, NULL);
	}
}

bool  MsXmlElement::RemoveSelfInParent()
{
	CComPtr<IXMLDOMNode> pParent = NULL;
	m_pElement->get_parentNode(&pParent);
	if (!pParent)
		return false;

	if (SUCCEEDED(pParent->removeChild(m_pElement, NULL)))
		return true;
	return false;
}

bool  MsXmlElement::AddChild(LPCTSTR szNodeName, IUIElement** pp)
{
    if (!szNodeName)
        return false;

    CComPtr<IXMLDOMDocument> pDoc = NULL;
    if (FAILED(m_pElement->get_ownerDocument(&pDoc)))
        return false;

    CComPtr<IXMLDOMElement>  pNewElement = NULL;
    if (FAILED(pDoc->createElement((BSTR)szNodeName, &pNewElement)))
        return false;
   
	int nDepth = GetDepth();
	AddNewLine(nDepth+1);

    CComPtr<IXMLDOMNode> pOutNewElem = NULL;
    if (FAILED(m_pElement->appendChild(pNewElement, &pOutNewElem)))
        return false;

    AddNewLine(nDepth);

    if (pp)
    {
        MsXmlElement::CreateInstance(pOutNewElem, m_pDocument, pp);
    }

    return true;
}

bool  MsXmlElement::AddChild(IUIElement*  pElem)
{
	if (!pElem)
		return false;

	MsXmlElement* pElement = static_cast<MsXmlElement*>(pElem);

	CComPtr<IXMLDOMNode> pOutNewElem = NULL;
	HRESULT hr = m_pElement->appendChild(pElement->m_pElement, &pOutNewElem);
	return SUCCEEDED(hr)? true:false;
}

// 如果为空，则表示插在最后面
bool  MsXmlElement::AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore)
{
	if (!pElem)
		return false;
    if (!pInsertBefore)
        return AddChild(pElem);

    MsXmlElement* pElement = static_cast<MsXmlElement*>(pElem);
    MsXmlElement* pElementBefore = static_cast<MsXmlElement*>(pInsertBefore);

    CComPtr<IXMLDOMNode> pOutNewElem = NULL;
    IDispatch* pDispath = NULL;
    pElementBefore->m_pElement.QueryInterface(&pDispath);

    CComVariant  var(pDispath);
    HRESULT hr = m_pElement->insertBefore(pElement->m_pElement, var, &pOutNewElem);

    SAFE_RELEASE(pDispath);
    return SUCCEEDED(hr)? true:false;
}

// ms 没有提供insertAfter的
bool  MsXmlElement::AddChildAfter(IUIElement* pElem, IUIElement* pInsertAfter)
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
    MsXmlElement* pElem2 = static_cast<MsXmlElement*>(pElem);
    MsXmlElement* pInsertAfter2 = static_cast<MsXmlElement*>(pInsertAfter);

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

bool  MsXmlElement::AddChildBefore(LPCTSTR szNodeName, IUIElement* pInsertBefore, IUIElement** pp)
{
	if (!szNodeName)
		return false;

	if (!pInsertBefore)
		return AddChild(szNodeName, pp);

	CComPtr<IXMLDOMDocument> pDoc = NULL;
	if (FAILED(m_pElement->get_ownerDocument(&pDoc)))
		return false;

	CComPtr<IXMLDOMElement>  pNewElement = NULL;
	if (FAILED(pDoc->createElement((BSTR)szNodeName, &pNewElement)))
		return false;

	IUIElement* pOutElement = NULL;
	MsXmlElement::CreateInstance(pNewElement, m_pDocument, &pOutElement);

	if (!AddChildBefore(pOutElement, pInsertBefore))
	{
		SAFE_RELEASE(pOutElement);
		return false;
	}

	if (pp)
		*pp = pOutElement;
	
	return true;
}
bool  MsXmlElement::AddChildAfter(LPCTSTR szNodeName, IUIElement* pInsertAfter, IUIElement** pp)
{
	if (!szNodeName)
		return false;

	CComPtr<IXMLDOMDocument> pDoc = NULL;
	if (FAILED(m_pElement->get_ownerDocument(&pDoc)))
		return false;

	CComPtr<IXMLDOMElement>  pNewElement = NULL;
	if (FAILED(pDoc->createElement((BSTR)szNodeName, &pNewElement)))
		return false;

	IUIElement* pOutElement = NULL;
	MsXmlElement::CreateInstance(pNewElement, m_pDocument, &pOutElement);

	if (!AddChildAfter(pOutElement, pInsertAfter))
		return false;

	if (pp)
		*pp = pOutElement;
	return true;
}

bool  MsXmlElement::MoveChildAfterChild(IUIElement* pChild2Move, IUIElement* pChildInsertAfter)
{
	if (!pChild2Move)
		return false;

	if (!this->RemoveChild(pChild2Move))
		return false;

	if (!this->AddChildAfter(pChild2Move, pChildInsertAfter))
		return false;

	return true;
}

bool  MsXmlElement::ClearAttrib()
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

MsXmlDocument::MsXmlDocument()
{
    m_bDirty = false;
    m_lRef = 0;
    m_pDoc = NULL;
}


// {F5078F1B-C551-11D3-89B9-0000F81FE221}
const GUID CLSID_MSXML2_DOMDocument26 = 
{ 0xF5078F1B, 0xC551, 0x11D3, { 0x89, 0xb9, 0x00, 0x00, 0xF8, 0x1F, 0xE2, 0x21 } };


// 注：调用m_pDoc->loadxml时，内部只能用unicode，因只不能直接将char*->wchat_t*，可能是将utf8->unicode
//     在这里改为使用原始字节流进行加载(loaddata)
bool  MsXmlDocument::LoadFile(LPCTSTR szFile)
{
    if (!szFile)
        return false;

    if (!PathFileExists(szFile))
        return false;

    HRESULT hr = S_OK;
    if (!m_pDoc)
    {
        hr = m_pDoc.CoCreateInstance(__uuidof(MSXML2::DOMDocument60));
        if (FAILED(hr))
        {
            // xp下创建
            hr = m_pDoc.CoCreateInstance(CLSID_MSXML2_DOMDocument26/*__uuidof(MSXML2::DOMDocument26)*/);
            if (FAILED(hr))
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

    CComVariant var(szFile);
    hr = m_pDoc->load(var, &bRet);

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

            UI_LOG_ERROR(_T("Parse Xml Failed. Path=%s, Error Code:%X, Line:%d(%d), Error Reason:%s"), 
                szFile, lCode, lLine, lLinePos, (BSTR)bstrReason);

            SAFE_RELEASE(pErr);
        }

        return false;
    }

    m_strPath = szFile;
    return true;
}

bool  MsXmlDocument::LoadData(const byte*  pData, int nDataSize)
{
    if (!pData || nDataSize <= 0)
        return false;

    HRESULT hr = S_OK;
    if (!m_pDoc)
    {
        if (FAILED(m_pDoc.CoCreateInstance(__uuidof(MSXML2::DOMDocument60))))
        {
            if (FAILED(m_pDoc.CoCreateInstance(CLSID_MSXML2_DOMDocument26/*__uuidof(MSXML2::DOMDocument26)*/)))  // xp下
            {
                MessageBox(NULL, _T("创建xml解析器失败"), _T("Error"), MB_OK);
                return false;
            }
        }
    }
    hr = m_pDoc->put_validateOnParse(VARIANT_TRUE);

    VARIANT_BOOL bRet = VARIANT_FALSE;

    try
    {
        SAFEARRAY*  psa = NULL;
        SAFEARRAYBOUND  bound = {nDataSize, 0};
        psa = SafeArrayCreate(VT_UI1, 1, &bound);
        for (long i = 0; i < nDataSize; i++)
        {
            SafeArrayPutElement(psa, &i, (void*)&pData[i]);
        }

        CComVariant  var;
        var.vt = VT_ARRAY|VT_UI1;
        var.parray = psa;

        hr = m_pDoc->load(var, &bRet);
    }
    catch (...)
    {

    }
    if (FAILED(hr) || VARIANT_FALSE == bRet)
    {
        return false;
    }

    m_strPath.clear();

    return true;
}

bool  MsXmlDocument::Save()
{
    CComVariant var(m_strPath.c_str());
    if (FAILED(m_pDoc->save(var)))
        return false;

    return true;
}
bool  MsXmlDocument::SaveAs(LPCTSTR szPath)
{
    if (NULL == szPath)
        return false;
    
    CComVariant var(szPath);
    if (FAILED(m_pDoc->save(var)))
        return false;

    return true;
}

// 多个路径之间用 / 进行分隔
bool  MsXmlDocument::FindElem(LPCTSTR szText, IUIElement** ppElem)
{
	if (!ppElem)
		return false;
		 
    CComBSTR bstr(szText);
    CComPtr<IXMLDOMNode>  pNode = NULL;
    HRESULT hr = m_pDoc->selectSingleNode(bstr, &pNode);
    if (FAILED(hr))
        return false;

    MsXmlElement::CreateInstance(pNode.p, this, ppElem);
	return true;
}

bool  MsXmlDocument::CreateInstance(MsXmlDocument** pp)
{
    if (!pp)
        return false;

    MsXmlDocument* p = new MsXmlDocument;
    p->AddRef();
    *pp = p;
    return true;
}

bool  MsXmlDocument::CreateInstance(IUIDocument** pp)
{
    if (!pp)
        return false;

    MsXmlDocument* p = NULL;
    if (!CreateInstance(&p))
        return false;

    *pp = static_cast<IUIDocument*>(p);
    return true;
}

long  MsXmlDocument::Release()
{
    m_lRef --;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  MsXmlDocument::AddRef()
{
    return ++ m_lRef;
}

void  MsXmlDocument::SetDirty(bool b)
{
    m_bDirty = b;
}
bool  MsXmlDocument::IsDirty()
{
    return m_bDirty;
}
LPCTSTR  MsXmlDocument::GetPath()
{ 
    return m_strPath.c_str();
}

}