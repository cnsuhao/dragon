#pragma once
#include "Inc\Interface\ilayout.h"
#include "Src\Helper\layout\layout.h"

namespace UI
{

class Object;
class VertLayout;

// enum VertLayoutParamVertAlignType
// {
// 	VertLayoutParamVertAlign_Top,
// 	VertLayoutParamVertAlign_Bottom,
// };
// 
// enum VertLayoutParamHorzAlignType
// {
// 	VertLayoutParamHorzAlign_LeftAndRight,  // 两侧对齐
// 	VertLayoutParamHorzAlign_Left,       // 左对齐
// 	VertLayoutParamHorzAlign_Right,      // 右对齐
// 	VertLayoutParamHorzAlign_Center,     // 居中
// };

// enum VertLayoutParamHeightValueType
// {
// 	VertLayoutParamHeightValue_Set,     // 指定的值
// 	VertLayoutParamHeightValue_Auto,    // 自动，忽略指定的值
// 	VertLayoutParamHeightValue_Avg,     // 取平均
// };
// 
// enum VertLayoutParamWidthValueType
// {
// 	VertLayoutParamWidthValue_Set,      // 指定的值
// 	VertLayoutParamWidthValue_Auto,     // 自动，忽略指定的值
// };


class VertLayoutParam : public LayoutParamImpl<IVertLayoutParam>
{
public:
    VertLayoutParam();
    ~VertLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_VERT; }
    virtual SIZE  CalcDesiredSize();
    virtual bool  IsSizedByContent();

public:
    virtual long  GetConfigWidth() override;
    virtual void  SetConfigWidth(long) override;
    void  LoadConfigWidth(long);
    long  SaveConfigWidth();

    virtual long  GetConfigHeight() override;
    virtual void  SetConfigHeight(long) override;
    void  LoadConfigHeight(long);
    long  SaveConfigHeight();

    virtual void  SetConfigLayoutFlags(long) override;
    virtual long  GetConfigLayoutFlags() override;

protected:
    long  m_nConfigWidth;  
    long  m_nConfigHeight; 
    long  m_nConfigLayoutFlags;
	LAYOUT_VALUE_TYPE  m_eWidthValueType;
	LAYOUT_VALUE_TYPE  m_eHeightValueType;

	friend VertLayout;
};

class VertLayout : public LayoutImpl<VertLayout, IVertLayout, VertLayoutParam, LAYOUT_TYPE_VERT>
{
public:
	VertLayout();
    ~VertLayout();

    virtual SIZE  Measure() override;
	virtual void  DoArrage(IObject* pObjToArrage = NULL) override;
	virtual void  Serialize(SERIALIZEDATA* pData) override;
    virtual void  OnChildObjectVisibleChanged(IObject* pObj) override;

    virtual void  SetSpace(int n);

protected:

    void  LoadGap(long);
    long  SaveGap();

public:
    long  m_nSpace;  // 两个子对象之间间隙
};



}