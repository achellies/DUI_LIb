// UIDesigner.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "UIDesigner.h"
#include "UIDesignerDlg.h"
#include "mem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIDesignerApp

BEGIN_MESSAGE_MAP(CUIDesignerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUIDesignerApp construction

CUIDesignerApp::CUIDesignerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CUIDesignerApp object

CUIDesignerApp theApp;


// CUIDesignerApp initialization

BOOL CUIDesignerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CPaintManagerUI::SetInstance(AfxGetInstanceHandle());

	HRESULT Hr = ::CoInitialize(NULL);

	CUIDesignerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	CPaintManagerUI::ShutDown();
	::CoUninitialize();

#ifdef DETECET_MEMORY_LEAK	

	for (map<DWORD,MemoryInfo>::iterator iter = g_MemList.begin(); iter != g_MemList.end(); ++iter)
	{
		printf("\tFile: %s;\tLine:%d;\tLeak Size :%d\n",iter->second.tszFile,iter->second.cchLine,iter->second.nMemSize);
	}
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
