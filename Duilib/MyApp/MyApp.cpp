#include "stdafx.h"
#include "MyApp.h"
#include <exdisp.h>
#include <comdef.h>
#include "COutlookBar.h"

#ifndef MAKEPOINTS
#define MAKEPOINTS(l)       (*((POINTS FAR *)&(l)))
#endif

class CMenuFrameWnd : public CWindowWnd, public INotifyUI
{
public:
	CMenuFrameWnd() { };
	LPCTSTR GetWindowClassName() const { return _T("UIMenuSkin"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };
	void Init()
	{

		m_pColorMenu	= static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("color")));
		m_pTextureMenu	= static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("basetexture")));
	}
	//显示之前
	void OnPrepare()
	{}

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) 
		{
			OnPrepare();
		}
		else if(msg.sType == _T("selectchanged"))
		{
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if (pControl )
			{
				pControl->SelectItem(pControl->GetCurSel()==1?0:1);
			}
		}
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

		tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("Skins\\MyApp_Skin\\");
		m_pm.SetResourcePath(tstrSkin.c_str());

		tstrSkin = m_pm.GetResourcePath() + _T("menuSkin.xml");
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, &cb, &m_pm);
		ASSERT(pRoot && _T("Failed to parse XML"));
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		SIZE szInitWindowSize = m_pm.GetInitSize();
		if( szInitWindowSize.cx != 0 ) 
		{
			::SetWindowPos(*this, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);
		}

		Init();
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

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 &&_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0)
			{
				return HTCAPTION;
			}
		}

		return HTCLIENT;
	}
#endif
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (this == NULL)
		{
			return 0;
		}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) 
		{
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
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		ShowWindow(false,false);
		return 0;
	}
	LRESULT OnMoveWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		RECT rtClient;
		POINTS pt	= MAKEPOINTS(lParam);
		int x = (int)(short) LOWORD(lParam);
		int y = (int)(short) HIWORD(lParam);
		::GetWindowRect(m_hWnd,&rtClient);
		return MoveWindow(m_hWnd,(int)(short) LOWORD(lParam),(int)(short) HIWORD(lParam),rtClient.right - rtClient.left,rtClient.bottom - rtClient.top,FALSE);;
	}
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) 
		{
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) 
		{
		case WM_CREATE:				lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		case WM_NCACTIVATE:			lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:			lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:				lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:			lRes = OnKillFocus(uMsg,wParam,lParam);break;
		case WM_MOVE:				lRes = OnMoveWindow(uMsg,wParam,lParam);break;
		case WM_WINDOWPOSCHANGED:	bHandled = TRUE; break;
		default:
			bHandled = FALSE;
		}
		if( bHandled ) 
			return lRes;

		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) 
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
public:
	CPaintManagerUI m_pm;
	CVerticalLayoutUI* m_pColorMenu;
	CVerticalLayoutUI* m_pTextureMenu;
};

class CMenuSysFrameWnd : public CWindowWnd, public INotifyUI
{
public:
	CMenuSysFrameWnd() { };
	LPCTSTR GetWindowClassName() const { return _T("UIMenuSys"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };
	void Init()
	{

	}
	//显示之前
	void OnPrepare()
	{

	}
	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) 
		{
			OnPrepare();
		}
		else if(msg.sType == _T("click"))
		{
			this->ShowWindow(false,false);
		}
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
		tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("Skins\\MyApp_Skin\\");
		m_pm.SetResourcePath(tstrSkin.c_str());

		tstrSkin = m_pm.GetResourcePath() + _T("menuSystem.xml");
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, &cb, &m_pm);
		ASSERT(pRoot && _T("Failed to parse XML"));
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		SIZE szInitWindowSize = m_pm.GetInitSize();
		if( szInitWindowSize.cx != 0 ) 
		{
			::SetWindowPos(*this, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);
		}

		Init();
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

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
			{
				return HTCAPTION;
			}
		}

		return HTCLIENT;
	}
#endif
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (this == NULL)
		{
			return 0;
		}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) 
		{
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
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		ShowWindow(false,false);
		return 0;
	}
	LRESULT OnMoveWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		RECT rtClient;
		POINTS pt	= MAKEPOINTS(lParam);
		int x = (int)(short) LOWORD(lParam);
		int y = (int)(short) HIWORD(lParam);
		::GetWindowRect(m_hWnd,&rtClient);
		return MoveWindow(m_hWnd,(int)(short) LOWORD(lParam),(int)(short) HIWORD(lParam),rtClient.right - rtClient.left,rtClient.bottom - rtClient.top,FALSE);;
	}
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) 
		{
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) 
		{
		case WM_CREATE:				lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		case WM_NCACTIVATE:			lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:			lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:				lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:			lRes = OnKillFocus(uMsg,wParam,lParam);break;
		case WM_MOVE:				lRes = OnMoveWindow(uMsg,wParam,lParam);break;
		case WM_WINDOWPOSCHANGED:	bHandled = TRUE; break;
		default:
			bHandled = FALSE;
		}
		if( bHandled ) 
			return lRes;

		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) 
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
public:
	CPaintManagerUI m_pm;
};


class CSettingDialog : public CWindowWnd, public INotifyUI
{
public:
	CSettingDialog() { };
	LPCTSTR GetWindowClassName() const { return _T("SettingDialog"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };
	void Init()
	{
		m_btnMin			= static_cast<CButtonUI*>(m_pm.FindControl(_T("btnMin")));
		m_btnClose			= static_cast<CButtonUI*>(m_pm.FindControl(_T("btnClose")));
		m_pPersonSetList	= static_cast<CListUI*>(m_pm.FindControl(_T("userlist")));
		m_pSystemSetList	= static_cast<CListUI*>(m_pm.FindControl(_T("syslist")));
		m_pTabPanel			= static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
	}
	//显示之前
	void OnPrepare()
	{

	}
	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) 
		{
			OnPrepare();
		}
		else if (msg.sType == _T("click"))
		{
			if (msg.pSender == m_btnMin)
			{
				ShowWindow(false,false);
			}
			else if (msg.pSender == m_btnClose)
			{
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#else
				ShowWindow(false,false);
				Close();
#endif
			}
			else if ((_tcsicmp(msg.pSender->GetName(), _T("btnOk")) == 0)
				|| (_tcsicmp(msg.pSender->GetName(), _T("btnApply")) == 0)
				|| (_tcsicmp(msg.pSender->GetName(), _T("btnCancel")) == 0))
			{
				ShowWindow(false,false);
				Close();
			}
		}
		else if (msg.sType == _T("itemselect"))
		{
			if (msg.pSender == m_pSystemSetList)
			{
				m_pPersonSetList->SelectItem(-1);
				m_pTabPanel->SelectItem(m_pPersonSetList->GetCount() + m_pSystemSetList->GetCurSel());
			}
			else if(msg.pSender == m_pPersonSetList)
			{
				m_pTabPanel->SelectItem(m_pPersonSetList->GetCurSel());
				m_pSystemSetList->SelectItem(-1);
			}
		}
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

		tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("Skins\\MyApp_Skin\\");
		m_pm.SetResourcePath(tstrSkin.c_str());

		tstrSkin = m_pm.GetResourcePath() + _T("SetDialog.xml");
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, &cb, &m_pm);
		ASSERT(pRoot && _T("Failed to parse XML"));
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		SIZE szInitWindowSize = m_pm.GetInitSize();
		if( szInitWindowSize.cx != 0 ) 
		{
			::SetWindowPos(*this, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);
		}

		Init();
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

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
			{
				return HTCAPTION;
			}
		}

		return HTCLIENT;
	}
#endif
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (this == NULL)
		{
			return 0;
		}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) 
		{
			RECT rcClient;
			::GetClientRect(*this, &rcClient);
			//RECT rc = { rcClient.left, rcClient.top + szRoundCorner.cx, rcClient.right, rcClient.bottom };
			//HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
			//HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
			HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, szRoundCorner.cx, szRoundCorner.cy);
			//::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
			::SetWindowRgn(*this, hRgn2, TRUE);
			//::DeleteObject(hRgn1);
			::DeleteObject(hRgn2);
		}
#endif
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnMoveWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		RECT rtClient;
		POINTS pt	= MAKEPOINTS(lParam);
		int x = (int)(short) LOWORD(lParam);
		int y = (int)(short) HIWORD(lParam);
		::GetWindowRect(m_hWnd,&rtClient);
		return MoveWindow(m_hWnd,(int)(short) LOWORD(lParam),(int)(short) HIWORD(lParam),rtClient.right - rtClient.left,rtClient.bottom - rtClient.top,FALSE);;
	}
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) 
		{
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) 
		{
		case WM_CREATE:				lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		case WM_NCACTIVATE:			lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:			lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:				lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_MOVE:				lRes = OnMoveWindow(uMsg,wParam,lParam);break;
		case WM_SYSCOMMAND:			lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_WINDOWPOSCHANGED:	bHandled = TRUE; break;
		default:
			bHandled = FALSE;
		}
		if( bHandled ) 
			return lRes;

		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) 
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
public:
	CPaintManagerUI m_pm;
	CButtonUI*	m_btnClose;
	CButtonUI*	m_btnMin;
	CListUI*	m_pPersonSetList;
	CListUI*	m_pSystemSetList;
	CTabLayoutUI*	m_pTabPanel;
};

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
	CFrameWindowWnd() { };
	LPCTSTR GetWindowClassName() const { return _T("UIGame"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	//初始化函数
	void Init()
	{
		m_btnClose = (CButtonUI*)m_pm.FindControl(_T("closebtn"));
		m_btnRestore = (CButtonUI*)m_pm.FindControl(_T("restorebtn"));
		m_btnMin = (CButtonUI*)m_pm.FindControl(_T("minbtn"));
		m_btnMax = (CButtonUI*)m_pm.FindControl(_T("maxbtn"));
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		pBroswer = static_cast<CActiveXUI*>(m_pm.FindControl(_T("ie")));
#endif
		m_btnColor = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnColor")));
		m_btnMenu = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnMenu")));
		m_pSystemMenu = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("menuSet")));
		m_pSkinMenu = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("SkinSet")));
		m_pOutLookBar	= static_cast<COutlookbar*>(m_pm.FindControl(_T("OutLookBar")));
		m_btnPersonSet	= (CButtonUI*)m_pm.FindControl(_T("personset"));
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		if( pBroswer ) 
		{
			IWebBrowser2* pWebBrowser = NULL;
			pBroswer->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
			if( pWebBrowser != NULL ) 
			{
				pWebBrowser->Navigate(L"http://www.google.com",NULL,NULL,NULL,NULL);  
				pWebBrowser->Release();
			}
		}
#endif

		m_pMenuFrame	= new CMenuFrameWnd();
		m_pMenuSystem	= new CMenuSysFrameWnd();		
		if( m_pMenuFrame == NULL || m_pMenuSystem == NULL) 
		{ 
			Close(); 
			return; 
		}
		m_pMenuFrame->Create(NULL, _T(""),UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG, 0, 0, 136,160, NULL);
		m_pMenuSystem->Create(NULL, _T(""),UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG, 0, 0, 120,135, NULL);
	}

	//显示之前
	void OnPrepare() 
	{ 
		if (m_pOutLookBar == NULL)
			return;
		Node* pCategoryNode = NULL;
		Node* pGameNode = NULL;
		Node* pServerNode = NULL;
		Node* pRoomNode = NULL;
		pCategoryNode = m_pOutLookBar->AddNode(_T("推荐游戏"));						//添加数据
		for( int i = 0; i < 4; ++i )
		{
			pGameNode = m_pOutLookBar->AddNode(_T("{x 4}{i res/gameicons.png 10}{x 4}四人斗地主"), pCategoryNode);
			for( int i = 0; i < 3; ++i )
			{
				pServerNode = m_pOutLookBar->AddNode(_T("{x 4}{i res/gameicons.png 10}{x 4}测试服务器"), pGameNode);
				for( int i = 0; i < 3; ++i )
				{
					pRoomNode = m_pOutLookBar->AddNode(_T("{x 4}{i res/gameicons.png 10}{x 4}测试房间"), pServerNode);
				}
			}
		}
		pCategoryNode = m_pOutLookBar->AddNode(_T("最近玩过的游戏"),NULL,OutLookBar_Root);
		for( int i = 0; i < 2; ++i )
		{
			m_pOutLookBar->AddNode(_T("三缺一"), pCategoryNode,OutLookBar_Node);
		}
		pCategoryNode = m_pOutLookBar->AddNode(_T("棋牌游戏"),NULL,OutLookBar_Root);
		for( int i = 0; i < 8; ++i )
		{
			m_pOutLookBar->AddNode(_T("双扣"), pCategoryNode,OutLookBar_Node);
		}
		pCategoryNode = m_pOutLookBar->AddNode(_T("休闲游戏"),NULL,OutLookBar_Root);
		for( int i = 0; i < 8; ++i )
		{
			m_pOutLookBar->AddNode(_T("飞行棋"), pCategoryNode,OutLookBar_Node);
		}
	}

	//消息
	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) 
		{
			OnPrepare();
		}
		else if(msg.sType == _T("click"))
		{
			if(msg.pSender == m_btnClose)
			{
				PostQuitMessage(0);
			}
			else if(msg.pSender == m_btnMax)					//最大化
			{
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);				
#else
				::ShowWindow(*this, SW_MAXIMIZE);

				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
#endif
			}
			else if(msg.pSender == m_btnMin)
			{
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);		//最小化
#else
				::ShowWindow(*this, SW_MINIMIZE);
#endif
			}
			else if(msg.pSender == m_btnRestore)
			{
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);		//还原
#else
				::ShowWindow(*this, SW_RESTORE);
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
#endif
			}
			else if(msg.pSender == m_btnColor)					//颜色设置按钮
			{
				POINT pt = msg.ptMouse;
				::ClientToScreen(m_hWnd, &pt);
				::PostMessage(m_pMenuFrame->GetHWND(),WM_MOVE,0,MAKELPARAM(pt.x,pt.y));
				m_pMenuFrame->ShowWindow();
			}
			else if(msg.pSender == m_btnMenu)					//皮肤设置菜单
			{
				POINT pt = msg.ptMouse;
				::ClientToScreen(m_hWnd, &pt);
				::PostMessage(m_pMenuSystem->GetHWND(),WM_MOVE,0,MAKELPARAM(pt.x,pt.y));
				m_pMenuSystem->ShowWindow();
			}
			else if (msg.pSender == m_btnPersonSet)				//系统设置菜单
			{
				CSettingDialog*		m_pSetDialog = new CSettingDialog();
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
				m_pSetDialog->Create(GetHWND(), _T("游戏设置"),UI_WNDSTYLE_DIALOGEX, UI_WNDSTYLE_EX_DIALOG |WS_EX_APPWINDOW, 0, 0, 650,490, NULL);
#else
				m_pSetDialog->Create(GetHWND(), _T("游戏设置"),UI_WNDSTYLE_DIALOGEX, UI_WNDSTYLE_EX_DIALOG, 0, 0, 600,500, NULL);
#endif
				m_pSetDialog->CenterWindow();
				m_pSetDialog->ShowWindow();
				ShowModal(*m_pSetDialog);
			}
		}
		else if( msg.sType == _T("itemselect") )				//游戏房间搜索
		{
			if( msg.pSender->GetName() == _T("seachcombo") ) 
			{
				CEditUI* pAccountEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("Seachedit")));

				if( pAccountEdit ) 
					pAccountEdit->SetText(msg.pSender->GetText());
			}
		}
		else if( msg.sType == _T("itemclick") ) 
		{
			if (m_pOutLookBar == NULL)
				return;
			if( m_pOutLookBar->GetItemIndex(msg.pSender) != -1 )
			{
				if( _tcsicmp(msg.pSender->GetClass(), _T("OutlookbarNode")) == 0 ) 
				{
					Node* node = (Node*)msg.pSender->GetTag();

					POINT pt = { 0 };
					::GetCursorPos(&pt);
					::ScreenToClient(m_pm.GetPaintWindow(), &pt);
					pt.x -= msg.pSender->GetX();
					SIZE sz = m_pOutLookBar->GetExpanderSizeX(node);
					if( pt.x >= sz.cx && pt.x < sz.cy )        //点击的是箭头             
					{
						if (node->data()._child_visible == false && node->data()._level	== 0)
						{
							if (m_pOutLookBar->m_pLastVisible != NULL)
							{
								m_pOutLookBar->SetChildVisible(m_pOutLookBar->m_pLastVisible,false);
							}
							m_pOutLookBar->SetChildVisible(node,true);
							m_pOutLookBar->m_pLastVisible	= node;
						}
						else if (node->_data._level != 0)
						{
							m_pOutLookBar->SetChildVisible(node,!node->_data._child_visible);
						}

					}
					else	//不是箭头就判断是否是树形节点
					{
						if (node->data()._child_visible == false && node->data()._level	== 0)
						{
							if (m_pOutLookBar->m_pLastVisible != NULL)
							{
								m_pOutLookBar->SetChildVisible(m_pOutLookBar->m_pLastVisible,false);
							}
							m_pOutLookBar->SetChildVisible(node,true);
							m_pOutLookBar->m_pLastVisible	= node;
						}
						else if (node->_data._level != 0 && node->_data.nodetype >= OutLookBar_Root)
						{
							m_pOutLookBar->SetChildVisible(node,!node->_data._child_visible);
						}
					}
				}
			}
		}
		else if( msg.sType == _T("itemactivate") ) //双击
		{
			if (m_pOutLookBar == NULL)
				return;
			if( m_pOutLookBar->GetItemIndex(msg.pSender) != -1 )
			{
				if( _tcsicmp(msg.pSender->GetClass(), _T("OutlookbarNode")) == 0 ) 
				{
					Node* node = (Node*)msg.pSender->GetTag();

					if (node->data()._child_visible == false && node->_data._level == 0)
					{
						m_pOutLookBar->SetChildVisible(m_pOutLookBar->m_pLastVisible,false);
						m_pOutLookBar->SetChildVisible(node,true);
						m_pOutLookBar->m_pLastVisible	= node;
					}
					else if (node->_data._level != 0)
					{
						m_pOutLookBar->SetChildVisible(node,!node->_data._child_visible);
					}
					if( node->data()._level == 3 ) 
					{
						//进入房间
					}
				}
			}

		}
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

		tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("Skins\\MyApp_Skin\\");
		m_pm.SetResourcePath(tstrSkin.c_str());

		tstrSkin = m_pm.GetResourcePath() + _T("MyApp.xml");
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, &cb, &m_pm);
		ASSERT(pRoot && _T("Failed to parse XML"));
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		SIZE szInitWindowSize = m_pm.GetInitSize();
		if( szInitWindowSize.cx != 0 ) 
		{
			::SetWindowPos(*this, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);
		}

		Init();
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

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
				return HTCAPTION;
		}

		return HTCLIENT;
	}
#endif
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (this == NULL)
		{
			return 0;
		}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) 
		{
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
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) 
		{
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ) 
		{
			if( !bZoomed ) 
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else 
			{
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
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) 
		{
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
		}
		if( bHandled ) 
			return lRes;

		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) 
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

public:
	CPaintManagerUI m_pm;
	CButtonUI*	m_btnClose;
	CButtonUI*	m_btnMax;
	CButtonUI*	m_btnMin;
	CButtonUI*	m_btnRestore;
	CButtonUI*	m_btnColor;
	CButtonUI*	m_btnMenu;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	CActiveXUI* pBroswer;
#endif
	CVerticalLayoutUI* m_pSystemMenu;
	CVerticalLayoutUI* m_pSkinMenu;
	COutlookbar* m_pOutLookBar;
	CMenuFrameWnd* m_pMenuFrame;
	CMenuSysFrameWnd*	m_pMenuSystem;	
	CButtonUI*	m_btnPersonSet;
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

	CFrameWindowWnd* pFrame = new CFrameWindowWnd();
	if( pFrame == NULL ) 
		return 0;

	RECT rcClient;
	rcClient.left = 0;
	rcClient.top = 0;
	rcClient.bottom = 800;
	rcClient.right = 600;
	pFrame->Create(NULL, _T("QQ游戏大厅"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->CenterWindow();
	pFrame->ShowWindow(true);
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::ShutDown();

	::CoUninitialize();
	return 0;
}
