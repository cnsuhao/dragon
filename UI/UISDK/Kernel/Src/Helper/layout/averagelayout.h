#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

// ƽ�����䲼��
namespace UI
{

class Object;

class AverageLayoutParam : public LayoutParamImpl<IAverageLayoutParam>
{
public:
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_AVERAGE; }
    virtual bool  IsSizedByContent() { return false; }
};


class AverageLayout : public LayoutImpl<AverageLayout, IAverageLayout, AverageLayoutParam, LAYOUT_TYPE_AVERAGE>
{
public:
	AverageLayout();
    ~AverageLayout();

    virtual SIZE  Measure() override;
	virtual void  Arrange(IObject* pObjToArrage = NULL, bool bUpdate=false) override;
	virtual void  Serialize(SERIALIZEDATA*) override;

    void  ArrangeObject_H(Object* pChildObj, CRect* prc);
    void  ArrangeObject_V(Object* pChildObj, CRect* prc);

public:
	LAYOUT_AVERAGE_DIRECTION  direction;    // ��ջ�Ų��ķ���
    Object*   m_pPanel;   // ��ò��ֹ�����panel
    int   m_nGap;  // �����Ӷ���֮���϶,
};



}