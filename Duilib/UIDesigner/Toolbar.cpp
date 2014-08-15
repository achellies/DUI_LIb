#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "Toolbar.h"
#include "Resource.h"
#include "UIDesignerDlg.h"
#include "DesignerCanvas.h"
#include "PropertiesSheet.h"
#include "DialogBuilderCallback.h"

CToolbar::CToolbar(CUIDesignerDlg* pMainHostWindow, CDesignerCanvas* pDesignerCanvas)
: m_pDesignerCanvas(pDesignerCanvas)
, m_pMainHostWindow(pMainHostWindow)
{
	if (m_pDesignerCanvas != NULL)
		m_pDesignerCanvas->SetToolbarPointer(this);
}

void CToolbar::Init()
{}

void CToolbar::OnPrepare()
{
	if (m_pDesignerCanvas != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
		SIZE szCanvasSize = m_pDesignerCanvas->GetFormInfo().szCanvasSize;
		CControlUI* pControlUI = paint_manager_.FindControl(_T("width"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), szCanvasSize.cx);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("height"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), szCanvasSize.cy);
			pControlUI->SetText(szBuf);
		}
	}
}

void CToolbar::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CToolbar::Notify(TNotifyUI& msg)
{
	if ( _tcsicmp(msg.sType, _T("windowinit")) == 0 )
	{
		OnPrepare();
	}
	else if( _tcsicmp(msg.sType, _T("click")) == 0 )
	{
		if (m_pDesignerCanvas != NULL)
		{
			CStdString name = msg.pSender->GetName();
			if (_tcsicmp(name, _T("open")) == 0)
			{
				m_pDesignerCanvas->OpenSkinFile();
			}
			else if (_tcsicmp(name, _T("new")) == 0)
			{
				m_pDesignerCanvas->NewSkinFile();
			}
			else if (_tcsicmp(name, _T("save")) == 0)
			{
				m_pDesignerCanvas->SaveSkinFile();
			}
			else if (_tcsicmp(name, _T("help")) == 0)
			{
				if (m_pMainHostWindow != NULL)
					m_pMainHostWindow->ShowAboutDialog();
			}
			else if (_tcsicmp(name, _T("delete")) == 0)
			{
				m_pDesignerCanvas->DeleteControl();
			}
			else if (_tcsicmp(name, _T("copy")) == 0)
			{
				m_pDesignerCanvas->CopyControl();
			}
			else if (_tcsicmp(name, _T("cut")) == 0)
			{
				m_pDesignerCanvas->CutControl();
			}
			else if (_tcsicmp(name, _T("paste")) == 0)
			{
				m_pDesignerCanvas->PasteControl();
			}
			else if (_tcsicmp(name, _T("refresh")) == 0)
			{
				m_pDesignerCanvas->Refresh();
			}
			else if (_tcsicmp(name, _T("undo")) == 0)
			{
				m_pDesignerCanvas->Undo();
			}
			else if (_tcsicmp(name, _T("redo")) == 0)
			{
				m_pDesignerCanvas->Redo();
			}
			else if (_tcsicmp(name, _T("exit")) == 0)
			{
				if ((m_pDesignerCanvas->PreExit()) && (m_pMainHostWindow != NULL))
					m_pMainHostWindow->EndDialog(IDOK);
			}
			else if (_tcsicmp(name, _T("setforminfo")) == 0)
			{
				int nWidth = 0;
				int nHeight = 0;
				CControlUI* pControlUI = paint_manager_.FindControl(_T("width"));
				if (pControlUI != NULL)
				{
					CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
					nWidth = _ttoi(cstdText);
				}

				pControlUI = paint_manager_.FindControl(_T("height"));
				if (pControlUI != NULL)
				{
					CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
					nHeight = _ttoi(cstdText);
				}

				bool bOpacity = true;
				DWORD dwTransparency = 0;

				pControlUI = paint_manager_.FindControl(_T("opacity"));
				if (pControlUI != NULL)
				{
					CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
					bOpacity = (pComboUI->GetCurSel() == 0);
				}

				pControlUI = paint_manager_.FindControl(_T("transparency"));
				if (pControlUI != NULL)
				{
					CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
					dwTransparency = _ttoi(cstdText);
				}

				if ((nWidth != 0) && (nHeight != 0))
				{
					SIZE szNewSize;
					szNewSize.cx = nWidth;
					szNewSize.cy = nHeight;
					_WindowFormInfo info;
					info.szCanvasSize = szNewSize;
					info.bOpacity = bOpacity;
					info.dwTransparency = dwTransparency;
					m_pDesignerCanvas->SetFormInfo(info);

					RECT rcClient = {0};
					GetClientRect(GetParent(*m_pDesignerCanvas), &rcClient);
					RECT rcCanvas = rcClient;
					rcCanvas.top += TOOLBAR_HEIGHT + 2 * MARGIN;;
					rcCanvas.left = rcClient.left + MARGIN;
					DWORD dwResizeCanvasWidth = szNewSize.cx;
					::MoveWindow(*m_pDesignerCanvas, rcCanvas.left + MARGIN, rcCanvas.top + MARGIN, dwResizeCanvasWidth, szNewSize.cy, TRUE);
				}
			}
		}
	}
	else if( _tcsicmp(msg.sType, _T("itemselect")) == 0 )
	{
		CStdString name = msg.pSender->GetName();
		if( _tcsicmp(name, _T("opacity")) == 0 )
		{
			CComboUI* pComboUI = static_cast<CComboUI*>(msg.pSender->GetInterface(_T("Combo")));
			bool bOpacity = (pComboUI->GetCurSel() == 0);

			CEditUI* pControl = static_cast<CEditUI*>(paint_manager_.FindControl(_T("transparency")));
			if (bOpacity)
			{
				pControl->SetText(_T("0"));
				pControl->SetReadOnly(true);
			}
			else
			{
				pControl->SetReadOnly(false);
			}
		}
	}
}

LRESULT CToolbar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	paint_manager_.Init(m_hWnd);
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb;

	tString tstrSkin = CPaintManagerUI::GetInstancePath() + lpszSkinPath;
	paint_manager_.SetResourcePath(tstrSkin.c_str());

#if defined(USING_ZIP_SKIN)
	paint_manager_.SetResourceZip(lpszZipSkin);
	CControlUI* pRoot = builder.Create(_T("uidesigner_toolbar.xml"), (UINT)0,  &cb, &paint_manager_);
#else
	tstrSkin = paint_manager_.GetResourcePath() + _T("uidesigner_toolbar.xml");
	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
#endif
	ASSERT(pRoot && _T("Failed to parse XML"));
	paint_manager_.AttachDialog(pRoot);
	paint_manager_.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CToolbar::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CToolbar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CToolbar::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CToolbar::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CToolbar::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CToolbar::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	//::ScreenToClient(*this, &pt);

	//RECT rcClient;
	//::GetClientRect(*this, &rcClient);

	//RECT rcCaption = paint_manager_.GetCaptionRect();
	//if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
	//	&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
	//		CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(pt));
	//		if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
	//			_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
	//			_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
	//			return HTCAPTION;
	//}

	return HTCLIENT;
}
#endif
LRESULT CToolbar::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = paint_manager_.GetRoundCorner();
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	if( !::IsIconic(*this) && !::IsIconic(GetParent(*this)) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
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
LRESULT CToolbar::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
#endif
LRESULT CToolbar::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) {
		bHandled = TRUE;
		return 0;
	}		
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CToolbar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	if( paint_manager_.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CToolbar::UpdateFormInfo()
{
	if (m_pDesignerCanvas != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
		SIZE szCanvasSize = m_pDesignerCanvas->GetFormInfo().szCanvasSize;
		CControlUI* pControlUI = paint_manager_.FindControl(_T("width"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), szCanvasSize.cx);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("height"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), szCanvasSize.cy);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("transparency"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), m_pDesignerCanvas->GetFormInfo().dwTransparency);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("opacity"));
		if (pControlUI != NULL)
		{
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			pComboUI->SelectItem(m_pDesignerCanvas->GetFormInfo().bOpacity?0:1);
		}

	}
}