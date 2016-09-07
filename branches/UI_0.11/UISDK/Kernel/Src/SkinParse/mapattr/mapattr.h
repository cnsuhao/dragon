#ifndef MAPATTR_H_7AE2A63B_631C_4dcf_9378_A72CCC795023
#define MAPATTR_H_7AE2A63B_631C_4dcf_9378_A72CCC795023
#include "UISDK\Kernel\Inc\Interface\imapattr.h"

namespace UI
{

class CMapAttribute : public IMapAttribute
{
public:
    CMapAttribute();
    ~CMapAttribute();
    CMapAttribute(const CMapAttribute& o);
    
	virtual void  SetTag(LPCTSTR szKey);
	virtual LPCTSTR  GetTag();

    virtual bool  HasAttrib(LPCTSTR szKey);
    virtual LPCTSTR GetAttr(LPCTSTR szKey, bool bErase);
    virtual LPCTSTR GetAttr(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase);
    virtual long  GetAttr_bool(LPCTSTR szKey, bool bErase, bool* pbGet);
    virtual long  GetAttr_bool(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, bool* pbGet);
    virtual long  GetAttr_int(LPCTSTR szKey, bool bErase, int* pnGet);
    virtual long  GetAttr_int(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, int* pnGet);
    virtual long  GetAttr_intarray(LPCTSTR szKey, bool bErase, int* pIntArray, unsigned int nSize);
    virtual long  GetAttr_intarray(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, int* pIntArray, unsigned int nSize);
    virtual long  GetAttr_CRegion4(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, CRegion4* prcGet);
    virtual long  GetAttr_Image9Region(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, C9Region* pRegion);
    virtual long  GetAttr_RenderBase(
                LPCTSTR szPrefix, 
                LPCTSTR szKey,
                bool bErase,
                IUIApplication* pUIApp, 
                IObject* pBindObj, 
                IRenderBase** ppGet);
    virtual long  GetAttr_TextRenderBase(
                LPCTSTR szPrefix, 
                LPCTSTR szKey,
                bool bErase, 
                IUIApplication* pUIApp, 
                IObject* pBindObj, 
                ITextRenderBase** ppGet);
    virtual long  GetAttr_Color(
                LPCTSTR szPrefix,
                LPCTSTR szKey, 
                bool bErase, 
                IUIApplication* pUIApp,
                Color** ppColor);

    virtual bool  AddAttr(LPCTSTR szKey, LPCTSTR szValue);
	virtual bool  AddAttr_bool(LPCTSTR szKey, bool bValue);
	virtual bool  AddAttr_REGION4(LPCTSTR szKey, REGION4* pr);
	virtual bool  AddAttr_int(LPCTSTR szKey, int nValue);

    virtual bool  ExtractMapAttrByPrefix(LPCTSTR szPrefix, bool bErase, /*out*/IMapAttribute** ppMapAttribute);
    virtual long  Release();
    virtual long  AddRef();
    virtual void  CreateCopy(IMapAttribute** ppNewCopy);
    virtual void  CopyTo(IMapAttribute* pDestMapAttrib, bool bOverride);

    virtual void  BeginEnum();
    virtual bool  EnumNext(const TCHAR** szKey, const TCHAR** szValue);
    virtual void  EndEnum();

public:
    LPCTSTR GetAttr(const String& strKey, bool bErase);
    void  AddAttr(const String& strKey, const String& strValue);

protected:
	String   m_strTag;      // 标签名，用于充分模拟xml标签数据
    ATTRMAP  m_mapAttr;
    String   m_strTemp;     // 用于删除要查找的对象时，能保存一份要删除的字符串引用
    int      m_lRef;

    ATTRMAP::iterator  m_iterEnum;
};


class ListAttrItem 
{
public:
    ListAttrItem () { pPrev = pNext = NULL; }

    String  strKey;
    String  strValue;
    ListAttrItem*  pNext;
    ListAttrItem*  pPrev;
};
class CListAttribute : public  IListAttribute 
{
public:
    CListAttribute();
    ~CListAttribute();

	virtual void  SetTag(LPCTSTR szKey);
    virtual LPCTSTR  GetTag();

    virtual bool  AddAttr(LPCTSTR szKey, LPCTSTR szValue);
    virtual LPCTSTR  GetAttr(LPCTSTR szKey);
    virtual bool  EraseAttr(LPCTSTR szKey);

    virtual void  BeginEnum();
    virtual bool  EnumNext(const TCHAR** szKey, const TCHAR** szValue);
    virtual void  EndEnum();

    virtual long  Release();
    virtual long  AddRef();

private:
    ListAttrItem* FindItem(LPCTSTR szKey);

private:
	String   m_strTag; 
    ListAttrItem*  m_pFirstItem;
    ListAttrItem*  m_pLastItem;

    ListAttrItem*  m_pEnum;  //  正在被Enum的对象
    int  m_lRef;
};

}

#endif MAPATTR_H_7AE2A63B_631C_4dcf_9378_A72CCC795023