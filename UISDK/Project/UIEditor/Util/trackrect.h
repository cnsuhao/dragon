#pragma once
//
// 2014.4.16
// ���MFC CTrackRect��ʵ�ֿؼ���ק
//
// ֧�������ק��������ק��֧����קģʽ����
//
enum RectTrackerHit
{
    hitNothing = -1,
    hitTopLeft = 0, 
    hitTopRight = 1, 
    hitBottomRight = 2, 
    hitBottomLeft = 3,
    hitTop = 4, 
    hitRight = 5, 
    hitBottom = 6, 
    hitLeft = 7, 
    hitMiddle = 8,   // ���ڱ�ʾ��קmove
    hitCount = 9,    // 
};

enum RectTrackerMask
{
    recttrackermask_none = 0,
    recttrackermask_move = 1,
    recttrackermask_size = 2,
    recttrackermask_sizemove = 3,
    recttrackermask_iscontainer = 4,  // ����һ���������ڲ���������Ԫ��
};

interface  IRectTrackerRsp
{
    // ֪ͨ�ⲿ��ʼ�ƶ����ȱ���һ��ԭʼλ��
    virtual void  OnRectTrackerDragStart() = 0;
    // ��ԭʼλ�õĻ������ƶ��ľ���
    virtual void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta) = 0;
    // ֪ͨ�ⲿ�ƶ����������浱ǰֵ
    virtual void  OnRectTrackerDragEnd() = 0;

    // ����ת��
    virtual void  DP2LP(POINT* p) = 0;
};
class RectTracker
{
public:
    static void Init();
    static void Release();
    
    RectTracker();
    ~RectTracker();

    void  SetInfo(HWND hWnd, IRectTrackerRsp* prsp)
    {
        m_hWnd = hWnd;
        m_prsp = prsp;
    }
    void  Update(RECT*  prc, UINT nMask);
    void  Draw(HDC  hDC);
    BOOL  ProcessMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, long* plRet);
    
protected:
    RectTrackerHit  HitTest(POINT  pt);
    RectTrackerHit  HitTestUnderCursor();

    bool   IsContainer();
    bool   CanSize();
    bool   CanMove();

	void   GetHandleRect(RectTrackerHit nIndex, RECT* prc);

public:
	bool     m_bStartup;  // �Ƿ���ѡ�ж���

    HWND     m_hWnd;
    CRect    m_rect;
    UINT     m_nMask;   
    int      m_nHandleSize;  
    IRectTrackerRsp*  m_prsp;      // ��Ӧ��

    RectTrackerHit  m_eActionHit;  // ��ǰ���/������קʱ�Ĳ���

    bool     m_bMouseDraging;      // ��־���Ƿ��������ק
    POINT    m_ptLButtonDown;      // ��¼��갴��ʱ��λ�ã������ƶ��ؼ�
    
    bool     m_bKeyboardDraging;   // ��־���Ƿ񿪻�������ק
    UINT     m_nKeyPushdown;       // �������ĸ���
    long     m_nKeyboardHitCount;  // �����¼���������(��ס����)

    static HCURSOR  s_hCursor[9];
    static long  s_lRef;
};