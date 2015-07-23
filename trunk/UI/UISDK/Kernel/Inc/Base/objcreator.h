#ifndef OBJCREATOR_H_6A6E5AFC_D1B4_4e7b_8C57_4F8ECD9296B0
#define OBJCREATOR_H_6A6E5AFC_D1B4_4e7b_8C57_4F8ECD9296B0
#include "UISDK\Kernel\Inc\Base\leakdetect.h"  // 这个头文件中需要创建uiobject，因此添加leakdetect检测

//////////////////////////////////////////////////////////////////////////
//
//    对象创建者，为了支持在对象创建和销毁的时候能调用虚函数，要求所有对象
//    调用UICreateInstance进行创建，将初始化代码和释放代码放在FinalConstruct
//    /FinalRelease里面
//
//////////////////////////////////////////////////////////////////////////
namespace UI
{
interface IUIApplication;
interface ISkinRes;

template<class T>
class UIObjCreator : public T
{
public:
    virtual ~UIObjCreator()
    {
        UISendMessage(this, UI_WM_FINALRELEASE);
    }
public:

	// 每个UI Object 的创建函数，将会被如下方式调用：
	// (s_UICreateInstancePtr)className::_CreatorClass::UICreateInstance
	static HRESULT UICreateInstanceBySkinRes(ISkinRes* pSkinRes, T** pOut)
    {
		UIASSERT(pSkinRes);
		return UICreateInstance(pSkinRes->GetUIApplication(), pSkinRes, pOut);
	}

	// 2015.7.18，增加ISkinRes参数。每个对象需要再指定自己属于哪个皮肤包，用于支持多皮肤包模式（插件模式）
	static HRESULT UICreateInstance(IUIApplication* pUIApp, ISkinRes* pSkinRes, T** pOut)
	{
        UIASSERT(pUIApp);

        UIObjCreator<T>* p = NULL; 
        HRESULT hr = S_OK;
        bool bRet = false;

        do 
        {
            // 在这里添加一个IUIApplication的参数是为了让IControlImpl能够在构造函数中拿到pApp去CreateInstance子控件
            p = new UIObjCreator<T>;
            p->CreateImpl();

            if (FAILED(UISendMessage(p, UI_WM_FINALCONSTRUCT, (WPARAM)pUIApp, (LPARAM)pSkinRes)))
                break;

            *pOut = static_cast<T*>(p);
            bRet = true;
        } while (0);

        if (!bRet)
        {
            delete p;
            return hr;
        }

        return hr;
    }

	// 用于renderbase的创建
    static HRESULT UICreateInstance2(T** ppOut)
    {
        if (NULL == ppOut)
            return E_INVALIDARG;

        UIObjCreator<T>* p = new UIObjCreator<T>;
        p->AddRef();
        p->CreateImpl();

        *ppOut = static_cast<T*>(p);
        return S_OK;
    };

protected:
    // 将delete放在这里是为了与new对称，避免new,delete在不同的模块中
    void  do_delete_this()
    {
        delete this;
    }
};

}
#endif // OBJCREATOR_H_6A6E5AFC_D1B4_4e7b_8C57_4F8ECD9296B0