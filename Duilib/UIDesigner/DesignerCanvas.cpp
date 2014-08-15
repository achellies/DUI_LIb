#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "ToolBox.h"
#include "Toolbar.h"
#include "Resource.h"
#include "ControlType.h"
#include "UIDesignerDlg.h"
#include "DesignerCanvas.h"
#include "DefaultFeatures.h"
#include "PropertiesSheet.h"
#include "DialogBuilderCallback.h"

using DuiLib::EVENTTYPE_UI;

CDesignerCanvas::CDesignerCanvas(CToolBox* pToolBoxWnd)
: m_bSkinFileChanged(false)
, m_pCanvasContainer(NULL)
, m_pPropertiesSheet(NULL)
, m_pToolBox(pToolBoxWnd)
, m_bMouseCaptured(false)
, m_pCopyControl(NULL)
, m_pCutControl(NULL)
, m_pToolbar(NULL)
{
	m_tstrDefaultFeaturesSkinPath = m_tstrSkinPath = CPaintManagerUI::GetInstancePath() + lpszSkinPath;
	m_tstrSkinFile = _T("uidesigner_canvas.xml");	
}

CDesignerCanvas::~CDesignerCanvas()
{
	if (m_pCutControl == m_pCopyControl)
		m_pCopyControl = NULL;

	if (m_pCutControl != NULL)
		DeleteControl(m_pCutControl, true);

	if (m_pCopyControl != NULL)
		DeleteControl(m_pCopyControl, true);
}

_WindowFormInfo CDesignerCanvas::GetFormInfo()
{
	return m_formInfo;
}

CPaintManagerUI* CDesignerCanvas::GetPaintManager()
{
	return &paint_manager_;
}

CString CDesignerCanvas::GetCurrentSkinPath()
{
	return m_tstrSkinPath.c_str();
}

void CDesignerCanvas::SetToolbarPointer(CToolbar* pToolbar)
{
	if (pToolbar != NULL)
		m_pToolbar = pToolbar;
}

void CDesignerCanvas::SetFormSize(SIZE szSize)
{
	m_formInfo.szCanvasSize = szSize;
	if ((szSize.cx != 0) && (szSize.cy != 0) && ((szSize.cx != m_formInfo.szCanvasSize.cx) || (szSize.cy != m_formInfo.szCanvasSize.cy)))
	{
		m_formInfo.szCanvasSize.cx = szSize.cx;
		m_formInfo.szCanvasSize.cy = szSize.cy;
		if (m_pToolbar != NULL)
			m_pToolbar->UpdateFormInfo();
	}
}

void CDesignerCanvas::SetFormInfo(_WindowFormInfo info)
{
	m_formInfo.bOpacity = info.bOpacity;
	m_formInfo.dwTransparency = info.dwTransparency;
	SIZE szSize = info.szCanvasSize;
	if ((szSize.cx != 0) && (szSize.cy != 0) && ((szSize.cx != m_formInfo.szCanvasSize.cx) || (szSize.cy != m_formInfo.szCanvasSize.cy)))
	{
		m_formInfo.szCanvasSize.cx = szSize.cx;
		m_formInfo.szCanvasSize.cy = szSize.cy;
		if (m_pToolbar != NULL)
			m_pToolbar->UpdateFormInfo();
	}
}

void CDesignerCanvas::Init()
{}

void CDesignerCanvas::OnPrepare()
{}

void CDesignerCanvas::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CDesignerCanvas::Notify(TNotifyUI& msg)
{
	if ( _tcsicmp(msg.sType, _T("windowinit")) == 0 )
	{
		OnPrepare();
	}
	else if( _tcsicmp(msg.sType, _T("click")) == 0 )
	{}
	else if( _tcsicmp(msg.sType, _T("itemclick")) == 0 )
	{}
	else if( _tcsicmp(msg.sType , _T("itemactivate")) == 0 )
	{}
}

LRESULT CDesignerCanvas::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	paint_manager_.Init(m_hWnd);
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb;

	paint_manager_.SetAssertWhenCustomDefinedNull(false);
	paint_manager_.SetResourcePath(m_tstrSkinPath.c_str());

#if defined(USING_ZIP_SKIN)
	paint_manager_.SetResourceZip(lpszZipSkin);
	CControlUI* pRoot = builder.Create(m_tstrSkinFile.c_str(), (UINT)0,  &cb, &paint_manager_);
#else
	tString tstrSkin = m_tstrSkinPath + m_tstrSkinFile;
	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
#endif

	ASSERT(pRoot && _T("Failed to parse XML"));
	paint_manager_.AttachDialog(pRoot);
	paint_manager_.AddNotifier(this);
	paint_manager_.SetUsedAsUIDesigner(true);

	Init();
	return 0;
}

LRESULT CDesignerCanvas::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pPropertiesSheet != NULL)
	{
		m_pPropertiesSheet->Close();
		m_pPropertiesSheet = NULL;
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CDesignerCanvas::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CDesignerCanvas::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CDesignerCanvas::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDesignerCanvas::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDesignerCanvas::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CDesignerCanvas::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CDesignerCanvas::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CDesignerCanvas::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) {
		bHandled = TRUE;
		return 0;
	}		
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CDesignerCanvas::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((paint_manager_.FindPitchUponControlCount() > 0))
	{
		CStdPtrArray controls;
		paint_manager_.FinAllPitchUponControl(&controls);

		for (int i = 0; i < controls.GetSize(); ++i)
		{
			TEventUI event = { 0 };
			event.Type = DuiLib::UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			event.wParam = wParam;
			event.lParam = lParam;
			event.dwTimestamp = ::GetTickCount();
			static_cast<CControlUI*>(controls.GetAt(i))->Event(event);
		}
	}

	if (GetKeyState(VK_CONTROL) < 0)
	{
		switch (wParam)
		{
		case 0x58:
			// Ctrl + X
			// 剪切
			CutControl();
			break;
		case 0x43:
			// Ctrl + C
			// 复制
			CopyControl();
			break;
		case 0x56:
			// Ctrl + V
			// 粘贴
			PasteControl();
			break;
		case 0x5A:
			// Ctrl + Z
			// 撤销
			Undo();
			break;
		case 0x41 + ('O' - 'A'):
			// Ctrl + O
			// 打开
			OpenSkinFile();
			break;
		case 0x41 + ('N' - 'A'):
			// Ctrl + N
			// 新建
			NewSkinFile();
			break;
		case 0x41 + ('S' - 'A'):
			// Ctrl + S
			// 保存
			SaveSkinFile();
			break;
		}
	}

	return 0;
}

LRESULT CDesignerCanvas::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_DELETE)
	{
		DeleteControl();
	}
	return 0;
}

LRESULT CDesignerCanvas::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYUP:			lRes = OnKeyUp(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( paint_manager_.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		m_bMouseCaptured = true;
		m_point.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (g_eControlType != eControl_Unkonwn)
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			IContainerUI* pContainer = NULL;
			CControlUI* pParent = NULL;
			CControlUI* pControl = NULL;
			if (m_pCanvasContainer == NULL)
			{
				CControlUI* pRoot = paint_manager_.GetRoot();
				IContainerUI* pRootContainer = static_cast<IContainerUI*>(pRoot->GetInterface(_T("IContainer")));
				if ((pRoot != NULL) && (pRootContainer != NULL))
				{
					m_pCanvasContainer = pRoot;
					pParent = m_pCanvasContainer;

					pContainer = static_cast<IContainerUI*>(m_pCanvasContainer->GetInterface(_T("IContainer")));
				}
			}
			else
			{
				CControlUI* pClickedControl = paint_manager_.FindControl(pt);
				if ((pClickedControl != NULL) && (((pClickedControl->GetParent() != NULL) && (pClickedControl->GetParent() != m_pCanvasContainer)) || (static_cast<IContainerUI*>(pClickedControl->GetInterface(_T("IContainer"))) != NULL)))
				{
					pContainer = static_cast<IContainerUI*>(pClickedControl->GetInterface(_T("IContainer")));
					if (pContainer != NULL)
					{
						pParent = pClickedControl;
					}
					else
					{
						pParent = pClickedControl->GetParent();
						pContainer = static_cast<IContainerUI*>(pClickedControl->GetParent()->GetInterface(_T("IContainer")));
					}
				}
				else
				{
					pParent = m_pCanvasContainer;
					pContainer = static_cast<IContainerUI*>(m_pCanvasContainer->GetInterface(_T("IContainer")));
				}
			}

			if ((pContainer != NULL) && (pParent != NULL))
			{
				m_bSkinFileChanged = true;

				TCHAR* pszFeature = new TCHAR[7 * 1024];
				if (pszFeature == NULL)
					return lRes;
				ZeroMemory(pszFeature, 7 * 1024);

				RECT rcParent = pParent->GetPos();
				RECT rcClient;
				rcClient.left = pt.x - rcParent.left;
				rcClient.top = pt.y - rcParent.top;
				rcClient.right = rcClient.left + g_dwDefaultWidth;
				rcClient.bottom = rcClient.top + g_dwDefaultHeight;
				switch (g_eControlType)
				{
				case eControl_Label:
					{
						pControl = new CLabelUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultLabelFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pControl->GetClass());
						}
						break;
					}
				case eControl_Edit:
					{
						pControl = new CEditUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultEditFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pControl->GetClass());
						}
						break;
					}
				case eControl_Button:
					{
						pControl = new CButtonUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultButtonFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pControl->GetClass());
						}
						break;
					}
				case eControl_CheckBox:
					{
						pControl = new COptionUI;
						if (pControl != NULL)
						{
							pControl->SetImageAbsolutePath(true);
							_stprintf(pszFeature, g_lpszDefaultCheckBoxFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pControl->GetClass(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_TabControl:
				case eControl_RadioBox:
					{
						pControl = new COptionUI;
						if (pControl != NULL)
						{
							pControl->SetImageAbsolutePath(true);
							_stprintf(pszFeature, g_lpszDefaultRadioBoxFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pControl->GetClass(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_HSlider:
					{
						pControl = new CSliderUI;
						if (pControl != NULL)
						{
							static_cast<CProgressUI*>(pControl)->SetHorizontal(true);
							pControl->SetImageAbsolutePath(true);
							_stprintf(pszFeature, g_lpszDefaultHSliderFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_VSlider:
					{
						pControl = new CSliderUI;
						if (pControl != NULL)
						{
							RECT rcParent = pParent->GetPos();
							RECT rcClient;
							rcClient.left = pt.x - rcParent.left;
							rcClient.top = pt.y - rcParent.top;
							rcClient.right = rcClient.left + g_dwDefaultHeight;
							rcClient.bottom = rcClient.top + g_dwDefaultWidth;
							static_cast<CProgressUI*>(pControl)->SetHorizontal(false);
							pControl->SetImageAbsolutePath(true);
							_stprintf(pszFeature, g_lpszDefaultVSliderFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_Combox:
					{
						pControl = new CComboUI;
						if (pControl != NULL)
						{
							pControl->SetImageAbsolutePath(true);
							_stprintf(pszFeature, g_lpszDefaultComboxFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pControl->GetClass(), m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_HProgress:
					{
						pControl = new CProgressUI;
						if (pControl != NULL)
						{
							rcClient.right += 2 * g_dwDefaultWidth;
							static_cast<CProgressUI*>(pControl)->SetHorizontal(true);
							pControl->SetImageAbsolutePath(true);
							_stprintf(pszFeature, g_lpszDefaultProgressFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_VProgress:
					{
						pControl = new CProgressUI;
						if (pControl != NULL)
						{
							static_cast<CProgressUI*>(pControl)->SetHorizontal(false);
							pControl->SetImageAbsolutePath(true);
							RECT rcParent = pParent->GetPos();
							RECT rcClient;
							rcClient.left = pt.x - rcParent.left;
							rcClient.top = pt.y - rcParent.top;
							rcClient.right = rcClient.left + g_dwDefaultHeight;
							rcClient.bottom = rcClient.top + g_dwDefaultWidth;
							rcClient.bottom += 2 * g_dwDefaultWidth;
							_stprintf(pszFeature, g_lpszDefaultProgressFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_Tree:
				case eControl_List:
					{
						pControl = new CListUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultListFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
						}
						break;
					}
				case eControl_HScrollbar:
					{
						pControl = new CScrollBarUI(true);
						if (pControl != NULL)
						{				RECT rcParent = pParent->GetPos();
						RECT rcClient;
						rcClient.left = pt.x - rcParent.left;
						rcClient.top = pt.y - rcParent.top;
						rcClient.right = rcClient.left + g_dwDefaultWidth;
						rcClient.bottom = rcClient.top + 16;
						rcClient.right += 2 * g_dwDefaultWidth;
						pControl->SetImageAbsolutePath(true);
						_stprintf(pszFeature, g_lpszDefaultHScrollbarFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
							m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
							m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
							m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
							m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
							m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_VScrollbar:
					{
						pControl = new CScrollBarUI(false);
						if (pControl != NULL)
						{
							RECT rcParent = pParent->GetPos();
							RECT rcClient;
							rcClient.left = pt.x - rcParent.left;
							rcClient.top = pt.y - rcParent.top;
							rcClient.right = rcClient.left + 16;
							rcClient.bottom = rcClient.top + g_dwDefaultWidth;
							rcClient.bottom += 2 * g_dwDefaultWidth;
							pControl->SetImageAbsolutePath(true);
							_stprintf(pszFeature, g_lpszDefaultVScrollbarFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
								m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
								m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
								m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
								m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(),
								m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str(), m_tstrDefaultFeaturesSkinPath.c_str());
						}
						break;
					}
				case eControl_TabLayout:
					{
						pControl = new CTabLayoutUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultContainerFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
						}
						break;
					}
				case eControl_Container:
					{
						pControl = new CContainerUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultContainerFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
						}
						break;
					}
				case eControl_HorizontalLayout:
					{
						pControl = new CHorizontalLayoutUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultContainerFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
						}
						break;
					}
				case eControl_VerticalLayout:
					{
						pControl = new CVerticalLayoutUI;
						if (pControl != NULL)
						{
							_stprintf(pszFeature, g_lpszDefaultContainerFeature, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
						}
						break;
					}
				default:
					break;
				}
				if (pControl != NULL)
				{
					pControl->SetManager(&paint_manager_, pParent, false);
					pControl->ApplyAttributeList(pszFeature);

					pControl->SetVisible();
					pContainer->Add(pControl);
					pParent->NeedUpdate();
				}
				delete[] pszFeature;
				pszFeature = NULL;
			}
			if (m_pToolBox != NULL)
				m_pToolBox->ResetControlSelection();
			break;
		}
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		if ((m_pPropertiesSheet != NULL) && m_bMouseCaptured)
		{
			if (paint_manager_.FindPitchUponControlCount() > 0)
				m_bSkinFileChanged = true;
			if (paint_manager_.FindPitchUponControlCount() == 1)
				m_pPropertiesSheet->AttachControlUI(paint_manager_.FindPitchUponControl());
			else
				m_pPropertiesSheet->DetachControlUI();
		}
		if (uMsg == WM_LBUTTONUP)
			m_bMouseCaptured = false;
		break;
	default:
		break;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

bool CDesignerCanvas::PreExit()
{
	if (m_bSkinFileChanged)
	{
		int nModuleResult = ::MessageBox(m_hWnd, _T("皮肤文件已被修改。\n想保存文件吗？"), _T("UIDesigner"), MB_YESNOCANCEL);
		switch (nModuleResult)
		{
		case IDYES:
			SaveSkinFile();
			break;
		case IDNO:
			m_bSkinFileChanged = false;
			break;
		case IDCANCEL:
			break;
		}
	}
	return !m_bSkinFileChanged;
}

void CDesignerCanvas::CopyControl()
{
	m_pToolBox->ResetTab();

	if (m_pCutControl == m_pCopyControl)
		m_pCutControl = NULL;

	if (m_pCutControl != NULL)
		DeleteControl(m_pCutControl, true);

	if (m_pCopyControl != NULL)
		DeleteControl(m_pCopyControl, true);

	if (paint_manager_.FindPitchUponControlCount() == 1)
	{
		CControlUI* pControlUI = paint_manager_.FindPitchUponControl();
		CopyControl(pControlUI, m_pCopyControl);
		if (m_pCopyControl)
			m_pCutControl = m_pCopyControl;
	}
	else if (paint_manager_.FindPitchUponControlCount() > 1)
	{
		AfxMessageBox(_T("该功能只支持单一控件！"));
	}
	else
		AfxMessageBox(_T("请先选择控件，可以通过按住键盘Ctrl或者Shit然后用鼠标左键单击控件进行选中，Ctrl为单选，Shit为多选！"));
}

void CDesignerCanvas::CutControl()
{
	m_pToolBox->ResetTab();

	if (m_pCutControl == m_pCopyControl)
		m_pCopyControl = NULL;

	if (m_pCutControl != NULL)
		DeleteControl(m_pCutControl, true);

	if (m_pCopyControl != NULL)
		DeleteControl(m_pCopyControl, true);

	if (paint_manager_.FindPitchUponControlCount() == 1)
	{
		m_bSkinFileChanged = true;

		CControlUI* pControlUI = paint_manager_.FindPitchUponControl();
		CopyControl(pControlUI, m_pCutControl);
		if (m_pCutControl)
			m_pCopyControl = m_pCutControl;

		DeleteControl();
	}
	else if (paint_manager_.FindPitchUponControlCount() > 1)
	{
		AfxMessageBox(_T("该功能只支持单一控件！"));
	}
	else
		AfxMessageBox(_T("请先选择控件，可以通过按住键盘Ctrl或者Shit然后用鼠标左键单击控件进行选中，Ctrl为单选，Shit为多选！"));
}

void CDesignerCanvas::PasteControl()
{
	if (m_pCopyControl != NULL)
	{
		CControlUI* newControl = NULL;
		CopyControl(m_pCopyControl, newControl);

		CRect rcClient;
		GetClientRect(m_hWnd, &rcClient);
		if (PtInRect(&rcClient, m_point))
		{
			m_bSkinFileChanged = true;

			POINT pt = { m_point.x, m_point.y };
			IContainerUI* pContainer = NULL;
			CControlUI* pParent = NULL;
			CControlUI* pControl = NULL;

			if (m_pCanvasContainer == NULL)
			{
				CControlUI* pRoot = paint_manager_.GetRoot();
				IContainerUI* pRootContainer = static_cast<IContainerUI*>(pRoot->GetInterface(_T("IContainer")));
				if ((pRoot != NULL) && (pRootContainer != NULL))
				{
					m_pCanvasContainer = pRoot;
					pParent = m_pCanvasContainer;

					pContainer = static_cast<IContainerUI*>(m_pCanvasContainer->GetInterface(_T("IContainer")));
				}
			}

			CControlUI* pClickedControl = paint_manager_.FindControl(pt);
			if ((pClickedControl != NULL) && (((pClickedControl->GetParent() != NULL) && (pClickedControl->GetParent() != m_pCanvasContainer)) || (static_cast<IContainerUI*>(pClickedControl->GetInterface(_T("IContainer"))) != NULL)))
			{
				pContainer = static_cast<IContainerUI*>(pClickedControl->GetInterface(_T("IContainer")));
				if (pContainer != NULL)
				{
					pParent = pClickedControl;
				}
				else
				{
					pParent = pClickedControl->GetParent();
					pContainer = static_cast<IContainerUI*>(pClickedControl->GetParent()->GetInterface(_T("IContainer")));
				}
			}
			else
			{
				pParent = m_pCanvasContainer;
				pContainer = static_cast<IContainerUI*>(m_pCanvasContainer->GetInterface(_T("IContainer")));
			}

			if ((pContainer != NULL) && (pParent != NULL))
			{
				newControl->SetManager(&paint_manager_, pParent, false);
				newControl->SetVisible();
				pContainer->Add(newControl);
				newControl->SetPitchUpon(true);
				newControl->Move(1, 1);
				pParent->NeedUpdate();
			}
		}
	}
}

void CDesignerCanvas::DeleteControl(CControlUI *&pCopiedOrCutControl, bool bDeleteLeafControl)
{
	if (pCopiedOrCutControl != NULL)
	{
		IContainerUI* pContainer = static_cast<IContainerUI*>(pCopiedOrCutControl->GetInterface(_T("IContainer")));
		if (pContainer != NULL)
		{
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pCopiedOrCutControl->GetInterface(_T("Container")));
			for (int j = 0; j < pContainer->GetCount(); ++j)
			{
				CControlUI* pControlUI = static_cast<CControlUI*>(pContainer->GetItemAt(j));
				DeleteControl(pControlUI, false);
			}
			pContainerUI->RemoveAll();
			if (bDeleteLeafControl)
			{
				delete pCopiedOrCutControl;
				pCopiedOrCutControl = NULL;
			}
		}
		else
		{
			if (bDeleteLeafControl)
			{
				delete pCopiedOrCutControl;
				pCopiedOrCutControl = NULL;
			}
		}
	}
}

void CDesignerCanvas::DeleteControl()
{
	m_pToolBox->ResetTab();

	if (paint_manager_.FindPitchUponControlCount() > 0)
	{
		m_bSkinFileChanged = true;

		CStdPtrArray controls;
		paint_manager_.FinAllPitchUponControl(&controls);
		for (int i = 0; i < controls.GetSize(); ++i)
		{
			CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(i));
			IContainerUI* pParentContainer = NULL;
			if ((pControlUI != NULL) && (pControlUI->GetParent() != NULL))
			{
				pParentContainer = static_cast<IContainerUI*>(pControlUI->GetParent()->GetInterface(_T("IContainer")));
				ASSERT(pParentContainer != NULL);
				IContainerUI* pContainer = static_cast<IContainerUI*>(pControlUI->GetInterface(_T("IContainer")));
				if (pContainer != NULL)
				{
					for (int j = 0; j < pContainer->GetCount(); ++j)
					{
						CControlUI* pControlUI = static_cast<CControlUI*>(pContainer->GetItemAt(j));
						paint_manager_.ReapObjects(pControlUI);
					}
					pContainer->RemoveAll();
					pParentContainer->Remove(pControlUI);
				}
				else
				{
					paint_manager_.ReapObjects(pControlUI);
					pParentContainer->Remove(pControlUI);
				}
			}
		}

		if (m_pPropertiesSheet != NULL)
			m_pPropertiesSheet->DetachControlUI();
	}
	else
		AfxMessageBox(_T("请先选择控件，可以通过按住键盘Ctrl或者Shit然后用鼠标左键单击控件进行选中，Ctrl为单选，Shit为多选！"));
}

void CDesignerCanvas::AdjustControls(eAdjustment adjust)
{
	if ((paint_manager_.FindPitchUponControlCount() > 1) && !((adjust == eAdjustmentHorizontalCenter) || (eAdjustmentVerticalLCenter == adjust)))
	{
		CStdPtrArray controls;
		paint_manager_.FinAllPitchUponControl(&controls);

		// 首先检查是否为同一canvas中的元素
		CControlUI* pParent = static_cast<CControlUI*>(controls.GetAt(0))->GetParent();
		for (int i = 1; i < controls.GetSize(); ++i)
		{
			if (pParent != static_cast<CControlUI*>(controls.GetAt(i))->GetParent())
			{
				AfxMessageBox(_T("只支持同一canvas下的控件！"));
				return;
			}
		}

		if ((adjust == eAdjustmentTransverseDistribution) || (eAdjustmentLongitudinalDistribution == adjust))
		{
			RECT rcParent = pParent->GetPos();
			LONG nWidths = 0;
			LONG nHeights = 0;
			LONG xMargin = 0;
			LONG yMargin = 0;
			LONG nMinLeft = 0;
			LONG nMaxRight = 0;
			LONG nMinTop = 0;
			LONG nMaxBottom = 0;
			{
				CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(0));
				RECT rcControl = pControlUI->GetPos();
				nWidths += rcControl.right - rcControl.left;
				nHeights += rcControl.bottom - rcControl.top;
				nMinLeft = rcControl.left;
				nMaxRight = rcControl.right;
				nMinTop = rcControl.top;
				nMaxBottom = rcControl.bottom;
			}

			for (int i = 1; i < controls.GetSize(); ++i)
			{
				CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(i));
				RECT rcControl = pControlUI->GetPos();
				nWidths += rcControl.right - rcControl.left;
				nHeights += rcControl.bottom - rcControl.top;
				if (nMinLeft > rcControl.left)
					nMinLeft = rcControl.left;
				if (nMaxRight < rcControl.right)
					nMaxRight = rcControl.right;
				if (nMinTop > rcControl.top)
					nMinTop = rcControl.top;
				if (nMaxBottom < rcControl.bottom)
					nMaxBottom = rcControl.bottom;
			}

			if (((nWidths < (rcParent.right - rcParent.left)) && (adjust == eAdjustmentTransverseDistribution)) ||
				((nHeights < (rcParent.bottom - rcParent.top)) && (eAdjustmentLongitudinalDistribution == adjust)))
			{
				switch (adjust)
				{
					// 横向分布
				case eAdjustmentTransverseDistribution:
					{
						// 按照左起始点x坐标的最小值进行排序
						for (int i = 0; i < controls.GetSize(); ++i)
						{
							CControlUI* pBaseCompared = static_cast<CControlUI*>(controls.GetAt(i));
							RECT rcBaseCompared = pBaseCompared->GetPos();
							for (int j = i + 1; j < controls.GetSize(); ++j)
							{
								CControlUI* pCompared = static_cast<CControlUI*>(controls.GetAt(j));
								RECT rcCompared = pCompared->GetPos();
								if (rcBaseCompared.left > rcCompared.left)
								{
									controls.SetAt(j, pBaseCompared);
									controls.SetAt(i, pCompared);
									pBaseCompared = pCompared;
								}
							}
						}

						// 计算边距
						xMargin = static_cast<LONG>((nMaxRight - nMinLeft - nWidths) / (controls.GetSize() - 1));

						// 依次设置各个控件的位置
						LONG nLeftBased = 0;
						{
							CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(0));
							RECT rcControlUI = pControlUI->GetPos();

							nLeftBased = rcControlUI.left;
							nLeftBased += rcControlUI.right - rcControlUI.left;
						}

						for (int i = 1; i < controls.GetSize() - 1; ++i)
						{
							CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(i));
							RECT rcControlUI = pControlUI->GetPos();

							nLeftBased += xMargin;
							pControlUI->Move(nLeftBased - rcControlUI.left, 0);
							nLeftBased += rcControlUI.right - rcControlUI.left;
						}
						break;
					}
					// 纵向分布
				case eAdjustmentLongitudinalDistribution:
					{
						// 按照左起始点y坐标的最小值进行排序
						for (int i = 0; i < controls.GetSize(); ++i)
						{
							CControlUI* pBaseCompared = static_cast<CControlUI*>(controls.GetAt(i));
							RECT rcBaseCompared = pBaseCompared->GetPos();
							for (int j = i + 1; j < controls.GetSize(); ++j)
							{
								CControlUI* pCompared = static_cast<CControlUI*>(controls.GetAt(j));
								RECT rcCompared = pCompared->GetPos();
								if (rcBaseCompared.top > rcCompared.top)
								{
									controls.SetAt(j, pBaseCompared);
									controls.SetAt(i, pCompared);
									pBaseCompared = pCompared;
								}
							}
						}

						// 计算边距
						yMargin = static_cast<LONG>((nMaxBottom - nMinTop - nHeights) / (controls.GetSize() - 1));

						// 依次设置各个控件的位置
						LONG nTopBased = 0;
						{
							CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(0));
							RECT rcControlUI = pControlUI->GetPos();

							nTopBased = rcControlUI.top;
							nTopBased += rcControlUI.bottom - rcControlUI.top;
						}

						for (int i = 1; i < controls.GetSize() - 1; ++i)
						{
							CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(i));
							RECT rcControlUI = pControlUI->GetPos();

							nTopBased += yMargin;
							pControlUI->Move(0, nTopBased - rcControlUI.top);
							nTopBased += rcControlUI.bottom - rcControlUI.top;
						}
						break;
					}
				}
			}
		}
		else
		{
			RECT rcBasedRect = {0};
			for (int i = 0; i < controls.GetSize(); ++i)
			{
				CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(i));
				if ((pControlUI != NULL) && pControlUI->IsPitchUponContinousTwice())
				{
					rcBasedRect = pControlUI->GetPos();
					break;
				}
			}

			if (IsRectEmpty(&rcBasedRect))
			{
				AfxMessageBox(_T("请选择一个基准控件，可以通过按住键盘Ctrl或者Shit键然后用鼠标左键单击之前已经选中的控件来设置基准控件！"));
				return;
			}

			for (int i = 0; i < controls.GetSize(); ++i)
			{
				CControlUI* pControlUI = static_cast<CControlUI*>(controls.GetAt(i));
				RECT rcItem = pControlUI->GetPos();
				LONG xOffset = 0;
				LONG yOffset = 0;
				LONG nWidth = rcItem.right - rcItem.left;
				LONG nHeight = rcItem.bottom - rcItem.top;
				RECT rcAdjust = rcItem;
				switch (adjust)
				{
					// 左对齐
				case eAdjustmentAlign2Left:
					xOffset = rcBasedRect.left - rcItem.left;
					yOffset = 0;
					break;
					// 右对齐
				case eAdjustmentAlign2Right:
					xOffset = rcBasedRect.right - rcItem.right;
					yOffset = 0;
					break;
					// 顶端对齐
				case eAdjustmentAlign2Top:
					xOffset = 0;
					yOffset = rcBasedRect.top - rcItem.top;
					break;
					// 底端对齐
				case eAdjustmentAlign2Bottom:
					xOffset = 0;
					yOffset = rcBasedRect.bottom - rcItem.bottom;
					break;
					// 左右居中
				case eAdjustmentAlign2VCenter:
					xOffset = (rcBasedRect.left + static_cast<LONG>((rcBasedRect.right - rcBasedRect.left) /2) - static_cast<LONG>(nWidth / 2)) - rcItem.left;
					yOffset = 0;
					break;
					// 上下居中
				case eAdjustmentAlign2HCenter:
					yOffset = (rcBasedRect.top + static_cast<LONG>((rcBasedRect.bottom - rcBasedRect.top) /2) - static_cast<LONG>(nHeight / 2)) - rcItem.top;
					xOffset = 0;
					break;
					// 使控件宽度相等
				case eAdjustmentSameWidth:
					rcAdjust.right = rcAdjust.left + rcBasedRect.right - rcBasedRect.left;
					break;
					// 使控件高度相等
				case eAdjustmentSameHeight:
					rcAdjust.bottom = rcAdjust.top + rcBasedRect.bottom - rcBasedRect.top;
					break;
					// 使控件大小相等
				case eAdjustmentSameSize:
					rcAdjust.right = rcAdjust.left + rcBasedRect.right - rcBasedRect.left;
					rcAdjust.bottom = rcAdjust.top + rcBasedRect.bottom - rcBasedRect.top;
					break;
				default:
					break;
				}

				if ((xOffset != 0) || (yOffset != 0))
					pControlUI->Move(xOffset, yOffset);

				if (!EqualRect(&rcAdjust, &rcItem))
				{
					SIZE szXY = {rcAdjust.left, rcAdjust.top};
					pControlUI->SetFixedXY(szXY);
					pControlUI->SetFixedWidth(rcAdjust.right - rcAdjust.left);
					pControlUI->SetFixedHeight(rcAdjust.bottom - rcAdjust.top);
					pControlUI->NeedParentUpdate();
				}
			}
		}
	}
	else if ((adjust == eAdjustmentHorizontalCenter) || (eAdjustmentVerticalLCenter == adjust))
	{
		if (paint_manager_.FindPitchUponControlCount() > 1)
		{
			AfxMessageBox(_T("该功能只支持单一选中的控件！"));
			return;
		}
		CControlUI* pControlUI = paint_manager_.FindPitchUponControl();
		if (pControlUI != NULL)
		{
			CControlUI* pParentUI = pControlUI->GetParent();
			RECT rcParent = pParentUI->GetPos();
			RECT rcControl = pControlUI->GetPos();
			LONG xOffset = 0;
			LONG yOffset = 0;

			switch (adjust)
			{
				// 单一控件水平居中
			case eAdjustmentHorizontalCenter:
				xOffset = (rcParent.left + static_cast<LONG>((rcParent.right - rcParent.left) /2) - static_cast<LONG>((rcControl.right - rcControl.left) / 2)) - rcControl.left;
				yOffset = 0;
				break;
				// 单一控件垂直居中
			case eAdjustmentVerticalLCenter:
				yOffset = (rcParent.top + static_cast<LONG>((rcParent.bottom - rcParent.top) /2) - static_cast<LONG>((rcControl.bottom - rcControl.top) / 2)) - rcControl.top;
				xOffset = 0;
				break;
			}

			if ((xOffset != 0) || (yOffset != 0))
				pControlUI->Move(xOffset, yOffset);
		}
		else
			AfxMessageBox(_T("请先选择控件，可以通过按住键盘Ctrl或者Shit然后用鼠标左键单击控件进行选中，Ctrl为单选，Shit为多选！"));
	}
	else
		AfxMessageBox(_T("请先选择控件，可以通过按住键盘Ctrl或者Shit然后用鼠标左键单击控件进行选中，Ctrl为单选，Shit为多选！"));
}

void CDesignerCanvas::ShowPropertiesSheet()
{
	RECT rcClient = {0};
	GetClientRect(GetParent(m_hWnd), &rcClient);

	RECT rcPropertiesSheet = rcClient;
	rcPropertiesSheet.top += 5*MARGIN;
	rcPropertiesSheet.right = rcPropertiesSheet.right - MARGIN;
	rcPropertiesSheet.left = rcPropertiesSheet.right - PROPERTIES_SHEET_WIDTH;

	if (m_pPropertiesSheet == NULL)
	{
		m_pPropertiesSheet = new CPropertiesSheet(NULL, this);
		if (m_pPropertiesSheet != NULL)
		{
			m_pPropertiesSheet->Create(GetParent(m_hWnd), _T("PropertiesSheet"), WS_CHILDWINDOW | WS_VISIBLE | WS_POPUP, 0, rcPropertiesSheet.left, rcPropertiesSheet.top + MARGIN, rcPropertiesSheet.right - rcPropertiesSheet.left, rcPropertiesSheet.bottom - rcPropertiesSheet.top - 2 * MARGIN);
			MoveWindow(*m_pPropertiesSheet, rcPropertiesSheet.left, rcPropertiesSheet.top + MARGIN, rcPropertiesSheet.right - rcPropertiesSheet.left, rcPropertiesSheet.bottom - rcPropertiesSheet.top - 2 * MARGIN, TRUE);
		}
	}
}

CPropertiesSheet* CDesignerCanvas::GetPropertiesSheet()
{
	return m_pPropertiesSheet;
}

void CDesignerCanvas::Refresh()
{
	if (!m_tstrSkinFile.empty() && !m_tstrSkinPath.empty())
	{
		tString tstrSkin = m_tstrSkinPath + m_tstrSkinFile;

		paint_manager_.RemoveNotifier(this);
		paint_manager_.ReapObjects(paint_manager_.GetRoot());

		CDialogBuilder builder;
		CDialogBuilderCallbackEx cb;

		paint_manager_.SetResourcePath(m_tstrSkinPath.c_str());		
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
		ASSERT(pRoot && _T("Failed to parse XML"));
		paint_manager_.AttachDialog(pRoot);
		paint_manager_.AddNotifier(this);
		paint_manager_.SetUsedAsUIDesigner(true);
		paint_manager_.NeedUpdate();

		m_bSkinFileChanged = false;
	}
}

void CDesignerCanvas::OpenSkinFile()
{
	m_pToolBox->ResetTab();

	OPENFILENAME ofn = {0};
	const TCHAR szfilters[] = _T("皮肤文件(*.xml)\0*.xml\0\0");
	TCHAR szSkinFile[MAX_PATH] = {0};

	// Initialize File Open structure.
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szSkinFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = szfilters;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
#if(WINVER >= 0x0400)
	ofn.Flags = ofn.Flags | OFN_EXPLORER | OFN_ENABLESIZING;
#endif	

	if ((GetOpenFileName(&ofn) > 0) && (_tcslen(szSkinFile) > 0))
	{
		TCHAR szDrive[MAX_PATH] = {0};
		TCHAR szDir[MAX_PATH] = {0};
		TCHAR szName[MAX_PATH] = {0};
		TCHAR szExt[MAX_PATH] = {0};
		_tsplitpath_s(szSkinFile, szDrive, MAX_PATH, szDir, MAX_PATH, szName, MAX_PATH, szExt, MAX_PATH);
		m_tstrSkinPath = szDrive;
		m_tstrSkinPath += szDir;
		m_tstrSkinFile = szName;
		m_tstrSkinFile += szExt;

		paint_manager_.RemoveNotifier(this);
		paint_manager_.ReapObjects(paint_manager_.GetRoot());

		paint_manager_.RemoveAllDefaultAttributeList();
		paint_manager_.RemoveAllFonts();
		paint_manager_.RemoveAllImages();

		CDialogBuilder builder;
		CDialogBuilderCallbackEx cb;

		paint_manager_.SetResourcePath(m_tstrSkinPath.c_str());

#if defined(USING_ZIP_SKIN)
		paint_manager_.SetResourceZip(_T(""));
#endif
		tString tstrSkin = m_tstrSkinPath + m_tstrSkinFile;
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
		ASSERT(pRoot && _T("Failed to parse XML"));
		paint_manager_.AttachDialog(pRoot);
		paint_manager_.AddNotifier(this);
		paint_manager_.SetUsedAsUIDesigner(true);		
		SIZE szInitSize = paint_manager_.GetInitSize();
		m_formInfo.bOpacity = paint_manager_.IsOpacity();
		m_formInfo.dwTransparency = paint_manager_.GetTransparency();
		if ((szInitSize.cx != 0) && (szInitSize.cy != 0) && ((szInitSize.cx != m_formInfo.szCanvasSize.cx) || (szInitSize.cy != m_formInfo.szCanvasSize.cy)))
		{
			m_formInfo.szCanvasSize = szInitSize;

			RECT rcClient = {0};
			GetClientRect(GetParent(m_hWnd), &rcClient);
			RECT rcCanvas = rcClient;
			rcCanvas.top += TOOLBAR_HEIGHT + 2 * MARGIN;;
			rcCanvas.left = rcClient.left + MARGIN;
			DWORD dwResizeCanvasWidth = szInitSize.cx;
			::MoveWindow(m_hWnd, rcCanvas.left + MARGIN, rcCanvas.top + MARGIN, dwResizeCanvasWidth, szInitSize.cy, TRUE);

		}
		if (m_pToolbar != NULL)
			m_pToolbar->UpdateFormInfo();

		SIZE szRoundCorner = paint_manager_.GetRoundCorner();

		CRect rcClient;
		GetClientRect(m_hWnd, &rcClient);
		paint_manager_.Invalidate(rcClient);

		m_bSkinFileChanged = false;
	}
}

void CDesignerCanvas::NewSkinFile()
{
	OPENFILENAME ofn = {0};
	const TCHAR szfilters[] = _T("皮肤文件(*.xml)\0*.xml\0\0");
	TCHAR szSkinFile[MAX_PATH] = {0};

	// Initialize File Open structure.
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szSkinFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = szfilters;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_CREATEPROMPT;
#if(WINVER >= 0x0400)
	ofn.Flags = ofn.Flags | OFN_EXPLORER | OFN_ENABLESIZING;
#endif	

	if ((GetSaveFileName(&ofn) > 0) && (_tcslen(szSkinFile) > 0))
	{
		TCHAR szDrive[MAX_PATH] = {0};
		TCHAR szDir[MAX_PATH] = {0};
		TCHAR szName[MAX_PATH] = {0};
		TCHAR szExt[MAX_PATH] = {0};
		_tsplitpath_s(szSkinFile, szDrive, MAX_PATH, szDir, MAX_PATH, szName, MAX_PATH, szExt, MAX_PATH);
		m_tstrSkinPath = szDrive;
		m_tstrSkinPath += szDir;
		m_tstrSkinFile = szName;
		if (_tcslen(szExt) == 0)
			_stprintf(szExt, _T(".xml"));
		m_tstrSkinFile += szExt;

		tString tstrSkin = m_tstrSkinPath + m_tstrSkinFile;
		tString tstrDefaultSkin = CPaintManagerUI::GetInstancePath() + lpszSkinPath;
		tstrDefaultSkin += _T("uidesigner_canvas.xml");	

		// 把默认的skin属性拷贝到新建的文档中
		DeleteFile(tstrSkin.c_str());
		CopyFile(tstrDefaultSkin.c_str(), tstrSkin.c_str(), FALSE);

		paint_manager_.RemoveNotifier(this);
		paint_manager_.ReapObjects(paint_manager_.GetRoot());

		CDialogBuilder builder;
		CDialogBuilderCallbackEx cb;

		paint_manager_.SetResourcePath(m_tstrSkinPath.c_str());		
		CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  &cb, &paint_manager_);
		ASSERT(pRoot && _T("Failed to parse XML"));
		paint_manager_.AttachDialog(pRoot);
		paint_manager_.AddNotifier(this);
		paint_manager_.SetUsedAsUIDesigner(true);
		paint_manager_.NeedUpdate();

		m_bSkinFileChanged = false;
	}
}

void CDesignerCanvas::Save_CControlUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	TCHAR szBuf[MAX_PATH] = {0};

	if (pControlUI->GetName() && _tcslen(pControlUI->GetName()) > 0)
		pNode->SetAttribute("name", StringConvertor::WideToUtf8(pControlUI->GetName()));

	if (pControlUI->GetText() && _tcslen(pControlUI->GetText()) > 0)
		pNode->SetAttribute("text", StringConvertor::WideToUtf8(pControlUI->GetText()));

	if (pControlUI->GetToolTip() && _tcslen(pControlUI->GetToolTip()) > 0)
		pNode->SetAttribute("tooltip", StringConvertor::WideToUtf8(pControlUI->GetToolTip()));

	if (!pControlUI->IsVisible() && !((static_cast<IContainerUI*>(pControlUI->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pControlUI->GetInterface(_T("Container"))) != NULL)))
	{
		CControlUI* pParent = pControlUI->GetParent();
		if ((pParent != NULL) && ((static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pParent->GetInterface(_T("Container"))) != NULL)))
		{
			// 如果同一层中所有元素都是不可见的，则不设置属性
			bool bVisible = false;
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
			for( int it = 0; it < pContainerUI->GetCount(); it++ )
			{
				CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
				bVisible = pControl->IsVisible();
				if (bVisible)
					break;
			}
			if (bVisible)
				pNode->SetAttribute("visible", "false");
		}
	}

	if (pControlUI->GetShortcut() != _T('\0'))
	{
		_stprintf(szBuf, _T("%c"),pControlUI->GetShortcut());
		pNode->SetAttribute("shortcut", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetBorderSize() != 1)
	{
		// 默认为1
		_stprintf(szBuf, _T("%d"),pControlUI->GetBorderSize());
		pNode->SetAttribute("bordersize", StringConvertor::WideToUtf8(szBuf));
	}

	SIZE borderRound = pControlUI->GetBorderRound();
	if ((borderRound.cx != 0) && (borderRound.cy != 0))
	{
		_stprintf(szBuf, _T("%d, %d"),borderRound.cx, borderRound.cy);
		pNode->SetAttribute("borderround", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->IsFloat())
	{
		pNode->SetAttribute("float", "true");

		_stprintf(szBuf, _T("%d,%d,%d,%d"), pControlUI->GetFixedXY().cx, pControlUI->GetFixedXY().cy, pControlUI->GetFixedXY().cx + pControlUI->GetFixedWidth(), pControlUI->GetFixedXY().cy + pControlUI->GetFixedHeight());
		pNode->SetAttribute("pos", StringConvertor::WideToUtf8(szBuf));
	}
	else
	{
		if ((pControlUI->GetFixedXY().cx != 0)
			|| (pControlUI->GetFixedXY().cy != 0)
			|| (pControlUI->GetFixedWidth() != 0)
			|| (pControlUI->GetFixedHeight() != 0))
		{
			_stprintf(szBuf, _T("%d,%d,%d,%d"), pControlUI->GetFixedXY().cx, pControlUI->GetFixedXY().cy, pControlUI->GetFixedXY().cx + pControlUI->GetFixedWidth(), pControlUI->GetFixedXY().cy + pControlUI->GetFixedHeight());
			pNode->SetAttribute("pos", StringConvertor::WideToUtf8(szBuf));
		}

		if (pControlUI->GetFixedWidth() > 0)
		{
			_stprintf(szBuf, _T("%d"), pControlUI->GetFixedWidth());
			pNode->SetAttribute("width", StringConvertor::WideToUtf8(szBuf));
		}

		if (pControlUI->GetFixedHeight() > 0)
		{
			_stprintf(szBuf, _T("%d"), pControlUI->GetFixedHeight());
			pNode->SetAttribute("height", StringConvertor::WideToUtf8(szBuf));
		}
	}

	RECT rcPadding = pControlUI->GetPadding();
	if ((rcPadding.left != 0) || (rcPadding.right != 0) || (rcPadding.bottom != 0) || (rcPadding.top != 0))
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom);
		pNode->SetAttribute("padding", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetBkImage() && _tcslen(pControlUI->GetBkImage()) > 0)
	{
		pNode->SetAttribute("bkimage", StringConvertor::WideToUtf8(pControlUI->GetBkImage()));
	}

	if (pControlUI->GetBkColor() != 0)
	{
		DWORD dwColor = pControlUI->GetBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("bkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetBkColor2() != 0)
	{
		DWORD dwColor = pControlUI->GetBkColor2();
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));

		pNode->SetAttribute("bkcolor2", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetBorderColor() != 0)
	{
		DWORD dwColor = pControlUI->GetBorderColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("bordercolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetFocusBorderColor() != 0)
	{
		DWORD dwColor = pControlUI->GetFocusBorderColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("focusbordercolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetMaxHeight() != 9999)
	{
		_stprintf(szBuf, _T("%d"), pControlUI->GetMaxHeight());
		pNode->SetAttribute("maxheight", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetMaxWidth() != 9999)
	{
		_stprintf(szBuf, _T("%d"), pControlUI->GetMaxWidth());
		pNode->SetAttribute("maxwidth", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetMinWidth() != 0)
	{
		_stprintf(szBuf, _T("%d"), pControlUI->GetMinWidth());
		pNode->SetAttribute("minwidth", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetMinHeight() != 0)
	{
		_stprintf(szBuf, _T("%d"), pControlUI->GetMinHeight());
		pNode->SetAttribute("minheight", StringConvertor::WideToUtf8(szBuf));
	}

	TRelativePosUI relativePos = pControlUI->GetRelativePos();
	if (relativePos.bRelative)
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), relativePos.nMoveXPercent, relativePos.nMoveYPercent, relativePos.nZoomXPercent, relativePos.nZoomYPercent);
		pNode->SetAttribute("relativepos", StringConvertor::WideToUtf8(szBuf));
	}

	if (pControlUI->GetStretchMode() && _tcslen(pControlUI->GetStretchMode()) > 0)
		pNode->SetAttribute("stretch", StringConvertor::WideToUtf8(pControlUI->GetStretchMode()));
}

void CDesignerCanvas::Save_CLabelUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CControlUI_Properties(pControlUI, pNode);

	CLabelUI* pLabelUI = static_cast<CLabelUI*>(pControlUI->GetInterface(_T("Label")));

	TCHAR szBuf[MAX_PATH] = {0};

	RECT rcTextPadding = pLabelUI->GetTextPadding();
	if ((rcTextPadding.left != 0) || (rcTextPadding.right != 0) || (rcTextPadding.bottom != 0) || (rcTextPadding.top != 0))
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), rcTextPadding.left, rcTextPadding.top, rcTextPadding.right, rcTextPadding.bottom);
		pNode->SetAttribute("textpadding", StringConvertor::WideToUtf8(szBuf));
	}

	if (pLabelUI->GetTextColor() != 0)
	{
		DWORD dwColor = pLabelUI->GetTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("textcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pLabelUI->GetDisabledTextColor() != 0)
	{
		DWORD dwColor = pLabelUI->GetDisabledTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("disabledtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pLabelUI->IsShowHtml())
		pNode->SetAttribute("showhtml", "true");

	tString tstrAlgin;
	UINT uTextStyle = pLabelUI->GetTextStyle();

	if (uTextStyle & DT_LEFT)
		tstrAlgin = _T("left");

	if(uTextStyle & DT_CENTER)
		tstrAlgin = _T("center");

	if(uTextStyle & DT_RIGHT)
		tstrAlgin = _T("right");

	if(uTextStyle & DT_TOP)
		tstrAlgin += _T("top");

	if(uTextStyle & DT_BOTTOM)
		tstrAlgin += _T("bottom");

	if(uTextStyle & DT_WORDBREAK)
		tstrAlgin += _T("wrap");

	if (!tstrAlgin.empty())
		pNode->SetAttribute("align", StringConvertor::WideToUtf8(tstrAlgin.c_str()));
}

void CDesignerCanvas::Save_CButtonUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CLabelUI_Properties(pControlUI, pNode);
	TCHAR szBuf[MAX_PATH] = {0};

	CButtonUI* pButtonUI = static_cast<CButtonUI*>(pControlUI->GetInterface(_T("Button")));
	if (pButtonUI->GetNormalImage() && _tcslen(pButtonUI->GetNormalImage()) > 0)
		pNode->SetAttribute("normalimage", StringConvertor::WideToUtf8(pButtonUI->GetNormalImage()));

	if (pButtonUI->GetHotImage() && _tcslen(pButtonUI->GetHotImage()) > 0)
		pNode->SetAttribute("hotimage", StringConvertor::WideToUtf8(pButtonUI->GetHotImage()));

	if (pButtonUI->GetPushedImage() && _tcslen(pButtonUI->GetPushedImage()) > 0)
		pNode->SetAttribute("pushedimage", StringConvertor::WideToUtf8(pButtonUI->GetPushedImage()));

	if (pButtonUI->GetFocusedImage() && _tcslen(pButtonUI->GetFocusedImage()) > 0)
		pNode->SetAttribute("focusedimage", StringConvertor::WideToUtf8(pButtonUI->GetFocusedImage()));

	if (pButtonUI->GetDisabledImage() && _tcslen(pButtonUI->GetDisabledImage()) > 0)
		pNode->SetAttribute("disabledimage", StringConvertor::WideToUtf8(pButtonUI->GetDisabledImage()));

	if (pButtonUI->GetForeImage() && _tcslen(pButtonUI->GetForeImage()) > 0)
		pNode->SetAttribute("foreimage", StringConvertor::WideToUtf8(pButtonUI->GetForeImage()));

	if (pButtonUI->GetFocusedTextColor() != 0)
	{
		DWORD dwColor = pButtonUI->GetFocusedTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("focusedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pButtonUI->GetPushedTextColor() != 0)
	{
		DWORD dwColor = pButtonUI->GetPushedTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("pushedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pButtonUI->GetHotTextColor() != 0)
	{
		DWORD dwColor = pButtonUI->GetHotTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("hottextcolor", StringConvertor::WideToUtf8(szBuf));
	}
}


void CDesignerCanvas::Save_COptionUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CButtonUI_Properties(pControlUI, pNode);
	COptionUI* pOptionUI = static_cast<COptionUI*>(pControlUI->GetInterface(_T("Option")));

	TCHAR szBuf[MAX_PATH] = {0};

	if (!pOptionUI->IsImageFitallArea())
		pNode->SetAttribute("fitallArea", "false");

	if (pOptionUI->GetGroup() && _tcslen(pOptionUI->GetGroup()) > 0)
		pNode->SetAttribute("group", StringConvertor::WideToUtf8(pOptionUI->GetGroup()));

	if (pOptionUI->IsSelected())
		pNode->SetAttribute("selected", pOptionUI->IsSelected()?"true":"false");

	if (pOptionUI->GetSelectedTextColor() != 0)
	{
		DWORD dwColor = pOptionUI->GetSelectedTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("selectedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pOptionUI->GetSelectedImage() && _tcslen(pOptionUI->GetSelectedImage()) > 0)
		pNode->SetAttribute("selectedimage", StringConvertor::WideToUtf8(pOptionUI->GetSelectedImage()));
}

void CDesignerCanvas::Save_CProgressUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CLabelUI_Properties(pControlUI, pNode);
	CProgressUI* pProgressUI = static_cast<CProgressUI*>(pControlUI->GetInterface(_T("Progress")));

	TCHAR szBuf[MAX_PATH] = {0};

	if (pProgressUI->GetFgImage() && _tcslen(pProgressUI->GetFgImage()) > 0)
		pNode->SetAttribute("fgimage", StringConvertor::WideToUtf8(pProgressUI->GetFgImage()));

	_stprintf(szBuf, _T("%d"), pProgressUI->GetMinValue());
	pNode->SetAttribute("min", StringConvertor::WideToUtf8(szBuf));


	_stprintf(szBuf, _T("%d"), pProgressUI->GetMaxValue());
	pNode->SetAttribute("max", StringConvertor::WideToUtf8(szBuf));

	_stprintf(szBuf, _T("%d"), pProgressUI->GetValue());
	pNode->SetAttribute("value", StringConvertor::WideToUtf8(szBuf));

	if (pProgressUI->IsHorizontal())
		pNode->SetAttribute("hor", pProgressUI->IsHorizontal()?"true":"false");
}

void CDesignerCanvas::Save_CSliderUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CProgressUI_Properties(pControlUI, pNode);

	CSliderUI* pSliderUI = static_cast<CSliderUI*>(pControlUI->GetInterface(_T("Slider")));
	TCHAR szBuf[MAX_PATH] = {0};

	if (pSliderUI->GetThumbImage() && _tcslen(pSliderUI->GetThumbImage()) > 0)
		pNode->SetAttribute("thumbimage", StringConvertor::WideToUtf8(pSliderUI->GetThumbImage()));

	if (pSliderUI->GetThumbHotImage() && _tcslen(pSliderUI->GetThumbHotImage()) > 0)
		pNode->SetAttribute("thumbhotimage", StringConvertor::WideToUtf8(pSliderUI->GetThumbHotImage()));

	if (pSliderUI->GetThumbPushedImage() && _tcslen(pSliderUI->GetThumbPushedImage()) > 0)
		pNode->SetAttribute("thumbpushedimage", StringConvertor::WideToUtf8(pSliderUI->GetThumbPushedImage()));

	_stprintf(szBuf, _T("%d,%d"), pSliderUI->GetThumbRect().right - pSliderUI->GetThumbRect().left, pSliderUI->GetThumbRect().bottom - pSliderUI->GetThumbRect().top);
	pNode->SetAttribute("thumbsize", StringConvertor::WideToUtf8(szBuf));
}

void CDesignerCanvas::Save_CEditUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CLabelUI_Properties(pControlUI, pNode);
	CEditUI* pEditUI = static_cast<CEditUI*>(pControlUI->GetInterface(_T("Edit")));

	TCHAR szBuf[MAX_PATH] = {0};	

	if (pEditUI->IsPasswordMode())
		pNode->SetAttribute("password", pEditUI->IsPasswordMode()?"true":"false");

	if (pEditUI->IsReadOnly())
		pNode->SetAttribute("readonly", pEditUI->IsReadOnly()?"true":"false");

	if (pEditUI->IsDigitalNumber())
		pNode->SetAttribute("digitalnumber", pEditUI->IsDigitalNumber()?"true":"false");

	if (pEditUI->IsMultiLine())
		pNode->SetAttribute("multiline", pEditUI->IsMultiLine()?"true":"false");
}

void CDesignerCanvas::Save_CScrollbarUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CControlUI_Properties(pControlUI, pNode);
	CScrollBarUI* pScrollbarUI = static_cast<CScrollBarUI*>(pControlUI->GetInterface(_T("ScrollBar")));
	TCHAR szBuf[MAX_PATH] = {0};

	if (pScrollbarUI->GetBkNormalImage() && _tcslen(pScrollbarUI->GetBkNormalImage()) > 0)
		pNode->SetAttribute("bknormalimage", StringConvertor::WideToUtf8(pScrollbarUI->GetBkNormalImage()));

	if (pScrollbarUI->GetBkHotImage() && _tcslen(pScrollbarUI->GetBkHotImage()) > 0)
		pNode->SetAttribute("bkhotimage", StringConvertor::WideToUtf8(pScrollbarUI->GetBkHotImage()));

	if (pScrollbarUI->GetBkPushedImage() && _tcslen(pScrollbarUI->GetBkPushedImage()) > 0)
		pNode->SetAttribute("bkpushedimage", StringConvertor::WideToUtf8(pScrollbarUI->GetBkPushedImage()));

	if (pScrollbarUI->GetBkDisabledImage() && _tcslen(pScrollbarUI->GetBkDisabledImage()) > 0)
		pNode->SetAttribute("bkdisabledimage", StringConvertor::WideToUtf8(pScrollbarUI->GetBkDisabledImage()));

	if (pScrollbarUI->GetRailNormalImage() && _tcslen(pScrollbarUI->GetRailNormalImage()) > 0)
		pNode->SetAttribute("railnormalimage", StringConvertor::WideToUtf8(pScrollbarUI->GetRailNormalImage()));

	if (pScrollbarUI->GetRailHotImage() && _tcslen(pScrollbarUI->GetRailHotImage()) > 0)
		pNode->SetAttribute("railhotimage", StringConvertor::WideToUtf8(pScrollbarUI->GetRailHotImage()));

	if (pScrollbarUI->GetRailPushedImage() && _tcslen(pScrollbarUI->GetRailPushedImage()) > 0)
		pNode->SetAttribute("railpushedimage", StringConvertor::WideToUtf8(pScrollbarUI->GetRailPushedImage()));

	if (pScrollbarUI->GetRailDisabledImage() && _tcslen(pScrollbarUI->GetRailDisabledImage()) > 0)
		pNode->SetAttribute("raildisabledimage", StringConvertor::WideToUtf8(pScrollbarUI->GetRailDisabledImage()));

	if (pScrollbarUI->GetThumbNormalImage() && _tcslen(pScrollbarUI->GetThumbNormalImage()) > 0)
		pNode->SetAttribute("thumbnormalimage", StringConvertor::WideToUtf8(pScrollbarUI->GetThumbNormalImage()));

	if (pScrollbarUI->GetThumbHotImage() && _tcslen(pScrollbarUI->GetThumbHotImage()) > 0)
		pNode->SetAttribute("thumbhotimage", StringConvertor::WideToUtf8(pScrollbarUI->GetThumbHotImage()));

	if (pScrollbarUI->GetThumbPushedImage() && _tcslen(pScrollbarUI->GetThumbPushedImage()) > 0)
		pNode->SetAttribute("thumbpushedimage", StringConvertor::WideToUtf8(pScrollbarUI->GetThumbPushedImage()));

	if (pScrollbarUI->GetThumbDisabledImage() && _tcslen(pScrollbarUI->GetThumbDisabledImage()) > 0)
		pNode->SetAttribute("thumbdisabledimage", StringConvertor::WideToUtf8(pScrollbarUI->GetThumbDisabledImage()));

	if (pScrollbarUI->GetButton2NormalImage() && _tcslen(pScrollbarUI->GetButton2NormalImage()) > 0)
		pNode->SetAttribute("button2normalimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton2NormalImage()));

	if (pScrollbarUI->GetButton2HotImage() && _tcslen(pScrollbarUI->GetButton2HotImage()) > 0)
		pNode->SetAttribute("button2hotimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton2HotImage()));

	if (pScrollbarUI->GetButton2PushedImage() && _tcslen(pScrollbarUI->GetButton2PushedImage()) > 0)
		pNode->SetAttribute("button2pushedimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton2PushedImage()));

	if (pScrollbarUI->GetButton2DisabledImage() && _tcslen(pScrollbarUI->GetButton2DisabledImage()) > 0)
		pNode->SetAttribute("button2disabledimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton2DisabledImage()));

	if (pScrollbarUI->GetButton1NormalImage() && _tcslen(pScrollbarUI->GetButton1NormalImage()) > 0)
		pNode->SetAttribute("button1normalimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton1NormalImage()));

	if (pScrollbarUI->GetButton1HotImage() && _tcslen(pScrollbarUI->GetButton1HotImage()) > 0)
		pNode->SetAttribute("button1hotimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton1HotImage()));

	if (pScrollbarUI->GetButton1PushedImage() && _tcslen(pScrollbarUI->GetButton1PushedImage()) > 0)
		pNode->SetAttribute("button1pushedimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton1PushedImage()));

	if (pScrollbarUI->GetButton1DisabledImage() && _tcslen(pScrollbarUI->GetButton1DisabledImage()) > 0)
		pNode->SetAttribute("button1disabledimage", StringConvertor::WideToUtf8(pScrollbarUI->GetButton1DisabledImage()));

	_stprintf(szBuf, _T("%d"), pScrollbarUI->GetLineSize());
	pNode->SetAttribute("linesize", StringConvertor::WideToUtf8(szBuf));

	_stprintf(szBuf, _T("%d"), pScrollbarUI->GetScrollRange());
	pNode->SetAttribute("range", StringConvertor::WideToUtf8(szBuf));

	_stprintf(szBuf, _T("%d"), pScrollbarUI->GetScrollPos());
	pNode->SetAttribute("value", StringConvertor::WideToUtf8(szBuf));

	if (pScrollbarUI->IsHorizontal())
		pNode->SetAttribute("hor",pScrollbarUI->IsHorizontal()?"true":"false");
}

void CDesignerCanvas::Save_CListUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CControlUI_Properties(pControlUI, pNode);
	CListUI* pListUI = static_cast<CListUI*>(pControlUI->GetInterface(_T("CList")));

	TCHAR szBuf[MAX_PATH] = {0};

	if (pListUI->GetHeader() && !pListUI->GetHeader()->IsVisible())
		pNode->SetAttribute("header", "hidden");

	if (pListUI->GetHeader() && pListUI->GetHeader()->GetBkImage() && _tcslen(pListUI->GetHeader()->GetBkImage()) > 0)
		pNode->SetAttribute("headerbkimage", StringConvertor::WideToUtf8(pListUI->GetHeader()->GetBkImage()));	

	RECT rcTextPadding = pListUI->GetItemTextPadding();
	if ((rcTextPadding.left != 0) || (rcTextPadding.right != 0) || (rcTextPadding.bottom != 0) || (rcTextPadding.top != 0))
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), rcTextPadding.left, rcTextPadding.top, rcTextPadding.right, rcTextPadding.bottom);
		pNode->SetAttribute("itemtextpadding", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itembkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetSelectedItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetSelectedItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetSelectedItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetSelectedItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetHotItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetHotItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhottextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetHotItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetHotItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhotbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetDisabledItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetDisabledItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetDisabledItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetDisabledItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetItemLineColor() != 0)
	{
		DWORD dwColor = pListUI->GetItemLineColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));

		pNode->SetAttribute("itemlinecolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetItemImage() && _tcslen(pListUI->GetItemImage()) > 0)
		pNode->SetAttribute("itemimage", StringConvertor::WideToUtf8(pListUI->GetItemImage()));

	if (pListUI->GetSelectedItemImage() && _tcslen(pListUI->GetSelectedItemImage()) > 0)
		pNode->SetAttribute("itemselectedimage", StringConvertor::WideToUtf8(pListUI->GetSelectedItemImage()));

	if (pListUI->GetHotItemImage() && _tcslen(pListUI->GetHotItemImage()) > 0)
		pNode->SetAttribute("itemhotimage", StringConvertor::WideToUtf8(pListUI->GetHotItemImage()));

	if (pListUI->GetDisabledItemImage() && _tcslen(pListUI->GetDisabledItemImage()) > 0)
		pNode->SetAttribute("itemdisabledimage", StringConvertor::WideToUtf8(pListUI->GetDisabledItemImage()));

	if (pListUI->IsItemShowHtml())
		pNode->SetAttribute("itemshowhtml",pListUI->IsItemShowHtml()?"true":"false");

	CContainerUI* pContainerUI = static_cast<CContainerUI*>(pControlUI->GetInterface(_T("Container")));
	RECT rcInset = pContainerUI->GetInset();
	if ((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), rcInset.left, rcInset.top, rcInset.right, rcInset.bottom);
		pNode->SetAttribute("inset", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListUI->GetVerticalScrollBar())
		pNode->SetAttribute("vscrollbar","true");

	if (pListUI->GetHorizontalScrollBar())
		pNode->SetAttribute("hscrollbar","true");

	if (pListUI->GetHeader())
	{
		CContainerUI* pContainerUI = static_cast<CContainerUI*>(pListUI->GetHeader()->GetInterface(_T("Container")));
		for( int it = 0; it < pContainerUI->GetCount(); it++ )
		{
			CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
			Save_Properties(pControl, pNode);
		}
	}
}

void CDesignerCanvas::Save_CComboUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CControlUI_Properties(pControlUI, pNode);
	CComboUI* pComboUI = static_cast<CComboUI*>(pControlUI->GetInterface(_T("Combo")));

	TCHAR szBuf[MAX_PATH] = {0};

	if ((pComboUI->GetDropBoxSize().cx != 0) || (pComboUI->GetDropBoxSize().cy != 0))
	{
		_stprintf(szBuf, _T("%d,%d"), pComboUI->GetDropBoxSize().cx, pComboUI->GetDropBoxSize().cy);
		pNode->SetAttribute("dropboxsize", StringConvertor::WideToUtf8(szBuf));
	}

	UINT dropType = pComboUI->GetDropType();
	switch (dropType)
	{
	case CComboUI::COMBODROP_SIMPLE:
		_stprintf(szBuf, _T("dropsimple"));
		pNode->SetAttribute("droptype", StringConvertor::WideToUtf8(szBuf));
		break;
	case CComboUI::COMBODROP_DOWN:
		_stprintf(szBuf, _T("dropdown"));
		pNode->SetAttribute("droptype", StringConvertor::WideToUtf8(szBuf));
		break;
	case CComboUI::COMBODROP_LIST:
		_stprintf(szBuf, _T("droplist"));
		pNode->SetAttribute("droptype", StringConvertor::WideToUtf8(szBuf));
		break;
	}

	tString tstrAlgin;
	UINT uTextStyle = pComboUI->GetDropBoxAlign();

	if (uTextStyle & CComboUI::DROPBOXALIGN_LEFT)
		tstrAlgin = _T("left");

	if(uTextStyle & CComboUI::DROPBOXALIGN_RIGHT)
		tstrAlgin = _T("right");

	if(uTextStyle & CComboUI::DROPBOXALIGN_TOP)
		tstrAlgin += _T("top");

	if(uTextStyle & CComboUI::DROPBOXALIGN_BOTTOM)
		tstrAlgin += _T("bottom");

	if (!tstrAlgin.empty())
		pNode->SetAttribute("dropboxalign", StringConvertor::WideToUtf8(tstrAlgin.c_str()));

	RECT rcItemPadding = pComboUI->GetItemTextPadding();
	if ((rcItemPadding.left != 0) || (rcItemPadding.right != 0) || (rcItemPadding.bottom != 0) || (rcItemPadding.top != 0))
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), rcItemPadding.left, rcItemPadding.top, rcItemPadding.right, rcItemPadding.bottom);
		pNode->SetAttribute("itemtextpadding", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemtextcolor", StringConvertor::WideToUtf8(szBuf));

	}

	if (pComboUI->GetItemBkColor() != 0)
	{
		DWORD dwColor = pComboUI->GetItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itembkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetSelectedItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetSelectedItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetSelectedItemBkColor() != 0)
	{
		DWORD dwColor = pComboUI->GetSelectedItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetHotItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetHotItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhottextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetHotItemBkColor() != 0)
	{
		DWORD dwColor = pComboUI->GetHotItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhotbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetDisabledItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetDisabledItemTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetDisabledItemBkColor() != 0)
	{

		DWORD dwColor = pComboUI->GetDisabledItemBkColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetItemLineColor() != 0)
	{
		DWORD dwColor = pComboUI->GetItemLineColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemlinecolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pComboUI->GetItemImage() && _tcslen(pComboUI->GetItemImage()) > 0)
		pNode->SetAttribute("itemimage", StringConvertor::WideToUtf8(pComboUI->GetItemImage()));

	if (pComboUI->GetSelectedItemImage() && _tcslen(pComboUI->GetSelectedItemImage()) > 0)
		pNode->SetAttribute("itemselectedimage", StringConvertor::WideToUtf8(pComboUI->GetSelectedItemImage()));

	if (pComboUI->GetHotItemImage() && _tcslen(pComboUI->GetHotItemImage()) > 0)
		pNode->SetAttribute("itemhotimage", StringConvertor::WideToUtf8(pComboUI->GetHotItemImage()));

	if (pComboUI->GetDisabledItemImage() && _tcslen(pComboUI->GetDisabledItemImage()) > 0)
		pNode->SetAttribute("itemdisabledimage", StringConvertor::WideToUtf8(pComboUI->GetDisabledItemImage()));

	if (pComboUI->GetDropBtnImage() && _tcslen(pComboUI->GetDropBtnImage()) > 0)
		pNode->SetAttribute("dropbtnimage", StringConvertor::WideToUtf8(pComboUI->GetDropBtnImage()));

	if (pComboUI->IsItemShowHtml())
		pNode->SetAttribute("itemshowhtml",pComboUI->IsItemShowHtml()?"true":"false");

	CContainerUI* pContainerUI = static_cast<CContainerUI*>(pControlUI->GetInterface(_T("Container")));
	RECT rcInset = pContainerUI->GetInset();
	if ((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), rcInset.left, rcInset.top, rcInset.right, rcInset.bottom);
		pNode->SetAttribute("inset", StringConvertor::WideToUtf8(szBuf));
	}
}

void CDesignerCanvas::Save_CListHeaderItemUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CControlUI_Properties(pControlUI, pNode);

	CListHeaderItemUI* pListHeaderItemUI = static_cast<CListHeaderItemUI*>(pControlUI->GetInterface(_T("ListHeaderItem")));

	TCHAR szBuf[MAX_PATH] = {0};

	if (pListHeaderItemUI->GetTextColor() != 0)
	{
		DWORD dwColor = pListHeaderItemUI->GetTextColor();					
		_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("textcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if (pListHeaderItemUI->GetSepWidth() != 0)
	{
		_stprintf(szBuf, _T("%d"), pListHeaderItemUI->GetSepWidth());
		pNode->SetAttribute("sepwidth", StringConvertor::WideToUtf8(szBuf));
	}

	if (!pListHeaderItemUI->IsDragable())
		pNode->SetAttribute("dragable", "false");

	tString tstrAlgin;
	UINT uTextStyle = pListHeaderItemUI->GetTextStyle();

	if (uTextStyle & DT_LEFT)
		tstrAlgin = _T("left");

	if(uTextStyle & DT_CENTER)
		tstrAlgin = _T("center");

	if(uTextStyle & DT_RIGHT)
		tstrAlgin = _T("right");

	if(uTextStyle & DT_TOP)
		tstrAlgin += _T("top");

	if(uTextStyle & DT_BOTTOM)
		tstrAlgin += _T("bottom");

	if(uTextStyle & DT_WORDBREAK)
		tstrAlgin += _T("wrap");

	if (!tstrAlgin.empty())
		pNode->SetAttribute("align", StringConvertor::WideToUtf8(tstrAlgin.c_str()));

	if (pListHeaderItemUI->GetNormalImage() && _tcslen(pListHeaderItemUI->GetNormalImage()) > 0)
		pNode->SetAttribute("normalimage", StringConvertor::WideToUtf8(pListHeaderItemUI->GetNormalImage()));

	if (pListHeaderItemUI->GetHotImage() && _tcslen(pListHeaderItemUI->GetHotImage()) > 0)
		pNode->SetAttribute("hotimage", StringConvertor::WideToUtf8(pListHeaderItemUI->GetHotImage()));

	if (pListHeaderItemUI->GetPushedImage() && _tcslen(pListHeaderItemUI->GetPushedImage()) > 0)
		pNode->SetAttribute("pushedimage", StringConvertor::WideToUtf8(pListHeaderItemUI->GetPushedImage()));

	if (pListHeaderItemUI->GetFocusedImage() && _tcslen(pListHeaderItemUI->GetFocusedImage()) > 0)
		pNode->SetAttribute("focusedimage", StringConvertor::WideToUtf8(pListHeaderItemUI->GetFocusedImage()));

	if (pListHeaderItemUI->GetSepImage() && _tcslen(pListHeaderItemUI->GetSepImage()) > 0)
		pNode->SetAttribute("sepimage", StringConvertor::WideToUtf8(pListHeaderItemUI->GetSepImage()));
}

void CDesignerCanvas::Save_CListElementUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CControlUI_Properties(pControlUI, pNode);
}

void CDesignerCanvas::Save_CContainerUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode)
{
	Save_CControlUI_Properties(pControlUI, pNode);
	CContainerUI* pContainerUI = static_cast<CContainerUI*>(pControlUI->GetInterface(_T("Container")));

	TCHAR szBuf[MAX_PATH] = {0};

	RECT rcInset = pContainerUI->GetInset();
	if ((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
	{
		_stprintf(szBuf, _T("%d,%d,%d,%d"), rcInset.left, rcInset.top, rcInset.right, rcInset.bottom);
		pNode->SetAttribute("inset", StringConvertor::WideToUtf8(szBuf));
	}
}

void CDesignerCanvas::Save_ContainerProperties(CControlUI* pControlUI, TiXmlElement* pParentNode)
{
	TiXmlElement* pNode = NULL;
	if ((static_cast<IContainerUI*>(pControlUI->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pControlUI->GetInterface(_T("Container"))) != NULL))
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);

		if (_tcsicmp(pControlUI->GetClass(), _T("ListUI")) == 0)
		{
			Save_CListUI_Properties(pControlUI, pNodeElement->ToElement());
		}
		else if (_tcsicmp(pControlUI->GetClass(), _T("ComboUI")) == 0)
		{
			Save_CComboUI_Properties(pControlUI, pNodeElement->ToElement());
		}
		else
			Save_CContainerUI_Properties(pControlUI, pNodeElement->ToElement());

		CContainerUI* pContainerUI = static_cast<CContainerUI*>(pControlUI->GetInterface(_T("Container")));
		for( int it = 0; it < pContainerUI->GetCount(); it++ )
		{
			CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
			Save_Properties(pControl, pNodeElement->ToElement());
		}
		
		delete pNode;
		pNode = NULL;
	}
}

void CDesignerCanvas::Save_SingleProperties(CControlUI* pControlUI, TiXmlElement* pParentNode)
{
	TiXmlElement* pNode = NULL;
	if (_tcsicmp(pControlUI->GetClass(), _T("ControlUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CControlUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("LabelUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CLabelUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("TextUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CLabelUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ButtonUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CButtonUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("OptionUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_COptionUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ProgressUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CProgressUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("SliderUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CSliderUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("EditUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CEditUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ScrollBarUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CScrollbarUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ListHeaderItemUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CListHeaderItemUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ListElementUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CListElementUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ListLabelElementUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CListElementUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ListTextElementUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CListElementUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}
	else if (_tcsicmp(pControlUI->GetClass(), _T("ListExpandElementUI")) == 0)
	{
		CString class_name = pControlUI->GetClass();
		class_name = class_name.Mid(0, class_name.GetLength() - 2);
		pNode = new TiXmlElement(StringConvertor::WideToUtf8(class_name.GetBuffer()));		
		Save_CListElementUI_Properties(pControlUI, pNode);
		TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	}

	if (pNode)
	{
		delete pNode;
		pNode = NULL;
	}
}

void CDesignerCanvas::Save_Properties(CControlUI* pControlUI, TiXmlElement* pParentNode)
{
	if ((pControlUI != NULL) && (pParentNode != NULL))
	{
		if ((static_cast<IContainerUI*>(pControlUI->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pControlUI->GetInterface(_T("Container"))) != NULL))
		{
			Save_ContainerProperties(pControlUI, pParentNode);
		}
		else
		{
			Save_SingleProperties(pControlUI, pParentNode);
		}
	}
}

void CDesignerCanvas::SaveSkinFile()
{
	tString tstrDefaultCanvas = m_tstrDefaultFeaturesSkinPath + _T("uidesigner_canvas.xml");
	tString tstrCurrentCanvas = m_tstrSkinPath + m_tstrSkinFile;
	if (_tcsicmp(tstrCurrentCanvas.c_str(), tstrDefaultCanvas.c_str()) == 0)
	{
		AfxMessageBox(_T("不支持修改默认画布，请通过新建或者打开菜单来打开一张别的画布！"));
		return;
	}
	else
	{
		HANDLE hFile = ::CreateFile(tstrCurrentCanvas.c_str(), GENERIC_ALL, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("打开画布失败，请检查皮肤文件！"));
			return;
		}
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		TCHAR szBuf[MAX_PATH] = {0};

		TiXmlDocument xmlDoc(StringConvertor::TcharToAnsi(tstrCurrentCanvas.c_str()));
		TiXmlDeclaration Declaration("1.0","utf-8","yes");
		xmlDoc.InsertEndChild(Declaration);

		TiXmlElement* pRootElm = new TiXmlElement("Window");

		CControlUI* pRoot = paint_manager_.GetRoot();
		if ((pRoot != NULL) && (static_cast<IContainerUI*>(pRoot->GetInterface(_T("IContainer"))) != NULL) && (static_cast<IContainerUI*>(pRoot->GetInterface(_T("Container"))) != NULL))
		{
			//<Window sizebox="4,4,6,6" caption="0,0,0,30" mininfo="800,570" roundcorner="20,20,20,20">
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pRoot->GetInterface(_T("Container")));
			RECT rcSize = paint_manager_.GetSizeBox();
			if ((rcSize.left != 0) || (rcSize.right != 0) || (rcSize.bottom != 0) || (rcSize.top != 0))
			{
				_stprintf(szBuf, _T("%d,%d,%d,%d"), rcSize.left, rcSize.top, rcSize.right, rcSize.bottom);
				pRootElm->SetAttribute("sizebox", StringConvertor::WideToUtf8(szBuf));
			}
			else
				pRootElm->SetAttribute("sizebox", "4,4,6,6");

			RECT rcCaption = paint_manager_.GetCaptionRect();
			if ((rcCaption.left != 0) || (rcCaption.right != 0) || (rcCaption.bottom != 0) || (rcCaption.top != 0))
			{
				_stprintf(szBuf, _T("%d,%d,%d,%d"), rcCaption.left, rcCaption.top, rcCaption.right, rcCaption.bottom);
				pRootElm->SetAttribute("caption", StringConvertor::WideToUtf8(szBuf));
			}
			else
				pRootElm->SetAttribute("caption", "0,0,0,30");

			SIZE szRoundCorner = paint_manager_.GetRoundCorner();
			if ((szRoundCorner.cx != 0) || (szRoundCorner.cy != 0))
			{
				_stprintf(szBuf, _T("%d,%d"), szRoundCorner.cx, szRoundCorner.cy);
				pRootElm->SetAttribute("roundcorner", StringConvertor::WideToUtf8(szBuf));
			}

			if ((m_formInfo.szCanvasSize.cx != 0) && (m_formInfo.szCanvasSize.cy != 0))
			{
				_stprintf(szBuf, _T("%d,%d"), m_formInfo.szCanvasSize.cx, m_formInfo.szCanvasSize.cy);
				pRootElm->SetAttribute("size", StringConvertor::WideToUtf8(szBuf));

				_stprintf(szBuf, _T("%d,%d"), m_formInfo.szCanvasSize.cx, m_formInfo.szCanvasSize.cy);
				pRootElm->SetAttribute("mininfo", StringConvertor::WideToUtf8(szBuf));
			}
			else
			{
				pRootElm->SetAttribute("mininfo", "0,0,0,30");
			}
			pRootElm->SetAttribute("opacity", m_formInfo.bOpacity?"true":"false");
			_stprintf(szBuf, _T("%d"), m_formInfo.dwTransparency);
			pRootElm->SetAttribute("transparency", StringConvertor::WideToUtf8(szBuf));

			if (paint_manager_.IsBackgroundTransparent())
				pRootElm->SetAttribute("bktrans", "true");

			if (paint_manager_.GetDefaultDisabledColor() != 0)
			{
				DWORD dwColor = paint_manager_.GetDefaultDisabledColor();
				_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
				pRootElm->SetAttribute("disabledfontcolor", StringConvertor::WideToUtf8(szBuf));
			}

			if (paint_manager_.GetDefaultFontColor() != 0)
			{
				DWORD dwColor = paint_manager_.GetDefaultFontColor();
				_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
				pRootElm->SetAttribute("defaultfontcolor", StringConvertor::WideToUtf8(szBuf));
			}

			if (paint_manager_.GetDefaultLinkFontColor() != 0)
			{
				DWORD dwColor = paint_manager_.GetDefaultLinkFontColor();
				_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
				pRootElm->SetAttribute("linkfontcolor", StringConvertor::WideToUtf8(szBuf));
			}

			if (paint_manager_.GetDefaultLinkHoverFontColor() != 0)
			{
				DWORD dwColor = paint_manager_.GetDefaultLinkHoverFontColor();
				_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
				pRootElm->SetAttribute("linkhoverfontcolor", StringConvertor::WideToUtf8(szBuf));
			}

			if (paint_manager_.GetDefaultSelectedBkColor() != 0)
			{
				DWORD dwColor = paint_manager_.GetDefaultSelectedBkColor();
				_stprintf(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
				pRootElm->SetAttribute("selectedcolor", StringConvertor::WideToUtf8(szBuf));
			}
		}
		TiXmlNode* pNode = xmlDoc.InsertEndChild(*pRootElm);

		if (paint_manager_.GetCustomFontCount() > 0)
		{
			TFontInfo* default_info = paint_manager_.GetDefaultFontInfo();
			HFONT hDefaultFont = default_info->hFont;
			LOGFONT lfDefault = {0};
			GetObject(hDefaultFont, sizeof(LOGFONT), &lfDefault);

			std::vector<LOGFONT> cachedFonts;

			for (DWORD index = 0; index < paint_manager_.GetCustomFontCount(); ++index)
			{
				HFONT hFont = paint_manager_.GetFont(index);
				LOGFONT lf = {0};
				GetObject(hFont, sizeof(LOGFONT), &lf);

				bool bSaved = false;
				for (std::vector<LOGFONT>::const_iterator citer = cachedFonts.begin(); citer != cachedFonts.end(); ++citer)
				{
					if ((citer->lfWeight == lf.lfWeight) && (citer->lfItalic == lf.lfItalic) && (citer->lfHeight == lf.lfHeight)
						&& (_tcsicmp(lf.lfFaceName, citer->lfFaceName) == 0))
					{
						bSaved = true;
						break;
					}
				}
				if (bSaved)
					continue;

				cachedFonts.push_back(lf);

				TiXmlElement* pFontElem = new TiXmlElement("Font");
				pFontElem->SetAttribute("name", StringConvertor::WideToUtf8(lf.lfFaceName));

				_stprintf(szBuf, _T("%d"), -lf.lfHeight);
				pFontElem->SetAttribute("size", StringConvertor::WideToUtf8(szBuf));

				pFontElem->SetAttribute("bold", (lf.lfWeight >= FW_BOLD)?"true":"false");
				pFontElem->SetAttribute("italic", lf.lfItalic?"true":"false");

				if ((lfDefault.lfWeight == lf.lfWeight) && (lfDefault.lfItalic == lf.lfItalic) && (lfDefault.lfHeight == lf.lfHeight)
					&& (_tcsicmp(lf.lfFaceName, lfDefault.lfFaceName) == 0))
					pFontElem->SetAttribute("default", "true");

				pNode->ToElement()->InsertEndChild(*pFontElem);

				delete pFontElem;
				pFontElem = NULL;
			}
		}

		const CStdStringPtrMap& defaultAttrHash = paint_manager_.GetDefaultAttribultes();
		if (defaultAttrHash.GetSize() > 0)
		{
			for (int index = 0; index < defaultAttrHash.GetSize(); ++index)
			{
				LPCTSTR lpstrKey = defaultAttrHash.GetAt(index);
				LPCTSTR lpstrAttribute = paint_manager_.GetDefaultAttributeList(lpstrKey);

				TiXmlElement* pAttributeElem = new TiXmlElement("Default");
				pAttributeElem->SetAttribute("name", StringConvertor::WideToUtf8(lpstrKey));

				tString tstrAttribute(lpstrAttribute);
				pAttributeElem->SetAttribute("value", StringConvertor::WideToUtf8(tstrAttribute.c_str()));

				pNode->ToElement()->InsertEndChild(*pAttributeElem);

				delete pAttributeElem;
				pAttributeElem = NULL;
			}
		}

		Save_Properties(pRoot, pNode->ToElement());

		delete pRootElm;
		xmlDoc.SaveFile();

		m_bSkinFileChanged = false;
	}
}

void CDesignerCanvas::Copy_CControlUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		if (pBasedControlUI->GetText() && _tcslen(pBasedControlUI->GetText()) > 0)
			pCopiedOrCutControl->SetText(pBasedControlUI->GetText());

		if (pBasedControlUI->GetToolTip() && _tcslen(pBasedControlUI->GetToolTip()) > 0)
			pCopiedOrCutControl->SetToolTip(pBasedControlUI->GetToolTip());

		pCopiedOrCutControl->SetVisible(pBasedControlUI->IsVisible());

		pCopiedOrCutControl->SetShortcut(pBasedControlUI->GetShortcut());

		if (pBasedControlUI->GetBorderSize() != 1)
			pCopiedOrCutControl->SetBorderSize(pBasedControlUI->GetBorderSize());

		if (pBasedControlUI->IsFloat())
		{
			pCopiedOrCutControl->SetFloat(true);
			pCopiedOrCutControl->SetFixedXY(pBasedControlUI->GetFixedXY());
			pCopiedOrCutControl->SetFixedWidth(pBasedControlUI->GetFixedWidth());
			pCopiedOrCutControl->SetFixedHeight(pBasedControlUI->GetFixedHeight());
		}
		else
		{
			if ((pBasedControlUI->GetFixedXY().cx != 0)
				|| (pBasedControlUI->GetFixedXY().cy != 0)
				|| (pBasedControlUI->GetFixedWidth() != 0)
				|| (pBasedControlUI->GetFixedHeight() != 0))
			{
				pCopiedOrCutControl->SetFixedXY(pBasedControlUI->GetFixedXY());
				pCopiedOrCutControl->SetFixedWidth(pBasedControlUI->GetFixedWidth());
				pCopiedOrCutControl->SetFixedHeight(pBasedControlUI->GetFixedHeight());
			}

			if (pBasedControlUI->GetFixedWidth() > 0)
				pCopiedOrCutControl->SetFixedWidth(pBasedControlUI->GetFixedWidth());

			if (pBasedControlUI->GetFixedHeight() > 0)
				pCopiedOrCutControl->SetFixedHeight(pBasedControlUI->GetFixedHeight());
		}

		RECT rcPadding = pBasedControlUI->GetPadding();
		if ((rcPadding.left != 0) || (rcPadding.right != 0) || (rcPadding.bottom != 0) || (rcPadding.top != 0))
			pCopiedOrCutControl->SetPadding(pBasedControlUI->GetPadding());

		if (pBasedControlUI->GetBkImage() && _tcslen(pBasedControlUI->GetBkImage()) > 0)
			pCopiedOrCutControl->SetBkImage(pBasedControlUI->GetBkImage());

		if (pBasedControlUI->GetBkColor() != 0)
			pCopiedOrCutControl->SetBkColor(pBasedControlUI->GetBkColor());

		if (pBasedControlUI->GetBkColor2() != 0)
			pCopiedOrCutControl->SetBkColor2(pBasedControlUI->GetBkColor2());

		if (pBasedControlUI->GetBorderColor() != 0)
			pCopiedOrCutControl->SetBorderColor(pBasedControlUI->GetBorderColor());

		if (pBasedControlUI->GetFocusBorderColor() != 0)
			pCopiedOrCutControl->SetFocusBorderColor(pBasedControlUI->GetFocusBorderColor());

		if (pBasedControlUI->GetMaxHeight() != 9999)
			pCopiedOrCutControl->SetMaxHeight(pBasedControlUI->GetMaxHeight());

		if (pBasedControlUI->GetMaxWidth() != 9999)
			pCopiedOrCutControl->SetMaxWidth(pBasedControlUI->GetMaxWidth());

		if (pBasedControlUI->GetMinWidth() != 0)
			pCopiedOrCutControl->SetMinWidth(pBasedControlUI->GetMinWidth());

		if (pBasedControlUI->GetMinHeight() != 0)
			pCopiedOrCutControl->SetMinHeight(pBasedControlUI->GetMinHeight());

		TRelativePosUI relativePos = pBasedControlUI->GetRelativePos();
		if (relativePos.bRelative)
		{
			SIZE szMove;
			SIZE szZoom;
			szMove.cx = relativePos.nMoveXPercent;
			szMove.cy = relativePos.nMoveYPercent;

			szZoom.cx = relativePos.nZoomXPercent;
			szZoom.cy = relativePos.nZoomYPercent;

			pCopiedOrCutControl->SetRelativePos(szMove, szZoom);
		}

		if (pBasedControlUI->GetStretchMode() && _tcslen(pBasedControlUI->GetStretchMode()) > 0)
			pCopiedOrCutControl->SetStretchMode(pBasedControlUI->GetStretchMode());
	}
}

void CDesignerCanvas::Copy_CLabelUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);

		CLabelUI* pBasedLabelUI = static_cast<CLabelUI*>(pBasedControlUI->GetInterface(_T("Label")));
		CLabelUI* pTargetLabelUI = static_cast<CLabelUI*>(pCopiedOrCutControl->GetInterface(_T("Label")));

		TCHAR szBuf[MAX_PATH] = {0};

		RECT rcTextPadding = pBasedLabelUI->GetTextPadding();
		if ((rcTextPadding.left != 0) || (rcTextPadding.right != 0) || (rcTextPadding.bottom != 0) || (rcTextPadding.top != 0))
			pTargetLabelUI->SetTextPadding(pBasedLabelUI->GetTextPadding());

		if (pBasedLabelUI->GetTextColor() != 0)
			pTargetLabelUI->SetTextColor(pBasedLabelUI->GetTextColor());

		if (pBasedLabelUI->GetDisabledTextColor() != 0)
			pTargetLabelUI->SetDisabledTextColor(pBasedLabelUI->GetDisabledTextColor());

		if (pBasedLabelUI->IsShowHtml())
			pTargetLabelUI->SetShowHtml(true);

		pTargetLabelUI->SetTextStyle(pBasedLabelUI->GetTextStyle());
	}
}

void CDesignerCanvas::Copy_CButtonUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CLabelUI_Properties(pBasedControlUI, pCopiedOrCutControl);

		CButtonUI* pBasedButtonUI = static_cast<CButtonUI*>(pBasedControlUI->GetInterface(_T("Button")));
		CButtonUI* pTargetButtonUI = static_cast<CButtonUI*>(pCopiedOrCutControl->GetInterface(_T("Button")));

		if (pBasedButtonUI->GetNormalImage() && _tcslen(pBasedButtonUI->GetNormalImage()) > 0)
			pTargetButtonUI->SetNormalImage(pBasedButtonUI->GetNormalImage());

		if (pBasedButtonUI->GetHotImage() && _tcslen(pBasedButtonUI->GetHotImage()) > 0)
			pTargetButtonUI->SetHotImage(pBasedButtonUI->GetHotImage());

		if (pBasedButtonUI->GetPushedImage() && _tcslen(pBasedButtonUI->GetPushedImage()) > 0)
			pTargetButtonUI->SetPushedImage(pBasedButtonUI->GetPushedImage());

		if (pBasedButtonUI->GetFocusedImage() && _tcslen(pBasedButtonUI->GetFocusedImage()) > 0)
			pTargetButtonUI->SetFocusedImage(pBasedButtonUI->GetFocusedImage());

		if (pBasedButtonUI->GetDisabledImage() && _tcslen(pBasedButtonUI->GetDisabledImage()) > 0)
			pTargetButtonUI->SetDisabledImage(pBasedButtonUI->GetDisabledImage());

		if (pBasedButtonUI->GetForeImage() && _tcslen(pBasedButtonUI->GetForeImage()) > 0)
			pTargetButtonUI->SetForeImage(pBasedButtonUI->GetForeImage());

		if (pBasedButtonUI->GetFocusedTextColor() != 0)
			pTargetButtonUI->SetFocusedTextColor(pBasedButtonUI->GetFocusedTextColor());					

		if (pBasedButtonUI->GetPushedTextColor() != 0)
			pTargetButtonUI->SetPushedTextColor(pBasedButtonUI->GetPushedTextColor());

		if (pBasedButtonUI->GetHotTextColor() != 0)
			pTargetButtonUI->SetHotTextColor(pBasedButtonUI->GetHotTextColor());
	}
}

void CDesignerCanvas::Copy_COptionUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CButtonUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		COptionUI* pBasedOptionUI = static_cast<COptionUI*>(pBasedControlUI->GetInterface(_T("Option")));
		COptionUI* pTargetOptionUI = static_cast<COptionUI*>(pCopiedOrCutControl->GetInterface(_T("Option")));

		TCHAR szBuf[MAX_PATH] = {0};

		pTargetOptionUI->SetImageFitAllArea(pBasedOptionUI->IsImageFitallArea());

		pTargetOptionUI->SetGroup(pBasedOptionUI->GetGroup());

		pTargetOptionUI->Selected(pBasedOptionUI->IsSelected());

		if (pBasedOptionUI->GetSelectedTextColor() != 0)
			pTargetOptionUI->SetSelectedTextColor(pBasedOptionUI->GetSelectedTextColor());

		if (pBasedOptionUI->GetSelectedImage() && _tcslen(pBasedOptionUI->GetSelectedImage()) > 0)
			pTargetOptionUI->SetSelectedImage(pBasedOptionUI->GetSelectedImage());
	}
}

void CDesignerCanvas::Copy_CProgressUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CLabelUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		CProgressUI* pBasedProgressUI = static_cast<CProgressUI*>(pBasedControlUI->GetInterface(_T("Progress")));
		CProgressUI* pTargetProgressUI = static_cast<CProgressUI*>(pCopiedOrCutControl->GetInterface(_T("Progress")));

		if (pBasedProgressUI->GetFgImage() && _tcslen(pBasedProgressUI->GetFgImage()) > 0)
			pTargetProgressUI->SetFgImage(pBasedProgressUI->GetFgImage());

		pTargetProgressUI->SetMinValue(pBasedProgressUI->GetMinValue());

		pTargetProgressUI->SetMaxValue(pBasedProgressUI->GetMaxValue());

		pTargetProgressUI->SetValue(pBasedProgressUI->GetValue());

		pTargetProgressUI->SetHorizontal(pBasedProgressUI->IsHorizontal());
	}
}

void CDesignerCanvas::Copy_CSliderUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CProgressUI_Properties(pBasedControlUI, pCopiedOrCutControl);

		CSliderUI* pBasedSliderUI = static_cast<CSliderUI*>(pBasedControlUI->GetInterface(_T("Slider")));
		CSliderUI* pTargetSliderUI = static_cast<CSliderUI*>(pCopiedOrCutControl->GetInterface(_T("Slider")));
		TCHAR szBuf[MAX_PATH] = {0};

		if (pBasedSliderUI->GetThumbImage() && _tcslen(pBasedSliderUI->GetThumbImage()) > 0)
			pTargetSliderUI->SetThumbImage(pBasedSliderUI->GetThumbImage());

		if (pBasedSliderUI->GetThumbHotImage() && _tcslen(pBasedSliderUI->GetThumbHotImage()) > 0)
			pTargetSliderUI->SetThumbHotImage(pBasedSliderUI->GetThumbHotImage());

		if (pBasedSliderUI->GetThumbPushedImage() && _tcslen(pBasedSliderUI->GetThumbPushedImage()) > 0)
			pTargetSliderUI->SetThumbPushedImage(pBasedSliderUI->GetThumbPushedImage());

		SIZE szThumb;
		szThumb.cx = pBasedSliderUI->GetThumbRect().right - pBasedSliderUI->GetThumbRect().left;
		szThumb.cy = pBasedSliderUI->GetThumbRect().bottom - pBasedSliderUI->GetThumbRect().top;
		pTargetSliderUI->SetThumbSize(szThumb);
	}
}

void CDesignerCanvas::Copy_CEditUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CLabelUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		CEditUI* pBasedEditUI = static_cast<CEditUI*>(pBasedControlUI->GetInterface(_T("Edit")));
		CEditUI* pTargetEditUI = static_cast<CEditUI*>(pCopiedOrCutControl->GetInterface(_T("Edit")));

		pTargetEditUI->SetPasswordMode(pBasedEditUI->IsPasswordMode());

		pTargetEditUI->SetReadOnly(pBasedEditUI->IsReadOnly());

		pTargetEditUI->SetDigitalNumber(pBasedEditUI->IsDigitalNumber());

		pTargetEditUI->SetMultiLine(pBasedEditUI->IsMultiLine());
	}
}

void CDesignerCanvas::Copy_CScrollbarUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		CScrollBarUI* pBasedScrollbarUI = static_cast<CScrollBarUI*>(pBasedControlUI->GetInterface(_T("ScrollBar")));
		CScrollBarUI* pTargetScrollbarUI = static_cast<CScrollBarUI*>(pCopiedOrCutControl->GetInterface(_T("ScrollBar")));

		if (pBasedScrollbarUI->GetBkNormalImage() && _tcslen(pBasedScrollbarUI->GetBkNormalImage()) > 0)
			pTargetScrollbarUI->SetBkNormalImage(pBasedScrollbarUI->GetBkNormalImage());

		if (pBasedScrollbarUI->GetBkHotImage() && _tcslen(pBasedScrollbarUI->GetBkHotImage()) > 0)
			pTargetScrollbarUI->SetBkHotImage(pBasedScrollbarUI->GetBkHotImage());

		if (pBasedScrollbarUI->GetBkPushedImage() && _tcslen(pBasedScrollbarUI->GetBkPushedImage()) > 0)
			pTargetScrollbarUI->SetBkPushedImage(pBasedScrollbarUI->GetBkPushedImage());

		if (pBasedScrollbarUI->GetBkDisabledImage() && _tcslen(pBasedScrollbarUI->GetBkDisabledImage()) > 0)
			pTargetScrollbarUI->SetBkDisabledImage(pBasedScrollbarUI->GetBkDisabledImage());

		if (pBasedScrollbarUI->GetRailNormalImage() && _tcslen(pBasedScrollbarUI->GetRailNormalImage()) > 0)
			pTargetScrollbarUI->SetRailNormalImage(pBasedScrollbarUI->GetRailNormalImage());

		if (pBasedScrollbarUI->GetRailHotImage() && _tcslen(pBasedScrollbarUI->GetRailHotImage()) > 0)
			pTargetScrollbarUI->SetRailHotImage(pBasedScrollbarUI->GetRailHotImage());

		if (pBasedScrollbarUI->GetRailPushedImage() && _tcslen(pBasedScrollbarUI->GetRailPushedImage()) > 0)
			pTargetScrollbarUI->SetRailPushedImage(pBasedScrollbarUI->GetRailPushedImage());

		if (pBasedScrollbarUI->GetRailDisabledImage() && _tcslen(pBasedScrollbarUI->GetRailDisabledImage()) > 0)
			pTargetScrollbarUI->SetRailDisabledImage(pBasedScrollbarUI->GetRailDisabledImage());

		if (pBasedScrollbarUI->GetThumbNormalImage() && _tcslen(pBasedScrollbarUI->GetThumbNormalImage()) > 0)
			pTargetScrollbarUI->SetThumbNormalImage(pBasedScrollbarUI->GetThumbNormalImage());

		if (pBasedScrollbarUI->GetThumbHotImage() && _tcslen(pBasedScrollbarUI->GetThumbHotImage()) > 0)
			pTargetScrollbarUI->SetThumbHotImage(pBasedScrollbarUI->GetThumbHotImage());

		if (pBasedScrollbarUI->GetThumbPushedImage() && _tcslen(pBasedScrollbarUI->GetThumbPushedImage()) > 0)
			pTargetScrollbarUI->SetThumbPushedImage(pBasedScrollbarUI->GetThumbPushedImage());

		if (pBasedScrollbarUI->GetThumbDisabledImage() && _tcslen(pBasedScrollbarUI->GetThumbDisabledImage()) > 0)
			pTargetScrollbarUI->SetThumbDisabledImage(pBasedScrollbarUI->GetThumbDisabledImage());

		if (pBasedScrollbarUI->GetButton2NormalImage() && _tcslen(pBasedScrollbarUI->GetButton2NormalImage()) > 0)
			pTargetScrollbarUI->SetButton2NormalImage(pBasedScrollbarUI->GetButton2NormalImage());

		if (pBasedScrollbarUI->GetButton2HotImage() && _tcslen(pBasedScrollbarUI->GetButton2HotImage()) > 0)
			pTargetScrollbarUI->SetButton2HotImage(pBasedScrollbarUI->GetButton2HotImage());

		if (pBasedScrollbarUI->GetButton2PushedImage() && _tcslen(pBasedScrollbarUI->GetButton2PushedImage()) > 0)
			pTargetScrollbarUI->SetButton2PushedImage(pBasedScrollbarUI->GetButton2PushedImage());

		if (pBasedScrollbarUI->GetButton2DisabledImage() && _tcslen(pBasedScrollbarUI->GetButton2DisabledImage()) > 0)
			pTargetScrollbarUI->SetButton2DisabledImage(pBasedScrollbarUI->GetButton2DisabledImage());

		if (pBasedScrollbarUI->GetButton1NormalImage() && _tcslen(pBasedScrollbarUI->GetButton1NormalImage()) > 0)
			pTargetScrollbarUI->SetButton1NormalImage(pBasedScrollbarUI->GetButton1NormalImage());

		if (pBasedScrollbarUI->GetButton1HotImage() && _tcslen(pBasedScrollbarUI->GetButton1HotImage()) > 0)
			pTargetScrollbarUI->SetButton1HotImage(pBasedScrollbarUI->GetButton1HotImage());

		if (pBasedScrollbarUI->GetButton1PushedImage() && _tcslen(pBasedScrollbarUI->GetButton1PushedImage()) > 0)
			pTargetScrollbarUI->SetButton1PushedImage(pBasedScrollbarUI->GetButton1PushedImage());

		if (pBasedScrollbarUI->GetButton1DisabledImage() && _tcslen(pBasedScrollbarUI->GetButton1DisabledImage()) > 0)
			pTargetScrollbarUI->SetButton1DisabledImage(pBasedScrollbarUI->GetButton1DisabledImage());

		pTargetScrollbarUI->SetLineSize(pBasedScrollbarUI->GetLineSize());

		pTargetScrollbarUI->SetScrollRange(pBasedScrollbarUI->GetScrollRange());

		pTargetScrollbarUI->SetScrollPos(pBasedScrollbarUI->GetScrollPos());

		pTargetScrollbarUI->SetHorizontal(pBasedScrollbarUI->IsHorizontal());
	}
}

void CDesignerCanvas::Copy_CListUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		CListUI* pBasedListUI = static_cast<CListUI*>(pBasedControlUI->GetInterface(_T("CList")));
		CListUI* pTargetListUI = static_cast<CListUI*>(pCopiedOrCutControl->GetInterface(_T("CList")));

		TCHAR szBuf[MAX_PATH] = {0};

		if (pBasedListUI->GetHeader() && !pBasedListUI->GetHeader()->IsVisible())
			pTargetListUI->GetHeader()->SetVisible(false);

		if (pBasedListUI->GetHeader() && pBasedListUI->GetHeader()->GetBkImage() && _tcslen(pBasedListUI->GetHeader()->GetBkImage()) > 0)
			pTargetListUI->GetHeader()->SetBkImage(pBasedListUI->GetHeader()->GetBkImage());	

		RECT rcTextPadding = pBasedListUI->GetItemTextPadding();
		if ((rcTextPadding.left != 0) || (rcTextPadding.right != 0) || (rcTextPadding.bottom != 0) || (rcTextPadding.top != 0))
			pTargetListUI->SetItemTextPadding(pBasedListUI->GetItemTextPadding());

		if (pBasedListUI->GetItemTextColor() != 0)
			pTargetListUI->SetItemTextColor(pBasedListUI->GetItemTextColor());

		if (pBasedListUI->GetItemBkColor() != 0)
			pTargetListUI->SetItemBkColor(pBasedListUI->GetItemBkColor());

		if (pBasedListUI->GetSelectedItemTextColor() != 0)
			pTargetListUI->SetSelectedItemTextColor(pBasedListUI->GetSelectedItemTextColor());

		if (pBasedListUI->GetSelectedItemBkColor() != 0)
			pTargetListUI->SetSelectedItemBkColor(pBasedListUI->GetSelectedItemBkColor());

		if (pBasedListUI->GetHotItemTextColor() != 0)
			pTargetListUI->SetHotItemTextColor(pBasedListUI->GetHotItemTextColor());

		if (pBasedListUI->GetHotItemBkColor() != 0)
			pTargetListUI->SetHotItemBkColor(pBasedListUI->GetHotItemBkColor());

		if (pBasedListUI->GetDisabledItemTextColor() != 0)
			pTargetListUI->SetDisabledItemTextColor(pBasedListUI->GetDisabledItemTextColor());

		if (pBasedListUI->GetDisabledItemBkColor() != 0)
			pTargetListUI->SetDisabledItemBkColor(pBasedListUI->GetDisabledItemBkColor());

		if (pBasedListUI->GetItemLineColor() != 0)
			pTargetListUI->SetItemLineColor(pBasedListUI->GetItemLineColor());

		if (pBasedListUI->GetItemImage() && _tcslen(pBasedListUI->GetItemImage()) > 0)
			pTargetListUI->SetItemImage(pBasedListUI->GetItemImage());

		if (pBasedListUI->GetSelectedItemImage() && _tcslen(pBasedListUI->GetSelectedItemImage()) > 0)
			pTargetListUI->SetSelectedItemImage(pBasedListUI->GetSelectedItemImage());

		if (pBasedListUI->GetHotItemImage() && _tcslen(pBasedListUI->GetHotItemImage()) > 0)
			pTargetListUI->SetHotItemImage(pBasedListUI->GetHotItemImage());

		if (pBasedListUI->GetDisabledItemImage() && _tcslen(pBasedListUI->GetDisabledItemImage()) > 0)
			pTargetListUI->SetDisabledItemImage(pBasedListUI->GetDisabledItemImage());

		if (pBasedListUI->IsItemShowHtml())
			pTargetListUI->SetItemShowHtml(true);

		CContainerUI* pBasedContainerUI = static_cast<CContainerUI*>(pBasedControlUI->GetInterface(_T("Container")));
		CContainerUI* pTargetContainerUI = static_cast<CContainerUI*>(pCopiedOrCutControl->GetInterface(_T("Container")));
		RECT rcInset = pBasedContainerUI->GetInset();
		if ((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
			pTargetContainerUI->SetInset(pBasedContainerUI->GetInset());
		
		pTargetListUI->EnableScrollBar(pBasedListUI->GetVerticalScrollBar() != NULL, pBasedListUI->GetHorizontalScrollBar() != NULL);

		if (pBasedListUI->GetHeader())
		{
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pBasedListUI->GetHeader()->GetInterface(_T("Container")));
			for( int it = 0; it < pContainerUI->GetCount(); it++ )
			{
				CControlUI* pNewControlUI = NULL;
				CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
				CopyControl(pControl, pNewControlUI);
				if (pNewControlUI != NULL)
					static_cast<CContainerUI*>(pTargetListUI->GetHeader()->GetInterface(_T("Container")))->Add(pNewControlUI);
			}
		}
	}
}

void CDesignerCanvas::Copy_CComboUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		CComboUI* pBasedComboUI = static_cast<CComboUI*>(pBasedControlUI->GetInterface(_T("Combo")));
		CComboUI* pTargetComboUI = static_cast<CComboUI*>(pCopiedOrCutControl->GetInterface(_T("Combo")));

		TCHAR szBuf[MAX_PATH] = {0};

		if ((pBasedComboUI->GetDropBoxSize().cx != 0) || (pBasedComboUI->GetDropBoxSize().cy != 0))
			pTargetComboUI->SetDropBoxSize(pBasedComboUI->GetDropBoxSize());

		pTargetComboUI->SetDropType(pBasedComboUI->GetDropType());

		RECT rcItemPadding = pBasedComboUI->GetItemTextPadding();
		if ((rcItemPadding.left != 0) || (rcItemPadding.right != 0) || (rcItemPadding.bottom != 0) || (rcItemPadding.top != 0))
			pTargetComboUI->SetItemTextPadding(pBasedComboUI->GetItemTextPadding());

		if (pBasedComboUI->GetItemTextColor() != 0)
			pTargetComboUI->SetItemTextColor(pBasedComboUI->GetItemTextColor());

		if (pBasedComboUI->GetItemBkColor() != 0)
			pTargetComboUI->SetItemBkColor(pBasedComboUI->GetItemBkColor());

		if (pBasedComboUI->GetSelectedItemTextColor() != 0)
			pTargetComboUI->SetSelectedItemTextColor(pBasedComboUI->GetSelectedItemTextColor());

		if (pBasedComboUI->GetSelectedItemBkColor() != 0)
			pTargetComboUI->SetSelectedItemBkColor(pBasedComboUI->GetSelectedItemBkColor());

		if (pBasedComboUI->GetHotItemTextColor() != 0)
			pTargetComboUI->SetHotItemTextColor(pBasedComboUI->GetHotItemTextColor());

		if (pBasedComboUI->GetHotItemBkColor() != 0)
			pTargetComboUI->SetHotItemBkColor(pBasedComboUI->GetHotItemBkColor());

		if (pBasedComboUI->GetDisabledItemTextColor() != 0)
			pTargetComboUI->SetDisabledItemTextColor(pBasedComboUI->GetDisabledItemTextColor());

		if (pBasedComboUI->GetDisabledItemBkColor() != 0)
			pTargetComboUI->SetDisabledItemBkColor(pBasedComboUI->GetDisabledItemBkColor());

		if (pBasedComboUI->GetItemLineColor() != 0)
			pTargetComboUI->SetItemLineColor(pBasedComboUI->GetItemLineColor());

		if (pBasedComboUI->GetItemImage() && _tcslen(pBasedComboUI->GetItemImage()) > 0)
			pTargetComboUI->SetItemImage(pBasedComboUI->GetItemImage());

		if (pBasedComboUI->GetSelectedItemImage() && _tcslen(pBasedComboUI->GetSelectedItemImage()) > 0)
			pTargetComboUI->SetSelectedItemImage(pBasedComboUI->GetSelectedItemImage());

		if (pBasedComboUI->GetHotItemImage() && _tcslen(pBasedComboUI->GetHotItemImage()) > 0)
			pTargetComboUI->SetHotItemImage(pBasedComboUI->GetHotItemImage());

		if (pBasedComboUI->GetDisabledItemImage() && _tcslen(pBasedComboUI->GetDisabledItemImage()) > 0)
			pTargetComboUI->SetDisabledItemImage(pBasedComboUI->GetDisabledItemImage());

		if (pBasedComboUI->GetDropBtnImage() && _tcslen(pBasedComboUI->GetDropBtnImage()) > 0)
			pTargetComboUI->SetDropBtnImage(pBasedComboUI->GetDropBtnImage());

		if (pBasedComboUI->IsItemShowHtml())
			pTargetComboUI->SetItemShowHtml(true);

		CContainerUI* pBasedContainerUI = static_cast<CContainerUI*>(pBasedComboUI->GetInterface(_T("Container")));
		CContainerUI* pTargetContainerUI = static_cast<CContainerUI*>(pTargetComboUI->GetInterface(_T("Container")));
		RECT rcInset = pBasedContainerUI->GetInset();
		if ((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
			pTargetComboUI->SetInset(pBasedContainerUI->GetInset());
	}
}

void CDesignerCanvas::Copy_CListHeaderItemUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);

		CListHeaderItemUI* pBasedListHeaderItemUI = static_cast<CListHeaderItemUI*>(pBasedControlUI->GetInterface(_T("ListHeaderItem")));
		CListHeaderItemUI* pTargetListHeaderItemUI = static_cast<CListHeaderItemUI*>(pCopiedOrCutControl->GetInterface(_T("ListHeaderItem")));

		if (pBasedListHeaderItemUI->GetTextColor() != 0)
			pTargetListHeaderItemUI->SetTextColor(pBasedListHeaderItemUI->GetTextColor());

		if (pBasedListHeaderItemUI->GetSepWidth() != 0)
			pTargetListHeaderItemUI->SetSepWidth(pBasedListHeaderItemUI->GetSepWidth());

		if (!pBasedListHeaderItemUI->IsDragable())
			pTargetListHeaderItemUI->SetDragable(false);

		pTargetListHeaderItemUI->SetTextStyle(pBasedListHeaderItemUI->GetTextStyle());

		if (pBasedListHeaderItemUI->GetNormalImage() && _tcslen(pBasedListHeaderItemUI->GetNormalImage()) > 0)
			pTargetListHeaderItemUI->SetNormalImage(pBasedListHeaderItemUI->GetNormalImage());

		if (pBasedListHeaderItemUI->GetHotImage() && _tcslen(pBasedListHeaderItemUI->GetHotImage()) > 0)
			pTargetListHeaderItemUI->SetHotImage(pBasedListHeaderItemUI->GetHotImage());

		if (pBasedListHeaderItemUI->GetPushedImage() && _tcslen(pBasedListHeaderItemUI->GetPushedImage()) > 0)
			pTargetListHeaderItemUI->SetPushedImage(pBasedListHeaderItemUI->GetPushedImage());

		if (pBasedListHeaderItemUI->GetFocusedImage() && _tcslen(pBasedListHeaderItemUI->GetFocusedImage()) > 0)
			pTargetListHeaderItemUI->SetFocusedImage(pBasedListHeaderItemUI->GetFocusedImage());

		if (pBasedListHeaderItemUI->GetSepImage() && _tcslen(pBasedListHeaderItemUI->GetSepImage()) > 0)
			pTargetListHeaderItemUI->SetSepImage(pBasedListHeaderItemUI->GetSepImage());
	}
}

void CDesignerCanvas::Copy_CListElementUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);
	}
}

void CDesignerCanvas::Copy_CContainerUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);

		CContainerUI* pBasedContainerUI = static_cast<CContainerUI*>(pBasedControlUI->GetInterface(_T("Container")));
		CContainerUI* pTargetContainerUI = static_cast<CContainerUI*>(pCopiedOrCutControl->GetInterface(_T("Container")));
		
		RECT rcInset = pBasedContainerUI->GetInset();
		if ((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
			pTargetContainerUI->SetInset(pBasedContainerUI->GetInset());
	}
}

void CDesignerCanvas::Copy_ContainerProperties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL) && (static_cast<IContainerUI*>(pBasedControlUI->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pBasedControlUI->GetInterface(_T("Container"))) != NULL))
	{
		if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListUI")) == 0)
		{
			Copy_CListUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ComboUI")) == 0)
		{
			Copy_CComboUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else
			Copy_CContainerUI_Properties(pBasedControlUI, pCopiedOrCutControl);

		CContainerUI* pContainerUI = static_cast<CContainerUI*>(pBasedControlUI->GetInterface(_T("Container")));
		for( int it = 0; it < pContainerUI->GetCount(); it++ )
		{
			CControlUI* pNewControlUI = NULL;
			CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
			CopyControl(pControl, pNewControlUI);
			if (pNewControlUI != NULL)
				static_cast<CContainerUI*>(pCopiedOrCutControl->GetInterface(_T("Container")))->Add(pNewControlUI);
		}
	}
}

void CDesignerCanvas::Copy_SingleProperties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		if (_tcsicmp(pBasedControlUI->GetClass(), _T("ControlUI")) == 0)
		{	
			Copy_CControlUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("LabelUI")) == 0)
		{	
			Copy_CLabelUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("TextUI")) == 0)
		{	
			Copy_CLabelUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ButtonUI")) == 0)
		{
			Copy_CButtonUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("OptionUI")) == 0)
		{	
			Copy_COptionUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ProgressUI")) == 0)
		{	
			Copy_CProgressUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("SliderUI")) == 0)
		{
			Copy_CSliderUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("EditUI")) == 0)
		{
			Copy_CEditUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ScrollBarUI")) == 0)
		{
			Copy_CScrollbarUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListHeaderItemUI")) == 0)
		{
			Copy_CListHeaderItemUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListElementUI")) == 0)
		{
			Copy_CListElementUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListLabelElementUI")) == 0)
		{
			Copy_CListElementUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListTextElementUI")) == 0)
		{
			Copy_CListElementUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
		else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListExpandElementUI")) == 0)
		{
			Copy_CListElementUI_Properties(pBasedControlUI, pCopiedOrCutControl);
		}
	}
}

void CDesignerCanvas::Copy_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl != NULL))
	{
		if ((static_cast<IContainerUI*>(pBasedControlUI->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pBasedControlUI->GetInterface(_T("Container"))) != NULL))
		{
			Copy_ContainerProperties(pBasedControlUI, pCopiedOrCutControl);
		}
		else
		{
			Copy_SingleProperties(pBasedControlUI, pCopiedOrCutControl);
		}
	}
}

void CDesignerCanvas::CopyControl(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl)
{
	if ((pBasedControlUI != NULL) && (pCopiedOrCutControl == NULL))
	{
		if ((static_cast<IContainerUI*>(pBasedControlUI->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pBasedControlUI->GetInterface(_T("Container"))) != NULL))
		{
			if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListUI")) == 0)
			{
				pCopiedOrCutControl = new CListUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ComboUI")) == 0)
			{
				pCopiedOrCutControl = new CComboUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("VerticalLayoutUI")) == 0)
			{
				pCopiedOrCutControl = new CVerticalLayoutUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("HorizontalLayoutUI")) == 0)
			{
				pCopiedOrCutControl = new CHorizontalLayoutUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("TileLayoutUI")) == 0)
			{
				pCopiedOrCutControl = new CTileLayoutUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("TabLayoutUI")) == 0)
			{
				pCopiedOrCutControl = new CDialogLayoutUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("TabLayoutUI")) == 0)
			{
				pCopiedOrCutControl = new CTabLayoutUI;
			}
			else
				pCopiedOrCutControl = new CContainerUI;
		}
		else
		{
			if (_tcsicmp(pBasedControlUI->GetClass(), _T("ControlUI")) == 0)
			{
				pCopiedOrCutControl = new CControlUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("LabelUI")) == 0)
			{
				pCopiedOrCutControl = new CLabelUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("TextUI")) == 0)
			{
				pCopiedOrCutControl = new CTextUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ButtonUI")) == 0)
			{
				pCopiedOrCutControl = new CButtonUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("OptionUI")) == 0)
			{
				pCopiedOrCutControl = new COptionUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ProgressUI")) == 0)
			{
				pCopiedOrCutControl = new CProgressUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("SliderUI")) == 0)
			{
				pCopiedOrCutControl = new CSliderUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("EditUI")) == 0)
			{
				pCopiedOrCutControl = new CEditUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ScrollBarUI")) == 0)
			{
				pCopiedOrCutControl = new CScrollBarUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListHeaderItemUI")) == 0)
			{
				pCopiedOrCutControl = new CListHeaderItemUI;
			}
			//else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListElementUI")) == 0)
			//{
			//	pCopiedOrCutControl = new CListElementUI;
			//}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListLabelElementUI")) == 0)
			{
				pCopiedOrCutControl = new CListLabelElementUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListTextElementUI")) == 0)
			{
				pCopiedOrCutControl = new CListTextElementUI;
			}
			else if (_tcsicmp(pBasedControlUI->GetClass(), _T("ListExpandElementUI")) == 0)
			{
				pCopiedOrCutControl = new CListExpandElementUI;
			}
		}
		if (pCopiedOrCutControl != NULL)
			Copy_Properties(pBasedControlUI, pCopiedOrCutControl);
	}
}

void CDesignerCanvas::Undo()
{
	AfxMessageBox(_T("该功能暂未实现！"));
}

void CDesignerCanvas::Redo()
{
	AfxMessageBox(_T("该功能暂未实现！"));
}