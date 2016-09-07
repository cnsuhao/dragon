#pragma 
#include "..\Ole\richeditolemgr.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

// richedit���ݱ���
//
// ���ݲ�û�в���ole��ʵ�֣���Ϊֻ��Ҫά��startcp/endcp������
// 
// buglist:
// 1.  2014.8.6. libo
//     ��������а���TAB�Ʊ�����ᵼ���Ҷ���ʱ��ȡ�п�Ȳ���ȷ�����������ܳ�������!
//     ��ν��? Ŀǰ��ʱ���ÿո��滻�ķ�ʽ
//       strContent.Replace(TEXT("\t"), TEXT("    "));
//     ����ν��copy������ʱӦ�û�ԭΪtab����
//
// 2. 2014.10.30 libo
//     ��RE�߶ȵ���Ϊ�պ�������һ���ܳ����Ҳ����ݣ������ڰ���һЩ���OLE������ͼƬ
//     Ȼ�󽫴��ڴ�С��С������������λ����˸������
//     ԭ����RE��С��RE�Ŀ�ȷŲ����Ҳ����ݣ�����Ҳ��������ݱ��ȱ�ߣ����¹��������֣�
//     RE�ͻ�����С��������������ƫ�ơ����ֹ����������ݵ���λ�õ������ǻ����µĿͻ�
//     ����С�������޷���λ���޹�������״̬��
//
//     ��ν������QQ�Ľ���취�ǣ�����ģʽ�£�������һֱռ�и�����RE�ͻ����򲻱�
//     �μ��� RichEdit::EnableBubbleMode
//    
// 3. Q: ��ʱ�����ݻ��Ƶ�λ�������ݵ�λ�ò�һ�£���ʲôԭ��
//    A: ���ݵ�λ�õ���������ʱ����txdraw�д��������txdraw֮ǰ�������ݵõ��Ľ���϶���֮��
//       ��ʾ������λ�ò�һ�¡���˻�����֮ǰ�����ȵ���һ��txdraw
//
interface ITextPara;
namespace UI
{
class BubbleManager;

class REBubble
{
public:
	REBubble();
	virtual ~REBubble();

public:
	void  SetBubbleManager(BubbleManager* pMgr);
    void  SetContentCPInfo(int nStart, int nEnd);
	
    IREBubble*  GetIREBubble();
    void  CreateIxxx(IREBubble* p);

protected:
	void  GetDrawRegion(RECT* prc);
    void  CalcBubblePos();
	void  SetDesiredAlign(BubbleAlign e);
    void  UpdateRightBubbleIndents(ITextPara*  pPara);
	void  UpdateCenterBubbleIndents(ITextPara*  pPara);

protected:
	// ��������
	virtual void  Draw(HDC hDC, RECT* prcDraw);
	// ��ȡ����������֮��ļ��
	virtual void  GetContentPadding(REGION4* pPadding);

protected:
	friend class BubbleManager;
	BubbleManager*  m_pBubbleMgr;
    IREBubble*  m_pIREBubble;
    bool  m_bNeedDeleteIREBubble;

    int     m_nContentWidth;
    int     m_nContentHeight;
    int     m_ncpStart;
    int     m_ncpEnd;
	
	BubbleAlign  m_eAlignDesired;  // �����Ķ��뷽ʽ
	BubbleAlign  m_eAlignReal;     // ʵ�ʵĶ��뷽ʽ
};

// ����+ͷ��
class REPortraitBubble : public REBubble
{
public:
	REPortraitBubble();
    ~REPortraitBubble();
	
	// ����ͷ�������ݵ�ƫ��
	enum
	{
		PORTRAIT_OFFSET_LEFT_X = 30,
		PORTRAIT_OFFSET_LEFT_T = 0,
		PORTRAIT_OFFSET_RIGHT_X = 30,
		PORTRAIT_OFFSET_RIGHT_Y = 0,
	};

	virtual void  Draw(HDC hDC, RECT* prcDraw);

	void  SetPortrait(ImageWrap* pPortrait);
    IREPortraitBubble*  GetIREPortraitBubble();
    void  CreateIxxx(IREPortraitBubble* p);

protected:
	IREPortraitBubble*   m_pIREPortraitBubble;
    bool  m_bNeedDeleteIREPortraitBubble;

	ImageWrap*  m_pImagePortrait;
};

class REInfoBubble : public REBubble
{
public:
	virtual void  Draw(HDC hDC, RECT* prcDraw){};
	virtual void  GetContentPadding(REGION4* pPadding){};
};


}