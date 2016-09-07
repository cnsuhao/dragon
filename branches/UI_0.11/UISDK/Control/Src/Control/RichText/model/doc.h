#pragma once
#include "UISDK\Control\Inc\Interface\irichtext.h"
#include "Element\text_element.h"
#include "..\helper\mousekeymanager.h"
#include "style.h"

// richtext����ģ�ͣ�
// ���ݶ���  
//  Doc, Unit, Element
// ���ֶ���
//  Page, Line, Run
//
// Doc������һ��RichText��ά��page�б�
// Unit: �����ⲿ�����һ�����ݵ�Ԫ���糬���ӣ���Ϊһ��������unit����ͳһˢ�¡������// ά��Element�б�
// Element: ��\r\n��uniscribe��һ��Unit���ݲ��Ϊ���Element��Element����������������
// Page: ���ı���һҳ��ά��Line�б�
// Line: һ�С�ά��Run�б�
// Run: һ��Element�ڲ���ʱ��ֳ����ĵ�Ԫ�����һ��װ����һ��Element���������Ϊ���Run
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

    // ��Ч����
    enum InvalidState
    {
        InvalidStateEmpty,
        InvalidStateFull,
        InvalidStatePart,
    };
    InvalidState  m_eInvalidState;
    RECT  m_rcInvalidInDoc;  // ȫ0��ʾ��ˢ�£�ȫ-1��ʾȫ��ˢ�£�������ʾˢ��ˢ��

};

}
}