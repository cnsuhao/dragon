#ifndef _IUIRES_H_
#define _IUIRES_H_

namespace UI
{
    class ColorManager;
    class Color;
    interface IColorRes;
	interface IUIDocument;
	interface IRenderBitmap;
	interface IRenderFont;

    interface UISDKAPI IColorManager : public IRootInterface
    {
        IColorManager(ColorManager* p);
        ColorManager*  GetImpl();

        IColorRes*  GetColorRes();

    private:
        ColorManager*  m_pImpl;
    };

	class ColorRes;
	interface IColorResItem;
	interface UISDKAPI IColorRes : public IRootInterface
	{
		IColorRes(ColorRes* p);
		ColorRes*  GetImpl();

		long  GetColorCount();
		IColorResItem*  GetColorResItem(long lIndex);
		void  GetColor(LPCTSTR szColorId, Color** pp);
		LPCTSTR  GetColorId(Color* p);

	private:
		ColorRes*  m_pImpl;
	};

	class  ColorResItem;
	interface UISDKAPI IColorResItem : public IRootInterface
	{
		IColorResItem(ColorResItem* p);
		ColorResItem*  GetImpl();

		LPCTSTR GetID();
		LPCTSTR GetColorString();
	private:
		ColorResItem*  m_pImpl;
	};

    class ImageResItem;
    interface UISDKAPI IImageResItem : public IRootInterface
    {
        IImageResItem(ImageResItem* p);
        ImageResItem*  GetImpl(); 

        void  SetId(const TCHAR*);
        void  SetPath(const TCHAR*);
        LPCTSTR GetId();
        LPCTSTR GetPath();
        bool  GetUseSkinHLS();
        bool  GetNeedAntiAliasing();
        void  SetUseSkinHLS(bool b);
        void  SetNeedAntiAliasing(bool b);
        bool  ModifyImage(LPCTSTR szPath);
        IMAGE_ITEM_TYPE  GetImageItemType();
        //        void  SetImageItemType(IMAGE_ITEM_TYPE e);

    private:
        ImageResItem*  m_pImpl;
    };
    class ImageListResItem;
    interface UISDKAPI IImageListResItem : public IImageResItem
    {
        IImageListResItem(ImageListResItem* p);
        ImageListResItem*  GetImpl(); 

        IMAGELIST_LAYOUT_TYPE  GetLayoutType();
        int  GetItemCount();
        void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);
        void  SetItemCount(int);

    private:
        ImageListResItem*  m_pImpl;
    };

    class ImageIconResItem;
    interface UISDKAPI IImageIconResItem : public IImageResItem
    {
        IImageIconResItem(ImageIconResItem* p);
        ImageIconResItem*  GetImpl(); 

    private:
        ImageIconResItem*  m_pImpl;
    };

    class ImageRes;
    interface UISDKAPI IImageRes : public IRootInterface
    {
        IImageRes(ImageRes* p);
        ImageRes*  GetImpl();

        long  GetImageCount();
        IImageResItem*  GetImageResItem(long lIndex);
        IImageResItem*  GetImageResItem(LPCTSTR szId);
        bool  ModifyImage(LPCTSTR szId, LPCTSTR szPath);
        bool  GetBitmap(LPCTSTR szImageId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderBitmap** ppRenderBitmap);
        HBITMAP  LoadBitmap(LPCTSTR szId);

        LPCTSTR  GetRenderBitmapId(IRenderBitmap* pBitmap);

    private:
        ImageRes*  m_pImpl;
    };


    class CursorRes;
    interface IUICursor;
    interface UISDKAPI ICursorRes : public IRootInterface
    {
        ICursorRes(CursorRes* p);
        CursorRes*  GetImpl(); 

        void  GetCursor(LPCTSTR szCursorID, IUICursor** pp);
        LPCTSTR  GetCursorId(IUICursor* p);

    private:
        CursorRes*  m_pImpl;
    };

    class CursorResItem;
    interface UISDKAPI ICursorResItem : public IRootInterface
    {
        ICursorResItem(CursorResItem* p);
        CursorResItem*  GetImpl(); 

    private:
        CursorResItem*  m_pImpl;
    };

   
    class FontResItem;
    interface UISDKAPI IFontResItem : public IRootInterface
    {
        IFontResItem(FontResItem*);
        FontResItem*  GetImpl();

        LPCTSTR  GetId();
        LOGFONT*  GetLogFont();
    private:
        FontResItem*  m_pImpl;
    };

    class GifRes;
    interface IGifImage;
    interface UISDKAPI IGifRes : public IRootInterface
    {
        IGifRes(GifRes* p);
        GifRes*  GetImpl();

        HRESULT  GetGifImage(LPCTSTR szId, IGifImage** ppGifImage);
    private:
        GifRes*  m_pImpl;
    };

    class GifResItem;
    interface UISDKAPI IGifResItem : public IRootInterface
    {
        IGifResItem(GifResItem* p);
        GifResItem*  GetImpl();

    private:
        GifResItem*  m_pImpl;
    };

    class FontRes;
    interface UISDKAPI IFontRes : public IRootInterface
    {
        IFontRes(FontRes* p);
        FontRes*  GetImpl();

        long  GetFontCount();
        bool  GetFontResItem(long lIndex, IFontResItem** ppResItem);
        bool  GetFont(LPCTSTR szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut);
        bool  GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont);
        LPCTSTR  GetDefaultFontId();
        LPCTSTR  GetRenderFontId(IRenderFont* pFont);
    private:
        FontRes*  m_pImpl;
    };

    class StyleResItem;
    interface UISDKAPI IStyleResItem : public IRootInterface
    {
        IStyleResItem(StyleResItem* p);
        StyleResItem*  GetImpl();

        STYLE_SELECTOR_TYPE  GetSelectorType();
        LPCTSTR  GetId();
        LPCTSTR  GetInherits();
		LPCTSTR  GetAttribute(LPCTSTR szKey);

	
		// 不提供。editor需要的话使用styleres::moidfy才会触发on moidify
// 		void  SetSelectorType(STYLE_SELECTOR_TYPE type);
// 		void  SetId(LPCTSTR szId);
// 		void  SetInherits(LPCTSTR);
// 		void  SetAttribute(LPCTSTR key, LPCTSTR value);
// 		void  RemoveAttribute(LPCTSTR key);

        void  GetAttributeMap(IMapAttribute** ppMapAttrib);

        void  SetXmlElement(IUIElement* p);
        IUIElement*  GetXmlElement();
        bool  GetXmlElement2(IUIElement** pp);

    private:
        StyleResItem*  m_pImpl;
    };

    class StyleRes;
    interface UISDKAPI IStyleRes : public IRootInterface
    {
        IStyleRes(StyleRes* p);
        StyleRes*  GetImpl();

        IStyleResItem*  Add(STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit);
		IStyleResItem*  Insert(STYLE_SELECTOR_TYPE type, LPCTSTR szId, long lPos);
		bool  Modify(IStyleResItem* pItem, STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit);
        bool  Remove(STYLE_SELECTOR_TYPE type, LPCTSTR szId);
		bool  Remove(IStyleResItem* pItem);

		bool  AddAttribute(IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue);
		bool  ModifyAttribute(IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue);
		bool  RemoveAttribute(IStyleResItem* pItem, LPCTSTR szKey);

        long  GetStyleCount();
        IStyleResItem*  GetItem(long lIndex);
		long  GetItemPos(IStyleResItem* p);

        IStyleResItem*  FindItem(STYLE_SELECTOR_TYPE type, LPCTSTR szId);

    private:
        StyleRes*  m_pImpl;
    };

    class ImageManager;
    interface UISDKAPI IImageManager : public IRootInterface	
    {
        IImageManager(ImageManager* p);
        ImageManager*  GetImpl();

        IImageRes*   GetImageRes();
        ICursorRes*  GetCursorRes();
        IGifRes*     GetGifRes();
        IUIElement*  GetImageXmlElem(LPCTSTR szId);

        IImageResItem*  InsertImageItem(IMAGE_ITEM_TYPE eType, LPCTSTR szID, LPCTSTR szPath);
        bool  ModifyImageItem(LPCTSTR szID, LPCTSTR szPath);
        bool  RemoveImageItem(LPCTSTR szID);

        bool  ModifyImageItemInRunTime(LPCTSTR szID, LPCTSTR szPath);
        bool  ModifyImageItemAlpha(LPCTSTR szID, byte nAlphaPercent);
    private:
        ImageManager*  m_pImpl;
    };

    class FontManager;
    interface UISDKAPI IFontManager : public IRootInterface 
    {
        IFontManager(FontManager* p);
        FontManager*  GetImpl();

        IFontRes*  GetFontRes();
    private:
        FontManager*  m_pImpl;
    };

    class StyleManager;
    interface UISDKAPI IStyleManager : public IRootInterface
    {
        IStyleManager(StyleManager*);
        StyleManager*  GetImpl();

        IStyleRes*  GetStyleRes();
        IUIElement*  GetStyleXmlElem(LPCTSTR szId);

        bool  ParseStyle(IMapAttribute* pMapAttrib);  // 内部间接调用LoadStyle
        bool  ReverseParseStyle(IListAttribute* pListAttrib);
        bool  LoadStyle(LPCTSTR szTagName, LPCTSTR szStyleClass, LPCTSTR szID, IMapAttribute* pMapStyle);

    private:
        StyleManager*  m_pImpl;
    };

    class LayoutManager;
    interface UISDKAPI ILayoutManager : public IRootInterface
    {
        ILayoutManager(LayoutManager*);
        LayoutManager*  GetImpl();

        IObject*  LoadLayout(LPCTSTR szWndName, LPCTSTR szWndId);
        IObject*  ParseElement(IUIElement* pUIElement, IObject* pParent);

        bool  FindWindowElement(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem);
        // bool  LoadWindowNodeList(ILayoutWindowNodeList**);

    private:
        LayoutManager*  m_pImpl;
    };
 
	// 国际化 
	interface II18nCallback
	{
		virtual LPCTSTR I18nMap(LPCTSTR key) = 0;
	};
}

#endif // _IUIRES_H_