#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

namespace UI
{

class Object;

class StackLayoutParam : public LayoutParamImpl<IStackLayoutParam>
{
public:
    StackLayoutParam();
    ~StackLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_STACK; }
    virtual SIZE  CalcDesiredSize();
    virtual bool  IsSizedByContent();

public:
    DECLARE_INT_SETGET(ConfigWidth)
    DECLARE_INT_SETGET(ConfigHeight)
    DECLARE_INT_SETGET(ConfigLayoutFlags)

private:
    long  m_nConfigWidth;  
    long  m_nConfigHeight; 
    long  m_nConfigLayoutFlags;
};
class StackLayout : public LayoutImpl<StackLayout, IStackLayout, StackLayoutParam, LAYOUT_TYPE_STACK>
{
public:
	StackLayout();
    ~StackLayout();

    virtual SIZE  Measure() override;
	virtual void  Arrange(IObject* pObjToArrage = NULL, bool bUpdate=false) override;
	virtual void  Serialize(SERIALIZEDATA* pData) override;

    virtual void  SetLayoutDirection(LAYOUT_STACK_DIRECTION e);
    virtual void  SetSpace(int n);

protected:
    void  ArrangeObject_Left(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size);
    void  ArrangeObject_Right(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size);
    void  ArrangeObject_Top(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size);
    void  ArrangeObject_Bottom(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size);

    bool  IsHorz();
    bool  IsVert();

public:
	LAYOUT_STACK_DIRECTION  m_eDirection;    // 堆栈排布的方向
    long  m_nSpace;  // 两个子对象之间间隙
};



}