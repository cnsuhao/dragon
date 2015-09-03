#pragma once
#include "UISDK\Control\Inc\Interface\irichtext.h"
#include "Element\text_element.h"
#include "..\helper\mousekeymanager.h"
#include "style.h"

// richtext对象模型：
// 内容对象  
//  Doc, Unit, Element
// 布局对象
//  Page, Line, Run
//
// Doc：代表一个RichText，维护page列表
// Unit: 代表外部插入的一个内容单元，如超链接，作为一个完整的unit对象统一刷新、点击。// 维护Element列表。
// Element: 被\r\n、uniscribe将一个Unit内容拆分为多个Element。Element将被遍历用来布局
// Page: 富文本的一页。维护Line列表
// Line: 一行。维护Run列表
// Run: 一个Element在布局时拆分出来的单元。如果一行装不下一个Element，将被拆分为多个Run
//
namespace UI
{
namespace RichTextObjectModel
{
class Page;
class Element;
class TextElement;
class ImageUnit;
class Line;
class Doc;

enum PageMode
{
	SingleLine,
	MultiLine,
	MultiPage,

	PageModeError,
};

class Doc
{
public:
	Doc();
	~Doc();

	void  Init(IRichText* pObj);
	void  Release();
	RichTextMKMgr*  GetMKMgr();
    PageMode  GetPageMode();
	IMessage*  GetIMessageObj();
    const Style&  GetStyle();

	void  Render2DC(HDC hDC, POINT pt);
    void  LayoutAll(uint nControlWidth, uint nControlHeight);

	void  AppendText(LPCTSTR szText);
	void  AppendText2(LPCTSTR szText, RichTextCharFormat* pcf);
	LinkUnit*  AppendLink(RichTextLinkInfo* pInfo);
	LinkUnit*  AppendLink2(RichTextLinkInfo* pInfo, RichTextCharFormat* pcf);
	bool  AppendLink3(LinkUnit* pUnit, LPCTSTR  szText, RichTextCharFormat* pcf);
	ImageUnit*  AppendImage(LPCTSTR szPath);
    void  CopyDefaultCharFormat(RichTextCharFormat* pcf);
	void  CopyDefaultLinkCharFormat(RichTextCharFormat* pcf);
    const RichTextParaFormat*  GetDefaultParaFormat();
	const RichTextCharFormat*  GetDefaultCharFormat();
	const RichTextCharFormat*  GetDefaultLinkCharFormat();

	Page*  GetFirstPage();
	Page*  PageHitTest(POINT ptDoc, __out POINT* ptInPage);
    void  InvalidRegion(RECT* prcInDoc);
    void  ClearInvlidRegion();
    void  Refresh();

	void  SetDefaultFont(RichTextCharFormat* pcf);
	void  SetDefaultLinkFont(RichTextCharFormat* pcf);
	void  CreateDefaultFont();
	void  CreateDefaultLinkFont();

private:
	Page*  CreatePage();

	void  Release_Page();
	void  Release_Unit();

	void  PreLayoutAll();
	void  LayoutAll_SingleLine(uint nControlWidth, uint nControlHeight);
	void  LayoutAll_MultiLine(uint nControlWidth, uint nControlHeight);
	void  LayoutAll_MultiPage(uint nControlWidth, uint nControlHeight);

	bool  layout_all_multiline_imageelement(MultiLineLayoutContext* pContext);
    void  append_text(LPCTSTR szText, TextUnit* pUnit, RichTextCharFormat* pcf);

private:

private:
    IRichText*  m_pIRichText;
	RichTextCharFormat  m_defaultcf;  
	RichTextCharFormat  m_defaultcf_link;  
	RichTextParaFormat  m_defaultpf;
    Style  m_richtextStyle; 

	Page*  m_pFirstPage;
	PageMode  m_ePageMode;

    list<Unit*>  m_listUnits;
	RichTextMKMgr  m_mkmgr;

    // 无效区域
    enum InvalidState
    {
        InvalidStateEmpty,
        InvalidStateFull,
        InvalidStatePart,
    };
    InvalidState  m_eInvalidState;
    RECT  m_rcInvalidInDoc;  // 全0表示无刷新，全-1表示全部刷新，其它表示刷新刷新

};

}
}