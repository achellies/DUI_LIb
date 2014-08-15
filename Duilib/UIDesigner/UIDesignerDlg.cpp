// UIDesignerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Toolbar.h"
#include "ToolBox.h"
#include "AboutDlg.h"
#include "UIDesigner.h"
#include "UIDesignerDlg.h"
#include "DesignerCanvas.h"
#include "PropertiesSheet.h"
#include "AdjustmentToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LPCTSTR lpszSkinPath = _T("skins\\UIDesigner\\");

LPCTSTR lpszZipSkin = _T("UIDesigner.zip");

// CUIDesignerDlg dialog


CUIDesignerDlg::CUIDesignerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUIDesignerDlg::IDD, pParent)
	, m_pAdjustmentToolbar(NULL)
	, m_pControlToolBox(NULL)
	, m_pToolbar(NULL)
	, m_pCanvas(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUIDesignerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUIDesignerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_GETDLGCODE()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CUIDesignerDlg message handlers

BOOL CUIDesignerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			return FALSE;
		default:
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

UINT CUIDesignerDlg::OnGetDlgCode()
{
	return DLGC_WANTARROWS | CDialog::OnGetDlgCode();
}

BOOL CUIDesignerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	TCHAR szVersion[MAX_PATH] = {0};
	_stprintf(szVersion, _T("UIDesigner Version 1.1.0.0"));

	TCHAR szCopyright[MAX_PATH] = {0};
	_stprintf(szCopyright, _T("Copyright (C) 2011 achellies All Right Reserved."));

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

	SetWindowText(szVersion);

	RECT rcClient = {0};
	GetClientRect(&rcClient);

	m_pControlToolBox = new CToolBox();

	RECT rcCanvas = rcClient;
	rcCanvas.top += TOOLBAR_HEIGHT + 2 * MARGIN;
	rcCanvas.left = rcClient.left + MARGIN;	
	m_pCanvas = new CDesignerCanvas(m_pControlToolBox);
	m_pControlToolBox->SetCanvas(m_pCanvas);

	SIZE szCanvasSize = m_pCanvas->GetFormInfo().szCanvasSize;
	if (m_pCanvas != NULL)
	{
		m_pCanvas->Create(m_hWnd, _T("Canvas"), WS_CHILDWINDOW | WS_VISIBLE, 0, rcCanvas.left + MARGIN, rcCanvas.top + MARGIN, szCanvasSize.cx, szCanvasSize.cy);
		::MoveWindow(*m_pCanvas, rcCanvas.left + MARGIN, rcCanvas.top + MARGIN, szCanvasSize.cx, szCanvasSize.cy, TRUE);
	}	
	
	RECT rcToolBox = rcClient;
	rcToolBox.left = rcClient.left + szCanvasSize.cx + 5 * MARGIN;
	rcToolBox.right = rcToolBox.left + TOOLBOX_WIDTH;

	rcToolBox.top += TOOLBAR_HEIGHT;
	rcToolBox.bottom = rcToolBox.top + 600;
	if (m_pControlToolBox != NULL)
	{
		m_pControlToolBox->Create(m_hWnd, _T("Toolbox"), WS_CHILDWINDOW | WS_VISIBLE | WS_POPUP, 0, rcToolBox.left + MARGIN, rcToolBox.top + MARGIN, rcToolBox.right - rcToolBox.left, rcToolBox.bottom - rcToolBox.top - 2 * MARGIN);
		::MoveWindow(*m_pControlToolBox, rcToolBox.left + MARGIN, rcToolBox.top + MARGIN, rcToolBox.right - rcToolBox.left, rcToolBox.bottom - rcToolBox.top - 2 * MARGIN, TRUE);
	}

	RECT rcAdjustment = rcClient;
	rcAdjustment.left = rcToolBox.right + 5 * MARGIN;
	rcAdjustment.right = rcAdjustment.left + ADJUSTMENTTOOLBAR_WIDTH;
	rcAdjustment.top += TOOLBAR_HEIGHT;
	rcAdjustment.bottom = rcAdjustment.top + ADJUSTMENTTOOLBAR_HEIGHT + MARGIN;
	m_pAdjustmentToolbar = new CAdjustmentToolbar(m_pCanvas);
	if (m_pAdjustmentToolbar != NULL)
	{
		m_pAdjustmentToolbar->Create(m_hWnd, _T("Adjust"), WS_CHILDWINDOW | WS_VISIBLE | WS_POPUP, 0, rcAdjustment.left, rcAdjustment.top, rcAdjustment.right - rcAdjustment.left, rcAdjustment.bottom - rcAdjustment.top);
		::MoveWindow(*m_pAdjustmentToolbar, rcAdjustment.left, rcAdjustment.top, rcAdjustment.right - rcAdjustment.left, rcAdjustment.bottom - rcAdjustment.top, TRUE);
	}

	RECT rcToolbar = rcClient;
	rcToolbar.left += MARGIN;
	rcToolbar.right = rcToolbar.left + TOOLBAR_WIDTH;
	rcToolbar.top += MARGIN;
	rcToolbar.bottom = rcToolbar.top + TOOLBAR_HEIGHT;
	m_pToolbar = new CToolbar(this, m_pCanvas);
	if (m_pToolbar != NULL)
	{
		m_pToolbar->Create(m_hWnd, _T("Toolbar"), WS_CHILDWINDOW | WS_VISIBLE, 0, rcToolbar.left, rcToolbar.top, rcToolbar.right - rcToolbar.left, rcToolbar.bottom - rcToolbar.top);
		::MoveWindow(*m_pToolbar, rcToolbar.left, rcToolbar.top, rcToolbar.right - rcToolbar.left, rcToolbar.bottom - rcToolbar.top, TRUE);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUIDesignerDlg::OnSize(UINT nType, int cx, int cy)
{
	if (IsIconic())
		return;

	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}

void CUIDesignerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUIDesignerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		DuiLib::CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting
		CBrush white_brush(RGB(0xF8, 0xF8, 0xF8));
		CRect rcClient;
		GetClientRect(&rcClient);
		dc.FillRect(&rcClient, &white_brush);
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUIDesignerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUIDesignerDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CUIDesignerDlg::OnClose()
{
	if (m_pControlToolBox != NULL)
	{
		m_pControlToolBox->Close();
	}

	if (m_pAdjustmentToolbar != NULL)
	{
		m_pAdjustmentToolbar->Close();
	}

	if (m_pCanvas != NULL)
	{
		m_pCanvas->Close();
	}

	CDialog::OnClose();
}

void CUIDesignerDlg::ShowAboutDialog()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}