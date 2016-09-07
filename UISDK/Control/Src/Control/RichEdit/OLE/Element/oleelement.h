#pragma once

// 2014/10/29 9:48
//
// OLE�ڲ���ȾԪ�أ������ı������ӡ���ť��ͼƬ��������
// 1. ��׼��ʵ������������Ԫ�ض���ͬһƽ�档
// 2. ��ʵ�ֲü���ƫ�ƣ���Ԫ���Լ�����
//

namespace UI
{
class CompositeREOle;

struct OleElementStyle
{
    bool  bVisible : 1;
    bool  bEnable : 1;
};
struct OleElementLayoutParam
{
    int  nLeft;
    int  nTop;
    int  nRight;
    int  nBottom;
    int  nWidth;
    int  nHeight;

    int  lFlags;

public:
    void  StretchAll()
    {
        nLeft = nTop = nRight = nBottom = 0;
        nWidth = nHeight = NDEF;
        lFlags = 0;
    }
};

class OleElement
{
public:
    OleElement();
    virtual ~OleElement();
    void  Release();

public:
	void  SetCompositeREOle(CompositeREOle* p);
    long  GetId();
    void  SetId(long lId);

    void  SetVisible(bool b);
    void  SetEnable(bool b);
    bool  IsEnable();
    bool  IsVisible();

    void  UpdateLayout(int cxOle, int cyOle);
    void  Invalidate();
    void  GetRectInOle(RECT* prc);

public:
    virtual long  GetType() PURE;
    virtual void  Draw(HDC hDC, int xOffset, int yOffset) PURE;
    virtual LRESULT  ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    long  m_lId;
    CRect  m_rcInOle;
    OleElementStyle  m_style;
	CompositeREOle*  m_pREOle;
public:
    OleElementLayoutParam  m_layoutParam;
};


}