#pragma once
namespace UI
{
namespace RichTextObjectModel
{
class Doc;
class Page;
class Line;
class Element;

class SingleLineLayoutContext
{
public:
    Doc*   pDoc;
    Page*  pPage;
    Line*  pLine;
    uint  xLineOffset;
};

// 多行排版时的上下文
class MultiLineLayoutContext
{
public: 
    Doc*   pDoc;
    Page*  pPage;             // 当前页
    Line*  pLine;             // 当前行
    Element*  pElement;       // 当前分析的元素
    int  nElementFrom;       // 从该元素的哪个字符开始

    uint  nPageContentWidth;   // 页面内容大小
    uint  nPageContentHeight;

    uint  yLine;               // 该行的纵向超始位置
    uint  xLineOffset;         // 该行run横向起始位置

public:
    void  SetElementLayoutFinish();
    bool  IsElementLayoutFinish();
    void  AppendNewLine();
    void  SetLineFinish();
};

}
}