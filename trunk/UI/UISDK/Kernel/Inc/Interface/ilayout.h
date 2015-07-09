#ifndef ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184
#define ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184

namespace UI
{
class Object;
interface IMapAttribute;


interface ILayoutParam : public IRootInterface
{
    virtual void  Release() = 0;

    //  在布局前，计算控件期望的大小
    virtual SIZE  CalcDesiredSize() = 0;

    // 由当前坐标RECT反推出布局参数
    virtual void  UpdateByRect() = 0;

    // 序列化
    virtual void  Serialize(SERIALIZEDATA* pData) = 0;

    // 接口类型判断
    virtual LAYOUTTYPE  GetLayoutType() = 0;

    // 自己是自适应大小，还是固定大小，用于优化updatelayout
    virtual bool  IsSizedByContent() = 0;
};


interface ICanvasLayoutParam : public ILayoutParam
{
    virtual void SetConfigWidth(int) = 0;
    virtual void SetConfigHeight(int) = 0;
    virtual void SetConfigLayoutFlags(int) = 0;
    virtual void SetConfigLeft(int) = 0;
    virtual void SetConfigRight(int) = 0;
    virtual void SetConfigTop(int) = 0;
    virtual void SetConfigBottom(int) = 0;

    virtual int GetConfigWidth() = 0;
    virtual int GetConfigHeight() = 0;
    virtual int GetConfigLayoutFlags() = 0;
    virtual int GetConfigLeft() = 0;
    virtual int GetConfigRight() = 0;
    virtual int GetConfigTop() = 0;
    virtual int GetConfigBottom() = 0;
};

interface IStackLayoutParam : public ILayoutParam
{
    virtual void SetConfigWidth(int) = 0;
    virtual void SetConfigHeight(int) = 0;
    virtual void SetConfigLayoutFlags(int) = 0;

    virtual int GetConfigWidth() = 0;
    virtual int GetConfigHeight() = 0;
    virtual int GetConfigLayoutFlags() = 0;
};

interface IGridLayoutParam : public ILayoutParam
{
    virtual void SetConfigWidth(int) = 0;
    virtual void SetConfigHeight(int) = 0;
    virtual void SetConfigLayoutFlags(int) = 0;
    virtual void SetRow(int) = 0;
    virtual void SetCol(int) = 0;
    virtual void SetRowSpan(int) = 0;
    virtual void SetColSpan(int) = 0;

    virtual int GetConfigWidth() = 0;
    virtual int GetConfigHeight() = 0;
    virtual int GetConfigLayoutFlags() = 0;
    virtual int GetRow() = 0;
    virtual int GetCol() = 0;
    virtual int GetRowSpan() = 0;
    virtual int GetColSpan() = 0;
};

interface ICardLayoutParam : public ILayoutParam
{

};
interface IAverageLayoutParam : public ILayoutParam
{

};

interface ILayout : public IRootInterface
{
    virtual void  Release() = 0;
    
    virtual LAYOUTTYPE  GetLayoutType() = 0;
    virtual SIZE  Measure() = 0;
    virtual void  Arrange(IObject* pObjToArrage, bool bReDraw=false) = 0;
    virtual void  Serialize(SERIALIZEDATA* pData) = 0;
    virtual ILayoutParam*  CreateLayoutParam(IObject* pObj) = 0;  
    virtual void  OnChildObjectVisibleChanged(IObject* pObj) = 0;
};

interface ICanvasLayout : public ILayout
{

};

interface IStackLayout : public ILayout
{
    virtual void  SetLayoutDirection(LAYOUT_STACK_DIRECTION e) PURE;
    virtual void  SetSpace(int n) PURE;
};

interface IGridLayout : public ILayout
{

};

interface ICardLayout : public ILayout
{

};

interface IAverageLayout : public ILayout
{

};

}
#endif