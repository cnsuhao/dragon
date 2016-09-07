#pragma once
#include "../Base/icommand.h"
#include "Business\Project\ProjectData\projectdata.h"

// image command�а�image item��xml node���浽uieditor���С�����ά�������Ƚ��鷳
// ��style command�г��԰�xml nodeֱ����style item���档
class StyleOpCommand : public CommandBase
{
public:
    StyleOpCommand()
    {
        m_pSkin = NULL;
    }
    ~StyleOpCommand()
    {
    }

    IStyleResItem*  AddStyle(STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit)
    {
        if (!m_pSkin || !szId)
            return NULL;

        // ��һ�ڣ�Ĭ�϶���ӵ���һ��<style>�����
        IStyleManager*  pStyleMgr = m_pSkin->GetStyleManager();
        IStyleRes*  pStyleRes = pStyleMgr->GetStyleRes();

        IUIElement*  pStyleXml = pStyleMgr->GetStyleXmlElem(NULL);
        if (!pStyleXml)
            return NULL;

        // ��ӵ�styleres
        IStyleResItem* pItem = pStyleRes->Add(type, szId, szInherit);
        if (!pItem)
            return NULL;

        // ��ӵ�xml
        IUIElement*  pUIElement = NULL;
        switch (type)
        {
        case STYLE_SELECTOR_TYPE_ID:
            pStyleXml->AddChild(XML_STYLE_SELECTOR_TYPE_ID, &pUIElement);
            pUIElement->AddAttrib(XML_ID, szId);
            break;

        case STYLE_SELECTOR_TYPE_CLASS:
            pStyleXml->AddChild(XML_STYLE_SELECTOR_TYPE_CLASS, &pUIElement);
            pUIElement->AddAttrib(XML_ID, szId);
            break;

        case STYLE_SELECTOR_TYPE_TAG:
            pStyleXml->AddChild(szId, &pUIElement);
            break;

        default:
            return NULL;
        }
        if (!pUIElement)
        {
            pStyleRes->Remove(type, szId);
            pItem = NULL;
            return NULL;
        }
        pItem->SetXmlElement(pUIElement);

        SetDirty(pUIElement);
        SAFE_RELEASE(pUIElement);

        return pItem;
    }

	IStyleResItem*  InsertStyle(STYLE_SELECTOR_TYPE type, LPCTSTR szId, long lPos)
	{
		if (!m_pSkin || !szId)
			return NULL;


		// ��һ�ڣ�Ĭ�϶���ӵ���һ��<style>�����
		IStyleManager*  pStyleMgr = m_pSkin->GetStyleManager();
		IStyleRes*  pStyleRes = pStyleMgr->GetStyleRes();

		IUIElement*  pStyleXml = pStyleMgr->GetStyleXmlElem(NULL);
		if (!pStyleXml)
			return NULL;

		IStyleResItem* pPrevItem = pStyleRes->GetItem(lPos);
		if (!pPrevItem)
			return AddStyle(type, szId, NULL);

		// ��ӵ�styleres
		IStyleResItem* pItem = pStyleRes->Insert(type, szId, lPos);
		if (!pItem)
			return NULL;

		// ��ӵ�xml
		IUIElement*  pUIElement = NULL;
		IUIElement*  pPrevUIElement = pPrevItem->GetXmlElement();
		switch (type)
		{
		case STYLE_SELECTOR_TYPE_ID:
			pStyleXml->AddChildBefore(XML_STYLE_SELECTOR_TYPE_ID, pPrevUIElement, &pUIElement);
			pUIElement->AddAttrib(XML_ID, szId);
			break;

		case STYLE_SELECTOR_TYPE_CLASS:
			pStyleXml->AddChildBefore(XML_STYLE_SELECTOR_TYPE_CLASS, pPrevUIElement, &pUIElement);
			pUIElement->AddAttrib(XML_ID, szId);
			break;

		case STYLE_SELECTOR_TYPE_TAG:
			pStyleXml->AddChildBefore(szId, pPrevUIElement, &pUIElement);
			break;

		default:
			return NULL;
		}
		if (!pUIElement)
		{
			pStyleRes->Remove(type, szId);
			pItem = NULL;
			return NULL;
		}
		pItem->SetXmlElement(pUIElement);

		SetDirty(pUIElement);
		SAFE_RELEASE(pUIElement);

		return pItem;
	}

	bool  DeleteStyle(STYLE_SELECTOR_TYPE type, LPCTSTR szId)
	{
		if (!m_pSkin || !szId)
			return false;

		// ��һ�ڣ�Ĭ�϶���ӵ���һ��<style>�����
		IStyleManager*  pStyleMgr = m_pSkin->GetStyleManager();
		IStyleRes*  pStyleRes = pStyleMgr->GetStyleRes();
		
		IStyleResItem*  pStyleItem = pStyleRes->FindItem(type, szId);
		if (!pStyleItem)
			return false;


		IUIElement*  pItemXml = NULL;
		pStyleItem->GetXmlElement2(&pItemXml);
		if (!pItemXml)
			return false;

		if (!pStyleRes->Remove(pStyleItem))
			return false;

		// ��xml��ɾ��
		SetDirty(pItemXml);
		pItemXml->RemoveSelfInParent();

		SAFE_RELEASE(pItemXml);
		return true;
	}

	bool  SaveXmlAttribute(STYLE_SELECTOR_TYPE type, LPCTSTR szId, IListAttribute** pp)
	{
		if (!m_pSkin || !szId)
			return false;

		// ��һ�ڣ�Ĭ�϶���ӵ���һ��<style>�����
		IStyleManager*  pStyleMgr = m_pSkin->GetStyleManager();
		IStyleRes*  pStyleRes = pStyleMgr->GetStyleRes();

		IStyleResItem*  pStyleItem = pStyleRes->FindItem(type, szId);
		if (!pStyleItem)
			return false;

		IUIElement*  pItemXml = NULL;
		pStyleItem->GetXmlElement2(&pItemXml);
		if (!pItemXml)
			return false;

		pItemXml->GetAttribList2(pp);
		SAFE_RELEASE(pItemXml);

		return true;
	}

	
	long*  GetKey(void)
	{
		UIASSERT(m_pSkin);
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		if (pStyleRes)
			return (long*)pStyleRes;
		else
			return (long*)m_pSkin;
	}

protected:
    ISkinRes*  m_pSkin;
};


// �������
class AddStyleItemCommand : public StyleOpCommand
{
public:
	static ICommand*  CreateInstance(ISkinRes* pSkinRes, STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit)
	{
		if (!pSkinRes || !szId)
			return NULL;

		if (pSkinRes->GetStyleRes()->FindItem(type, szId))
		{
			String  str = _T("AddStyle Id�ظ�: ");
			str.append(szId);
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
			return NULL;
		}

		AddStyleItemCommand* p = new AddStyleItemCommand;
		p->AddRef();

		p->m_pSkin = pSkinRes;
		p->m_eType = type;
		p->m_strId = szId;
		if (szInherit)
			p->m_strInherit = szInherit;

		return p;
	}
	virtual long execute()
	{
		__super::AddStyle(m_eType, m_strId.c_str(), m_strInherit.c_str());

		{
			String str(TEXT("Add Style Item: "));
			str.append(m_strId.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}
		return 0;
	}
	virtual long undo()
	{
		__super::DeleteStyle(m_eType, m_strId.c_str());

		{
			String str(TEXT("Delete Style Item: "));
			str.append(m_strId.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}
		return 0;
	}

private:
	STYLE_SELECTOR_TYPE  m_eType;
	String  m_strId;
	String  m_strInherit;
};


// �������
class DeleteStyleItemCommand : public StyleOpCommand
{
public:
	DeleteStyleItemCommand()
	{
		m_lSavePos = -1;
		m_pAtributes = NULL;
		m_eType = STYLE_SELECTOR_TYPE_CLASS;
	}
	~DeleteStyleItemCommand()
	{
		SAFE_RELEASE(m_pAtributes);
	}
	static ICommand*  CreateInstance(ISkinRes* pSkinRes, IStyleResItem* pItem)
	{
		if (!pSkinRes || !pItem)
			return NULL;

		DeleteStyleItemCommand* p = new DeleteStyleItemCommand;
		p->AddRef();
		p->m_pSkin = pSkinRes;
		p->m_eType = pItem->GetSelectorType();
		p->m_strId = pItem->GetId();
		p->m_lSavePos = pSkinRes->GetStyleRes()->GetItemPos(pItem);

		// ��ȡ���е����ԣ��������������ڻ�ԭ
		if (!p->SaveXmlAttribute(p->m_eType, p->m_strId.c_str(), &p->m_pAtributes))
		{
			delete p;
			return NULL;
		}

		return p;
	}
	virtual long execute()
	{
		__super::DeleteStyle(m_eType, m_strId.c_str());

		{
			String str(TEXT("Delete Style Item: "));
			str.append(m_strId.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}
		return 0;
	}
	virtual long undo()
	{
		IStyleResItem*  pItem = __super::InsertStyle(m_eType, m_strId.c_str(), m_lSavePos);
		if (!pItem)
			return 0;

		IUIElement* pElement = pItem->GetXmlElement();
		if (!pElement)
			return 0;

		pElement->SetAttribList2(m_pAtributes);

		{
			String str(TEXT("Add Style Item: "));
			str.append(m_strId.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}
		return 0;
	}

private:
	STYLE_SELECTOR_TYPE  m_eType;
	String  m_strId;
	long  m_lSavePos;

	IListAttribute*  m_pAtributes;
};


// �������
class ModifyStyleItemCommand : public StyleOpCommand
{
public:
	static ICommand*  CreateInstance(ISkinRes* pSkinRes, 
		STYLE_SELECTOR_TYPE type, LPCTSTR szId, 
		STYLE_SELECTOR_TYPE typeNew, LPCTSTR szIdNew, LPCTSTR szInheritNew)
	{
		if (!pSkinRes || !szId || !szIdNew)
			return NULL;

		IStyleResItem*  pItem = pSkinRes->GetStyleRes()->FindItem(type, szId);
		if (!pItem)
		{
			String  str = _T("ModifyStyle Id��Ч: ");
			str.append(szId);
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
			return NULL;
		}

		LPCTSTR szInherit = pItem->GetInherits();

		// �ж������޸�
		bool bSameTypeAndId = false;
		if (type == typeNew)
		{
			if (0 == _tcscmp(szId, szIdNew))
			{
				bSameTypeAndId = true;

				if (szInherit == szInheritNew)
					return NULL;

				if (szInheritNew && szInherit && (0 == _tcscmp(szId, szInheritNew)))
					return NULL;
			}
		}

		// �ж������ظ�
		if (!bSameTypeAndId)
		{
			if (pSkinRes->GetStyleRes()->FindItem(typeNew, szIdNew))
			{
				String  str = _T("ModifyStyle Id new�ظ�: ");
				str.append(szIdNew);
				g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
				return NULL;
			}
		}

		ModifyStyleItemCommand* p = new ModifyStyleItemCommand;
		p->AddRef();

		p->m_pSkin = pSkinRes;
		p->m_bSameTypeAndId = bSameTypeAndId;
		p->m_eType = type;
		p->m_strId = szId;
		if (szInherit)
			p->m_strInherit = szInherit;

		p->m_eTypeNew = typeNew;
		p->m_strIdNew = szIdNew;
		if (szInheritNew)
			p->m_strInheritNew = szInheritNew;
		return p;
	}
	virtual long execute()
	{
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		IStyleResItem* pItem = pStyleRes->FindItem(m_eType, m_strId.c_str());
		IUIElement* pElement = pItem->GetXmlElement();

		pElement->RemoveAttrib(XML_ID);
		switch (m_eTypeNew)
		{
		case STYLE_SELECTOR_TYPE_ID:
			pElement->SetTagName(XML_STYLE_SELECTOR_TYPE_ID);
			pElement->AddAttrib(XML_ID, m_strIdNew.c_str());
			break;

		case STYLE_SELECTOR_TYPE_CLASS:
			pElement->SetTagName(XML_STYLE_SELECTOR_TYPE_CLASS);
			pElement->AddAttrib(XML_ID, m_strIdNew.c_str());
			break;

		case STYLE_SELECTOR_TYPE_TAG:
			pElement->SetTagName(m_strIdNew.c_str());
			break;
		}
		if (m_strInheritNew.length())
			pElement->AddAttrib(XML_STYLE_INHERIT, m_strInheritNew.c_str());
		else
			pElement->RemoveAttrib(XML_STYLE_INHERIT);
	
		pStyleRes->Modify(pItem, m_eTypeNew, m_strIdNew.c_str(), m_strInheritNew.c_str());
		ShowStatusText(m_strIdNew.c_str());

		SetDirty(pElement);
		return 0;
	}
	virtual long undo()
	{
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		IStyleResItem* pItem = pStyleRes->FindItem(m_eTypeNew, m_strIdNew.c_str());
		IUIElement* pElement = pItem->GetXmlElement();

		pElement->RemoveAttrib(XML_ID);
		switch (m_eType)
		{
		case STYLE_SELECTOR_TYPE_ID:
			pElement->SetTagName(XML_STYLE_SELECTOR_TYPE_ID);
			pElement->AddAttrib(XML_ID, m_strId.c_str());
			break;

		case STYLE_SELECTOR_TYPE_CLASS:
			pElement->SetTagName(XML_STYLE_SELECTOR_TYPE_CLASS);
			pElement->AddAttrib(XML_ID, m_strId.c_str());
			break;

		case STYLE_SELECTOR_TYPE_TAG:
			pElement->SetTagName(m_strId.c_str());
			break;
		}
		if (m_strInherit.length())
			pElement->AddAttrib(XML_STYLE_INHERIT, m_strInherit.c_str());
		else
			pElement->RemoveAttrib(XML_STYLE_INHERIT);

		pStyleRes->Modify(pItem, m_eType, m_strId.c_str(), m_strInherit.c_str());
		ShowStatusText(m_strId.c_str());
		SetDirty(pElement);
		return 0;
	}

	void  ShowStatusText(LPCTSTR szId)
	{
		String str(TEXT("Modify Style Item: "));
		if (szId)
			str.append(szId);
		g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
	}

private:
	STYLE_SELECTOR_TYPE  m_eType;
	String  m_strId;
	String  m_strInherit;

	STYLE_SELECTOR_TYPE  m_eTypeNew;
	String  m_strIdNew;
	String  m_strInheritNew;

	bool  m_bSameTypeAndId;
};


class AddStyleItemAttributeCommand : public StyleOpCommand
{
public:
	AddStyleItemAttributeCommand()
	{
	}
	~AddStyleItemAttributeCommand()
	{
	}
	static ICommand*  CreateInstance(ISkinRes* pSkinRes, IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue)
	{
		if (!pSkinRes || !pItem || !szKey)
			return NULL;

		AddStyleItemAttributeCommand* p = new AddStyleItemAttributeCommand;
		p->AddRef();
		p->m_pSkin = pSkinRes;
		p->m_eType = pItem->GetSelectorType();
		p->m_strId = pItem->GetId();

		p->m_strKey = szKey;
		if (szValue)
			p->m_strValue = szValue;

		return p;
	}

	virtual long execute()
	{
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		IStyleResItem*  pStyleItem = pStyleRes->FindItem(m_eType, m_strId.c_str());
		if (!pStyleItem)
			return E_FAIL;

		IUIElement*  pXmlElement = pStyleItem->GetXmlElement();
		if (!pXmlElement)
			return E_FAIL;

		pStyleRes->AddAttribute(pStyleItem, m_strKey.c_str(), m_strValue.c_str());
		pXmlElement->AddAttrib(m_strKey.c_str(), m_strValue.c_str());

		{
			String str(TEXT("Add Style Attribute: "));
			str.append(m_strKey.c_str());
			str.append(TEXT(" = "));
			str.append(m_strValue.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}

		SetDirty(pXmlElement);
		return 0;
	}


	virtual long undo()
	{
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		IStyleResItem*  pStyleItem = pStyleRes->FindItem(m_eType, m_strId.c_str());
		if (!pStyleItem)
			return E_FAIL;

		IUIElement*  pXmlElement = pStyleItem->GetXmlElement();
		if (!pXmlElement)
			return E_FAIL;

		pStyleRes->RemoveAttribute(pStyleItem, m_strKey.c_str());
		pXmlElement->RemoveAttrib(m_strKey.c_str());

		{
			String str(TEXT("Remove Style Attribute: "));
			str.append(m_strKey.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}
		SetDirty(pXmlElement);
		return 0;
	}

private:
	STYLE_SELECTOR_TYPE  m_eType;
	String  m_strId;

	String  m_strKey;
	String  m_strValue;
};


class ModifyStyleItemAttributeCommand : public StyleOpCommand
{
public:
	static ICommand*  CreateInstance(ISkinRes* pSkinRes, IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue)
	{
		if (!pSkinRes || !pItem || !szKey)
			return NULL;

		ModifyStyleItemAttributeCommand* p = new ModifyStyleItemAttributeCommand;
		p->AddRef();
		p->m_pSkin = pSkinRes;
		p->m_eType = pItem->GetSelectorType();
		p->m_strId = pItem->GetId();

		p->m_strKey = szKey;

		LPCTSTR szOldValue = pItem->GetAttribute(szKey);
		if (szOldValue)
			p->m_strOldValue = szOldValue;
		if (szValue)
			p->m_strNewValue = szValue;

		return p;
	}

	long  Modify(LPCTSTR szValue)
	{
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		IStyleResItem*  pStyleItem = pStyleRes->FindItem(m_eType, m_strId.c_str());
		if (!pStyleItem)
			return E_FAIL;

		IUIElement*  pXmlElement = pStyleItem->GetXmlElement();
		if (!pXmlElement)
			return E_FAIL;

		pStyleRes->ModifyAttribute(pStyleItem, m_strKey.c_str(), szValue);
		pXmlElement->SetAttrib(m_strKey.c_str(), szValue);
		ShowStatusText(m_strKey.c_str(), szValue);

		SetDirty(pXmlElement);
		return 0;
	}

	void  ShowStatusText(LPCTSTR szKey, LPCTSTR szValue)
	{
		String str(TEXT("Modify Style Attribute: "));
		if (szKey)
			str.append(szKey);
		str.append(TEXT(" = "));
		if (szValue)
			str.append(szValue);
		g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
	}

	virtual long execute()
	{
		return Modify(m_strNewValue.c_str());
	}

	virtual long undo()
	{
		return Modify(m_strOldValue.c_str());
	}


private:
	STYLE_SELECTOR_TYPE  m_eType;
	String  m_strId;

	String  m_strKey;
	String  m_strOldValue;
	String  m_strNewValue;
};


class DeleteStyleItemAttributeCommand : public StyleOpCommand
{
public:
	static ICommand*  CreateInstance(ISkinRes* pSkinRes, IStyleResItem* pItem, LPCTSTR szKey)
	{
		if (!pSkinRes || !pItem || !szKey)
			return NULL;

		if (0 == _tcscmp(szKey, XML_ID))
		{
			g_pGlobalData->SetStatusText2(TEXT("����ɾ�� id ����"));
			return NULL;
		}

		IUIElement*  pXmlElement = pItem->GetXmlElement();
		if (!pXmlElement)
			return NULL;

		if (!pXmlElement->HasAttrib(szKey))
			return NULL;

		// TODO: ���ʵ�ֱ������Ե�λ�ã���undoʱ�ܹ���ԭλ�û�ԭ??

		DeleteStyleItemAttributeCommand* p = new DeleteStyleItemAttributeCommand;
		p->AddRef();
		p->m_pSkin = pSkinRes;
		p->m_eType = pItem->GetSelectorType();
		p->m_strId = pItem->GetId();
		p->m_strKey = szKey;

		LPCTSTR szValue = pItem->GetAttribute(szKey);
		if (szValue)
			p->m_strValue = szValue;

			
		return p;
	}


	virtual long execute()
	{
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		IStyleResItem*  pStyleItem = pStyleRes->FindItem(m_eType, m_strId.c_str());
		if (!pStyleItem)
			return E_FAIL;

		IUIElement*  pXmlElement = pStyleItem->GetXmlElement();
		if (!pXmlElement)
			return E_FAIL;

		pStyleRes->RemoveAttribute(pStyleItem, m_strKey.c_str());
		pXmlElement->RemoveAttrib(m_strKey.c_str());
		
		{
			String str(TEXT("Delete Style Attribute: "));
			str.append(m_strKey.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}

		SetDirty(pXmlElement);

		return 0;
	}

	virtual long undo()
	{
		IStyleRes*  pStyleRes = m_pSkin->GetStyleRes();
		IStyleResItem*  pStyleItem = pStyleRes->FindItem(m_eType, m_strId.c_str());
		if (!pStyleItem)
			return E_FAIL;

		IUIElement*  pXmlElement = pStyleItem->GetXmlElement();
		if (!pXmlElement)
			return E_FAIL;

		pStyleRes->AddAttribute(pStyleItem, m_strKey.c_str(), m_strValue.c_str());
		pXmlElement->AddAttrib(m_strKey.c_str(), m_strValue.c_str());

		{
			String str(TEXT("Add Style Attribute: "));
			str.append(m_strKey.c_str());
			str.append(TEXT(" = "));
			str.append(m_strValue.c_str());
			g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());	
		}

		SetDirty(pXmlElement);

		return 0;
	}

private:
	STYLE_SELECTOR_TYPE  m_eType;
	String  m_strId;
	String  m_strKey;
	String  m_strValue;
};
