#include "stdafx.h"
#include "pugixmlwrap.h"
#include "Inc\Interface\imapattr.h"


PugiXmlElement::PugiXmlElement(pugi::xml_node& node, UIDocument* pDoc) :
	m_IUIElement(this)
{
    m_lRef = 0;
    m_node = node;
    m_pDocument = pDoc;
}
PugiXmlElement::~PugiXmlElement()
{
    
}

IUIElement*  PugiXmlElement::GetIUIElement()
{
	return &m_IUIElement;
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


LPCTSTR  PugiXmlElement::GetTagName()
{
    return m_node.name();
}

void  PugiXmlElement::SetTagName(LPCTSTR szTag)
{
	m_node.set_name(szTag);
}
UIDocument*  PugiXmlElement::GetDocument()
{
    return m_pDocument;
}

LPCTSTR  PugiXmlElement::GetData()
{
    return m_node.text().as_string();
}

bool  PugiXmlElement::SetData(LPCTSTR szText)
{
    m_node.text() = szText;
    return true;
}

UIElementProxy  PugiXmlElement::AddChild(LPCTSTR szNodeName)
{
	if (!szNodeName)
		return false;

	pugi::xml_node node = m_node.append_child(szNodeName);
	if (node.empty())
		return UIElementProxy();

	return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

bool  PugiXmlElement::AddChild(UIElement* p)
{
    if (!p)
        return false;

    PugiXmlElement* pElement = static_cast<PugiXmlElement*>(p);
    return m_node.append_copy(pElement->m_node).empty() ? false:true;
}

bool  PugiXmlElement::RemoveChild(UIElement* p)
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
bool  PugiXmlElement::AddChildBefore(UIElement* pElem, UIElement* pInsertBefore)
{
	if (!pElem)
		return false;

	if (!pInsertBefore)
		return AddChild(pElem);

	PugiXmlElement* pElement = static_cast<PugiXmlElement*>(pElem);
	PugiXmlElement* pElementBefore = static_cast<PugiXmlElement*>(pInsertBefore);

	return m_node.insert_copy_before(pElement->m_node, pElementBefore->m_node).empty() ? false:true;
}

bool  PugiXmlElement::AddChildAfter(UIElement*  pElem, UIElement* pInsertAfter)
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


bool  PugiXmlElement::MoveChildAfterChild(UIElement* pChild2Move, UIElement* pChildInsertAfter)
{
	if (!pChild2Move)
		return false;

	PugiXmlElement* _pChild2Move = static_cast<PugiXmlElement*>(pChild2Move);

	if (pChildInsertAfter)
	{
		PugiXmlElement* _pChildInsertAfter = static_cast<PugiXmlElement*>(pChildInsertAfter);
		pugi::xml_node new_node = m_node.insert_copy_after(_pChild2Move->m_node, _pChildInsertAfter->m_node);
		if (new_node.empty())
			return false;

		m_node.remove_child(_pChild2Move->m_node);
		_pChild2Move->m_node = new_node;
	}
	else
	{
		pugi::xml_node new_node = m_node.prepend_copy(_pChild2Move->m_node);
		if (new_node.empty())
			return false;

		m_node.remove_child(_pChild2Move->m_node);
		_pChild2Move->m_node = new_node;
	}

	return true;
}

// 如果为空，则表示插在最后面
UIElementProxy  PugiXmlElement::AddChildBefore(LPCTSTR szNodeName, UIElement* pInsertBefore)
{
	if (!szNodeName)
		return UIElementProxy();

	if (!pInsertBefore)
		return AddChild(szNodeName);

	PugiXmlElement* pElementInsertBefore = static_cast<PugiXmlElement*>(pInsertBefore);
	pugi::xml_node node = m_node.insert_child_before(szNodeName, pElementInsertBefore->m_node);
	if (node.empty())
		return UIElementProxy();

    return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

UIElementProxy  PugiXmlElement::AddChildAfter(LPCTSTR szNodeName, UIElement* pInsertAfter)
{
	if (!szNodeName)
		return UIElementProxy();

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

	return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

UIElementProxy PugiXmlElement::FirstChild()
{
    pugi::xml_node nodeChild = m_node.first_child();
	if (!nodeChild)
		return UIElementProxy();

    return UIElementProxy(new PugiXmlElement(nodeChild, m_pDocument));
}

UIElementProxy  PugiXmlElement::NextElement()
{
    pugi::xml_node nodeNext = m_node;
    while (nodeNext = nodeNext.next_sibling())
    {
        if (nodeNext.type() == pugi::node_comment)
            continue;

        break;
    }

	if (!nodeNext)
		return UIElementProxy();
    
	return UIElementProxy(new PugiXmlElement(nodeNext, m_pDocument));
}

UIElementProxy PugiXmlElement::FindChild(LPCTSTR szChildName)
{
    if (!szChildName)
        return UIElementProxy();

    pugi::xml_node nodeChild = m_node.child(szChildName);
    return UIElementProxy(new PugiXmlElement(nodeChild, m_pDocument));
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

	// 加载子结点中的属性
	pugi::xml_node nodeProp = m_node.child(XML_PROP);
	if (!nodeProp)
		return;
	/*
	bkgnd.render.type = "color" bkgnd.render.color = "0xFFF" 等效于下面的形式：

	<prop>
		<render key="bkgnd.render" type="color" color = "0xFFF"/>  // render tag名称未使用
	</prop>
	*/

	pugi::xml_node child = nodeProp.first_child();
	while (child)
	{
		String strKey;

		// 如果没有配置key，或者key为空，则不加前缀
		pugi::xml_attribute attr = child.attribute(XML_KEY);
		if (!attr.empty())
		{
			strKey = attr.as_string();
		}

		attr = child.first_attribute();
		for (; !attr.empty(); attr = attr.next_attribute())
		{
			if (0 == _tcscmp(attr.name(), XML_KEY))
				continue;

			if (strKey.empty())
			{
				(*ppMapAttrib)->AddAttr(attr.name(), attr.as_string());
			}
			else
			{
				String strName = strKey;
				strName.append(TEXT("."));
				strName.append(attr.name());
				(*ppMapAttrib)->AddAttr(strName.c_str(), attr.as_string());
			}
		}

		child = child.next_sibling();
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


PugiXmlDocument::PugiXmlDocument() : m_IUIDocument(this)
{
    m_bDirty = false;
	m_lRef = 0;
}
PugiXmlDocument::~PugiXmlDocument()
{
}

IUIDocument*  PugiXmlDocument::GetIUIDocument()
{
	return &m_IUIDocument;
}

void  PugiXmlDocument::SetSkinPath(LPCTSTR szPath)
{
	if (szPath)
		m_strPath = szPath;
	else
		m_strPath.clear();
}
LPCTSTR  PugiXmlDocument::GetSkinPath()
{
	return m_strPath.c_str();
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
UIElementProxy PugiXmlDocument::FindElem(LPCTSTR szText)
{
    if (!szText)
        return UIElementProxy();

    pugi::xml_node node = m_doc.child(szText);
    if (node.empty())
    {
        return UIElementProxy();
    }

    return UIElementProxy(new PugiXmlElement(node, this));
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