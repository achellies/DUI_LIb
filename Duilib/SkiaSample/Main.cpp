#define _WIN32_WINNT 0x0501
#include <objbase.h>
#include <zmouse.h>
#include <exdisp.h>
#include <comdef.h>
#include <UIlib.h>
#include <vector>
#include <sstream>
#include "stdafx.h"

#pragma warning(disable : 4996)

class QQDownload : public CSkUIWindow, public INotifyUI, public IDialogBuilderCallback
{
	typedef CSkUIWindow INHERITED;
public:
	QQDownload() {}
	LPCTSTR GetWindowClassName() const
	{ 
		return _T("QQDownload"); 
	}

	UINT GetClassStyle() const 
	{ 
		return CS_DBLCLKS; 
	}

	void OnFinalMessage(HWND /*hWnd*/) 
	{ 
		delete this;
	}

	void Init() 
	{
		m_pCloseBtn = static_cast<CButtonUI*>(m_paintManager.FindControl(_T("closebtn")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_paintManager.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_paintManager.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_paintManager.FindControl(_T("minbtn")));
	}

	void OnPrepare(TNotifyUI& msg) 
	{	  
	}

	CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager) 
	{
		return NULL;
	}

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) 
			OnPrepare(msg);
		else if( msg.sType == _T("click") ) 
		{
			if( msg.pSender == m_pCloseBtn ) 
			{
				PostQuitMessage(0);
				return; 
			}
			else if( msg.pSender == m_pMinBtn ) 
			{ 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return; 
			}
			else if( msg.pSender == m_pMaxBtn ) 
			{ 
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; 
			}
			else if( msg.pSender == m_pRestoreBtn ) 
			{ 
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; 
			}
		}
		else if(msg.sType==_T("setfocus"))
		{
		}
		else if( msg.sType == _T("itemclick") ) 
		{
		}
		else if( _tcsicmp(msg.sType, _T("selectchanged")) == 0 ) {
			CStdString name = msg.pSender->GetName();
			if( _tcsicmp(name, _T("todaydown")) == 0 ) {
				CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_paintManager.FindControl(_T("switch")));
				if( pControl && pControl->GetCurSel() != 0 ) pControl->SelectItem(0);
			}
			else if( _tcsicmp(name, _T("historydown")) == 0 ) {
				CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_paintManager.FindControl(_T("switch")));
				if( pControl && pControl->GetCurSel() != 1 ) pControl->SelectItem(1);
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

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;

		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) ) {
			RECT rcSizeBox = m_paintManager.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_paintManager.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_paintManager.FindControl(pt));
				if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_paintManager.GetRoundCorner();
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

		LRESULT lRes = this->INHERITED::OnSize(uMsg, wParam, lParam, bHandled);

		return 0;
	}

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

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		LRESULT lRes = CSkUIWindow::OnSysCommand(uMsg, wParam, lParam, bHandled);
		BOOL bZoomed = ::IsZoomed(*this);		
		if( ::IsZoomed(*this) != bZoomed ) {
			if( !bZoomed ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_paintManager.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_paintManager.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_paintManager.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_paintManager.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
		bHandled = TRUE;
		return lRes;
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

		m_paintManager.Init(m_hWnd);
		CDialogBuilder builder;

		tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("Skins\\QQDownload\\");
		m_paintManager.SetResourcePath(tstrSkin.c_str());

		tstrSkin = m_paintManager.GetResourcePath() + _T("skin.xml");
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, this, &m_paintManager);
		ASSERT(pRoot && _T("Failed to parse XML"));
		m_paintManager.AttachDialog(pRoot);
		m_paintManager.AddNotifier(this);

		SIZE szInitWindowSize = m_paintManager.GetInitSize();
		if( szInitWindowSize.cx != 0 ) {
			::SetWindowPos(*this, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOZORDER | SWP_NOMOVE);
		}

		this->INHERITED::OnCreate(uMsg, wParam, lParam, bHandled);

		Init();
		return 0;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	QQDownload* pFrame = new QQDownload();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("Skia 2D Graphic Library Demo"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 380, 500);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	CPaintManagerUI::ShutDown();

	::CoUninitialize();
	return 0;
}