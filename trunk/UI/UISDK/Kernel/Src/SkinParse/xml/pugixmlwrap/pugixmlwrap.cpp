#include "stdafx.h"
#include "pugixmlwrap.h"

using namespace UI;



PugiXmlChildNodeEnum::PugiXmlChildNodeEnum(pugi::xml_node& node, IUIDocument* pDoc)
{
    m_node = node;
    m_pDocument = pDoc;
}

bool PugiXmlChildNodeEnum::CreateInstance(pugi::xml_node& node, IUIDocument* pDoc, PugiXmlChildNodeEnum** pp)
{
    if (!pp)
        return false;

    *pp = new PugiXmlChildNodeEnum(node, pDoc);
    return true;
}

void  PugiXmlChildNodeEnum::Release()
{
    delete this;
}

// 查找元素，跳过其它类型
// 注：需要Release
IUIElement*  PugiXmlChildNodeEnum::NextElement()
{
    if (m_node.empty())
        return NULL;

    if (m_node.type() != pugi::node_element)
    {
        m_node = m_node.next_sibling();
        return this->NextElement();
    }   

    IUIElement* p = NULL;
    PugiXmlElement::CreateInstance(m_node, m_pDocument, &p);

    m_node = m_node.next_sibling();
    return p;
}

//////////////////////////////////////////////////////////////////////////

PugiXmlElement::PugiXmlElement(pugi::xml_node& node, IUIDocument* pDoc)
{
    m_lRef = 0;
    m_node = node;
    m_pDocument = pDoc;
}
PugiXmlElement::~PugiXmlElement()
{
    
}

bool  PugiXmlElement::CreateInstance(pugi::xml_node& node, IUIDocument* pDoc, PugiXmlElement** pp)
{
    if (!pp)
        return false;

    PugiXmlElement* p = new PugiXmlElement(node, pDoc);
    p->AddRef();

    *pp = p;
    return true;
}

bool  PugiXmlElement::CreateInstance(pugi::xml_node& node, IUIDocument* pDoc, IUIElement** pp)
{
    if (!pp)
        return false;

    PugiXmlElement* p = NULL;   
    if (!PugiXmlElement::CreateInstance(node, pDoc, &p))
        return false;

    *pp = static_cast<IUIElement*>(p);
    return true;
}

long  PugiXmlElement::AddRef()
{
    return ++m_lRef;
}
long  PugiXmlElement::Release()
{
    m_lRef--;

    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }

    return m_lRef;
}


void  PugiXmlElement::GetTagName(BSTR* pbstr)
{
    if (!pbstr)
        return;

    *pbstr = SysAllocString(m_node.name());
}

void  PugiXmlElement::SetTagName(LPCTSTR szTag)
{
	m_node.set_name(szTag);
}

bool  PugiXmlElement::GetDocument(IUIDocument** pp)
{
    if (NULL == m_pDocument)
        return false;

    if (NULL == pp)
        return false;

    *pp = m_pDocument;
    m_pDocument->AddRef();
    return true;
}

bool  PugiXmlElement::GetData(BSTR* pbstr)
{
    if (!pbstr)
        return false;

    *pbstr = ::SysAllocString(m_node.text().as_string());
    return true;
}
bool  PugiXmlElement::SetData(LPCTSTR szText)
{
    m_node.text() = szText;
    return true;
}

bool  PugiXmlElement::AddChild(LPCTSTR szNodeName, IUIElement** pp)
{
	if (!szNodeName)
		return false;

	pugi::xml_node node = m_node.append_child(szNodeName);
	if (node.empty())
		return false;

	if (pp)
	{
		PugiXmlElement::CreateInstance(node, m_pDocument, pp);
	}
	return true;
}

bool  PugiXmlElement::AddChild(IUIElement* p)
{
	if (!p)
		return false;

	PugiXmlElement* pElement = static_cast<PugiXmlElement*>(p);
	return m_node.append_copy(pElement->m_node).empty() ? false:true;
}

bool  PugiXmlElement::RemoveChild(IUIElement* p)
{
	if (!p)
		return false;

	PugiXmlElement* pElem = (PugiXmlElement*)p;
	return m_node.remove_child(pElem->m_node);
}

bool  PugiXmlElement::RemoveSelfInParent()
{
	if (m_node.parent().empty())
		return false;

	return m_node.parent().remove_child(m_node);
}

// 如果为空，则表示插在最后面
bool  PugiXmlElement::AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore)
{
	if (!pElem)
		return false;

	if (!pInsertBefore)
		return AddChild(pElem);

	PugiXmlElement* pElement = static_cast<PugiXmlElement*>(pElem);
	PugiXmlElement* pElementBefore = static_cast<PugiXmlElement*>(pInsertBefore);

	return m_node.insert_copy_before(pElement->m_node, pElementBefore->m_node).empty() ? false:true;
}

bool  PugiXmlElement::AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter)
{
	if (!pElem)
		return false;

	PugiXmlElement* pElement = static_cast<PugiXmlElement*>(pElem);

	// 插在最前面
	if (!pInsertAfter)
	{
		return m_node.prepend_copy(pElement->m_node).empty() ? false:true;
	}

	// 查看after的下一个，插在其前面
	PugiXmlElement* pElementInsertAfter = static_cast<PugiXmlElement*>(pInsertAfter);

	return m_node.insert_copy_after(pElement->m_node, pElementInsertAfter->m_node).empty() ? false:true;
}

// 如果为空，则表示插在最后面
bool  PugiXmlElement::AddChildBefore(LPCTSTR szNodeName, IUIElement* pInsertBefore, IUIElement** pp)
{
	if (!szNodeName)
		return false;

	if (!pInsertBefore)
		return AddChild(szNodeName, pp);

	PugiXmlElement* pElementInsertBefore = static_cast<PugiXmlElement*>(pInsertBefore);
	pugi::xml_node node = m_node.insert_child_before(szNodeName, pElementInsertBefore->m_node);
	if (node.empty())
		return false;

	if (pp)
	{
		PugiXmlElement::CreateInstance(node, m_pDocument, pp);
	}
	return true;
}
bool  PugiXmlElement::AddChildAfter(LPCTSTR szNodeName, IUIElement* pInsertAfter, IUIElement** pp)
{
	if (!szNodeName)
		return false;

	pugi::xml_node node;

	// 插在最前面
	if (!pInsertAfter)
	{
		node = m_node.prepend_child(szNodeName);
	}
	else
	{
		PugiXmlElement* pElementInsertAfter = static_cast<PugiXmlElement*>(pInsertAfter);
		node = m_node.insert_child_after(szNodeName, pElementInsertAfter->m_node);
	}
	
	if (node.empty())
		return false;

	if (pp)
	{
		PugiXmlElement::CreateInstance(node, m_pDocument, pp);
	}
	return true;
}

bool  PugiXmlElement::FirstChild(IUIElement** ppElem)
{
    if (!ppElem)
        return false;

    pugi::xml_node nodeChild = m_node.first_child();
    return PugiXmlElement::CreateInstance(nodeChild, m_pDocument, ppElem);
}

bool  PugiXmlElement::FindChild(LPCTSTR szChildName, IUIElement** ppElem)
{
    if (!ppElem)
        return false;

    pugi::xml_node nodeChild = m_node.child(szChildName);
    return PugiXmlElement::CreateInstance(nodeChild, m_pDocument, ppElem);
}

IUIChildNodeEnum*  PugiXmlElement::EnumChild()   // 注：需要Release
{
    pugi::xml_node nodeChild = m_node.first_child();
    
    PugiXmlChildNodeEnum* p = NULL;
    if (!PugiXmlChildNodeEnum::CreateInstance(nodeChild, m_pDocument, &p))
        return NULL;
    return static_cast<IUIChildNodeEnum*>(p);
}


void  PugiXmlElement::GetAttribList(IMapAttribute** ppMapAttrib)
{
    UICreateIMapAttribute(ppMapAttrib);

    (*ppMapAttrib)->SetTag(m_node.name());

    for (pugi::xml_attribute attr = m_node.first_attribute(); 
        !attr.empty(); 
        attr = attr.next_attribute())
    {
        (*ppMapAttrib)->AddAttr(attr.name(), attr.as_string());
    }
}
void  PugiXmlElement::GetAttribList2(IListAttribute** ppListAttrib)
{
    UICreateIListAttribute(ppListAttrib);

    (*ppListAttrib)->SetTag(m_node.name());

    for (pugi::xml_attribute attr = m_node.first_attribute(); 
        !attr.empty(); 
        attr = attr.next_attribute())
    {
        (*ppListAttrib)->AddAttr(attr.name(), attr.as_string());
    }
}

bool  PugiXmlElement::SetAttribList2(IListAttribute* pListAttrib)
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

bool  PugiXmlElement::GetAttrib(LPCTSTR szKey, BSTR* pbstrValue)
{
    if (NULL == szKey)
        return false;

    pugi::xml_attribute attr = m_node.attribute(szKey);
    if (attr.empty())
        return false;

    if (NULL != pbstrValue)
        *pbstrValue = ::SysAllocString(attr.as_string());

    return true;
}
bool  PugiXmlElement::HasAttrib(LPCTSTR szKey)
{
    if (NULL == szKey)
        return false;

    if (0 == _tcslen(szKey))
        return false;

    return (!m_node.attribute(szKey).empty());
}
bool  PugiXmlElement::AddAttrib(LPCTSTR szKey, LPCTSTR szValue)
{
    if (NULL == szKey || NULL == szValue)
        return false;

    m_node.append_attribute(szKey) = szValue;

    return true;
}

bool  PugiXmlElement::SetAttrib(LPCTSTR szKey, LPCTSTR szValue)
{
	if (NULL == szKey || NULL == szValue)
		return false;

	pugi::xml_attribute attr = m_node.attribute(szKey);
	if (attr.empty())
		return false;

	return attr.set_value(szValue);
}

bool  PugiXmlElement::RemoveAttrib(LPCTSTR szKey)
{
    if (NULL == szKey)
        return false;

    return m_node.remove_attribute(szKey);
}
bool  PugiXmlElement::ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue)
{
    return AddAttrib(szKey, szValue);
}
bool  PugiXmlElement::ClearAttrib()
{
	pugi::xml_attribute_iterator iter = m_node.attributes_begin();
	for (; iter != m_node.attributes_end(); ++iter)
	{
		m_node.remove_attribute(*iter);
	}
    return true;
}

bool  PugiXmlElement::GetAttribInt(LPCTSTR szKey, int* pInt)
{
	if (NULL == szKey || NULL == pInt)
		return false;

	CComBSTR  bstrValue;
	if (!GetAttrib(szKey, &bstrValue))
		return false;

	*pInt = _wtoi(bstrValue);
	return true;
}
bool  PugiXmlElement::AddAttribInt(LPCTSTR szKey, int nInt)
{
	TCHAR szValue[16] = {0};
	_stprintf(szValue, TEXT("%d"), nInt);

	return AddAttrib(szKey, szValue);
}
//////////////////////////////////////////////////////////////////////////


PugiXmlDocument::PugiXmlDocument()
{
    m_bDirty = false;
	m_lRef = 0;
}
PugiXmlDocument::~PugiXmlDocument()
{
}


bool  PugiXmlDocument::CreateInstance(PugiXmlDocument** pp)
{
    if (!pp)
        return false;

    PugiXmlDocument* p = new PugiXmlDocument;
    p->AddRef();
    *pp = p;
    return true;
}

bool  PugiXmlDocument::CreateInstance(IUIDocument** pp)
{
    if (!pp)
        return false;

    PugiXmlDocument* p = NULL;
    if (!CreateInstance(&p))
        return false;

    *pp = static_cast<IUIDocument*>(p);
    return true;
}

bool  PugiXmlDocument::LoadFile(LPCTSTR szFile)
{
    if (!szFile)
        return false;

    if (!PathFileExists(szFile))
        return false;

    m_strPath.clear();
	if (pugi::status_ok != m_doc.load_file(szFile, pugi::parse_full).status)
        return false;

    m_strPath = szFile;
    return true;
}

bool  PugiXmlDocument::LoadData(const byte*  pData, int nDataSize)
{
    if (!pData || nDataSize <= 0)
        return false;

    m_strPath.clear();
    if (pugi::status_ok != m_doc.load_buffer((void*)pData, nDataSize).status)
        return false;
    
    return true;
}
bool  PugiXmlDocument::FindElem(LPCTSTR szText, IUIElement** ppElem) 
{
    if (!szText || !ppElem)
        return false;

    pugi::xml_node node = m_doc.child(szText);
    if (node.empty())
    {
        *ppElem = NULL;
        return false;
    }

    return PugiXmlElement::CreateInstance(node, static_cast<IUIDocument*>(this), ppElem);
}

bool  PugiXmlDocument::Save()
{
    return m_doc.save_file(m_strPath.c_str(), PUGIXML_TEXT("\t"), 
            pugi::format_default | pugi::format_write_bom, pugi::encoding_auto);
}

bool  PugiXmlDocument::SaveAs(LPCTSTR szPath)
{
    return m_doc.save_file(szPath, PUGIXML_TEXT("\t"), 
            pugi::format_default | pugi::format_write_bom, pugi::encoding_auto);
}

long  PugiXmlDocument::Release()
{
    m_lRef --;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  PugiXmlDocument::AddRef()
{
    return ++m_lRef;
}

void  PugiXmlDocument::SetDirty(bool b)
{
    m_bDirty = b;
}
bool  PugiXmlDocument::IsDirty()
{
    return m_bDirty;
}

LPCTSTR  PugiXmlDocument::GetPath()
{ 
    return m_strPath.c_str();
}