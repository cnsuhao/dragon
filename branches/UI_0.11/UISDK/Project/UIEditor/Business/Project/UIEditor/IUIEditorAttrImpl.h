#pragma once
#include "UISDK\Kernel\Inc\Interface\iuieditor.h"

#if 0
template<class T>
class IAttributeImpl : public T
{
public:
	IAttributeImpl()
	{
	}
	virtual ~IAttributeImpl()
	{

	}
	virtual void  Release()
	{
		delete this;
	}
	
	virtual const TCHAR* GetName()
	{
		return m_strName.c_str();
	}
	virtual const TCHAR* GetDesc()
	{
		return m_strDesc.c_str();
	}

	virtual void  SetName(const TCHAR* szText)
	{
		if (szText)
			m_strName = szText;
		else
			m_strName.clear();
	}
	virtual void  SetDesc(const TCHAR* szText)
	{
		if (szText)
			m_strDesc = szText;
		else
			m_strDesc.clear();
	}

	virtual void Insert2PropertyCtrl(IPropertyCtrl* p)
	{

	}

protected:
	String   m_strName;
	String   m_strDesc;
	String   m_strDefValue;
};

class CGroupAttribute : public IAttributeImpl<UI::IUIEditorGroupAttribute>
{
public:
	CGroupAttribute();
	~CGroupAttribute();

public:
	virtual void  AddAttribute(IUIEditorAttrBase* p);
	virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);

public:
	list<IUIEditorAttrBase*>  m_AttrList;
	bool  m_bRoot;
};

template <class T>
class IUIEditorAttributeImpl : public IAttributeImpl<T>
{
public:
	virtual void  SetDefaultValue(const TCHAR* szText)
	{
		if (szText)
			m_strDefValue = szText;
		else
			m_strDefValue.clear();
	}

    virtual void  SetKey(const TCHAR* szText)
    {
        if (szText)
            m_strKey = szText;
        else
            m_strKey.clear();
    }

protected:
    String  m_strKey;
};

class CTextAttribute : public IUIEditorAttributeImpl<IUIEditorTextAttribute>
{
public:
    virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
};

class CBoolAttribute : public IUIEditorAttributeImpl<IUIEditorBoolAttribute>
{
public:
	virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
};


struct ComboBoxOptionItem
{
    String  strText;
    String  strValue;
};
class CComboBoxAttribute : public IUIEditorAttributeImpl<IUIEditorComboBoxAttribute>
{
public:
    CComboBoxAttribute();
    ~CComboBoxAttribute();

    virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
    virtual IUIEditorComboBoxAttribute*  AddOption(const TCHAR* szText, const TCHAR* szValue = NULL);
    virtual void  SetReloadOnChanged(bool b);
    virtual bool  GetReloadOnChanged();

protected:
    vector<ComboBoxOptionItem*>  m_vecItems;

    // ��ѡ��ı�ʱ�����¼��ض���������б������޸�bkgnd.render.type֮�󣬶��������ѡ����Ҫ���¼���
    // ��ǰΪ�˼������ֱ�ӽ��ö�������������б�����գ������¼���һ�Ρ�
    bool   m_bReloadOnChanged;  
};


class CButtonAttribute : public IUIEditorAttributeImpl<IUIEditorButtonAttribute>
{
public:
	virtual void  Insert2PropertyCtrl(IPropertyCtrl* pCtrl, IListItemBase* pParent);
};
#endif