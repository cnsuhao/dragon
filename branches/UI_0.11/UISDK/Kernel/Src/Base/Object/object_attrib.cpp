#include "stdafx.h"
#include "object.h"
#include "UISDK\Kernel\Inc\base\messagedefine.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

#include "UISDK\Kernel\Src\Base\Attribute\attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\bool_attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\enum_attribute.h"
#include "UISDK\Kernel\Src\Base\Attribute\stringselect_attribute.h"

using namespace UI;

// ��xml�õ�����ת�ɶ�������ԣ�ע�⣬�������ظú���ʱ�������ȵ��ø���ĸ÷���
// bReload��ʾΪ����ʱ���ã���ʱ�����ٸ�ĳЩ���Ը�ֵ������text����
void  Object::OnSerialize(SERIALIZEDATA* pData)
{
    IMapAttribute*  pMapAttrib = pData->pMapAttrib;
	if (pData->IsReload())
	{
		SAFE_RELEASE(m_pLayoutParam);
		SAFE_RELEASE(m_pBkgndRender);
		SAFE_RELEASE(m_pForegndRender);
		SAFE_RELEASE(m_pTextRender);
		SAFE_RELEASE(m_pCursor);
	}

    if (pData->IsLoad())
    {
        // �������ԣ�������չ��
        SAFE_RELEASE(m_pIMapAttributeRemain);
        m_pIMapAttributeRemain = pMapAttrib;
        if (m_pIMapAttributeRemain)
            m_pIMapAttributeRemain->AddRef();

        if (NULL == m_pUIApplication || NULL == pMapAttrib)
        {
            UI_LOG_ERROR(_T("Invalid Argument."));
            return;
        }
    }

    {
		AttributeSerializer s(pData, TEXT("Object"));
        s.AddString(XML_ID, m_strId);

		// styelclass���޸�ʱ��Ӧ�����½�����������
        s.AddString(XML_STYLECLASS, m_strStyleClass)->ReloadOnChanged();
        s.AddString(XML_TOOLTIP, m_strTooltip);

	    s.AddRect(XML_MARGIN, m_rcMargin);
	    s.AddRect(XML_PADDING, m_rcPadding);
	    s.AddRect(XML_BORDER, m_rcBorder);

        s.AddEnum(XML_VISIBLE, this,
            memfun_cast<pfnLongSetter>(&Object::LoadVisibleEx), 
            memfun_cast<pfnLongGetter>(&Object::SaveVisibleEx))
			->AddOption(VISIBILITY_COLLAPSED, XML_VISIBILITY_COLLAPSED)
			->AddOption(VISIBILITY_VISIBLE, XML_VISIBILITY_VISIBLE)
			->AddOption(VISIBILITY_HIDDEN, XML_VISIBILITY_HIDDEN)
		    ->SetDefault(VISIBILITY_VISIBLE)->AsData();

        s.AddBool(XML_DISABLE, this, 
            memfun_cast<pfnBoolSetter>(&Object::SetDisableDirect), 
            memfun_cast<pfnBoolGetter>(&Object::IsSelfDisable))
            ->AsData();

	    s.AddBool(XML_BACKGND_IS_TRANSPARENT, this, 
		    memfun_cast<pfnBoolSetter>(&Object::SetTransparent), 
		    memfun_cast<pfnBoolGetter>(&Object::IsTransparent))
            ->SetDefault(m_objStyle.default_transparent);

		s.AddLong(XML_ZORDER, m_lzOrder);  // z�� (ע��Ĭ���ڴ�xml����֮��AddChild֮ǰ���Ƚ���һ��)
	    s.AddBool(XML_NO_CLIP_DRAW, this, 
		    memfun_cast<pfnBoolSetter>(&Object::SetNoClip), 
		    memfun_cast<pfnBoolGetter>(&Object::IsNoClip));

        s.AddBool(XML_REJEST_MOUSE_MSG, this, 
            memfun_cast<pfnBoolSetter>(&Object::SetRejectMouseMsgAll), 
            memfun_cast<pfnBoolGetter>(&Object::IsRejectMouseMsgAll))
            ->SetDefault(m_objStyle.default_reject_all_mouse_msg);

		s.AddBool(XML_ISNCCHILD, this,
			memfun_cast<pfnBoolSetter>(&Object::SetAsNcObject), 
			memfun_cast<pfnBoolGetter>(&Object::IsNcObject))
			->SetDefault(m_objStyle.default_ncobject);

        s.AddBool(XML_LAYER, this, 
            memfun_cast<pfnBoolSetter>(&Object::SetRenderLayer), 
            memfun_cast<pfnBoolGetter>(&Object::HasRenderLayer));

	    s.AddString(XML_CURSOR, this, 
		    memfun_cast<pfnStringSetter>(&Object::SetCursorId),
		    memfun_cast<pfnStringGetter>(&Object::SaveCursorId));

//      s.AddRenderBase(XML_BACKGND_RENDER_PREFIX, XML_RENDER_TYPE, m_pIObject, m_pBkgndRender);  // ���ñ�����Ⱦ��
//      s.AddRenderBase(XML_FOREGND_RENDER_PREFIX, XML_RENDER_TYPE, m_pIObject, m_pForegndRender); // ����ǰ������

        s.AddStringEnum(XML_BACKGND_RENDER_PREFIX XML_RENDER_TYPE, this,
            memfun_cast<pfnStringSetter>(&Object::LoadBkgndRender),
            memfun_cast<pfnStringGetter>(&Object::SaveBkgndRender))
            ->FillRenderBaseTypeData()
            ->ReloadOnChanged();
        s.AddStringEnum(XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE, this,
            memfun_cast<pfnStringSetter>(&Object::LoadForegndRender),
            memfun_cast<pfnStringGetter>(&Object::SaveForegndRender))
            ->FillRenderBaseTypeData()
            ->ReloadOnChanged();
    }

	// ��������
	if (m_pLayoutParam)
	{
		m_pLayoutParam->Serialize(pData);
	}
	else if (pData->IsReload())
	{
		CreateLayoutParam();
	}

	// ��������
	if (m_pBkgndRender)
	{
		SERIALIZEDATA data(*pData);
		data.szParentKey = XML_BACKGND_RENDER_PREFIX XML_RENDER_TYPE;
		data.szPrefix = XML_BACKGND_RENDER_PREFIX;

        // ��editor�У���̬�޸�render type��Ҫ������ԡ�
        // 1. ���Կ��ܹ���һ��key����render.image=����ʹ����type������Ҳ���Թ���
        // 2. Ҫʵ��undo/redo�����ܶ�������
        if (data.pUIApplication->IsDesignMode()) 
            data.SetErase(false);

		m_pBkgndRender->Serialize(&data);
	}

	// ǰ������
	if (m_pForegndRender)
	{
		SERIALIZEDATA data(*pData);
		data.szParentKey = XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE;
		data.szPrefix = XML_FOREGND_RENDER_PREFIX;
        if (data.pUIApplication->IsDesignMode())
            data.SetErase(false);

		m_pForegndRender->Serialize(&data);
	}
}

