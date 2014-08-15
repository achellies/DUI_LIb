#include "UIlib.h"

#include <windows.h>
#include <objbase.h>

#pragma comment(lib, "DirectUI.lib")


#include <exdisp.h>
#include <comdef.h>
#include "UserList.h"

#pragma warning(disable : 4996)

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
    CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager) 
    {
        if( _tcsicmp(pstrClass, _T("gamelist")) == 0 ) return new GameListUI;
        return NULL;
    }
};


class IMMainForm : public CWindowWnd, public INotifyUI
{
public:
	IMMainForm() { };
	LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void Init() {
		m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	}

	void OnPrepare()
	{ 
		GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("gamelist")));
		if (pGameList == NULL)
			return;

        GameListUI::Node* pCategoryNode = NULL;
        GameListUI::Node* pGameNode = NULL;
        GameListUI::Node* pServerNode = NULL;
        GameListUI::Node* pRoomNode = NULL;
		pCategoryNode = pGameList->AddNode(_T("{x 4}{i icon_c.png -1}{f 0}好友{/f}"));
        for( int i = 0; i < 4; ++i )
        {
            for( int i = 0; i < 3; ++i )
            {
				pGameNode = pGameList->AddNode(_T("{x 4}{i icon_c.png -1}{x 2}{f 0}tojen{/f}"), pCategoryNode);
            }
        }
		pCategoryNode = pGameList->AddNode(_T("{x 4}{i icon_c.png -1}{f 0}客户{/f}"));
        for( int i = 0; i < 4; ++i )
        {
            for( int i = 0; i < 3; ++i )
            {
				pGameNode = pGameList->AddNode(_T("{x 4}{i icon_c.png -1}{x 2}{f 0}tojen{/f}"), pCategoryNode);
            }
        }
		pCategoryNode = pGameList->AddNode(_T("{x 4}{i icon_a.png -1}{f 0}其他{/f}"));
        for( int i = 0; i < 4; ++i )
        {
            for( int i = 0; i < 3; ++i )
            {
				pGameNode = pGameList->AddNode(_T("{x 4}{i icon_c.png -1}{x 2}{f 0}tojen{/f}"), pCategoryNode);
            }
        }
        
	}

	LPCTSTR GetXMLSkin() const
	{
		return _T("skin.xml");
	}

    CControlUI* CreateControl(LPCTSTR pstrClass) 
    {
        if( _tcsicmp(pstrClass, _T("GameList")) == 0 ) return new GameListUI;
      
        return NULL;
    }

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) OnPrepare();
		else if( msg.sType == _T("click") ) {
			if( msg.pSender == m_pCloseBtn ) {
				PostQuitMessage(0);
				return; 
			}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
			else if( msg.pSender == m_pMinBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
			else if( msg.pSender == m_pMaxBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; }
			else if( msg.pSender == m_pRestoreBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; }
#else
			else if( msg.pSender == m_pMinBtn ) { 
				::ShowWindow(*this, SW_MINIMIZE); return;
			}
			else if( msg.pSender == m_pMaxBtn ) { 
				::ShowWindow(*this, SW_MAXIMIZE);
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
				return;
			}
			else if( msg.pSender == m_pRestoreBtn ) { 
				::ShowWindow(*this, SW_RESTORE);

				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
				return;
			}
#endif
		}
		else if(msg.sType==_T("setfocus"))
		{
			CStdString name = msg.pSender->GetName();
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if(name==_T("friend"))
				 pControl->SelectItem(0);
			else if(name==_T("group"))
				 pControl->SelectItem(1);
			else if(name==_T("other"))
				pControl->SelectItem(2);
			
		}
		else if( msg.sType == _T("itemclick") ) {
            GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("gamelist")));
            if( pGameList->GetItemIndex(msg.pSender) != -1 )
            {
                if( _tcsicmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
                    GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();

                    POINT pt = { 0 };
                    ::GetCursorPos(&pt);
                    ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
                    pt.x -= msg.pSender->GetX();
                    SIZE sz = pGameList->GetExpanderSizeX(node);
                    if( pt.x >= sz.cx && pt.x < sz.cy )                     
                        pGameList->SetChildVisible(node, !node->data()._child_visible);
                }
            }
        }
        else if( msg.sType == _T("itemactivate") ) {
            GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("gamelist")));
            if( pGameList->GetItemIndex(msg.pSender) != -1 )
            {
                if( _tcsicmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
                    GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();
                    pGameList->SetChildVisible(node, !node->data()._child_visible);
                    if( node->data()._level == 3 ) {
                        COptionUI* pControl = static_cast<COptionUI*>(m_pm.FindControl(_T("roomswitch")));
                        if( pControl ) {
                            pControl->SetVisible(true);
                            pControl->Activate();
                        }
                    }
                }
            }

        }
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);

		bHandled = FALSE;
		return 0;
	}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

// 		if( !::IsZoomed(*this) ) {
// 			RECT rcSizeBox = m_pm.GetSizeBox();
// 			if( pt.y < rcClient.top + rcSizeBox.top ) {
// 				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
// 				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
// 				return HTTOP;
// 			}
// 			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
// 				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
// 				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
// 				return HTBOTTOM;
// 			}
// 			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
// 			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
// 		}

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
				if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}
#endif
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			RECT rcClient;
			::GetClientRect(*this, &rcClient);
			RECT rc = { rcClient.left, rcClient.top + szRoundCorner.cx, rcClient.right, rcClient.bottom };
			HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
			HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
			::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
			::SetWindowRgn(*this, hRgn1, TRUE);
			::DeleteObject(hRgn1);
			::DeleteObject(hRgn2);
		}
#endif
		bHandled = FALSE;
		return 0;
	}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-rcWork.left, -rcWork.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;
		lpMMI->ptMaxSize.x		= rcWork.right;
		lpMMI->ptMaxSize.y		= rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}
#endif
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ) {
			if( !bZoomed ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
#else
		LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
#endif
		return lRes;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;	
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;	
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		default:
		bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
        styleValue &= ~WS_CAPTION;
        ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        RECT rcClient;
        ::GetClientRect(*this, &rcClient);
        ::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
            rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

        m_pm.Init(m_hWnd);
        CDialogBuilder builder;
        CDialogBuilderCallbackEx cb;

		tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("skins\\IM\\");
		m_pm.SetResourcePath(tstrSkin.c_str());

		tstrSkin = m_pm.GetResourcePath() + _T("skin.xml");
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, &cb, &m_pm);
        ASSERT(pRoot && _T("Failed to parse XML"));
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);

        SIZE szInitWindowSize = m_pm.GetInitSize();
        if( szInitWindowSize.cx != 0 ) {
            ::SetWindowPos(*this, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);
        }

        Init();
        return 0;
    }

private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	//...

public:
    CPaintManagerUI m_pm;
};


#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow)
#endif
{
	CPaintManagerUI::SetInstance(hInstance);

#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	HRESULT Hr = ::CoInitialize(NULL);
#else
	HRESULT Hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	if( FAILED(Hr) ) return 0;

	IMMainForm* pFrame = new IMMainForm();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("IM"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 269, 590);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::ShutDown();

	::CoUninitialize();
	return 0;
}