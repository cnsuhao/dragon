#pragma once

namespace UI
{
namespace RichTextObjectModel
{
class Line;
class Element;
class Doc;

class Page
{
public:
	Page(Doc* pDoc);
	~Page();

	Page*  GetNextPage();
	Page*  GetPrevPage();
	Line*  AppendLine();
	Line*  GetFirstLine();
	Line*  GetLastLine();
	Line*  LineHitTest(POINT ptInPage, __out POINT* ptInLine);
	void  PageRect2DocRect(RECT* prcPage, RECT* prcDoc);

	void  Draw(HDC hDC, POINT ptDoc);
    void  SetSize(int cx, int cy);
	void  SetVisible(bool b);
	bool  IsVisible();
	void  GetPageContentRegion(POINT ptDoc, RECT* prc);
private:
    

private:
	Doc*  m_pDoc;
	Page*  m_pNextPage;
	Page*  m_pPrevPage;
	Line*  m_pFirstLine;

    POINT  m_posInDoc;   // ҳ����doc�е�λ��
    SIZE  m_sizePage;  // ҳ���С(px)
	
	bool  m_bVisible;
};

}
}