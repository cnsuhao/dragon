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

// �����Ű�ʱ��������
class MultiLineLayoutContext
{
public: 
    Doc*   pDoc;
    Page*  pPage;             // ��ǰҳ
    Line*  pLine;             // ��ǰ��
    Element*  pElement;       // ��ǰ������Ԫ��
    int  nElementFrom;       // �Ӹ�Ԫ�ص��ĸ��ַ���ʼ

    uint  nPageContentWidth;   // ҳ�����ݴ�С
    uint  nPageContentHeight;

    uint  yLine;               // ���е�����ʼλ��
    uint  xLineOffset;         // ����run������ʼλ��

public:
    void  SetElementLayoutFinish();
    bool  IsElementLayoutFinish();
    void  AppendNewLine();
    void  SetLineFinish();
};

}
}