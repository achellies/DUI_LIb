// ImageDialog.cpp : implementation file
//
#include "stdafx.h"
#include "UIDesigner.h"
#include "ImageDialog.h"
#include "DesignerCanvas.h"

static const int nImageHorzMargin = 8;
const CString CImageDialog::m_strNullImage=_T("");

//////////////////////////////////////////////////////////////////////////
// CImageDialog dialog

IMPLEMENT_DYNAMIC(CImageDialog, CDialog)

CImageDialog::CImageDialog(const CString& strImageProperty, CDesignerCanvas* pDesignerCanvas, CWnd* pParent /*=NULL*/)
: CDialog(CImageDialog::IDD, pParent)
, m_pDesignerCanvas(pDesignerCanvas)
, m_strDest(_T("0,0,0,0"))
, m_strSource(_T("0,0,0,0"))
, m_strCorner(_T("0,0,0,0"))
, m_strImageSize(_T("0, 0"))
, m_nFade(255)
, m_bHole(FALSE)
, m_pManager(NULL)
, m_crMask(0xFFFFFFFF)
{
	m_strImageProperty=strImageProperty;
	m_rcDest.SetRectEmpty();
	m_rcSource.SetRectEmpty();
	m_rcCorner.SetRectEmpty();
}

CImageDialog::~CImageDialog()
{
}

void CImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGE_RESOURCE, m_lstImages);
	DDX_Text(pDX, IDC_EDIT_IMAGE_DEST, m_strDest);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SOURCE, m_strSource);
	DDX_Text(pDX, IDC_EDIT_IMAGE_CORNER, m_strCorner);
	DDX_Text(pDX, IDC_EDIT_DIV_OP1, m_strDivOp1);
	DDX_Text(pDX, IDC_EDIT_DIV_OP2, m_strDivOp2);
	DDX_Text(pDX, IDC_EDIT_DIV_RESULT, m_strDivResult);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SIZE, m_strImageSize);
	DDX_Control(pDX, IDC_BUTTON_DIV, m_btnDiv);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_MASK, m_btnMask);
	DDX_Control(pDX, IDC_SLIDER_IMAGE_FADE, m_ctlFade);
	DDX_Text(pDX, IDC_EDIT_IMAGE_FADE, m_nFade);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_HOLE_FALSE, m_bHole);
	DDX_Control(pDX, IDC_STATIC_IMAGE_PREVIEW, m_ImagePreview);
}

BEGIN_MESSAGE_MAP(CImageDialog, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_IMPORT, &CImageDialog::OnBnClickedButtonImageImport)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_CLEAR, &CImageDialog::OnBnClickedButtonImageClear)
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_FADE, &CImageDialog::OnEnKillfocusEditImageFade)
	ON_LBN_DBLCLK(IDC_LIST_IMAGE_RESOURCE, &CImageDialog::OnLbnDblclkListImageResource)
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_DEST, &CImageDialog::OnEnKillfocusEditImageDest)
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_SOURCE, &CImageDialog::OnEnKillfocusEditImageSource)
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_CORNER, &CImageDialog::OnEnKillfocusEditImageCorner)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_MASK, &CImageDialog::OnBnClickedButtonImageMask)
	ON_BN_CLICKED(IDC_BUTTON_DIV, &CImageDialog::OnBnClickedButton_Div)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_HOLE_FALSE, &CImageDialog::OnBnClickedRadioImageHoleFalse)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_HOLE_TRUE, &CImageDialog::OnBnClickedRadioImageHoleTrue)
	ON_BN_CLICKED(IDOK, &CImageDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CImageDialog message handlers

CString CImageDialog::GetImage() const
{
	return m_strImageProperty;
}

BOOL CImageDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ctlFade.SetRange(0,255);
	m_ctlFade.SetPos(255);

	m_pManager = m_pDesignerCanvas->GetPaintManager();
	m_ImagePreview.SetManager(m_pManager);

	m_strImagePathName=m_strImageProperty;
	LPCTSTR pStrImage=m_strImageProperty;
	CStdString sItem;
	CStdString sValue;
	LPTSTR pstr = NULL;
	while( *pStrImage != _T('\0') ) {
		sItem.Empty();
		sValue.Empty();
		while( *pStrImage != _T('\0') && *pStrImage != _T('=') ) {
			LPTSTR pstrTemp = ::CharNext(pStrImage);
			while( pStrImage < pstrTemp) {
				sItem += *pStrImage++;
			}
		}
		if( *pStrImage++ != _T('=') ) break;
		if( *pStrImage++ != _T('\'') ) break;
		while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
			LPTSTR pstrTemp = ::CharNext(pStrImage);
			while( pStrImage < pstrTemp) {
				sValue += *pStrImage++;
			}
		}
		if( *pStrImage++ != _T('\'') ) break;
		if( !sValue.IsEmpty() ) {
			if( sItem == _T("file"))
				m_strImagePathName = sValue;
			else if( sItem == _T("dest") )
				m_strDest = sValue;
			else if( sItem == _T("source") )
				m_strSource = sValue;
			else if( sItem == _T("corner") )
				m_strCorner = sValue;
			else if( sItem == _T("mask") ) {
				if( sValue[0] == _T('#')) m_crMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
				else m_crMask = _tcstoul(sValue.GetData(), &pstr, 16);
				m_crMask &= 0xFFFFFFFF;
			}
			else if( sItem == _T("fade") ) {
				m_nFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
			}
			else if( sItem == _T("hole") ) {
				m_bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
			}
		}
		if( *pStrImage++ != _T(' ') ) break;
	}
	m_ctlFade.SetPos(m_nFade);
	
	UpdateData(FALSE);
	SetImageProperty(m_strImagePathName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImageDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_nFade=m_ctlFade.GetPos();
	UpdateData(FALSE);
	SetImageProperty(m_strImagePathName);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImageDialog::OnBnClickedButtonImageImport()
{
	// TODO: 在此添加控件通知处理程序代码
	SetCurrentDirectory(m_pDesignerCanvas->GetCurrentSkinPath());

	CFileDialog dlg(TRUE,_T(""),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("图片文件(*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png|所有文件(*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		CString strImagePath = dlg.GetPathName();
		if (strImagePath.Find(m_pDesignerCanvas->GetCurrentSkinPath()) != -1)
		{
			CString strImageName = strImagePath.Mid(m_pDesignerCanvas->GetCurrentSkinPath().GetLength());
			SetImageProperty(strImageName);
		}
		else
			AfxMessageBox(_T("只支持和皮肤文件在同一路径下或者皮肤文件路径的子目录下的图片！"));
	}
}

void CImageDialog::OnBnClickedButtonImageClear()
{
	// TODO: 在此添加控件通知处理程序代码
#if 1
	SetImageProperty(m_strImagePathName);
#else
	m_strDest=_T("0,0,0,0");
	m_strSource=_T("0,0,0,0");
	m_strCorner=_T("0,0,0,0");
	m_nFade=255;
	m_bHole=FALSE;
	m_ctlFade.SetPos(m_nFade);

	SetImageProperty(m_strNullImage);
#endif
}

void CImageDialog::SetImageProperty(LPCTSTR pstrPathName)
{
	UpdateData(TRUE);

	m_strImagePathName = pstrPathName;
	m_rcDest = StringToRect(m_strDest);
	m_rcSource = StringToRect(m_strSource);
	m_rcCorner = StringToRect(m_strCorner);
	COLORREF clrMask = (RGB(GetBValue(m_crMask),GetGValue(m_crMask),GetRValue(m_crMask)) | 0xFF000000);

	m_strImageSize = m_ImagePreview.SetImageProperty(pstrPathName,m_rcDest,m_rcSource,m_rcCorner,clrMask,m_nFade,m_bHole);
	UpdateData(FALSE);
}

RECT CImageDialog::StringToRect(CString& str)
{
	RECT rc={0};
	LPTSTR pstr = NULL;

	rc.left = _tcstol(str, &pstr, 10);		  ASSERT(pstr);
	rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
	rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

	return rc;
}

BOOL CImageDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN)
		{
			switch(this->GetFocus()->GetDlgCtrlID())
			{
			case IDC_EDIT_IMAGE_FADE:
				UpdateData(TRUE);
				if(m_nFade>255)
					m_nFade=255;
				else if(m_nFade<0)
					m_nFade=0;
				m_ctlFade.SetPos(m_nFade);
				UpdateData(FALSE);
			case IDC_EDIT_IMAGE_DEST:
			case IDC_EDIT_IMAGE_SOURCE:
			case IDC_EDIT_IMAGE_CORNER:
				SetImageProperty(m_strImagePathName);

				break;
			}

			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CImageDialog::OnEnKillfocusEditImageFade()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_nFade>255)
		m_nFade=255;
	else if(m_nFade<0)
		m_nFade=0;
	m_ctlFade.SetPos(m_nFade);

	UpdateData(FALSE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnLbnDblclkListImageResource()
{
	// TODO: Add your control notification handler code here
	int nIndex=m_lstImages.GetCurSel();
	LPCTSTR pstrImage=(LPCTSTR)m_lstImages.GetItemDataPtr(nIndex);

	SetImageProperty(pstrImage);
}

void CImageDialog::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void CImageDialog::OnEnKillfocusEditImageDest()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnEnKillfocusEditImageSource()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnEnKillfocusEditImageCorner()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnBnClickedButtonImageMask()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CColorDialog dlg;

	if (dlg.DoModal() == IDOK)
	{
		TCHAR szBuf[MAX_PATH] = {0};
		COLORREF crColor = dlg.GetColor();
		m_crMask = crColor | 0xFF000000;
		SetImageProperty(m_strImagePathName);
	}	
}

void CImageDialog::OnBnClickedButton_Div()
{
	UpdateData(TRUE);

	if (!m_strDivOp1.IsEmpty() && !m_strDivOp2.IsEmpty())
	{
		float fOp1 = static_cast<float>(_ttoi(m_strDivOp1.GetBuffer()));
		float fOp2 = static_cast<float>(_ttoi(m_strDivOp2.GetBuffer()));
		float fResult = fOp1 / fOp2;
		TCHAR szBuf[MAX_PATH] = {0};
		_stprintf(szBuf, _T("%.2f"), fResult);
		m_strDivResult = szBuf;

		UpdateData(FALSE);
	}
}

void CImageDialog::OnBnClickedRadioImageHoleFalse()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnBnClickedRadioImageHoleTrue()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}
void CImageDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_strImageProperty=m_ImagePreview.GetImageProperty();

	OnOK();
}