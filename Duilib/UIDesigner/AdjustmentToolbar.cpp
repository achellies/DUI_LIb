#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "Resource.h"
#include "UIDesignerDlg.h"
#include "DesignerCanvas.h"
#include "AdjustmentToolbar.h"
#include "DialogBuilderCallback.h"

CAdjustmentToolbar::CAdjustmentToolbar(CDesignerCanvas* pDesignerCanvas)
: m_pDesignerCanvas(pDesignerCanvas)
{}

void CAdjustmentToolbar::Init()
{}

void CAdjustmentToolbar::OnPrepare()
{}

void CAdjustmentToolbar::Notify(TNotifyUI& msg)
{
	if ( _tcsicmp(msg.sType, _T("windowinit")) == 0 )
	{
		OnPrepare();
	}
	else if( _tcsicmp(msg.sType, _T("click")) == 0 )
	{
		eAdjustment adjust = eAdjustmentUnknown;
		CStdString name = msg.pSender->GetName();
		if (_tcsicmp(name, _T("eAdjustmentAlign2Left")) == 0)
		{
			adjust = eAdjustmentAlign2Left;
		}
		else if (_tcsicmp(name, _T("eAdjustmentAlign2VCenter")) == 0)
		{
			adjust = eAdjustmentAlign2VCenter;
		}
		else if (_tcsicmp(name, _T("eAdjustmentAlign2Right")) == 0)
		{
			adjust = eAdjustmentAlign2Right;
		}
		else if (_tcsicmp(name, _T("eAdjustmentAlign2Top")) == 0)
		{
			adjust = eAdjustmentAlign2Top;
		}
		else if (_tcsicmp(name, _T("eAdjustmentAlign2HCenter")) == 0)
		{
			adjust = eAdjustmentAlign2HCenter;
		}
		else if (_tcsicmp(name, _T("eAdjustmentAlign2Bottom")) == 0)
		{
			adjust = eAdjustmentAlign2Bottom;
		}
		else if (_tcsicmp(name, _T("eAdjustmentTransverseDistribution")) == 0)
		{
			adjust = eAdjustmentTransverseDistribution;
		}
		else if (_tcsicmp(name, _T("eAdjustmentLongitudinalDistribution")) == 0)
		{
			adjust = eAdjustmentLongitudinalDistribution;
		}
		else if (_tcsicmp(name, _T("eAdjustmentHorizontalCenter")) == 0)
		{
			adjust = eAdjustmentHorizontalCenter;
		}
		else if (_tcsicmp(name, _T("eAdjustmentVerticalLCenter")) == 0)
		{
			adjust = eAdjustmentVerticalLCenter;
		}
		else if (_tcsicmp(name, _T("eAdjustmentSameWidth")) == 0)
		{
			adjust = eAdjustmentSameWidth;
		}
		else if (_tcsicmp(name, _T("eAdjustmentSameHeight")) == 0)
		{
			adjust = eAdjustmentSameHeight;
		}
		else if (_tcsicmp(name, _T("eAdjustmentSameSize")) == 0)
		{
			adjust = eAdjustmentSameSize;
		}
		else if (_tcsicmp(name, _T("eAdjustmentProperties")) == 0)
		{
			adjust = eAdjustmentProperties;
		}
		if ((adjust != eAdjustmentUnknown) && (m_pDesignerCanvas != NULL))
		{
			if (eAdjustmentProperties == adjust)
				m_pDesignerCanvas->ShowPropertiesSheet();
			else
				m_pDesignerCanvas->AdjustControls(adjust);
		}
	}
}

LRESULT CAdjustmentToolbar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
	CControlUI* pRoot = builder.Create(_T("uidesigner_adjustment.xml"), (UINT)0,  &cb, &paint_manager_);
#else
	tstrSkin = paint_manager_.GetResourcePath() + _T("uidesigner_adjustment.xml");
	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
#endif
	ASSERT(pRoot && _T("Failed to parse XML"));
	paint_manager_.AttachDialog(pRoot);
	paint_manager_.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CAdjustmentToolbar::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CAdjustmentToolbar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CAdjustmentToolbar::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CAdjustmentToolbar::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CAdjustmentToolbar::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CAdjustmentToolbar::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = paint_manager_.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}
#endif
LRESULT CAdjustmentToolbar::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CAdjustmentToolbar::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CAdjustmentToolbar::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) {
		bHandled = TRUE;
		return 0;
	}		
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CAdjustmentToolbar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
