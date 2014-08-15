#pragma once
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
// CImagePreview

class CImagePreview : public CStatic
{
	DECLARE_DYNAMIC(CImagePreview)

public:
	CImagePreview();
	virtual ~CImagePreview();

public:
	CString SetImageProperty(LPCTSTR pstrImage,CRect& rcDest,CRect& rcSource,CRect& rcCorner,COLORREF clrMask,int nFade,BOOL bHole);
	LPCTSTR GetImageProperty() const { return m_strProperty; }
	void SetManager(CPaintManagerUI* pManager);

private:
	bool DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, 
		LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

private:
	CString m_strImage;
	CString m_strProperty;
	CRect m_rcImage;
	CPaintManagerUI* m_pManager;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


