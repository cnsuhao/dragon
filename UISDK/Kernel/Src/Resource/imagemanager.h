#pragma once
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Src\Util\DataStruct\list.h"
#include "imageres.h"
#include "cursorres.h"
#include "gifres.h"

namespace UI
{
class  ImageTagElementInfo
{
public:
	String  strId;  // ģ����
	IUIElement*  pXmlElement;  // xml���
};

class ImageManager
{
public:
	ImageManager(SkinRes* pSkinRes, IUIApplication* pUIApp);
	~ImageManager(void);

    IImageManager*  GetIImageManager();

	ImageRes&    GetImageRes();
	CursorRes&   GetCursorRes();
	GifRes&      GetGifRes();
    IUIElement*  GetImageXmlElem(LPCTSTR szId);

	IImageResItem*  InsertImageItem(IMAGE_ITEM_TYPE eType, LPCTSTR szID, LPCTSTR szPath);
	bool  ModifyImageItem(LPCTSTR szID, LPCTSTR szPath);
	bool  RemoveImageItem(LPCTSTR szID);

	bool  ModifyImageItemInRunTime(LPCTSTR szID, LPCTSTR szPath);
	bool  ModifyImageItemAlpha(LPCTSTR szID, byte nAlphaPercent);

	void  Clear();
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

//--	bool       GetCursor(const String& strID, UICursor** ppCursor);
//--	IRenderBitmap*   GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
//--	GifImageBase* GetGifImage( const String& strID);

	int  GetImageCount( );
	IImageResItem*  GetImageItemInfo(int nIndex);

	static HRESULT  UIParseImageTagCallback(IUIElement*, ISkinRes* pSkinRes);

private:
    // ISkinTagParse
    HRESULT  ParseNewElement(IUIElement* pElem);
    void  OnNewChild(IUIElement* pElem);

private:
	// ���ݳ־ò�
    IImageManager*  m_pIImageManager;

	typedef  UIList<ImageTagElementInfo>  _MyList;
	typedef  UIListItem<ImageTagElementInfo>  _MyListItem;
	_MyList  m_listUIElement;

	// ��������
	ImageRes        m_resImage;    // ����ͼƬ
	CursorRes       m_resCursor;   // �����ʽͼƬ
	GifRes          m_resGif;      // ����

	SkinRes*        m_pSkinRes;
};

} 