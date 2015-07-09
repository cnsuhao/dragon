#ifndef IXMLWRAP_H_201504161620
#define IXMLWRAP_H_201504161620

namespace UI
{
interface IMapAttribute;
interface IUIElement;

interface IUIChildNodeEnum
{
    virtual void  Release() = 0;
    virtual IUIElement*  NextElement() = 0;  // 注：需要Release
};

interface IUIElement
{
    virtual long  AddRef() = 0;
    virtual long  Release() = 0;

    virtual void  GetTagName(BSTR* pbstr) = 0;
	virtual void  SetTagName(LPCTSTR) = 0;
    virtual bool  GetDocument(IUIDocument** pp) = 0; 

    virtual bool  GetData(BSTR* pbstr) = 0;
    virtual bool  SetData(LPCTSTR szText) = 0;

    virtual bool  AddChild(LPCTSTR szNodeName, IUIElement** pp) = 0;
	virtual bool  AddChild(IUIElement*  pElem) = 0;
    virtual bool  AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore) = 0;
    virtual bool  AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter) = 0;
	virtual bool  AddChildBefore(LPCTSTR szNodeName, IUIElement* pInsertBefore, IUIElement** pp) = 0;
	virtual bool  AddChildAfter(LPCTSTR szNodeName, IUIElement* pInsertAfter, IUIElement** pp) = 0;
    virtual bool  RemoveChild(IUIElement*) = 0;
	virtual bool  RemoveSelfInParent() = 0;
	virtual bool  FirstChild(IUIElement** ppElem) = 0;
	virtual bool  FindChild(LPCTSTR szChildName, IUIElement** ppElem) = 0;
    virtual IUIChildNodeEnum*  EnumChild() = 0;  // 注：需要Release

    virtual void  GetAttribList(IMapAttribute** ppMapAttrib) = 0;
    virtual void  GetAttribList2(IListAttribute** ppListAttrib) = 0;
    virtual bool  SetAttribList2(IListAttribute* pListAttrib) = 0;
    virtual bool  GetAttrib(LPCTSTR szKey, BSTR* pbstrValue) = 0;
    virtual bool  HasAttrib(LPCTSTR szKey) = 0;
    virtual bool  AddAttrib(LPCTSTR szKey, LPCTSTR szValue) = 0;
	virtual bool  SetAttrib(LPCTSTR szKey, LPCTSTR szValue) = 0;
    virtual bool  RemoveAttrib(LPCTSTR szKey) = 0;
    virtual bool  ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue) = 0;
    virtual bool  ClearAttrib() = 0;

    virtual bool  GetAttribInt(LPCTSTR szKey, int* pInt) = 0;
    virtual bool  AddAttribInt(LPCTSTR szKey, int nInt) = 0;
};

interface IUIDocument
{
    virtual long  AddRef() = 0;
    virtual long  Release() = 0;

    virtual bool  LoadFile(LPCTSTR szFile) = 0;
    virtual bool  LoadData(const byte*  pData, int nDataSize) = 0;
    virtual bool  FindElem(LPCTSTR szText, IUIElement** ppElem) = 0;
    virtual bool  Save() = 0;
    virtual bool  SaveAs(LPCTSTR szPath) = 0;
    virtual LPCTSTR  GetPath() = 0;

    virtual void  SetDirty(bool b) = 0;
    virtual bool  IsDirty() = 0;
};

enum XML_ENGINE
{
    XML_ENGINE_DEFAULT,
    MSXML,
    PUGIXML,
};
bool  CreateXmlDocument(XML_ENGINE e, IUIDocument**);

}

#endif // IXMLWRAP_H_201504161620