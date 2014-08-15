#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "DUI_Callback.h"
#include "frameWnd.h"
#include "SysConfigWnd.h"
#include "BookStoreWnd.h"

void CFrameWnd::Init() {
	m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
	m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
}

void CFrameWnd::OnPrepare() { 
	CDirectUITree* pGameList = static_cast<CDirectUITree*>(m_pm.FindControl(_T("treedemo")));
	if (pGameList != NULL)
	{
		CDirectUITree::Node* pCategoryNode = NULL;
		CDirectUITree::Node* pGameNode = NULL;
		CDirectUITree::Node* pServerNode = NULL;
		CDirectUITree::Node* pRoomNode = NULL;
		pCategoryNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 2}{x 4}推荐游戏"));
		for( int i = 0; i < 4; ++i )
		{
			pGameNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 4}{x 4}四人斗地主"), pCategoryNode);
			for( int i = 0; i < 3; ++i )
			{
				pServerNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 10}{x 4}测试服务器"), pGameNode);
				for( int i = 0; i < 3; ++i )
				{
					pRoomNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 12}{x 4}测试房间"), pServerNode);
				}
			}
		}
	}

   CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("userlist")));
   if (pList != NULL)
   {
	   pList->SetTextCallback(this);

	   TCHAR szBuf[MAX_PATH] = {0};
	   for (int i = 0; i < 26; ++i)
	   {
		   CListExpandElementUI* pListElement = new CListExpandElementUI;
		   if (pListElement != NULL)
		   {
			   pList->Add(pListElement);
		   }
	   }
   }
}

LPCTSTR CFrameWnd::GetItemText(CControlUI* pControl, int iIndex, int iSubItem)
{
	static TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pParent = pControl->GetParent();
	if ((pControl != NULL) && (pParent->GetParent() != NULL) && (_tcsicmp(pControl->GetClass(), _T("ListExpandElementUI")) == 0 ) && (_tcsicmp(pParent->GetParent()->GetName(), _T("userlist")) == 0))
	{
		TCHAR cChar = _T('A');
		TCHAR cChar2 = _T('a');
		switch (iSubItem)
		{
		case 0:
			_stprintf(szBuf, _T("%c"), static_cast<int>(cChar) + iIndex);
			break;
		case 1:
			_stprintf(szBuf, _T("%c"), static_cast<int>(cChar2) + iIndex);
			break;
		}
		pControl->SetText(szBuf);
	}

	return szBuf;
}

void CFrameWnd::Notify(TNotifyUI& msg)
{
	if ( _tcsicmp(msg.sType, _T("windowinit")) == 0 ) OnPrepare();
	else if( _tcsicmp(msg.sType, _T("click")) == 0 ) {
		if( msg.pSender == m_pCloseBtn ) {
			PostQuitMessage(0);
			return;
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("sysconfig")) == 0)
		{
			CSystemSettingWnd* pSysConfig = new CSystemSettingWnd();
			if (pSysConfig == NULL) return;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
			pSysConfig->Create(NULL, _T(""), UI_WNDSTYLE_FRAME, 0L, 0, 0, 600, 800);
#else
			pSysConfig->Create(NULL, _T(""), UI_WNDSTYLE_FRAME, 0L, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif
			pSysConfig->CenterWindow();
			::ShowWindow(*pSysConfig, SW_SHOW);
			return;
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("bookstore")) == 0)
		{
			CBookStoreWnd* pBookStore = new CBookStoreWnd();
			if (pBookStore == NULL) return;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
			pBookStore->Create(NULL, _T(""), UI_WNDSTYLE_FRAME, 0L, 0, 0, 600, 800);
#else
			pBookStore->Create(NULL, _T(""), UI_WNDSTYLE_FRAME, 0L, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif
			pBookStore->CenterWindow();
			::ShowWindow(*pBookStore, SW_SHOW);
			return;
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("changeskin")) == 0)
		{
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
	else if( _tcsicmp(msg.sType, _T("itemclick")) == 0 ) {
		CDirectUITree* pGameList = static_cast<CDirectUITree*>(m_pm.FindControl(_T("treedemo")));
		if (pGameList == NULL)
			return;
		if( pGameList->GetItemIndex(msg.pSender) != -1 )
		{
			if( _tcsicmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
				CDirectUITree::Node* node = (CDirectUITree::Node*)msg.pSender->GetTag();

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
	else if( _tcsicmp(msg.sType , _T("itemactivate")) == 0 ) {
		CDirectUITree* pGameList = static_cast<CDirectUITree*>(m_pm.FindControl(_T("treedemo")));
		if (pGameList == NULL)
			return;
		if( pGameList->GetItemIndex(msg.pSender) != -1 )
		{
			if( _tcsicmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
				CDirectUITree::Node* node = (CDirectUITree::Node*)msg.pSender->GetTag();
				pGameList->SetChildVisible(node, !node->data()._child_visible);
				if( node->data()._level == 3 ) {
				}
			}
		}

	}
	else if( _tcsicmp(msg.sType, _T("selectchanged")) == 0 ) {
		CStdString name = msg.pSender->GetName();
		if( _tcsicmp(name, _T("combobox")) == 0 ) {
			CEditUI* pControl = static_cast<CEditUI*>(m_pm.FindControl(_T("singlelineedit")));
			if (pControl != NULL) pControl->SetText(msg.pSender->GetText());
		}
		else if( _tcsicmp(name, _T("tabBtn1")) == 0 ) {
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("Tab1")));
			if( pControl && pControl->GetCurSel() != 0 ) pControl->SelectItem(0);
		}
		else if( _tcsicmp(name, _T("tabBtn2")) == 0 ) {
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("Tab1")));
			if( pControl && pControl->GetCurSel() != 1 ) pControl->SelectItem(1);
		}
		else if( _tcsicmp(name , _T("tabBtn3")) == 0 ) {
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("Tab2")));
			if( pControl && pControl->GetCurSel() != 0 ) pControl->SelectItem(0);
		}
		else if( _tcsicmp(name , _T("tabBtn4")) == 0 ) {
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("Tab2")));
			if( pControl && pControl->GetCurSel() != 1 ) pControl->SelectItem(1);
		}
	}
}

LRESULT CFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("skins\\eBook\\");
	m_pm.SetResourcePath(tstrSkin.c_str());

	tstrSkin = m_pm.GetResourcePath() + _T("main.xml");
	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &m_pm);
	ASSERT(pRoot && _T("Failed to parse XML"));
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CFrameWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CFrameWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);

	bHandled = FALSE;
	return 0;
}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CFrameWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CFrameWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFrameWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFrameWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
#if 0
		RECT rc = { rcClient.left, rcClient.top + szRoundCorner.cx, rcClient.right, rcClient.bottom };
		HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
		HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
		::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
		::SetWindowRgn(*this, hRgn1, TRUE);
		::DeleteObject(hRgn1);
		::DeleteObject(hRgn2);
#else
		HRGN hRGN = CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this,hRGN,TRUE);
		::DeleteObject(hRGN);
#endif
	}
#endif
	bHandled = FALSE;
	return 0;
}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CFrameWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CFrameWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}		
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
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
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
#endif
	return lRes;
}

LRESULT CFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
		case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}
