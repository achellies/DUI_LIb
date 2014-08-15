#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "Resource.h"
#include "ImageDialog.h"
#include "UIDesignerDlg.h"
#include "DesignerCanvas.h"
#include "PropertiesSheet.h"
#include "DialogBuilderCallback.h"

static LPCTSTR lpszColorRefFormat = _T("#%02X%02X%02X%02X");

CPropertiesSheet::CPropertiesSheet(CControlUI* pHostControlUI, CDesignerCanvas* pDesignerCanvas)
: m_pDesignerCanvas(pDesignerCanvas)
, m_pHostControlUI(pHostControlUI)
, m_pSpecialPropertiesTab(NULL)
{}

void CPropertiesSheet::Init()
{}

void CPropertiesSheet::OnPrepare()
{
	CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("ProtertiesTab")));
	if (pControl != NULL)
		LoadSpecialPropertiesTab(pControl);
}

void CPropertiesSheet::Notify(TNotifyUI& msg)
{
	if ( _tcsicmp(msg.sType, _T("windowinit")) == 0 )
	{
		OnPrepare();
	}
	else if( _tcsicmp(msg.sType, _T("click")) == 0 )
	{
		if ( _tcsicmp(msg.pSender->GetName(), _T("applybtn")) == 0 )
		{
			ApplySettings();
		}
	}
	else if( _tcsicmp(msg.sType, _T("selectchanged")) == 0 )
	{
		CStdString name = msg.pSender->GetName();
		if ( _tcsicmp(name, _T("CommonProperties")) == 0 )
		{
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("ProtertiesTab")));
			if ( pControl && pControl->GetCurSel() != 0 )
			{
				pControl->SelectItem(0);
				UpdateLayout();
			}
		}
		else if ( _tcsicmp(name, _T("SpecialProperties")) == 0 )
		{
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("ProtertiesTab")));
			if ( pControl && pControl->GetCurSel() != 1 )
			{
				pControl->SelectItem(1);
				UpdateLayout();
			}
		}
		else if ( _tcsicmp(msg.pSender->GetName(), _T("ChooseColor")) == 0 )
		{
			ChangeColor(msg.pSender);
		}
		else if ( _tcsicmp(msg.pSender->GetName(), _T("ChooseImage")) == 0 )
		{
			ChangeImage(msg.pSender);
		}
	}
}

void CPropertiesSheet::ChangeImage(CControlUI* pSender)
{
	if (pSender == NULL)
		return;

	AfxMessageBox(_T("只支持和皮肤文件在同一路径下或者皮肤文件路径的子目录下的图片！"));

	if (m_pDesignerCanvas != NULL)
	{
		CControlUI* pParent = pSender->GetParent();
		if ((pParent != NULL) && ((static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pParent->GetInterface(_T("Container"))) != NULL)))
		{
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
			for( int it = 0; it < pContainerUI->GetCount(); it++ )
			{
				CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
				if (pControl->GetInterface(_T("Edit")) != NULL)
				{
					CImageDialog dlg(pControl->GetText().GetData(), m_pDesignerCanvas);
					if ((dlg.DoModal()==IDOK) && (m_pHostControlUI != NULL))
					{
						CString strImage = dlg.GetImage();
						pControl->SetText(strImage.GetBuffer());
					}
				}
			}
		}
	}
}

void CPropertiesSheet::ChangeColor(CControlUI* pSender)
{
	if (pSender == NULL)
		return;

	COLORREF clrInit = RGB(255,255,255);

	if (m_pHostControlUI != NULL)
	{
		CControlUI* pParent = pSender->GetParent();
		if ((pParent != NULL) && ((static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pParent->GetInterface(_T("Container"))) != NULL)))
		{
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
			for( int it = 0; it < pContainerUI->GetCount(); it++ )
			{
				CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
				if (pControl->GetInterface(_T("Edit")) != NULL)
				{
					CStdString cstdText = static_cast<CEditUI*>(pControl->GetInterface(_T("Edit")))->GetText();
					if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
					{
						cstdText = cstdText.Mid(1);
						LPTSTR pstr = NULL;
						DWORD dwColor = _tcstoul(cstdText, &pstr, 16);

						clrInit = RGB(static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
					}
				}
			}
		}
	}

	CColorDialog dlg(clrInit);

	if ((dlg.DoModal() == IDOK) && (m_pHostControlUI != NULL))
	{
		TCHAR szBuf[MAX_PATH] = {0};
		COLORREF crColor = dlg.GetColor();
		_stprintf(szBuf, _T("#FF%02X%02X%02X"), GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
		
		CControlUI* pParent = pSender->GetParent();
		if ((pParent != NULL) && ((static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pParent->GetInterface(_T("Container"))) != NULL)))
		{
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
			for( int it = 0; it < pContainerUI->GetCount(); it++ )
			{
				CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
				if (pControl->GetInterface(_T("Edit")) != NULL)
				{
					pControl->SetText(szBuf);
				}
			}
		}
	}
}

LRESULT CPropertiesSheet::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
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
	CControlUI* pRoot = builder.Create(_T("uidesigner_properties.xml"), (UINT)0,  &cb, &paint_manager_);
#else
	tstrSkin = paint_manager_.GetResourcePath() + _T("uidesigner_properties.xml");
	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
#endif
	ASSERT(pRoot && _T("Failed to parse XML"));
	paint_manager_.AttachDialog(pRoot);
	paint_manager_.AddNotifier(this);

	Init();
	return 0;
}

void CPropertiesSheet::OnFinalMessage(HWND hWnd)
{
	//ReleaseCapture();
	__super::OnFinalMessage(hWnd);
	delete this;
}

LRESULT CPropertiesSheet::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPropertiesSheet::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CPropertiesSheet::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CPropertiesSheet::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPropertiesSheet::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPropertiesSheet::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CPropertiesSheet::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CPropertiesSheet::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CPropertiesSheet::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) {
		bHandled = TRUE;
		return 0;
	}		
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CPropertiesSheet::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

	//if (GetCapture() != m_hWnd)
	//	SetCapture(m_hWnd);
	//
	//if (WM_LBUTTONDOWN == uMsg)
	//{
	//	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	//	ClientToScreen(m_hWnd, &pt);
	//	RECT rcParent = {0};
	//	GetWindowRect(m_hWnd, &rcParent);
	//	if (!PtInRect(&rcParent, pt) && (m_pDesignerCanvas != NULL))
	//		m_pDesignerCanvas->ClosePropertiesSheet();
	//}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CPropertiesSheet::DetachControlUI()
{
	bool bRelayoutSpecialTab = false;
	if (m_pHostControlUI != NULL)
		bRelayoutSpecialTab = true;

	m_pHostControlUI = NULL;

	CTabLayoutUI* pTabLayoutUI = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("ProtertiesTab")));
	if (bRelayoutSpecialTab && (pTabLayoutUI != NULL))
	{
		if (m_pSpecialPropertiesTab != NULL)
		{
			pTabLayoutUI->SetAutoDestroy(true);
			pTabLayoutUI->Remove(m_pSpecialPropertiesTab);
			m_pSpecialPropertiesTab = NULL;
		}

		LoadSpecialPropertiesTab(pTabLayoutUI);
	}

	UpdateLayout();
}

void CPropertiesSheet::AttachControlUI(CControlUI* pHostControlUI)
{
	CTabLayoutUI* pTabLayoutUI = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("ProtertiesTab")));
	if ((pTabLayoutUI != NULL) && (pHostControlUI != m_pHostControlUI))
	{
		if (m_pSpecialPropertiesTab != NULL)
		{
			pTabLayoutUI->SetAutoDestroy(true);
			pTabLayoutUI->Remove(m_pSpecialPropertiesTab);
			m_pSpecialPropertiesTab = NULL;
		}

		m_pHostControlUI = pHostControlUI;
		LoadSpecialPropertiesTab(pTabLayoutUI);

		UpdateLayout();
	}
	else
	{
		m_pHostControlUI = pHostControlUI;
		UpdateLayout();
	}
}

void CPropertiesSheet::SetEditUIReadOnly(CControlUI* pControlUI, bool bReadOnly)
{
	if (pControlUI != NULL)
	{
		CEditUI* pEditUI = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")));
		if (pEditUI != NULL)
		{
			pEditUI->SetReadOnly(bReadOnly);
		}
	}
}

void CPropertiesSheet::UpdateLayout()
{
	LoadCommonProperties();
	LoadSpecialProperties();

	// enable or disable the appling button
	CControlUI* pControlUI = paint_manager_.FindControl(_T("applybtn"));
	if (pControlUI != NULL)
		static_cast<CButtonUI*>(pControlUI->GetInterface(_T("Button")))->SetEnabled((m_pHostControlUI != NULL)?true:false);
}

void CPropertiesSheet::ApplySettings()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;

	pControlUI = paint_manager_.FindControl(_T("absolutepath"));
	if ((pControlUI != NULL) && static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)
	{
		AfxMessageBox(_T("请把图片的路径改成相对路径！"));
		return;
	}

	if (m_pHostControlUI != NULL)
	{
		paint_manager_.SetFocus(NULL);
		ApplyCommonProperties();

		ApplySpecialProperties();

		// make the control update
		m_pHostControlUI->NeedParentUpdate();
	}
}

void CPropertiesSheet::LoadCommonProperties()
{
	CContainerUI* pControl = static_cast<CContainerUI*>(paint_manager_.FindControl(_T("CommonProperties")));
	if ((pControl == NULL) || !pControl->IsVisible())
		return;

	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;

	pControlUI = paint_manager_.FindControl(_T("type"));
	if (pControlUI != NULL)
	{
		CString class_name;
		if (m_pHostControlUI!= NULL)			
		{
			class_name = m_pHostControlUI->GetClass();
			class_name = class_name.Mid(0, class_name.GetLength() - 2);
		}
		else
			class_name = _T("");
		pControlUI->SetText(class_name.GetBuffer());
	}

	pControlUI = paint_manager_.FindControl(_T("name"));
	if (pControlUI != NULL)
	{
		pControlUI->SetText((m_pHostControlUI != NULL)?m_pHostControlUI->GetName():_T(""));
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("width"));
	if (pControlUI != NULL)
	{
		if (m_pHostControlUI != NULL)
			_stprintf(szBuf, _T("%d"), m_pHostControlUI->GetFixedWidth());
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("height"));
	if (pControlUI != NULL)
	{
		if (m_pHostControlUI != NULL)
			_stprintf(szBuf, _T("%d"), m_pHostControlUI->GetFixedHeight());
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("absolutepath"));
	if (pControlUI != NULL)
		static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->SelectItem(((m_pHostControlUI != NULL) && m_pHostControlUI->IsImageAbsolutePath())?0:1);

	pControlUI = paint_manager_.FindControl(_T("float"));
	if (pControlUI != NULL)
		static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->SelectItem(((m_pHostControlUI != NULL) && m_pHostControlUI->IsFloat())?0:1);

	pControlUI = paint_manager_.FindControl(_T("visible"));
	if (pControlUI != NULL)
		static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->SelectItem(((m_pHostControlUI != NULL) && m_pHostControlUI->IsVisible())?0:1);

	pControlUI = paint_manager_.FindControl(_T("Text"));
	if (pControlUI != NULL)
	{
		pControlUI->SetText((m_pHostControlUI != NULL)?m_pHostControlUI->GetText():_T(""));
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("Tooltip"));
	if (pControlUI != NULL)
	{
		pControlUI->SetText((m_pHostControlUI != NULL)?m_pHostControlUI->GetToolTip():_T(""));
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("bkcolor"));
	if (pControlUI != NULL)
	{
		DWORD dwColor = (m_pHostControlUI != NULL)?m_pHostControlUI->GetBkColor():0xFF000000;
		
		_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("bkcolor2"));
	if (pControlUI != NULL)
	{
		DWORD dwColor = (m_pHostControlUI != NULL)?m_pHostControlUI->GetBkColor2():0xFF000000;
		_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("bkimage"));
	if (pControlUI != NULL)
	{
		pControlUI->SetText((m_pHostControlUI != NULL)?m_pHostControlUI->GetBkImage():_T(""));
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("bordercolor"));
	if (pControlUI != NULL)
	{
		DWORD dwColor = (m_pHostControlUI != NULL)?m_pHostControlUI->GetBorderColor():0xFF000000;
		_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("focusbordercolor"));
	if (pControlUI != NULL)
	{
		DWORD dwColor = (m_pHostControlUI != NULL)?m_pHostControlUI->GetFocusBorderColor():0xFF000000;
		_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("bordersize"));
	if (pControlUI != NULL)
	{
		DWORD dwBorderSize = (m_pHostControlUI != NULL)?m_pHostControlUI->GetBorderSize():0;
		_stprintf(szBuf, _T("%d"), dwBorderSize);
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("padding_left"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetPadding().left:0);		
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("padding_top"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetPadding().top:0);
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("padding_right"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetPadding().right:0);		
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("padding_bottom"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetPadding().bottom:0);		
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("Relative_Move"));
	if (pControlUI != NULL)
	{
		SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("Relative_Zoom"));
	if (pControlUI != NULL)
	{
		SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("minwidth"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetMinWidth():0);		
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("maxwidth"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetMaxWidth():0);		
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("minheight"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetMinHeight():0);		
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}

	pControlUI = paint_manager_.FindControl(_T("maxheight"));
	if (pControlUI != NULL)
	{
		_stprintf(szBuf, _T("%d"), (m_pHostControlUI != NULL)?m_pHostControlUI->GetMaxHeight():0);		
		pControlUI->SetText(szBuf);
		if (m_pHostControlUI !=NULL)
			SetEditUIReadOnly(pControlUI, false);
		else
			SetEditUIReadOnly(pControlUI, true);
	}
}

void CPropertiesSheet::ApplyCommonProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;

	pControlUI = paint_manager_.FindControl(_T("name"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		if (!cstdText.IsEmpty())
			m_pHostControlUI->SetName(cstdText);
	}

	pControlUI = paint_manager_.FindControl(_T("width"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int cx = _ttoi(cstdText);
		m_pHostControlUI->SetFixedWidth(cx);
	}

	pControlUI = paint_manager_.FindControl(_T("height"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int cy = _ttoi(cstdText);
		m_pHostControlUI->SetFixedHeight(cy);
	}

	pControlUI = paint_manager_.FindControl(_T("float"));
	if (pControlUI != NULL)
		m_pHostControlUI->SetFloat(static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0);

	pControlUI = paint_manager_.FindControl(_T("absolutepath"));
	if (pControlUI != NULL)
		m_pHostControlUI->SetImageAbsolutePath(static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0);

	pControlUI = paint_manager_.FindControl(_T("visible"));
	if (pControlUI != NULL)
		m_pHostControlUI->SetVisible(static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0);

	pControlUI = paint_manager_.FindControl(_T("Text"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		m_pHostControlUI->SetText(cstdText);
	}

	pControlUI = paint_manager_.FindControl(_T("Tooltip"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		m_pHostControlUI->SetToolTip(cstdText);
	}

	pControlUI = paint_manager_.FindControl(_T("bkcolor"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

		if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
		{
			cstdText = cstdText.Mid(1);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
			m_pHostControlUI->SetBkColor(clrColor);
		}
	}

	pControlUI = paint_manager_.FindControl(_T("bkcolor2"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

		if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
		{
			cstdText = cstdText.Mid(1);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
			m_pHostControlUI->SetBkColor2(clrColor);
		}
	}

	pControlUI = paint_manager_.FindControl(_T("bkimage"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		m_pHostControlUI->SetBkImage(cstdText);
	}

	pControlUI = paint_manager_.FindControl(_T("bordercolor"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

		if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
		{
			cstdText = cstdText.Mid(1);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
			m_pHostControlUI->SetBorderColor(clrColor);
		}
	}

	pControlUI = paint_manager_.FindControl(_T("focusbordercolor"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

		if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
		{
			cstdText = cstdText.Mid(1);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
			m_pHostControlUI->SetFocusBorderColor(clrColor);
		}
	}

	pControlUI = paint_manager_.FindControl(_T("bordersize"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int bordersize = _ttoi(cstdText);
		m_pHostControlUI->SetBorderSize(bordersize);
	}

	RECT rcPending = {0};
	pControlUI = paint_manager_.FindControl(_T("padding_left"));
	if (pControlUI != NULL)
		rcPending.left = _ttoi(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

	pControlUI = paint_manager_.FindControl(_T("padding_top"));
	if (pControlUI != NULL)
		rcPending.top = _ttoi(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

	pControlUI = paint_manager_.FindControl(_T("padding_right"));
	if (pControlUI != NULL)
		rcPending.right = _ttoi(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

	pControlUI = paint_manager_.FindControl(_T("padding_bottom"));
	if (pControlUI != NULL)
		rcPending.bottom = _ttoi(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

	m_pHostControlUI->SetPadding(rcPending);

	pControlUI = paint_manager_.FindControl(_T("bordersize"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int bordersize = _ttoi(cstdText);
		m_pHostControlUI->SetBorderSize(bordersize);
	}

	pControlUI = paint_manager_.FindControl(_T("minwidth"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int nMinWidth = _ttoi(cstdText);
		m_pHostControlUI->SetMinWidth(nMinWidth);
	}

	pControlUI = paint_manager_.FindControl(_T("maxwidth"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int nMaxWidth = _ttoi(cstdText);
		m_pHostControlUI->SetMaxWidth(nMaxWidth);
	}

	pControlUI = paint_manager_.FindControl(_T("minheight"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int nMinHeight = _ttoi(cstdText);
		m_pHostControlUI->SetMinHeight(nMinHeight);
	}

	pControlUI = paint_manager_.FindControl(_T("maxheight"));
	if (pControlUI != NULL)
	{
		CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
		int nMaxHeight = _ttoi(cstdText);
		m_pHostControlUI->SetMaxHeight(nMaxHeight);
	}

	if ((m_pDesignerCanvas != NULL) && (m_pHostControlUI->GetParent() == NULL) && (static_cast<IContainerUI*>(m_pHostControlUI->GetInterface(_T("IContainer"))) != NULL))
	{
		// 根画布，一般都不是float的，随着窗口的大小而定，如果要改变画布的大小则需要调整窗口大小
		int nWidth = 0;
		int nHeight = 0;
		pControlUI = paint_manager_.FindControl(_T("width"));
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

		if ((nWidth != 0) && (nHeight != 0))
		{
			SIZE szNewSize;
			szNewSize.cx = nWidth;
			szNewSize.cy = nHeight;
			m_pDesignerCanvas->SetFormSize(szNewSize);

			RECT rcClient = {0};
			GetClientRect(GetParent(*m_pDesignerCanvas), &rcClient);

			RECT rcPropertiesSheetWndRect = {0};
			::GetWindowRect(*this, &rcPropertiesSheetWndRect);
			if ((rcPropertiesSheetWndRect.bottom - rcPropertiesSheetWndRect.top) <= PROPERTIES_SHEET_MIN_HEIGHT)
			{
				RECT rcCanvas = rcClient;
				rcCanvas.top += TOOLBAR_HEIGHT + 2 * MARGIN;;
				rcCanvas.left = rcClient.left + TOOLBOX_WIDTH + 4 * MARGIN + ADJUSTMENTTOOLBAR_WIDTH;
				rcCanvas.right = rcCanvas.left + nWidth;
				::MoveWindow(*m_pDesignerCanvas, rcCanvas.left + MARGIN, rcCanvas.top + MARGIN, nWidth, nHeight, TRUE);
				
				InvalidateRect(GetParent(*m_pDesignerCanvas), NULL, TRUE);
				UpdateWindow(GetParent(*m_pDesignerCanvas));
			}
			else
			{
				DWORD dwResizeCanvasWidth = nWidth;
				RECT rcCanvas = rcClient;
				rcCanvas.top += TOOLBAR_HEIGHT + 2 * MARGIN;;
				rcCanvas.left = rcClient.left + TOOLBOX_WIDTH + 4 * MARGIN + ADJUSTMENTTOOLBAR_WIDTH;
				rcCanvas.right -= PROPERTIES_SHEET_WIDTH + MARGIN;
				if (rcCanvas.right > (rcCanvas.left + nWidth))
					dwResizeCanvasWidth = nWidth;
				else
					dwResizeCanvasWidth = rcCanvas.right - rcCanvas.left;

				::MoveWindow(*m_pDesignerCanvas, rcCanvas.left + MARGIN, rcCanvas.top + MARGIN, dwResizeCanvasWidth, nHeight, TRUE);

				UpdateLayout();
			}
		}
	}
}

void CPropertiesSheet::LoadSpecialPropertiesTab(CTabLayoutUI* pTabLayoutUI)
{
	if (pTabLayoutUI != NULL)
	{
		if ((m_pSpecialPropertiesTab == NULL) && (m_pHostControlUI == NULL))
		{
/*
		  <VerticalLayout name="Special" bkcolor="#FFFFFFFF" vscrollbar="true">
			<VerticalLayout height="30">
			  <HorizontalLayout height="30">
			  </HorizontalLayout>
			</VerticalLayout>
		  </VerticalLayout>
*/
			m_pSpecialPropertiesTab = new CVerticalLayoutUI;
			if (m_pSpecialPropertiesTab != NULL)
			{
				m_pSpecialPropertiesTab->SetName(_T("Special"));

				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(_T("FFFFFFFF"), &pstr, 16);
				m_pSpecialPropertiesTab->SetBkColor(clrColor);
				static_cast<CVerticalLayoutUI*>(m_pSpecialPropertiesTab)->EnableScrollBar(true, false);
				m_pSpecialPropertiesTab->SetFloat(false);
				static_cast<CVerticalLayoutUI*>(m_pSpecialPropertiesTab)->SetAutoDestroy(true);
				pTabLayoutUI->Add(m_pSpecialPropertiesTab);
			}
		}
		else
		{
			if (m_pHostControlUI != NULL)
			{
				pTabLayoutUI->SetAutoDestroy(true);
				pTabLayoutUI->Remove(m_pSpecialPropertiesTab);
				m_pSpecialPropertiesTab = NULL;

				UINT nID = 0;
				if (_tcsicmp(m_pHostControlUI->GetClass(), _T("LabelUI")) == 0)
				{
					nID = IDR_LabelUI;
				}
				if (_tcsicmp(m_pHostControlUI->GetClass(), _T("TextUI")) == 0)
				{
					nID = IDR_LabelUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ButtonUI")) == 0)
				{
					nID = IDR_ButtonUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("OptionUI")) == 0)
				{
					nID = IDR_OptionUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ProgressUI")) == 0)
				{
					nID = IDR_ProgressUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("SliderUI")) == 0)
				{
					nID = IDR_SliderUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("EditUI")) == 0)
				{
					nID = IDR_EditUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ScrollBarUI")) == 0)
				{
					nID = IDR_ScrollbarUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ComboUI")) == 0)
				{
					nID = IDR_ComboUI;
				}
				else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ListUI")) == 0)
				{
					nID = IDR_ListUI;
				}

				STRINGorID xml(nID);
				STRINGorID type(_T("Properties"));
				CDialogBuilder builder;
				CDialogBuilderCallbackEx cb;

				m_pSpecialPropertiesTab = builder.Create(xml, type,  &cb, &paint_manager_);
				if (m_pSpecialPropertiesTab != NULL)
				{
					m_pSpecialPropertiesTab->SetFloat(false);
					static_cast<CVerticalLayoutUI*>(m_pSpecialPropertiesTab)->SetAutoDestroy(true);
					pTabLayoutUI->AddAt(m_pSpecialPropertiesTab, 1);
					UpdateLayout();
				}
				else
				{
					m_pSpecialPropertiesTab = new CVerticalLayoutUI;
					if (m_pSpecialPropertiesTab != NULL)
					{
						m_pSpecialPropertiesTab->SetName(_T("Special"));

						LPTSTR pstr = NULL;
						DWORD clrColor = _tcstoul(_T("FFFFFFFF"), &pstr, 16);
						m_pSpecialPropertiesTab->SetBkColor(clrColor);
						static_cast<CVerticalLayoutUI*>(m_pSpecialPropertiesTab)->EnableScrollBar(true, false);
						m_pSpecialPropertiesTab->SetFloat(false);
						static_cast<CVerticalLayoutUI*>(m_pSpecialPropertiesTab)->SetAutoDestroy(true);
						pTabLayoutUI->Add(m_pSpecialPropertiesTab);
					}
				}
			}
		}
		COptionUI* pOptionUI = static_cast<COptionUI*>(paint_manager_.FindControl(_T("CommonProperties")));
		if (pOptionUI != NULL)
			pOptionUI->Selected(true);
	}
}


void CPropertiesSheet::LoadSpecialProperties()
{
	CContainerUI* pControl = static_cast<CContainerUI*>(paint_manager_.FindControl(_T("SpecialProperties")));
	if ((pControl == NULL) || !pControl->IsVisible())
		return;

	if (m_pHostControlUI != NULL)
	{
		if (_tcsicmp(m_pHostControlUI->GetClass(), _T("LabelUI")) == 0)
		{
			LoadLabelUIProperties();
		}
		if (_tcsicmp(m_pHostControlUI->GetClass(), _T("TextUI")) == 0)
		{
			LoadLabelUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ButtonUI")) == 0)
		{
			LoadButtonUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("OptionUI")) == 0)
		{
			LoadOptionUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ProgressUI")) == 0)
		{
			LoadProgressUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("SliderUI")) == 0)
		{
			LoadSliderUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("EditUI")) == 0)
		{
			LoadEditUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ScrollBarUI")) == 0)
		{
			LoadScrollbarUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ComboUI")) == 0)
		{
			LoadComboUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ListUI")) == 0)
		{
			LoadListUIProperties();
		}
	}
}

void CPropertiesSheet::ApplySpecialProperties()
{
	if (m_pHostControlUI != NULL)
	{
		if (_tcsicmp(m_pHostControlUI->GetClass(), _T("LabelUI")) == 0)
		{
			ApplyLabelUIProperties();
		}
		if (_tcsicmp(m_pHostControlUI->GetClass(), _T("TextUI")) == 0)
		{
			ApplyLabelUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ButtonUI")) == 0)
		{
			ApplyButtonUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("OptionUI")) == 0)
		{
			ApplyOptionUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ProgressUI")) == 0)
		{
			ApplyProgressUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("SliderUI")) == 0)
		{
			ApplySliderUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("EditUI")) == 0)
		{
			ApplyEditUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ScrollBarUI")) == 0)
		{
			ApplyScrollbarUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ComboUI")) == 0)
		{
			ApplyComboUIProperties();
		}
		else if (_tcsicmp(m_pHostControlUI->GetClass(), _T("ListUI")) == 0)
		{
			ApplyListUIProperties();
		}
	}
}

void CPropertiesSheet::LoadListUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	CListUI* pListUI = NULL;
	if ((m_pHostControlUI != NULL) && ((pListUI = static_cast<CListUI*>(m_pHostControlUI->GetInterface(_T("CList")))) != NULL))
	{
		pControlUI = paint_manager_.FindControl(_T("textpadding_left"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pListUI->GetItemTextPadding().left);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_top"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pListUI->GetItemTextPadding().top);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_right"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pListUI->GetItemTextPadding().right);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_bottom"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pListUI->GetItemTextPadding().bottom);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pListUI->GetItemImage());

		pControlUI = paint_manager_.FindControl(_T("SelectedItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetSelectedItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("SelectedItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetSelectedItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("SelectedItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pListUI->GetSelectedItemImage());

		pControlUI = paint_manager_.FindControl(_T("HotItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetHotItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("HotItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetHotItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("GetHotItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pListUI->GetSelectedItemImage());

		pControlUI = paint_manager_.FindControl(_T("DisabledItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetDisabledItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetDisabledItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pListUI->GetDisabledItemImage());

		pControlUI = paint_manager_.FindControl(_T("ItemLineColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pListUI->GetItemLineColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemShowhtml"));
		if (pControlUI != NULL)
		{
			int nIndex = pListUI->IsItemShowHtml()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}
	}
}

void CPropertiesSheet::ApplyListUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	CListUI* pListUI = NULL;
	if ((m_pHostControlUI != NULL) && ((pListUI = static_cast<CListUI*>(m_pHostControlUI->GetInterface(_T("CList")))) != NULL))
	{

		RECT rcItemTextPadding = {0};
		pControlUI = paint_manager_.FindControl(_T("textpadding_left"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.left = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_top"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.top = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_right"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.right = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_bottom"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.bottom = _ttoi(cstdText);
		}

		pListUI->SetItemTextPadding(rcItemTextPadding);

		pControlUI = paint_manager_.FindControl(_T("ItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("ItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetItemBkColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("ItemImage"));
		if (pControlUI != NULL)
			pListUI->SetItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("SelectedItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetSelectedItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("SelectedItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetSelectedItemBkColor(clrColor);
			}
		}


		pControlUI = paint_manager_.FindControl(_T("SelectedItemImage"));
		if (pControlUI != NULL)
			pListUI->SetSelectedItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("HotItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetHotItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("HotItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetHotItemBkColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("GetHotItemImage"));
		if (pControlUI != NULL)
			pListUI->SetHotItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("DisabledItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetDisabledItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetDisabledItemBkColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemImage"));
		if (pControlUI != NULL)
			pListUI->SetDisabledItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("ItemLineColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pListUI->SetItemLineColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("ItemShowhtml"));
		if (pControlUI != NULL)
			pListUI->SetItemShowHtml((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
	}
}

void CPropertiesSheet::LoadComboUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	CComboUI* pComboUI = NULL;
	if ((m_pHostControlUI != NULL) && ((pComboUI = static_cast<CComboUI*>(m_pHostControlUI->GetInterface(_T("Combo")))) != NULL))
	{
		pControlUI = paint_manager_.FindControl(_T("DropBox"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pComboUI->GetDropBoxSize().cy);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("DropBtnImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pComboUI->GetDropBtnImage());

		pControlUI = paint_manager_.FindControl(_T("DropType"));
		if (pControlUI != NULL)
		{
			int nIndex = pComboUI->GetDropType() - 1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_left"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pComboUI->GetItemTextPadding().left);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_top"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pComboUI->GetItemTextPadding().top);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_right"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pComboUI->GetItemTextPadding().right);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_bottom"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pComboUI->GetItemTextPadding().bottom);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pComboUI->GetItemImage());

		pControlUI = paint_manager_.FindControl(_T("SelectedItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetSelectedItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("SelectedItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetSelectedItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("SelectedItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pComboUI->GetSelectedItemImage());

		pControlUI = paint_manager_.FindControl(_T("HotItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetHotItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("HotItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetHotItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("GetHotItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pComboUI->GetSelectedItemImage());

		pControlUI = paint_manager_.FindControl(_T("DisabledItemTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetDisabledItemTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemBkColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetDisabledItemBkColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pComboUI->GetDisabledItemImage());

		pControlUI = paint_manager_.FindControl(_T("ItemLineColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pComboUI->GetItemLineColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ItemShowhtml"));
		if (pControlUI != NULL)
		{
			int nIndex = pComboUI->IsItemShowHtml()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}
	}
}

void CPropertiesSheet::ApplyComboUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	CComboUI* pComboUI = NULL;
	if ((m_pHostControlUI != NULL) && ((pComboUI = static_cast<CComboUI*>(m_pHostControlUI->GetInterface(_T("Combo")))) != NULL))
	{
		pControlUI = paint_manager_.FindControl(_T("DropBox"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			SIZE dropSize;
			dropSize.cx = 0;
			dropSize.cy = _ttoi(cstdText);
			pComboUI->SetDropBoxSize(dropSize);
		}

		pControlUI = paint_manager_.FindControl(_T("DropBtnImage"));
		if (pControlUI != NULL)
			pComboUI->SetDropBtnImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("DropType"));
		if (pControlUI != NULL)
		{
			int nIndex = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() + 1;
			pComboUI->SetDropType(nIndex);
		}

		RECT rcItemTextPadding = {0};
		pControlUI = paint_manager_.FindControl(_T("textpadding_left"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.left = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_top"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.top = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_right"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.right = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_bottom"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcItemTextPadding.bottom = _ttoi(cstdText);
		}

		pComboUI->SetItemTextPadding(rcItemTextPadding);

		pControlUI = paint_manager_.FindControl(_T("ItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("ItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetItemBkColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("ItemImage"));
		if (pControlUI != NULL)
			pComboUI->SetItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("SelectedItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetSelectedItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("SelectedItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetSelectedItemBkColor(clrColor);
			}
		}


		pControlUI = paint_manager_.FindControl(_T("SelectedItemImage"));
		if (pControlUI != NULL)
			pComboUI->SetSelectedItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("HotItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetHotItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("HotItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetHotItemBkColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("GetHotItemImage"));
		if (pControlUI != NULL)
			pComboUI->SetHotItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("DisabledItemTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetDisabledItemTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemBkColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetDisabledItemBkColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("DisabledItemImage"));
		if (pControlUI != NULL)
			pComboUI->SetDisabledItemImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("ItemLineColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pComboUI->SetItemLineColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("ItemShowhtml"));
		if (pControlUI != NULL)
			pComboUI->SetItemShowHtml((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
	}
}

void CPropertiesSheet::LoadScrollbarUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	CScrollBarUI* pScrollbarUI = NULL;
	if ((m_pHostControlUI != NULL) && ((pScrollbarUI = static_cast<CScrollBarUI*>(m_pHostControlUI->GetInterface(_T("ScrollBar")))) != NULL))
	{
		pControlUI = paint_manager_.FindControl(_T("ScrollRange"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pScrollbarUI->GetScrollRange());
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("ScrollPos"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pScrollbarUI->GetScrollPos());
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("LineSize"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pScrollbarUI->GetLineSize());
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("Horizontal"));
		if (pControlUI != NULL)
		{
			int nIndex = pScrollbarUI->IsHorizontal()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}

		pControlUI = paint_manager_.FindControl(_T("Button1NormalImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton1NormalImage());

		pControlUI = paint_manager_.FindControl(_T("Button1HotImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton1HotImage());

		pControlUI = paint_manager_.FindControl(_T("Button1PushedImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton1PushedImage());

		pControlUI = paint_manager_.FindControl(_T("Button1DisabledImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton1DisabledImage());

		pControlUI = paint_manager_.FindControl(_T("Button2NormalImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton2NormalImage());

		pControlUI = paint_manager_.FindControl(_T("Button2HotImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton2HotImage());

		pControlUI = paint_manager_.FindControl(_T("Button2PushedImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton2PushedImage());

		pControlUI = paint_manager_.FindControl(_T("Button2DisabledImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetButton2DisabledImage());
		pControlUI = paint_manager_.FindControl(_T("ThumbNormalImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetThumbNormalImage());

		pControlUI = paint_manager_.FindControl(_T("ThumbHotImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetThumbHotImage());

		pControlUI = paint_manager_.FindControl(_T("ThumbPushedImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetThumbPushedImage());

		pControlUI = paint_manager_.FindControl(_T("ThumbDisabledImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetThumbDisabledImage());

		pControlUI = paint_manager_.FindControl(_T("RailNormalImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetRailNormalImage());

		pControlUI = paint_manager_.FindControl(_T("RailHotImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetRailHotImage());

		pControlUI = paint_manager_.FindControl(_T("RailPushedImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetRailPushedImage());

		pControlUI = paint_manager_.FindControl(_T("RailDisabledImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetRailDisabledImage());

		pControlUI = paint_manager_.FindControl(_T("BkNormalImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetBkNormalImage());

		pControlUI = paint_manager_.FindControl(_T("BkHotImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetBkHotImage());

		pControlUI = paint_manager_.FindControl(_T("BkPushedImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetBkPushedImage());

		pControlUI = paint_manager_.FindControl(_T("BkDisabledImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pScrollbarUI->GetBkDisabledImage());
	}
}

void CPropertiesSheet::ApplyScrollbarUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	CScrollBarUI* pScrollbarUI = NULL;
	if ((m_pHostControlUI != NULL) && ((pScrollbarUI = static_cast<CScrollBarUI*>(m_pHostControlUI->GetInterface(_T("ScrollBar")))) != NULL))
	{
		pControlUI = paint_manager_.FindControl(_T("ScrollRange"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			int nRange = _ttoi(cstdText);
			pScrollbarUI->SetScrollRange(nRange);
		}

		pControlUI = paint_manager_.FindControl(_T("ScrollPos"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			int nPos = _ttoi(cstdText);
			pScrollbarUI->SetScrollPos(nPos);
		}

		pControlUI = paint_manager_.FindControl(_T("LineSize"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			int nLineSize = _ttoi(cstdText);
			pScrollbarUI->SetLineSize(nLineSize);
		}

		pControlUI = paint_manager_.FindControl(_T("Horizontal"));
		if (pControlUI != NULL)
		{
			pScrollbarUI->SetHorizontal((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
		}

		pControlUI = paint_manager_.FindControl(_T("Button1NormalImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton1NormalImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("Button1HotImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton1HotImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("Button1PushedImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton1PushedImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("Button1DisabledImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton1DisabledImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("Button2NormalImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton2NormalImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("Button2HotImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton2HotImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("Button2PushedImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton2PushedImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("Button2DisabledImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetButton2DisabledImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("ThumbNormalImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetThumbNormalImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("ThumbHotImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetThumbHotImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("ThumbPushedImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetThumbPushedImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("ThumbDisabledImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetThumbDisabledImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("RailNormalImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetRailNormalImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("RailHotImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetRailHotImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("RailPushedImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetRailPushedImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("RailDisabledImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetRailDisabledImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("BkNormalImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetBkNormalImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("BkHotImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetBkHotImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("BkPushedImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetBkPushedImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());

		pControlUI = paint_manager_.FindControl(_T("BkDisabledImage"));
		if (pControlUI != NULL)
			pScrollbarUI->SetBkDisabledImage(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());
	}
}

void CPropertiesSheet::LoadEditUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Edit")) != NULL))
	{
		CEditUI* pEditUI = static_cast<CEditUI*>(m_pHostControlUI->GetInterface(_T("Edit")));
		LoadLabelUIProperties();

		pControlUI = paint_manager_.FindControl(_T("PasswordChar"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%c"), pEditUI->GetPasswordChar());
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("readonly"));
		if (pControlUI != NULL)
		{
			int nIndex = pEditUI->IsReadOnly()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}

		pControlUI = paint_manager_.FindControl(_T("digitalNumber"));
		if (pControlUI != NULL)
		{
			int nIndex = pEditUI->IsDigitalNumber()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}

		pControlUI = paint_manager_.FindControl(_T("PasswordMode"));
		if (pControlUI != NULL)
		{
			int nIndex = pEditUI->IsPasswordMode()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}
	}
}

void CPropertiesSheet::ApplyEditUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Edit")) != NULL))
	{
		CEditUI* pEditUI = static_cast<CEditUI*>(m_pHostControlUI->GetInterface(_T("Edit")));
		ApplyLabelUIProperties();

		pControlUI = paint_manager_.FindControl(_T("PasswordChar"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			TCHAR cPassword = cstdText.IsEmpty()?_T('*'):cstdText.GetAt(0);
			pEditUI->SetPasswordChar(cPassword);
		}

		pControlUI = paint_manager_.FindControl(_T("readonly"));
		if (pControlUI != NULL)
		{
			pEditUI->SetReadOnly((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
		}

		pControlUI = paint_manager_.FindControl(_T("digitalNumber"));
		if (pControlUI != NULL)
		{
			pEditUI->SetDigitalNumber((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
		}

		pControlUI = paint_manager_.FindControl(_T("PasswordMode"));
		if (pControlUI != NULL)
		{
			pEditUI->SetPasswordMode((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
		}
	}
}

void CPropertiesSheet::LoadSliderUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Slider")) != NULL))
	{
		CSliderUI* pSliderUI = static_cast<CSliderUI*>(m_pHostControlUI->GetInterface(_T("Slider")));
		LoadProgressUIProperties();
	}
}

void CPropertiesSheet::ApplySliderUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Slider")) != NULL))
	{
		CSliderUI* pSliderUI = static_cast<CSliderUI*>(m_pHostControlUI->GetInterface(_T("Slider")));
		ApplyProgressUIProperties();
	}
}

void CPropertiesSheet::LoadProgressUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Progress")) != NULL))
	{
		CProgressUI* pProgressUI = static_cast<CProgressUI*>(m_pHostControlUI->GetInterface(_T("Progress")));
		LoadLabelUIProperties();

		pControlUI = paint_manager_.FindControl(_T("FgImage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pProgressUI->GetFgImage());

		pControlUI = paint_manager_.FindControl(_T("MinValue"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pProgressUI->GetMinValue());
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("MaxValue"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pProgressUI->GetMaxValue());
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("Value"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pProgressUI->GetValue());
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("Horizontal"));
		if (pControlUI != NULL)
		{
			int nIndex = pProgressUI->IsHorizontal()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}
	}
}

void CPropertiesSheet::ApplyProgressUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Progress")) != NULL))
	{
		CProgressUI* pProgressUI = static_cast<CProgressUI*>(m_pHostControlUI->GetInterface(_T("Progress")));
		ApplyLabelUIProperties();

		pControlUI = paint_manager_.FindControl(_T("FgImage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pProgressUI->SetFgImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("MinValue"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			int value = _ttoi(cstdText);
			pProgressUI->SetMinValue(value);
		}

		pControlUI = paint_manager_.FindControl(_T("MaxValue"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			int value = _ttoi(cstdText);
			pProgressUI->SetMaxValue(value);
		}

		pControlUI = paint_manager_.FindControl(_T("Value"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			int value = _ttoi(cstdText);
			pProgressUI->SetValue(value);
		}

		pControlUI = paint_manager_.FindControl(_T("Horizontal"));
		if (pControlUI != NULL)
		{
			pProgressUI->SetHorizontal((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
		}
	}
}

void CPropertiesSheet::LoadOptionUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Option")) != NULL))
	{
		COptionUI* pOptionUI = static_cast<COptionUI*>(m_pHostControlUI->GetInterface(_T("Option")));
		LoadButtonUIProperties();

		pControlUI = paint_manager_.FindControl(_T("selectedimage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pOptionUI->GetSelectedImage());

		pControlUI = paint_manager_.FindControl(_T("SelectedTextColor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pOptionUI->GetSelectedTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("group"));
		if (pControlUI != NULL)
		{
			pControlUI->SetText(pOptionUI->GetGroup());
		}

		pControlUI = paint_manager_.FindControl(_T("isselected"));
		if (pControlUI != NULL)
		{
			int nIndex = pOptionUI->IsSelected()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}
	}
}

void CPropertiesSheet::ApplyOptionUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Option")) != NULL))
	{
		COptionUI* pOptionUI = static_cast<COptionUI*>(m_pHostControlUI->GetInterface(_T("Option")));
		ApplyButtonUIProperties();

		pControlUI = paint_manager_.FindControl(_T("selectedimage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pOptionUI->SetSelectedImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("SelectedTextColor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pOptionUI->SetSelectedTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("group"));
		if (pControlUI != NULL)
		{
			pOptionUI->SetGroup(static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText());
		}

		pControlUI = paint_manager_.FindControl(_T("isselected"));
		if (pControlUI != NULL)
		{
			pOptionUI->Selected((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
		}
	}
}

void CPropertiesSheet::LoadButtonUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Button")) != NULL))
	{
		CButtonUI* pButtonUI = static_cast<CButtonUI*>(m_pHostControlUI->GetInterface(_T("Button")));
		LoadLabelUIProperties();

		pControlUI = paint_manager_.FindControl(_T("normalimage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pButtonUI->GetNormalImage());

		pControlUI = paint_manager_.FindControl(_T("hotimage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pButtonUI->GetHotImage());

		pControlUI = paint_manager_.FindControl(_T("pushedimage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pButtonUI->GetPushedImage());

		pControlUI = paint_manager_.FindControl(_T("focusimage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pButtonUI->GetFocusedImage());

		pControlUI = paint_manager_.FindControl(_T("disableimage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pButtonUI->GetDisabledImage());

		pControlUI = paint_manager_.FindControl(_T("foreimage"));
		if (pControlUI != NULL)
			pControlUI->SetText(pButtonUI->GetForeImage());

		pControlUI = paint_manager_.FindControl(_T("enable"));
		if (pControlUI != NULL)
		{
			int nIndex = pButtonUI->IsEnabled()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}
	}
}

void CPropertiesSheet::ApplyButtonUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && (m_pHostControlUI->GetInterface(_T("Button")) != NULL))
	{
		CButtonUI* pButtonUI = static_cast<CButtonUI*>(m_pHostControlUI->GetInterface(_T("Button")));
		ApplyLabelUIProperties();

		pControlUI = paint_manager_.FindControl(_T("normalimage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pButtonUI->SetNormalImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("hotimage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pButtonUI->SetHotImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("pushedimage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pButtonUI->SetPushedImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("focusimage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pButtonUI->SetFocusedImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("disableimage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pButtonUI->SetDisabledImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("foreimage"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			pButtonUI->SetForeImage(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("enable"));
		if (pControlUI != NULL)
			pButtonUI->SetEnabled((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
	}
}

void CPropertiesSheet::LoadLabelUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && ((m_pHostControlUI->GetInterface(_T("Label")) != NULL) || (m_pHostControlUI->GetInterface(_T("Text")) != NULL)))
	{
		CLabelUI* pLabelUI = static_cast<CLabelUI*>(m_pHostControlUI->GetInterface(_T("Label")));

		pControlUI = paint_manager_.FindControl(_T("textcolor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pLabelUI->GetTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("disabletextcolor"));
		if (pControlUI != NULL)
		{
			DWORD dwColor = pLabelUI->GetDisabledTextColor();
			_stprintf(szBuf, lpszColorRefFormat, HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_left"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pLabelUI->GetTextPadding().left);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_top"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pLabelUI->GetTextPadding().top);
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_right"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pLabelUI->GetTextPadding().right);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_bottom"));
		if (pControlUI != NULL)
		{
			_stprintf(szBuf, _T("%d"), pLabelUI->GetTextPadding().bottom);		
			pControlUI->SetText(szBuf);
		}

		pControlUI = paint_manager_.FindControl(_T("showhtml"));
		if (pControlUI != NULL)
		{
			int nIndex = pLabelUI->IsShowHtml()?0:1;
			CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));
			if (pComboUI->GetCurSel() != nIndex)
				pComboUI->SelectItem(nIndex);
		}
	}
}

void CPropertiesSheet::ApplyLabelUIProperties()
{
	TCHAR szBuf[MAX_PATH] = {0};
	CControlUI* pControlUI = NULL;
	if ((m_pHostControlUI != NULL) && ((m_pHostControlUI->GetInterface(_T("Label")) != NULL) || (m_pHostControlUI->GetInterface(_T("Text")) != NULL)))
	{
		CLabelUI* pLabelUI = static_cast<CLabelUI*>(m_pHostControlUI->GetInterface(_T("Label")));

		pControlUI = paint_manager_.FindControl(_T("textcolor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pLabelUI->SetTextColor(clrColor);
			}
		}

		pControlUI = paint_manager_.FindControl(_T("disabletextcolor"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();

			if ((cstdText.GetLength() == 9) && cstdText.GetAt(0) == _T('#'))
			{
				cstdText = cstdText.Mid(1);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(cstdText, &pstr, 16);
				pLabelUI->SetDisabledTextColor(clrColor);
			}
		}

		RECT rcTextPadding = {0};
		pControlUI = paint_manager_.FindControl(_T("textpadding_left"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcTextPadding.left = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_top"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcTextPadding.top = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_right"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcTextPadding.right = _ttoi(cstdText);
		}

		pControlUI = paint_manager_.FindControl(_T("textpadding_bottom"));
		if (pControlUI != NULL)
		{
			CStdString cstdText = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")))->GetText();
			rcTextPadding.bottom = _ttoi(cstdText);
		}

		pLabelUI->SetTextPadding(rcTextPadding);

		pControlUI = paint_manager_.FindControl(_T("showhtml"));
		if (pControlUI != NULL)
			pLabelUI->SetShowHtml((static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")))->GetCurSel() == 0)?true:false);
	}
}