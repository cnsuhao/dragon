#pragma once
#include "node.h"
#include "Inc\Interface\irichtext.h"

namespace UI
{
namespace RT
{

class Link : public Node, public ILink
{
public:
    Link();
    ~Link(){};

    virtual NodeType  GetNodeType() override { return NodeType::Link; }
    virtual const CharFormat*  GetCharFormat() override;
    virtual BOOL  OnSetCursor() override;
    virtual void  OnClick(UINT nFlags, POINT ptDoc) override;
    virtual void  OnMouseEnter() override;
    virtual void  OnMouseLeave() override;

    virtual void  SetId(LPCTSTR szId) override;
    virtual LPCTSTR  GetId() override;
    virtual void  SetToolTip(LPCTSTR) override;
    virtual LPCTSTR  GetToolTip() override;
    virtual LPCTSTR  GetReference() override;
    virtual void  SetReference(LPCTSTR) override;

	void  SetWParam(WPARAM);
	void  SetLParam(LPARAM);
	WPARAM  GetWParam();
	LPARAM  GetLParam();

protected:
    String  m_strId;
	String  m_strToolTip;
    String  m_strRefefence;
	WPARAM  m_wParam;
	LPARAM  m_lParam;
};
}
}