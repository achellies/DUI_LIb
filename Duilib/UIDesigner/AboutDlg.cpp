// UIDesignerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AboutDlg.h"
#include "UIDesignerDlg.h"
#include "DialogBuilderCallback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "Version.lib")

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	this->MoveWindow(0, 0, 560, 250);
	::SetWindowLongPtr(this->GetSafeHwnd(), GWLP_USERDATA, reinterpret_cast<LONG>(&m_dlgWnd));
	m_dlgWnd.Subclass(this->GetSafeHwnd());
	m_dlgWnd.SetHostDlg(this);
	m_dlgWnd.LoadSkin();
	m_dlgWnd.CenterWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAboutDlg::Close()
{
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CDialogUI::LoadSkin()
{
	paint_manager_.Init(m_hWnd);
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb;

	tString tstrSkin = CPaintManagerUI::GetInstancePath() + lpszSkinPath;
	paint_manager_.SetResourcePath(tstrSkin.c_str());

#if defined(USING_ZIP_SKIN)
	paint_manager_.SetResourceZip(lpszZipSkin);
	CControlUI* pRoot = builder.Create(_T("uidesigner_about.xml"), (UINT)0,  &cb, &paint_manager_);
#else
	tstrSkin = paint_manager_.GetResourcePath() + _T("uidesigner_about.xml");
	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
#endif
	ASSERT(pRoot && "Failed to parse XML");
	paint_manager_.AttachDialog(pRoot);
	paint_manager_.AddNotifier(this);

	TCHAR szVersion[MAX_PATH] = {0};
	_stprintf(szVersion, _T("UIDesigner Version 1.1.0.0"));

	TCHAR szCopyright[MAX_PATH] = {0};
	_stprintf(szCopyright, _T("Copyright (C) 2011 achellies All Right Reserved."));

	CTextUI* pTextUI = static_cast<CTextUI*>(paint_manager_.FindControl(_T("comment")));
	if (pTextUI != NULL)
		pTextUI->SetText(_T("Comment:{n}欢迎使用Duilib的界面编辑器，如果大家在使用过程中遇到什么问题或者程序本身的bug欢迎与我联系。{n}可以通过我的博客 {a http://blog.csdn.net/achellies}http://blog.csdn.net/achellies{/a}{n}或者Duilib的官网论坛 {a http://www.duilib.com}http://www.duilib.com{/a} 来反馈。"));

	// Get version information from the application
	TCHAR *szBuf = (TCHAR *)malloc(sizeof(TCHAR)*128*7);
	if (szBuf != NULL) 
	{
		TCHAR *szComment = &(szBuf[128*0]);
		TCHAR *szCompanyName = &(szBuf[128*1]); 
		TCHAR *szFileDescription = &(szBuf[128*2]);
		TCHAR *szFileVersion = &(szBuf[128*3]);
		TCHAR *szLegalCopyright = &(szBuf[128*4]);
		TCHAR *szProductName = &(szBuf[128*5]);
		TCHAR *szProductVersion = &(szBuf[128*6]);
		if (!GetVerString(128, szComment,
			szCompanyName, szFileDescription, szFileVersion, szLegalCopyright,
			szProductName, szProductVersion))
		{
			goto allDone;
		}
		_stprintf(szVersion, _T("UIDesigner Version %s"), szProductVersion);
		_stprintf(szCopyright, _T("%s"), szLegalCopyright);
	}
allDone:
	if (szBuf != NULL)
	{
		free(szBuf);
		szBuf = NULL;
	}

	CControlUI* pControlUI = static_cast<CControlUI*>(paint_manager_.FindControl(_T("version")));
	if (pControlUI != NULL)
		pControlUI->SetText(szVersion);

	pControlUI = static_cast<CControlUI*>(paint_manager_.FindControl(_T("copyright")));
	if (pControlUI != NULL)
		pControlUI->SetText(szCopyright);
}


// ...........................................................................
//
// NAME:	CDialogUI::GetVerString
//
// IMPLEMENTATION NOTES:
// ...........................................................................
bool GetVerString(unsigned int maxStrSize, TCHAR *szComments,
						TCHAR *szCompanyName, TCHAR *szFileDescription,
						TCHAR *szFileVersion, TCHAR *szLegalCopyright,
						TCHAR *szProductName, TCHAR *szProductVersion)
{
	struct TTransArray
	{
		WORD LanguageID, CharacterSet;
	};

	// Get pathname of this executable
	// If hInst is NULL, GetModuleFileName will retrieve the module name of 
	// the executable of this process.
	TCHAR szModuleFullPath[256];
	szModuleFullPath[0] = _T('\0');
	::GetModuleFileName(AfxGetInstanceHandle(), szModuleFullPath, sizeof(szModuleFullPath));
	TRACE(_T("CPdkApp::GetVerString: szModuleFullPath: '%s'\n"), 
		szModuleFullPath);

	// Get size of the current file version structure
	DWORD dwVerHnd;
	DWORD verInfoSize = ::GetFileVersionInfoSize(szModuleFullPath, &dwVerHnd);
	if ((verInfoSize == 0) || (maxStrSize == 0))
	{
		TRACE(_T("#### CPdkApp::GetVerString: verInfoSize: %ld, ")
			_T("maxStrSize: %d\n"), verInfoSize, maxStrSize);
		return false;
	}

	// Allocate space for file information block and retrieve it
	char *pMem;				// Pointer to mem cpy of version resource
	TCHAR *szCurInfo = NULL;// Pointer into pMem block for currently requested
	// information.
	unsigned int curInfoLen;	// Length of info in szCurInfo in characters
	if ((pMem = (char *)malloc((size_t)verInfoSize)) == NULL)
	{
		//TRACE(_T("#### CPdkApp::GetVerString: failed to alloc.\n"));
		return false;
	}

	if (::GetFileVersionInfo(szModuleFullPath, dwVerHnd, verInfoSize, pMem) == 0)
	{
		free(pMem);
		//TRACE(_T("#### CPdkApp::GetVerString: GetFileVersionInfo() returned zero.\n"));
		return false;
	}

	// Get the array of available languages for this version block
	::VerQueryValue(pMem, _T("\\VarFileInfo\\Translation"), 
		(void **)&szCurInfo, &curInfoLen);
	TTransArray *arrAvailLangs = (TTransArray *)szCurInfo;

	// Get the Comments information.
	TCHAR szQuery[256];
	if (szComments)
	{
		wsprintf(szQuery, _T("\\StringFileInfo\\%04x%04x\\Comments"),
			arrAvailLangs[0].LanguageID, arrAvailLangs[0].CharacterSet);
		if (::VerQueryValue(pMem, szQuery, (void **)&szCurInfo, &curInfoLen) != 0)
		{
			_tcsncpy(szComments, szCurInfo, min(maxStrSize, curInfoLen));
			szComments[maxStrSize - 1] = '\0';
		}
	}

	// Get the CompanyName information
	if (szCompanyName)
	{
		wsprintf(szQuery, _T("\\StringFileInfo\\%04x%04x\\CompanyName"),
			arrAvailLangs[0].LanguageID, arrAvailLangs[0].CharacterSet);
		if (::VerQueryValue(pMem, szQuery, (void **)&szCurInfo, &curInfoLen) != 0)
		{
			_tcsncpy(szCompanyName, szCurInfo, min(maxStrSize, curInfoLen));
			szCompanyName[maxStrSize - 1] = '\0';
		}
	}

	// Get the FileDescription information
	if (szFileDescription)
	{
		wsprintf(szQuery, _T("\\StringFileInfo\\%04x%04x\\FileDescription"),
			arrAvailLangs[0].LanguageID, arrAvailLangs[0].CharacterSet);
		if (::VerQueryValue(pMem, szQuery, (void **)&szCurInfo, &curInfoLen) != 0)
		{
			_tcsncpy(szFileDescription, szCurInfo, min(maxStrSize, curInfoLen));
			szFileDescription[maxStrSize - 1] = '\0';
		}
	}

	// Get the FileVersion information
	if (szFileVersion)
	{
		wsprintf(szQuery, _T("\\StringFileInfo\\%04x%04x\\FileVersion"),
			arrAvailLangs[0].LanguageID, arrAvailLangs[0].CharacterSet);
		if (::VerQueryValue(pMem, szQuery, (void **)&szCurInfo, &curInfoLen) != 0)
		{
			_tcsncpy(szFileVersion, szCurInfo, min(maxStrSize, curInfoLen));
			szFileVersion[maxStrSize - 1] = '\0';
		}
	}

	// Get the LegalCopyright information
	if (szLegalCopyright)
	{
		wsprintf(szQuery, _T("\\StringFileInfo\\%04x%04x\\LegalCopyright"),
			arrAvailLangs[0].LanguageID, arrAvailLangs[0].CharacterSet);
		if (::VerQueryValue(pMem, szQuery, (void **)&szCurInfo, &curInfoLen) != 0)
		{
			_tcsncpy(szLegalCopyright, szCurInfo, min(maxStrSize, curInfoLen));
			szLegalCopyright[maxStrSize - 1] = '\0';
		}
	}

	// Get the ProductName information
	if (szProductName)
	{
		wsprintf(szQuery, _T("\\StringFileInfo\\%04x%04x\\ProductName"),
			arrAvailLangs[0].LanguageID, arrAvailLangs[0].CharacterSet);
		if (::VerQueryValue(pMem, szQuery, (void **)&szCurInfo, &curInfoLen) != 0)
		{
			_tcsncpy(szProductName, szCurInfo, min(maxStrSize, curInfoLen));
			szProductName[maxStrSize - 1] = '\0';
		}
	}

	// Get the ProductVersion information
	if (szProductVersion)
	{
		wsprintf(szQuery, _T("\\StringFileInfo\\%04x%04x\\ProductVersion"),
			arrAvailLangs[0].LanguageID, arrAvailLangs[0].CharacterSet);
		if (::VerQueryValue(pMem, szQuery, (void **)&szCurInfo, &curInfoLen) != 0)
		{
			_tcsncpy(szProductVersion, szCurInfo, min(maxStrSize, curInfoLen));
			szProductVersion[maxStrSize - 1] = '\0';
		}
	}

	// Free the space for file information block
	free(pMem);

	return true;
}


void CDialogUI::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0) {
		if ( _tcsicmp(msg.pSender->GetName(), _T("okbtn")) == 0 )
		{
			m_pHostDlg->Close();
		}
		if ( _tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0 )
		{
			m_pHostDlg->Close();
		}
	}
	else if (_tcsicmp(msg.sType, _T("link")) == 0) {
		CTextUI* pTextUI = static_cast<CTextUI*>(msg.pSender->GetInterface(_T("Text")));
		if (pTextUI != NULL) {
			CStdString* pLinkText = pTextUI->GetLinkContent(static_cast<int>(msg.wParam));
			if (pLinkText != NULL)
				ShellExecute(NULL, _T("open"), pLinkText->GetData(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void CDialogUI::SetHostDlg(CAboutDlg* pDlg)
{
	m_pHostDlg = pDlg;
}

LRESULT CDialogUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDialogUI::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDialogUI::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CDialogUI::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDialogUI::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDialogUI::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	//if( !::IsZoomed(*this) ) {
	//	RECT rcSizeBox = paint_manager_.GetSizeBox();
	//	if( pt.y < rcClient.top + rcSizeBox.top ) {
	//		if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
	//		if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
	//		return HTTOP;
	//	}
	//	else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
	//		if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
	//		if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
	//		return HTBOTTOM;
	//	}
	//	if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
	//	if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	//}

	RECT rcCaption = paint_manager_.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CDialogUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = paint_manager_.GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		HRGN hRGN = CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this,hRGN,TRUE);
		::DeleteObject(hRGN);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CDialogUI::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	DuiLib::CRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT CDialogUI::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
		if( !bZoomed ) {
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(false);
		}
	}
	return lRes;
}

LRESULT CDialogUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( paint_manager_.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}