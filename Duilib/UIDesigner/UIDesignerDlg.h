// UIDesignerDlg.h : header file
//

#pragma once

#define MARGIN 5
#define TOOLBOX_WIDTH 400
#define TOOLBAR_WIDTH 800
#define TOOLBAR_HEIGHT 90
#define PROPERTIES_SHEET_MIN_HEIGHT 32
#define ADJUSTMENTTOOLBAR_WIDTH 45
#define ADJUSTMENTTOOLBAR_HEIGHT ((14 * 28) + 32)
#define PROPERTIES_SHEET_WIDTH 500

extern LPCTSTR lpszSkinPath;/* = _T("skins\\UIDesigner\\");*/
extern LPCTSTR lpszZipSkin;/* = _T("UIDesigner.zip");*/

//#define USING_ZIP_SKIN

class CToolbar;
class CToolBox;
class CDesignerCanvas;
class CAdjustmentToolbar;

// CUIDesignerDlg dialog
class CUIDesignerDlg : public CDialog
{
// Construction
public:
	CUIDesignerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_UIDESIGNER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void ShowAboutDialog();

private:
	CToolbar*		m_pToolbar;
	CToolBox*		m_pControlToolBox;
	CAdjustmentToolbar*	m_pAdjustmentToolbar;
	CDesignerCanvas*	m_pCanvas;
};
