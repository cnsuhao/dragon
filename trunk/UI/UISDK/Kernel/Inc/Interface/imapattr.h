#ifndef IMAPATTR_H_78679F82_81BB_4847_A83B_2DC7DAD24870
#define IMAPATTR_H_78679F82_81BB_4847_A83B_2DC7DAD24870

namespace UI
{
#define  MAPATTR_RET_OK              0
#define  MAPATTR_RET_ERROR          -1  // 其它错误
#define  MAPATTR_RET_NOT_EXIST      -2  // 请求的key不存在
#define  MAPATTR_RET_INVLID_VALUE   -3  // value的值非法

interface IRenderBase;
interface ITextRenderBase;

interface IMapAttribute
{
	virtual void  SetTag(LPCTSTR szKey) = 0;
	virtual LPCTSTR  GetTag() = 0;

    virtual bool  HasAttrib(LPCTSTR szKey) = 0;
    virtual bool  ExtractMapAttrByPrefix(LPCTSTR szPrefix, bool bErase, /*out*/IMapAttribute** pMapAttribute) = 0;
    virtual long  Release() = 0;
    virtual long  AddRef() = 0;
    virtual void  CreateCopy(IMapAttribute** ppNewCopy) = 0;
    virtual void  CopyTo(IMapAttribute* pDestMapAttrib, bool bOverwrite) = 0;

    virtual LPCTSTR GetAttr(LPCTSTR szKey, bool bErase) = 0;
    virtual LPCTSTR GetAttr(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase) = 0;
    virtual long  GetAttr_bool(LPCTSTR szKey, bool bErase, bool* pbGet) = 0;
    virtual long  GetAttr_bool(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, bool* pbGet) = 0;
    virtual long  GetAttr_int(LPCTSTR szKey, bool bErase, int* pnGet) = 0;
    virtual long  GetAttr_int(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, int* pnGet) = 0;
    virtual long  GetAttr_intarray(LPCTSTR szKey, bool bErase, int* pIntArray, unsigned int nSize) = 0;
    virtual long  GetAttr_intarray(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, int* pIntArray, unsigned int nSize) = 0;
    virtual long  GetAttr_CRegion4(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, CRegion4* prcGet) = 0;
    virtual long  GetAttr_Image9Region(LPCTSTR szPrefix, LPCTSTR szKey, bool bErase, C9Region* pRegion) = 0;
    virtual long  GetAttr_RenderBase(
                LPCTSTR szPrefix, 
                LPCTSTR szKey, 
                bool bErase, 
                IUIApplication* pUIApp,
                IObject* pBindObj, 
                IRenderBase** ppGet) = 0;
    virtual long  GetAttr_TextRenderBase(
                LPCTSTR szPrefix,
                LPCTSTR szKey,
                bool bErase, 
                IUIApplication* pUIApp, 
                IObject* pBindObj, 
                ITextRenderBase** ppGet) = 0;
    virtual long  GetAttr_Color(
                LPCTSTR szPrefix,
                LPCTSTR szKey, 
                bool bErase, 
                IUIApplication* pUIApp,
                Color** ppColor) = 0;

	virtual bool  AddAttr(LPCTSTR szKey, LPCTSTR szValue) = 0;
	virtual bool  AddAttr_bool(LPCTSTR szKey, bool bValue) = 0;
	virtual bool  AddAttr_REGION4(LPCTSTR szKey, REGION4* pr) = 0;
	virtual bool  AddAttr_int(LPCTSTR szKey, int nValue) = 0;

    virtual void  BeginEnum() = 0;
    virtual bool  EnumNext(const TCHAR** szKey, const TCHAR** szValue) = 0;
    virtual void  EndEnum() = 0;
};

// 与IMapAttribute不同的是，IListAttribute不自动排序
interface IListAttribute 
{
	virtual void  SetTag(LPCTSTR szKey) = 0;
    virtual LPCTSTR  GetTag() = 0;

    virtual bool  AddAttr(LPCTSTR szKey, LPCTSTR szValue) = 0;
    virtual LPCTSTR  GetAttr(LPCTSTR szKey) = 0;
    virtual bool  EraseAttr(LPCTSTR szKey) = 0;

    virtual void  BeginEnum() = 0;
    virtual bool  EnumNext(const TCHAR** szKey, const TCHAR** szValue) = 0;
    virtual void  EndEnum() = 0;

    virtual long  Release() = 0;
    virtual long  AddRef() = 0;
};
UISDKAPI HRESULT  UICreateIMapAttribute(IMapAttribute** ppOut);
UISDKAPI HRESULT  UICreateIListAttribute(IListAttribute** ppOut);


}

#endif // IMAPATTR_H_78679F82_81BB_4847_A83B_2DC7DAD24870