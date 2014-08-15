// UIDesignerDlg.h : header file
//

#pragma once
#include "stdafx.h"
#include "resource.h"

using DuiLib::CControlUI;
using DuiLib::INotifyUI;

bool GetVerString(unsigned int maxStrSize, TCHAR *szComments,
				  TCHAR *szCompanyName, TCHAR *szFileDescription,
				  TCHAR *szFileVersion, TCHAR *szLegalCopyright,
				  TCHAR *szProductName, TCHAR *szProductVersion);

class CAboutDlg;

class CDialogUI:public CWindowWnd, public INotifyUI
{
public:
	CDialogUI()
		: m_pHostDlg(NULL)
	{}
	~CDialogUI(){}
	LPCTSTR GetWindowClassName() const { return _T("UIDialog"); }
	UINT GetClassStyle() const { return CS_DBLCLKS; }

	void SetHostDlg(CAboutDlg* pDlg);

	void LoadSkin();

	void Notify(TNotifyUI& msg);

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CPaintManagerUI paint_manager_;
	CAboutDlg*		m_pHostDlg;
};

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

public:
	void Close();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

private:
	CDialogUI m_dlgWnd;
};