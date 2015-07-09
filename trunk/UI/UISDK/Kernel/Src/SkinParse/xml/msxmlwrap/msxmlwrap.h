#pragma once
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"

// 2015.4.18日废弃，改为使用pugixml解析器
namespace UI
{

class MsXmlElement;
class MsXmlChildNodeEnum : public IUIChildNodeEnum
{
protected:
    MsXmlChildNodeEnum(IXMLDOMNodeList*, IUIDocument*);
public:
    static bool CreateInstance(IXMLDOMNodeList* pNode, IUIDocument* pDoc, MsXmlChildNodeEnum** pp);
    void  Release();
    IUIElement*  NextElement();

public:
    CComPtr<IXMLDOMNodeList>  m_pNodeList; 
    IUIDocument*  m_pDocument;   // 元素所在文档指针。仅引用指针，未保持计数
};

class MsXmlElement : public IUIElement
{
protected:
    MsXmlElement(IXMLDOMElement* pNode, IUIDocument* pDoc);
public:
    static void  CreateInstance(IXMLDOMNode* pNode, IUIDocument*, MsXmlElement** pp);
    static void  CreateInstance(IXMLDOMNode* pNode, IUIDocument*, IUIElement** pp);
    long  Release();
    long  AddRef();

    bool  GetDocument(IUIDocument** pp);
    void  GetTagName(BSTR* pbstr);
	void  SetTagName(LPCTSTR);
    void  GetAttribList(IMapAttribute** ppMapAttrib);
    void  GetAttribList2(IListAttribute** ppListAttrib);
    bool  SetAttribList2(IListAttribute* pListAttrib);

    bool  GetData(BSTR* pbstr);
    bool  SetData(LPCTSTR szText);

    bool  HasAttrib(LPCTSTR szKey);
    bool  GetAttrib(LPCTSTR szKey, BSTR* pbstrValue);
    bool  AddAttrib(LPCTSTR szKey, LPCTSTR szValue);
	bool  SetAttrib(LPCTSTR szKey, LPCTSTR szValue);
    bool  GetAttribInt(LPCTSTR szKey, int* pInt);
    bool  AddAttribInt(LPCTSTR szKey, int nInt);
    bool  ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue);
    bool  RemoveAttrib(LPCTSTR szKey);
    bool  ClearAttrib();
    
    bool  FirstChild(IUIElement** ppElem);
    bool  FindChild(LPCTSTR szChildName, IUIElement** ppElem);
    bool  NextSiblingElem(IUIElement** ppElem);
    IUIChildNodeEnum*  EnumChild();
    bool  RemoveChild(IUIElement*);
	bool  RemoveSelfInParent();
    bool  AddChild(LPCTSTR szNodeName, IUIElement** pp);
	bool  AddChild(IUIElement*  pElem);
    bool  AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore);
    bool  AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter);
	bool  AddChildBefore(LPCTSTR szNodeName, IUIElement* pInsertBefore, IUIElement** pp);
	bool  AddChildAfter(LPCTSTR szNodeName, IUIElement* pInsertAfter, IUIElement** pp);

private:
	long  GetDepth();
	void  AddNewLine(int nTabs);

public:
    long  m_lRef;
    CComPtr<IXMLDOMElement>  m_pElement; 

    IUIDocument*  m_pDocument;   // 元素所在文档指针。仅引用指针，未保持计数
};

class MsXmlDocument : public IUIDocument
{
public:
    static bool  CreateInstance(MsXmlDocument** pp);
    static bool  CreateInstance(IUIDocument** pp);
    long  Release();
    long  AddRef();

    MsXmlDocument();
    bool  LoadFile(LPCTSTR szFile);
    bool  LoadData(const byte*  pData, int nDataSize);

	bool  FindElem(LPCTSTR szText, IUIElement** ppElem);
    bool  SaveAs(LPCTSTR szPath);
    bool  Save();
    LPCTSTR  GetPath();
    void  SetDirty(bool b);
    bool  IsDirty();

public:
    String  m_strPath;  // 全路径

    long    m_lRef;
    bool    m_bDirty;

    CComPtr<IXMLDOMDocument>  m_pDoc;
};

}