// SimpleDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SimpleDemo.h"

#include "UI/UISDK/Inc/inc.h"
#include "UI/UISDK/Inc/Util/util.h"
#pragma comment(lib, "UISDK.lib")

#include "UI/UICTRL/Inc/inc.h"
#pragma comment(lib, "UICTRL.lib")

#include "UI/UICTRL/Inc/Interface/ibutton.h"
#include "UI/UISDK/Inc/Interface/icustomwindow.h"
#include "UI/UICTRL/Inc/Interface/ilabel.h"
#include "UI/UICTRL/Inc/Interface/imenu.h"
#include "UI/UICTRL/Inc/Interface/iedit.h"
#include <atlcomcli.h>
#include "UI/UICTRL/Inc/Interface/ilistbox.h"
#include "UI/UICTRL/Inc/Interface/icombobox.h"
#include "UI/UICTRL/Inc/Interface/irichtext.h"

// #include "UI/UICef/Inc/inc.h"
// #include "UI/UICef/Inc/icefctrl.h"
// #pragma comment(lib, "UICef.lib")


#define PI 3.141592653f
float __stdcall TimingFuction(float f)
{
    return sin(PI * f);
}

class LoginWindow : public UI::IWindowDelegate, public UIA::IAnimateEventCallback
{
public:
	LoginWindow()
	{
		m_pWindow = nullptr;
	}

	void  Create(UI::ISkinRes* pSkinRes)
	{
		m_pWindow = UI::ICustomWindow::CreateInstance(pSkinRes);
		m_pWindow->SetWindowMessageCallback(this);

		m_pWindow->Create(L"LoginWnd");
		m_pWindow->ShowWindow();
	}

protected:
	virtual BOOL  OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult)
	{
		if (msg == WM_DESTROY)
		{
			PostQuitMessage(0);
		}
		else if (msg == WM_MOUSEWHEEL)
		{
		}
		return FALSE;
	}
	virtual BOOL  OnWindowUIMessage(UI::UIMSG* pMsg) override
	{
		if (pMsg->message == WM_CONTEXTMENU)
		{
			UI::LoadMenuData data = { 0 };
			data.pSkinRes = m_pWindow->GetSkinRes();
			data.hWndParent = m_pWindow->GetHWND();
			UI::CMenuPtr pMenu = UILoadMenu(&data);

			pMenu->AppendString(L"AAAA", 1);
			pMenu->AppendSeparator(0);
			pMenu->AppendString(L"BBBB", 2);

			POINT pt;
			GetCursorPos(&pt);
			pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, 0);
		}
		return FALSE;
	}


    void onInsert()
    {
        UI::IRichText* pRichText = (UI::IRichText*)
            m_pWindow->FindObject(TEXT("richtext"));
        if (pRichText)
        {
            pRichText->ReplaceText(L"Insert");
        }
    }
    void  OnRichTextLinkClicked(UI::IRichText*, UI::RT::ILink* pLink)
    {
        LPCTSTR szText = pLink->GetId();
        OutputDebugString(szText);
        OutputDebugString(L"\n");
        ShellExecute(nullptr, L"open", pLink->GetReference(), 0, 0, SW_SHOW);
    }
    virtual void  OnWindowInitialize()
	{
        UI::IButton* pBtn = (UI::IButton*)m_pWindow->FindObject(TEXT("btnInsert"));
        if (pBtn)
            pBtn->ClickedEvent().connect0(this, &LoginWindow::onInsert);
        
        UI::IRichText* pRichText = (UI::IRichText*)
            m_pWindow->FindObject(TEXT("richtext"));
        if (pRichText)
        {
            pRichText->LickClickedEvent().connect(this, &LoginWindow::OnRichTextLinkClicked);
            pRichText->SetPageMode(UI::PageMode::multiline);
            //pRichText->AppendText(TEXT("1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\nBCDE"));
     
//             {
// 
//                 UI::RT::IChatMessage* msg = pRichText->BeginInsertMessage();
//                 msg->SetAlign(UI::RT::HALIGN::left);
// 
//                 pRichText->ReplaceText(L"[路透社记者]:");
// 
//                 HANDLE bubble = msg->EnterBubble();
//                 pRichText->ReplaceText(L"近期中国股市和汇市的波动引起了国际投资者的高度关注。请问总理，您认为中国的金融市场目前面临哪些主要问题和挑战？中国政府对金融市场未来的发展和加强监管有什么计划？股市汇市和债券市场将会有哪些重点改革措施？近期的市场波动会不会影响改革的进度？深港通会不会年内推出？谢谢。");
//                 msg->LeaveBubble(bubble);
// 
//                 pRichText->EndInsertMessage(msg);
//             }
#if 1
            UI::RT::IParagraph* para = nullptr;
            
            pRichText->ReplaceText(L"李克强总理答中外记者问(实录)");
            para = pRichText->GetSelectionAnchorParagraph();
            UIASSERT(para);
            {
                UI::CharFormat cf;
                pRichText->GetDefaultCharFormat(&cf);

                cf.bBold = true;
                cf.lFontSize = 14;
                para->SetCharformat(&cf);

                para->SetAlign(UI::RT::HALIGN::center);
                para->SetBottomSpace(10);
                pRichText->ReplaceText(L"\n");
            }

            // link
            {
                
                UI::RT::ILink* pLink = pRichText->ReplaceLink(
                    L"http://news.qq.com/a/20160316/015694.htm");
                if (pLink)
                {
                    pLink->SetId(L"AAA");
                    pLink->SetReference(L"http://news.qq.com/a/20160316/015694.htm");
                    pLink->SetToolTip(L"李克强总理答中外记者问");
                }
                para = pRichText->GetSelectionAnchorParagraph();
                para->SetBottomSpace(10);

                pRichText->ReplaceText(L"\n");
            }

            // image
            {
                para = pRichText->GetSelectionAnchorParagraph();
                UIASSERT(para);
                {
                    para->SetAlign(UI::RT::HALIGN::center);
                    para->SetBottomSpace(10);
                }

                TCHAR szPath[MAX_PATH] = { 0 };
                UI::Util::PathInBin(L"Sample\\132451642.jpg", szPath);
                pRichText->AppendImage(szPath);
                pRichText->ReplaceText(L"\n国务院总理李克强回答中外记者问。人民网记者 于凯 摄");
                para = pRichText->GetSelectionAnchorParagraph();
                UIASSERT(para);
                {
                    para->SetAlign(UI::RT::HALIGN::center);
                    para->SetBottomSpace(20);
                }
                pRichText->ReplaceText(L"\n");
            }

            if (1)
            {
                pRichText->ReplaceText(L"3月16日上午十二届全国人大四次会议闭幕后，国务院总理李克强在人民大会堂三楼金色大厅会见采访十二届全国人大四次会议的中外记者并回答记者提出的问题。\r\n");
            }
            if (1)
            {
                UI::RT::IChatMessage* msg = pRichText->BeginInsertMessage();
                pRichText->ReplaceText(L"[傅莹]:");
                HANDLE bubble = msg->EnterBubble();
                pRichText->ReplaceText(L"大家好，今天我们很荣幸地邀请到李克强总理与中外记者见面。今天到场的记者还是比较多的，所以请每位被点到的记者每次就提一个问题，这样大家的机会可以更多一些。现在请李克强总理讲几句话。");
                msg->LeaveBubble(bubble);
                pRichText->EndInsertMessage(msg);
            }

            {
                
                UI::RT::IChatMessage* msg = pRichText->BeginInsertMessage();
                msg->SetAlign(UI::RT::HALIGN::right);
                pRichText->ReplaceText(L"[李克强]:");

                HANDLE bubble = msg->EnterBubble();
                pRichText->ReplaceText(L"欢迎大家来参加记者会，感谢你们对中国两会的关注和付出的辛劳。刚才主持人说了，时间有限，就单刀直入吧。");
                msg->LeaveBubble(bubble);
                pRichText->EndInsertMessage(msg);
            }

            
            {
                UI::RT::IChatMessage* msg = pRichText->BeginInsertMessage();
                msg->SetAlign(UI::RT::HALIGN::left);

                pRichText->ReplaceText(L"[路透社记者]:");
                
                HANDLE bubble = msg->EnterBubble();
                pRichText->ReplaceText(L"近期中国股市和汇市的波动引起了国际投资者的高度关注。请问总理，您认为中国的金融市场目前面临哪些主要问题和挑战？中国政府对金融市场未来的发展和加强监管有什么计划？股市汇市和债券市场将会有哪些重点改革措施？近期的市场波动会不会影响改革的进度？深港通会不会年内推出？谢谢。");
                msg->LeaveBubble(bubble);

                pRichText->EndInsertMessage(msg);
            }
            
            {
                UI::RT::IChatMessage* msg = pRichText->BeginInsertMessage();
                msg->SetAlign(UI::RT::HALIGN::right);

                pRichText->ReplaceText(L"[李克强]:");

                HANDLE bubble = msg->EnterBubble();
                pRichText->ReplaceText(
                   L"请你问第一个问题，你就把股市汇市等金融市场问题当“当头炮”，不过也可以理解。因为许多金融问题的表现往往早于经济问题的发生。但是金融首要任务还是要支持实体经济的发展，实体经济不发展，是金融最大的风险。去年我们采取了一系列像降息、降准、定向降准等措施，这不是量化宽松，我们始终注意把握货币供应量的松紧适度，主要还是为了降低实体经济融资的成本。所以金融机构还是要着力去支持实体经济，特别是小微企业的健康发展，绝不能脱实向虚。\n"
                   L"当然金融也有其自身的规律，要防范风险。我更关注的是金融机构本身，去年由于一些行业、企业经营困难，金融机构不良贷款比例是在上升的。但是我们有抵御风险的能力，因为商业银行的资本充足率超过了13%，高于国际标准，拨备覆盖率达到180%以上，高于我们定的150%的标准。而且我们还可以利用市场化的手段降低企业的债务率。企业债务率高是老问题了，因为中国是间接融资为主，但是我们的居民储蓄率也比较高。即便如此，不管市场发生怎样的波动，我们还是要坚定不移地发展多层次的资本市场，而且也可以通过市场化债转股的方式来逐步降低企业的杠杆率。\n"
                   L"去年由于多重因素的原因，中国股票市场发生了异常波动，有关方面采取综合性稳定市场的举措，实际上是要防范发生系统性的金融风险，这一点是做到了的。下一步怎么办？前两天我们新上任的证监会主席关于具体问题已经作了阐述，因为时间问题我不展开了。不论是股市、债市、汇市这些金融市场，本质上是市场，还是要推进市场化、法治化的改革。当然，政府有监管的责任，现在看，随着形势的变化，需要改革和完善我们的金融监管体系，要实现全覆盖，因为现在金融创新的产品很多，不能留下监管空白；要增强协调性，因为金融市场产品之间关联度比较高，协调要有权威，还要做到权责一致。中央有关部门和地方要分层负责，发现问题要及时处置，防止苗头性的问题蔓延，当然也不能容忍道德风险。总之，还是要瞪大眼睛，练就一双加强监管的“火眼金睛”。\n"
                   L"借这个机会我还要强调一点，完善和改革金融监管制度是一个过程，当前各有关部门和地方还是要按照已定的职能履行职责，守土有责，绝不能有任何的松懈，而且还要总结经验和教训，这也是保护好金融消费者和投资者的合法权益，否则可就要拿你是问了。谢谢！\n"
                   );
                msg->LeaveBubble(bubble);
                pRichText->EndInsertMessage(msg);
            }
#endif
        }
	}

    virtual UIA::E_ANIMATE_TICK_RESULT  OnAnimateTick(
                    UIA::IStoryboard* p) override
    {
        int x = p->GetTimeline(0)->GetCurrentIntValue();
        int y = p->GetTimeline(1)->GetCurrentIntValue();
        UI::IPanel* pPanel = (UI::IPanel*)m_pWindow->FindObject(TEXT("test"));
        pPanel->SetObjectPos(x, y, 0, 0, SWP_NOSIZE);

        return UIA::ANIMATE_TICK_RESULT_CONTINUE;
    }

    void on_listitem_btn_click(/*UI::IButton* pBtn*/)
    {
    }

	void on_click(/*UI::IButton**/)
	{
        UI::IGifCtrl* pGif = (UI::IGifCtrl*)m_pWindow->FindObject(TEXT("gif"));
        if (pGif)
        {
            pGif->SetVisible(!pGif->IsSelfVisible());
        }
	}

private:
	UI::ICustomWindow*  m_pWindow;
	UI::bindable_int  m_nCount;
	UI::bindable_string  m_strName;
};

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UI::IUIApplication* pUIApp = NULL;
	UI::CreateUIApplication(&pUIApp);
	UICTRL_RegisterUIObject(pUIApp);
//    UICef_RegisterUIObject(pUIApp);

	// 顶级资源
	TCHAR szResPath[MAX_PATH] = { 0 };
	UI::Util::PathInBin(L"Skin\\Root", szResPath);
	UI::ISkinRes* pRootSkinRes = pUIApp->LoadSkinRes(szResPath);

	// 登录插件资源
	UI::Util::PathInBin(L"Skin\\Plugin\\Login", szResPath);
	UI::ISkinRes* pLoginSkinRes = pUIApp->LoadSkinRes(szResPath);

	// 绑定
	pLoginSkinRes->SetParentSkinRes(pRootSkinRes);

	LoginWindow m;
	m.Create(pLoginSkinRes);
	

// 	MainWindow m2;
// 	m2.Create(pSkinRes2);

	pUIApp->MsgHandleLoop();
	pUIApp->Release();
	
	return 0;
}