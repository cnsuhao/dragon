#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

// 平均分配布局
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
	LAYOUT_AVERAGE_DIRECTION  direction;    // 堆栈排布的方向
    Object*   m_pPanel;   // 与该布局关联的panel
    int   m_nGap;  // 两个子对象之间间隙,
};



}