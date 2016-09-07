#pragma once
#include "Inc\Interface\iattribute.h"
#include "Src\Resource\skinres.h"

namespace UI
{
    class AttributeSerializer;
    class AttributeEditorProxy;
	class UIApplication;

    // 属性基类
    class AttributeBase
    {
    public:
        AttributeBase()
        {
            /*m_pSerializer = NULL;*/
            m_pUIApplication = NULL;
            m_bData = false;
            m_bParentKeyIsNull = true;
			m_bReloadOnChanged = false;
        }

        virtual ~AttributeBase() {};
        virtual void  Release() { delete this; };

        virtual LPCTSTR  Get() PURE; 
        virtual void  Set(LPCTSTR) PURE;
        // reload时，如果在xml中没有显式配置该属性，则调用Reset，而不是Set
		virtual void  Reset() PURE;   
        virtual void  Editor(AttributeEditorProxy* p, EditorAttributeFlag e) PURE;
        virtual bool  IsDefaultValue() PURE;

        virtual void  SetBindValue(void*) PURE;
        virtual void  SetBindFuction(void* _this, void* _setter, void* _getter) PURE;

    public:
        void  SetKey(LPCTSTR szKey)
        {
            SETSTRING(m_strKey, szKey);
        }
        LPCTSTR  GetKey()
        {
            return m_strKey.c_str();
        }
		AttributeBase*  SetCompatibleKey(LPCTSTR szKey)
		{
			SETSTRING(m_strKeyCompatible, szKey);
			return this;
		}
		LPCTSTR  GetCompatibleKey()
		{
			return m_strKeyCompatible.c_str();
		}
		bool  IsKey(LPCTSTR szKey)
		{
			if (!szKey || !szKey[0])
				return false;
			if (m_strKey == szKey)
				return true;
			if (m_strKeyCompatible == szKey)
				return true;

			return false;
		}

        LPCTSTR  GetDesc()
        {
            return m_strDesc.c_str();
        }

        AttributeBase*  AsData()
        {
            m_bData = true; 
			return this;
        }
        bool  IsData() 
        {
            return m_bData; 
        }

        AttributeBase*  ReloadOnChanged()
        {
            m_bReloadOnChanged = true;
            return this;
        }
        bool  IsReloadOnChanged()
        {
            return m_bReloadOnChanged;
        }

		AttributeBase*  SetParentKey(LPCTSTR szParent)
		{
            if (szParent)
            {
                m_strParentKey = szParent;
                m_bParentKeyIsNull = false;
            }
            else
            {
                m_strParentKey.clear();
                m_bParentKeyIsNull = true;
            }
			return this;
		}
        LPCTSTR  GetParentKey()
        {
            if (m_bParentKeyIsNull)
                return NULL;
            return m_strParentKey.c_str();
        }

		void  SetGroupName(LPCTSTR szGroupName)
		{
			SETSTRING(m_strGroupName, szGroupName);
		}
		LPCTSTR  GetGroupName()
		{
			return m_strGroupName.c_str();
		}
        void  SetUIApplication(UIApplication* p)
        {
            m_pUIApplication = p;
        }
		void  SetSkinRes(SkinRes* p)
		{
			m_pSkinRes = p;
		}
        UIApplication*  GetUIApplication()
        {
            return m_pUIApplication;
        }

    protected:
        LPCTSTR  ConstructTempLPCTSTR(long lValue);

    private:
        String  m_strKey;
		String  m_strKeyCompatible; // 兼容key。例如：width="100"，也可以写成 layout.width="100"
		String  m_strDesc;
        String  m_strGroupName;  // 属性所属的对象名，例如"Object" "ImageRender"
        String  m_strParentKey;  // 父结点fullkey。例如bkg.render.type
        bool  m_bParentKeyIsNull;

        // 该属性是否是一个控件data。data在换肤的时候不用再加载一次
        bool  m_bData;  

        // 对于bkg.render.type属性修改时，在editor中重新加载这个控件的所有属性，来切换具体的render属性
        // 目前没想到一个较好的办法来只加载/删除相应的属性。
        bool  m_bReloadOnChanged;  

    protected:
        UIApplication*  m_pUIApplication;
		SkinRes*  m_pSkinRes;
    };
}
