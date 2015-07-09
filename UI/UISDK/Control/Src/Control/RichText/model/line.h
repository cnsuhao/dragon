#pragma once
#include "run.h"

namespace UI
{
namespace RichTextObjectModel
{
class Page;

class Line 
{
public:
	Line(Page* pPage, int nDefaultHeight);
	~Line();

	void  Draw(HDC hDC, RECT* prcPage);
	Run*  AppendRun(Element* pElement, uint nRunWidth, uint nRunHeight);
	Run*  GetFirstRun();
	Run*  GetLastRun();
	bool  HasRun();
	Line*  GetNextLine();
	Line*  GetPrevLine();
	void  SetNextLine(Line*);
	void  SetPrevLine(Line*);


	Run*  RunHitTest(POINT ptInLine, __out POINT* ptInRun);
	void  GetRectInPage(RECT* prc);
	void  GetRectInDoc(RECT* prc);

    void  SetPosition(int xInPage, int yInPage);
    void  SetHeight(uint nHeight);
    uint  GetHeight();
	
	void  SetFirstParaLine(bool b);

private:
	Page*  m_pPage;
	Line*  m_pNextLine;
	Line*  m_pPrevLine;
	Run*  m_pFirstRun;

    int  m_xInPage;
    int  m_yInPage;
    uint  m_nHeight;   // ������������Ҫ�ĸ߶ȡ��������м��
	uint  m_nRunsWidth;  // ��������runs���ܿ��

	bool  m_bFirstParaLine; // ������
};

}
}