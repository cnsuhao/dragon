#pragma once

namespace UI
{
class UISDKAPI RenderContext
{
public:
	RenderContext(RECT* prcRenderRegion, bool bClip, bool bRequireAlphaChannel);

    void  DrawClient(IObject* pObject);
	void  Scroll(IObject* pObjScroll);
    bool  DrawChild(IObject* pChild);
	bool  DrawListItem(IListItemBase* pItem);
	
	void  Update(IRenderTarget* pRenderTarget);
	void  Reset(IRenderTarget* pRenderTarget);

public:
	bool      m_bUpdateClip;   // �Ƿ���Ҫά��CLIP����
    bool      m_bRequireAlphaChannel;  // �Ƿ���Ҫalpha�����ڷֲ㴰�ں�areo����

	RECT      m_rcDrawRegion;  // ��������
	POINT     m_ptOffset;      // ��ǰҪ���ƵĶ���ƫ��������ʵ�����豸����
};

}

