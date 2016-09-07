#ifndef _UI_ISKINRES_H_
#define _UI_ISKINRES_H_

namespace UI
{
	interface ISkinManager;
	interface IImageManager;
	interface IImageManager;
	interface IColorManager;
	interface IFontManager;
	interface IStyleManager;
	interface ILayoutManager;
	interface IImageRes;
	interface IFontRes;
	interface IColorRes;
	interface IStyleRes;
	interface ISkinDataSource;
	interface IUIDocument;
	class SkinRes;

	interface UISDKAPI ISkinRes
	{
		ISkinRes(SkinRes*);
		SkinRes*  GetImpl();
		void  SetParentSkinRes(ISkinRes*);

		IUIApplication*  GetUIApplication();
		ISkinManager*    GetSkinManager();
		IImageManager*   GetImageManager();
		IColorManager*   GetColorManager();
		IFontManager*    GetFontManager();
		IStyleManager*   GetStyleManager();
		ILayoutManager*  GetLayoutManager();
		IImageRes*       GetImageRes();
		IFontRes*        GetFontRes();
		IColorRes*       GetColorRes();
		IStyleRes*       GetStyleRes();

		ISkinDataSource*  GetDataSource();

		UINT  GetXmlDocCount();
		IUIDocument*  GetXmlDoc(UINT nIndex);
		IUIDocument*  GetXmlDocByName(LPCTSTR szName);

		LPCTSTR GetName();
		LPCTSTR GetPath();

		bool  Load();
		bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

	private:
		SkinRes*  m_pImpl;
	};


	interface ISkinDataSource;
	class SkinManager;
	interface UISDKAPI ISkinManager : public IRootInterface
	{
		ISkinManager(SkinManager*);
		SkinManager*  GetImpl();

		IUIApplication*  GetUIApplication();
		void  ChangeSkinHLS(short h, short l, short s, int nFlag);
		bool  Save(ISkinRes* pSkinRes);

		unsigned int  GetSkinResCount();
		ISkinRes*  GetSkinResByIndex(unsigned int i);
		ISkinRes*  GetSkinResByName(LPCTSTR szName);

	private:
		SkinManager*  m_pImpl;
	};
}
#endif