#include "stdafx.h"
#include "control.h"
#include "UISDK\Kernel\Src\Base\Attribute\attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\bool_attribute.h"
#include "UISDK\Kernel\Src\Base\Application\uiapplication.h"
#include "UISDK\Kernel\Src\Base\Attribute\stringselect_attribute.h"

using namespace UI;

Control::Control()
{
    m_pIControl = NULL;
	memset(&m_controlStyle, 0, sizeof(m_controlStyle));
	m_objStyle.tabstop = 1;
    m_objStyle.default_tabstop = 1;
}

void  Control::OnSerialize(SERIALIZEDATA* pData)
{
    __super::OnSerialize(pData);

	{
		AttributeSerializer s(pData, TEXT("Control"));

		s.AddBool(XML_CONTROL_STYLE_TABABLE, this, 
			memfun_cast<pfnBoolSetter>(&Object::SetTabstop),
			memfun_cast<pfnBoolGetter>(&Object::IsTabstop))
			->SetDefault(m_objStyle.default_tabstop);

		s.AddBool(XML_CONTROL_STYLE_GROUP, this, 
			memfun_cast<pfnBoolSetter>(&Control::SetGroup),
			memfun_cast<pfnBoolGetter>(&Control::IsGroup));

		//s.AddTextRenderBase(NULL, XML_TEXTRENDER_TYPE, m_pIObject, m_pTextRender);
		s.AddStringEnum(XML_TEXTRENDER_TYPE, this,
			memfun_cast<pfnStringSetter>(&Object::LoadTextRender),
			memfun_cast<pfnStringGetter>(&Object::SaveTextRender))
			->FillTextRenderBaseTypeData()
			->ReloadOnChanged();

	}

	// 字体
	if (m_pTextRender)
	{
        SERIALIZEDATA data(*pData);
        data.szParentKey = XML_TEXTRENDER_TYPE;
		m_pTextRender->Serialize(&data);
	}
}

void  Control::ModifyControlStyle(ControlStyle* add, ControlStyle* remove)
{
	if (add)
	{
		if (add->group)
			m_controlStyle.group = 1;
		if (add->ownerdraw)
			m_controlStyle.ownerdraw = 1;
	}

	if (remove)
	{
		if (remove->group)
			m_controlStyle.group = 0;
		if (remove->ownerdraw)
			m_controlStyle.ownerdraw = 0;
	}
}
bool  Control::TestControlStyle(ControlStyle* test)
{
	if (!test)
		return false;

	if (test->group && m_controlStyle.group)
		return true;
	if (test->ownerdraw && m_controlStyle.ownerdraw)
		return true;

	return false;
}

bool Control::IsGroup()
{
	return  m_controlStyle.group;
}
void Control::SetGroup(bool b)
{
	m_controlStyle.group = b;
}

// 如果没有在皮肤中配置字体，则外部可调用该函数在Paint时创建一个默认的字体
ITextRenderBase*  Control::CreateDefaultTextRender()
{
    if (!m_pTextRender && m_pUIApplication)
    {
        m_pUIApplication->CreateTextRenderBase(
            TEXTRENDER_TYPE_SIMPLE, m_pIObject, &m_pTextRender);

        if (m_pTextRender)
        {
            SERIALIZEDATA data = {0};
            data.pUIApplication = m_pUIApplication->GetIUIApplication();
            data.pMapAttrib = m_pIMapAttributeRemain;
            data.szPrefix = NULL;
            data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
            m_pTextRender->Serialize(&data);
        }
    }

    return m_pTextRender;
}