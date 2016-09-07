#pragma once
#include <usp10.h>

namespace UI
{
namespace RichTextObjectModel
{
class Element;
class Line;

class Run
{
public:
	Run(Element*, int w);
	~Run();

    Element*  GetElement();
	void  Draw(HDC hDC, RECT* prcLine);
	void  SetRange(uint nStart, uint nEnd);
	void  SetStart(uint nStart);
	void  SetEnd(uint nEnd);
    void  SetPosInLine(int x);
	uint  GetWidth();
	int  GetXInLine();

	Run*  GetNextRun();
	Run*  GetPrevRun();
	void  SetNextRun(Run* p);
	void  SetPrevRun(Run* p);

	void  OnAdd2Line(Line* p);
	Line*  GetRelativeLine();

private:
	Element*  m_pElement;
	Line*  m_pLine;

	Run*  m_pNextRun;
	Run*  m_pPrevRun;

	uint  m_nStartIndexInElement;
	uint  m_nEndIndexInElement;

    int  m_xInLine;
    uint  m_w;
};

}
}