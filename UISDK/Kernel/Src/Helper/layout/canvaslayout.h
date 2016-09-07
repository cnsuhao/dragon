#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

namespace UI{

class Object;

class CanvasLayoutParam : public LayoutParamImpl<ICanvasLayoutParam>
{
public:
    CanvasLayoutParam();
    ~CanvasLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_CANVAS; }
    virtual SIZE  CalcDesiredSize();
    virtual bool  IsSizedByContent();

    static int  ParseAlignAttr(LPCTSTR);
public:
    DECLARE_INT_SETGET(ConfigWidth)
    DECLARE_INT_SETGET(ConfigHeight)
    DECLARE_INT_SETGET(ConfigLayoutFlags)
    DECLARE_INT_SETGET(ConfigLeft)
    DECLARE_INT_SETGET(ConfigRight)
    DECLARE_INT_SETGET(ConfigTop)
    DECLARE_INT_SETGET(ConfigBottom)

private:
    long  m_nConfigWidth;   // ����Ŀ�ȣ���ȡֵ�� ��ֵ | "auto" . ������window����width ��ָclient����Ĵ�С�������������ڵĴ�С��width����padding����������margin��
    long  m_nConfigHeight;  // ����ĸ߶ȣ���ȡֵ�� ��ֵ | "auto" . ������window����height��ָclient����Ĵ�С�������������ڵĴ�С��height����padding����������margin��
    // ��������Ҫ˵�������������ռ�õĿ��= margin.left + width + margin.right
    // Ҳ����˵�����width = padding.left + padding.right + content.width
    long  m_nConfigLeft;
    long  m_nConfigRight;
    long  m_nConfigTop;
    long  m_nConfigBottom;
    long  m_nConfigLayoutFlags;
};


//
//	ָ���������Ե��λ��
//
class CanvasLayout : public LayoutImpl<CanvasLayout, ICanvasLayout, CanvasLayoutParam, LAYOUT_TYPE_CANVAS>
{
public:
    virtual void  Serialize(SERIALIZEDATA*) override{};
    virtual SIZE  Measure() override;
    virtual void  Arrange(IObject* pObjToArrage=NULL, bool bUpdate=false) override;

public:
    static void  ArrangeObject(Object*  pChild, const int& nWidth, const int& nHeight);
};

}
