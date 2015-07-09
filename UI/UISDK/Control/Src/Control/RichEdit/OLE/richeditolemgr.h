#pragma once
#include <Richedit.h>

//////////////////////////////////////////////////////////////////////////
//
//                      ����richedit�в����ole����
//
//////////////////////////////////////////////////////////////////////////


//  Q1. ����InsertObjectʱ��һ��Ҫ����һ��Storageָ����
//  A1. �������һ��NULL�����ڸ��Ƹö���󣬽��޷�ճ�����ڲ���ԭ��Ҳ��û�и����
//      ILockBytes������һ��Storage��洢����֮���������StgCreateDocfileOnILockBytes
//      �������ڴ��д���һ���洢������Ӧ��StgCreateDocFile��ʾʹ���ļ�����һ��
//      �洢����
//
//   Q2. IDataObject��ô���������̷��ʵģ�
//   A2. ��Ŀǰ�鵽����������ʾ��Ӧ����Marshal�Ľ��
//
//   Q3. �����˳���Ϊʲô���ܽ��п���ճ��?
//   A3. ��ʵ��ȫ��OleFlushClipboard�����Ὣ��ǰ�������е�IDataObject���и��ƣ�����һ��
//       ��ʱ��IDataObject�����ͷ�֮ǰ��IDataObject��������ڳ����˳�ǰӦ�õ���һ��
//       �������
//
//   Q4. ����ж�һ��OLE�Ƿ�ɼ����Ƿ���Ҫ���ƻ��ƣ�
//   A4. ���취����ȡoleλ�ã���ȡre�Ŀ��ӷ�Χλ�ã��ж�ole�Ƿ��������Χ��
//       ���ɣ�ÿ��re����ǰ����ole����Ϊ���ɼ������ole��Ӧ��iviewobject::draw���������
//       ole�ǿɼ���
//
//
//
// �����ʵ��ϸ�ڿ��Բο�atl��Դ���룺CComControlBase  IOleObjectImpl
//
//
interface IRichEditOle;
namespace UI
{
class WindowlessRichEdit;
class REOleManager;
interface IREOleExternalData;


class REOleBase : public IUnknown
{
public:
	REOleBase();
	virtual ~REOleBase();

    // ��ȡOLE�����IOleObject�ӿ�
	virtual HRESULT  GetOleObject(IOleObject** ppOleObject, bool bAddRef=true) = 0;	

    // ��ȡole����ļ���������
	virtual HRESULT  GetClipboardData(UINT nClipFormat, __out BSTR* pbstrData) = 0;
    // ��ѡ��һ��OLEʱ����ȡ���ḻ�ļ���������
    virtual HRESULT  GetClipboardDataEx(IDataObject* pDataObject) = 0;

    virtual long  GetOleType() = 0;                  // ��ȡ�Լ�������
	virtual void  OnDraw(HDC hDC, RECT* prc){};      // ����ole
	virtual void  GetSize(SIZE* pSize){};            // ��ȡole��С
    virtual void  OnSizeChanged(int cx, int cy) {};  // ole���ƴ�С�����ı�

    // ������RE
    virtual void  OnPreInsert2RE(){};
    virtual void  OnInsert2RE(){};

    // ole�Ƿ���Ҫ���/������Ϣ
    virtual bool  NeedWindowMessage();
    virtual LRESULT  ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	// ����ӿ�
    void  SetExternalData(IREOleExternalData* p);
    IREOleExternalData*  GetExternalData();

public:
    WindowlessRichEdit*  GetWindowlessRE();
	void  SetOleObjectManager(REOleManager* pMgr);

    long  AddRefImpl();
    long  ReleaseImpl();
	bool  IsVisibleInField();
    void  ClearVisibleInFieldFlag();
    void  GetDrawRect(RECT* prc);
	void  OnInternalDraw(HDC, RECT*);

protected:
    long   m_dwRef;
	REOleManager*  m_pOleObjectMgr;
	CRect  m_rcDrawSave;       // ������һ�εĻ������򣬿ؼ����Ͻ�Ϊ(0,0)
	bool   m_bVisibleInField;  // �Ƿ��ڿ��ӷ�Χ�ڣ��Ƿ���Ҫ���ƣ�
	
    IREOleExternalData*  m_pExternalData;
};

typedef list<REOleBase*>   OLELIST;
typedef OLELIST::iterator  OLELISTITER;
typedef OLELIST::reverse_iterator  OLELISTRITER;
typedef set<IDataObject*>  DATAOBJECTSET;

// ����richedit�е� ole�����Ӧ�Ľṹ���б�
class REOleManager : public IDataObjectSource
{
public:
	REOleManager(WindowlessRichEdit* pRichEdit);
	~REOleManager();

	void   SetUIApplication(IUIApplication* p);
	bool   AddOle(REOleBase* pItem);
    void   OnOleDelete(REOleBase* pItem);
//  REOleBase*  FindOle(long lType, LPCTSTR  szId);

	void   CreateDataObject(IDataObject** p);
    WindowlessRichEdit*  GetRE();
    IRichEditOle*  GetREOle();
    REOleBase*  HitTest(POINT ptInControl);
	void  OnPreDraw(HDC hDC, RECT* prcDraw);

    LRESULT  OnWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    LRESULT  OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnMouseLeave(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled); 
    LRESULT  OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled); 
    void  WindowPoint2ObjectClientPoint(POINT* ptWnd, POINT* ptClient);

protected:
	virtual BOOL OnRenderData(
		IDataObject* pDataObject,
		LPFORMATETC lpFormatEtc,
		LPSTGMEDIUM lpStgMedium 
		) override;

	virtual void  OnDataRelease(
		IDataObject* p
		) override;

protected:
	WindowlessRichEdit*   m_pRichEdit;
    IUIApplication*       m_pUIApp;
    bool                  m_bDestroying;     // ����Լ���������

	OLELIST               m_listOleObj;
	DATAOBJECTSET         m_setDataObject;   // ���ڼ�¼���Ƶ�ole dataָ�롣��RE����ʱ�ͷţ����ύ�����а�.

    REOleBase*   m_pHoverOle;
    REOleBase*   m_pPressOle;
};


}