#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "ControlEx.h"

class CTestFrameWnd : public CWindowWnd, public INotifyUI, public IListCallbackUI, public IDialogBuilderCallback
{
public:
	CTestFrameWnd() { };
	LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void Init() {
		m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	}

	LPCTSTR GetItemText(CControlUI* pControl, int iIndex, int iSubItem)
	{
		static TCHAR szBuf[MAX_PATH] = {0};
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

	   return szBuf;
	}

	void OnPrepare() { 
	   //CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("userlist")));
	   //if (pList != NULL)
	   //{
		  // pList->SetTextCallback(this);

		  // TCHAR szBuf[MAX_PATH] = {0};
		  // for (int i = 0; i < 26; ++i)
		  // {
			 //  CListExpandElementUI* pListElement = new CListExpandElementUI;
			 //  if (pListElement != NULL)
			 //  {
				//   pList->Add(pListElement);
			 //  }
		  // }
	   //}
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

		tString tstrSkin = CPaintManagerUI::GetInstancePath() + _T("skins\\TestSkin\\");
		m_pm.SetResourcePath(tstrSkin.c_str());

		tstrSkin = m_pm.GetResourcePath() + _T("test1.xml");
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0, this, &m_pm);		
		ASSERT(pRoot && _T("Failed to parse XML"));
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		Init();
		return 0;
	}

	CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager) 
    {
        return NULL;
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
		SIZE szRoundCorner = m_pm.GetRoundCorner();
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
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
		// ��ʱ�����յ�WM_NCDESTROY���յ�wParamΪSC_CLOSE��WM_SYSCOMMAND
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

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

public:
	CPaintManagerUI m_pm;

private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	//...
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

	CTestFrameWnd* pFrame = new CTestFrameWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T(""), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::ShutDown();

	::CoUninitialize();
	return 0;
}