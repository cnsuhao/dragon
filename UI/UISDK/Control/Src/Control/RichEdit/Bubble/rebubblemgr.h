#pragma 
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

namespace UI
{
class REBubble;
class WindowlessRichEdit;

class BubbleManager
{
public:
	BubbleManager(WindowlessRichEdit*  p);
    ~BubbleManager();

public:
	void  Init();
	REBubble*  CreateBubble(BubbleType eType, BubbleAlign eAlign, int nStartCP, int nEndCP);
	WindowlessRichEdit*  GetRE();
	ImageWrap*  GetLeftBubbleImage();
	ImageWrap*  GetRightBubbleImage();
    C9Region*  Get9RegionLeftBubble();
    C9Region*  Get9RegionRightBubble();

    REBubble*  FindBubble(int ncp);
	bool  IsInBubble(int ncp, __out REBubble** ppBubble);

    void  OnSize();
    void  OnDraw(HDC hDC);

    void  InvalidBubbleSize(REBubble* pBubble);

protected:
    void  Clear();
    bool  PreCalcBubblePos();
    void  PostCalcBubblePos(bool bPreRet);

    void  UpdateSingleBubblePos(REBubble* pBubble);
    void  RefreshInvalidBubbleSize();

private:
	WindowlessRichEdit*  m_pWindowlessRE;
	ImageWrap  m_imageLeftBubble;
	ImageWrap  m_imageRightBubble;
    C9Region  m_9regionLeftBubble;
    C9Region  m_9regionRightBubble;

    typedef list<REBubble*>::iterator _MyIter;
    typedef list<REBubble*>::reverse_iterator _MyReverseIter;
    
    // �����б����鰴˳�����У�����cp����ʱ��������
    list<REBubble*>  m_bubbleList;  

    // ��Ҫ�������ݴ�С�������б���ղ���ʱ��������ole��С�ı�ʱ
    list<REBubble*>  m_bubbleSizeInvalidList;
};

}