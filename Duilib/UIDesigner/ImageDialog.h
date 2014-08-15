#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ImagePreview.h"

//////////////////////////////////////////////////////////////////////////
// CImageDialog dialog
class CDesignerCanvas;

class CImageDialog : public CDialog
{
	DECLARE_DYNAMIC(CImageDialog)

public:
	CImageDialog(const CString& strImageProperty, CDesignerCanvas* pDesignerCanvas, CWnd* pParent = NULL);
	virtual ~CImageDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMAGE };

public:
	CString GetImage() const;

private:
	CString m_strImageProperty;
	CString m_strImagePathName;
	CRect m_rcDest;
	CRect m_rcSource;
	CRect m_rcCorner;
	CPaintManagerUI* m_pManager;
	CDesignerCanvas* m_pDesignerCanvas;

	static const CString m_strNullImage;

protected:
	RECT StringToRect(CString& str);
	void SetImageProperty(LPCTSTR pstrPathName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_lstImages;
	CString m_strDest;
	CString m_strSource;
	CString m_strCorner;
	CButton m_btnMask;
	CButton m_btnDiv;
	CString m_strDivOp1;
	CString m_strDivOp2;
	CString m_strDivResult;
	CString m_strImageSize;
	CSliderCtrl m_ctlFade;
	int m_nFade;
	BOOL m_bHole;
	DWORD m_crMask;
	CImagePreview m_ImagePreview;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonImageImport();
	afx_msg void OnBnClickedButtonImageClear();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnKillfocusEditImageFade();
	afx_msg void OnLbnDblclkListImageResource();
	afx_msg void OnDestroy();
	afx_msg void OnEnKillfocusEditImageDest();
	afx_msg void OnEnKillfocusEditImageSource();
	afx_msg void OnEnKillfocusEditImageCorner();
	afx_msg void OnBnClickedButtonImageMask();
	afx_msg void OnBnClickedRadioImageHoleFalse();
	afx_msg void OnBnClickedRadioImageHoleTrue();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton_Div();
};
