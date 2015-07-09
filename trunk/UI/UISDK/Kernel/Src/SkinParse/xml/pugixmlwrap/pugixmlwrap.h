#pragma once
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"

#include "3rd/pugixml/pugixml.hpp"

namespace UI
{

interface IMapAttribute;
interface IUIElement;

class PugiXmlChildNodeEnum : public IUIChildNodeEnum
{
public:
    PugiXmlChildNodeEnum(pugi::xml_node& node, IUIDocument* pDoc);
    ~PugiXmlChildNodeEnum(){}

    static bool CreateInstance(pugi::xml_node& node, IUIDocument* pDoc, PugiXmlChildNodeEnum** pp);

public:
    virtual void  Release() override;
    virtual IUIElement*  NextElement() override;  // 注：需要Release

private:
    pugi::xml_node  m_node;
    IUIDocument*  m_pDocument;  // 没有引用计数
};

class PugiXmlElement : public IUIElement
{
public:
    PugiXmlElement(pugi::xml_node& node, IUIDocument* pDoc);
    ~PugiXmlElement();

    static  bool  CreateInstance(pugi::xml_node& node, IUIDocument* pDoc, PugiXmlElement** pp);
    static  bool  CreateInstance(pugi::xml_node& node, IUIDocument* pDoc, IUIElement** pp);

public:
    virtual long  AddRef() override;
    virtual long  Release() override;

    virtual void  GetTagName(BSTR* pbstr) override;
	virtual void  SetTagName(LPCTSTR) override;
    virtual bool  GetDocument(IUIDocument** pp) override; 

    virtual bool  GetData(BSTR* pbstr) override;
    virtual bool  SetData(LPCTSTR szText) override;

    virtual bool  AddChild(LPCTSTR szNodeName, IUIElement** pp) override;
    virtual bool  AddChild(IUIElement*  pElem) override;
    virtual bool  AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore) override;
    virtual bool  AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter) override;
	virtual bool  AddChildBefore(LPCTSTR szNodeName, IUIElement* pInsertBefore, IUIElement** pp) override;
	virtual bool  AddChildAfter(LPCTSTR szNodeName, IUIElement* pInsertAfter, IUIElement** pp) override;
    virtual bool  RemoveChild(IUIElement*) override;
    virtual bool  RemoveSelfInParent() override;
	virtual bool  FirstChild(IUIElement** ppElem) override;
	virtual bool  FindChild(LPCTSTR szChildName, IUIElement** ppElem) override;
    virtual IUIChildNodeEnum*  EnumChild() override;  // 注：需要Release

    virtual void  GetAttribList(IMapAttribute** ppMapAttrib) override;
    virtual void  GetAttribList2(IListAttribute** ppListAttrib) override;
    virtual bool  SetAttribList2(IListAttribute* pListAttrib) override;
    virtual bool  GetAttrib(LPCTSTR szKey, BSTR* pbstrValue) override;
    virtual bool  HasAttrib(LPCTSTR szKey) override;
    virtual bool  AddAttrib(LPCTSTR szKey, LPCTSTR szValue) override;
	virtual bool  SetAttrib(LPCTSTR szKey, LPCTSTR szValue) override;
    virtual bool  RemoveAttrib(LPCTSTR szKey) override;
    virtual bool  ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue) override;
    virtual bool  ClearAttrib() override;

    virtual bool  GetAttribInt(LPCTSTR szKey, int* pInt) override;
    virtual bool  AddAttribInt(LPCTSTR szKey, int nInt) override;

private:
    long  m_lRef;
    pugi::xml_node  m_node;
    IUIDocument*  m_pDocument;  // 没有引用计数
};

class PugiXmlDocument : public IUIDocument
{
public:
    PugiXmlDocument();
    ~PugiXmlDocument();

    static bool  CreateInstance(PugiXmlDocument** pp);
    static bool  CreateInstance(IUIDocument** pp);

    virtual long  AddRef() override;
    virtual long  Release() override;

    virtual bool  LoadFile(LPCTSTR szFile) override;
    virtual bool  LoadData(const byte*  pData, int nDataSize) override;
    virtual bool  FindElem(LPCTSTR szText, IUIElement** ppElem) override;
    virtual bool  Save() override;
    virtual bool  SaveAs(LPCTSTR szPath) override;
    virtual LPCTSTR  GetPath() override;

    virtual void  SetDirty(bool b) override;
    virtual bool  IsDirty() override;

private:
    String  m_strPath;  // 全路径

    long    m_lRef;
    bool    m_bDirty;

    pugi::xml_document m_doc;
};

}