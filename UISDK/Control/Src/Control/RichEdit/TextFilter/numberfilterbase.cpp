#include "stdafx.h"
#include "numberfilterbase.h"

using namespace UI;


RETextFilterBase::RETextFilterBase()
{
    m_pRE = NULL;

    m_bOldEnableIme = false;
    m_bOldRichText = false;
    m_bOldEnableDragdrop = false;
    m_bOldMultiline = false;
}

void  RETextFilterBase::BindRichEdit(IRichEdit* pRE)
{
    this->m_pRE = pRE;

	if (!m_pRE)
		return;

    // �������뷨
    ObjStyle s = {0};
    s.enable_ime = 1;
    m_bOldEnableIme = m_pRE->TestObjectStyle(s);
    if (m_bOldEnableIme)
        m_pRE->ModifyObjectStyle(NULL, &s);

    // ����
    m_bOldMultiline = m_pRE->IsMultiLine();
    if (m_bOldMultiline)
        m_pRE->SetMultiLine(false);

    // ֻ����CF_TEXT/CF_UNICODETEXT
    m_bOldRichText = m_pRE->GetRichTextFlag();
    if (m_bOldRichText)
        m_pRE->SetRichTextFlag(false);  // �������Ѿ�����drop��

    // ��������ק
    //     m_bOldEnableDragdrop = m_pRE->CanDragDrop();
    //     if (m_bOldEnableDragdrop)
    //         m_pRE->EnableDragDrop(false);
}
void  RETextFilterBase::UnbindRichEdit()
{
    if (!this->m_pRE)
        return;

    if (m_bOldEnableIme)
    {
        ObjStyle s = {0};
        s.enable_ime = 1;
        m_pRE->ModifyObjectStyle(&s, NULL);
    }
    if (m_bOldRichText)
        m_pRE->SetRichTextFlag(true);

    if (m_bOldMultiline)
        m_pRE->SetMultiLine(m_bOldMultiline);

    //     if (m_bOldEnableDragdrop)
    //         m_pRE->EnableDragDrop(true);

}

void  RETextFilterBase::Release(bool bRestoreState)
{
    if (bRestoreState)
    {
        UnbindRichEdit();
    }
    delete this;
}

void  RETextFilterBase::OnInvalid()
{
    if (Util::IsKeyDown(VK_CONTROL) || Util::IsKeyDown(VK_SHIFT))
    {
        // ����ǿ�ݼ���������
        return;
    }

    MessageBeep(-1);
}