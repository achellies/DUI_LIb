#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "frameWnd.h"

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

	CFrameWnd* pFrame = new CFrameWnd();
	if( pFrame == NULL ) return 0;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	pFrame->Create(NULL, _T("iReaderÔÄ¶ÁÆ÷"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 600, 800);
#else
	pFrame->Create(NULL, _T("iReaderÔÄ¶ÁÆ÷"), UI_WNDSTYLE_FRAME, 0L, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::ShutDown();

	::CoUninitialize();
	return 0;
}