// ImagePreview.cpp : 实现文件
//

#include "stdafx.h"
#include "UIDesigner.h"
#include "ImagePreview.h"
#include <afxwin.h>

// CImagePreview

IMPLEMENT_DYNAMIC(CImagePreview, CStatic)

CImagePreview::CImagePreview()
{
	m_strImage.Empty();
	m_strProperty.Empty();
	m_pManager=NULL;
	m_rcImage.SetRectEmpty();
}

CImagePreview::~CImagePreview()
{
}


BEGIN_MESSAGE_MAP(CImagePreview, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CImagePreview 消息处理程序

bool CImagePreview::DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, 
									LPCTSTR pStrImage, LPCTSTR pStrModify)
{
	// 1、aaa.jpg
	// 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
	// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

	CStdString sImageName = pStrImage;
	CStdString sImageResType;
	RECT rcItem = rc;
	RECT rcBmpPart = {0};
	RECT rcCorner = {0};
	DWORD dwMask = 0;
	BYTE bFade = 0xFF;
	bool bHole = false;
	bool bTiledX = false;
	bool bTiledY = false;

	CStdString sItem;
	CStdString sValue;
	LPTSTR pstr = NULL;

	for( int i = 0; i < 2; ++i ) {
		if( i == 1) {
			pStrImage = pStrModify;
		}
		if( !pStrImage ) continue;

		while( *pStrImage != _T('\0') ) {
			sItem.Empty();
			sValue.Empty();
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sItem += *pStrImage++;
				}
			}
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('=') ) break;
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('\'') ) break;
			while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sValue += *pStrImage++;
				}
			}
			if( *pStrImage++ != _T('\'') ) break;
			if( !sValue.IsEmpty() ) {
				if( sItem == _T("file") || sItem == _T("res") ) {
					sImageName = sValue;
				}
				else if( sItem == _T("restype") ) {
					sImageResType = sValue;
				}
				else if( sItem == _T("dest") ) {
					//rcItem.left = rc.left + _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
					//rcItem.top = rc.top + _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					//rcItem.right = rc.left + _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					//rcItem.bottom = rc.top + _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					//if (rcItem.right > rc.right) rcItem.right = rc.right;
					//rcItem.bottom = rc.top + _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					//if (rcItem.bottom > rc.bottom) rcItem.bottom = rc.bottom;
				}
				else if( sItem == _T("source") ) {
					rcBmpPart.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
					rcBmpPart.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcBmpPart.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if( sItem == _T("corner") ) {
					rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
					rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if( sItem == _T("mask") ) {
					if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
					else dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
				}
				else if( sItem == _T("fade") ) {
					bFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
				}
				else if( sItem == _T("hole") ) {
					bHole = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
				}
				else if( sItem == _T("xtiled") ) {
					bTiledX = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
				}
				else if( sItem == _T("ytiled") ) {
					bTiledY = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
				}
			}
			if( *pStrImage++ != _T(' ') ) break;
		}
	}


	STRINGorID type(sImageName);
	TImageInfo* pImage = CRenderEngine::LoadImage(type, pManager, type, dwMask);
	if (pImage == NULL)
		return false;  

	if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
		rcBmpPart.right = pImage->nX;
		rcBmpPart.bottom = pImage->nY;
	}
	if (rcBmpPart.right > pImage->nX) rcBmpPart.right = pImage->nX;
	if (rcBmpPart.bottom > pImage->nY) rcBmpPart.bottom = pImage->nY;

	RECT rcTemp;
	if( !::IntersectRect(&rcTemp, &rcItem, &rc) )
	{
		::DeleteObject(pImage->hBitmap);
		delete pImage;
		return true;
	}

	if( !::IntersectRect(&rcTemp, &rcItem, &rcPaint) )
	{
		::DeleteObject(pImage->hBitmap);
		delete pImage;
		return true;
	}
	CRenderEngine::DrawImage(hDC, pImage->hBitmap, rcItem, rcPaint, rcBmpPart, rcCorner, pImage->alphaChannel, bFade, bHole, bTiledX, bTiledY);

	::DeleteObject(pImage->hBitmap);
	delete pImage;
	return true;
}

void CImagePreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	CDC memDC;
	memDC.Attach(dc.GetSafeHdc());

	CRect rcClient;
	this->GetClientRect(&rcClient);
	CBrush* pOldBrush= memDC.SelectObject((CBrush*)memDC.SelectStockObject(BLACK_BRUSH));
	memDC.Rectangle(&rcClient);
	pOldBrush=memDC.SelectObject((CBrush*)memDC.SelectStockObject(NULL_BRUSH));
	CRect rcBorder=m_rcImage;
	rcBorder.DeflateRect(-1,-1);
	memDC.Rectangle(rcBorder);
	memDC.SelectObject(pOldBrush);

	DrawImageString(memDC.GetSafeHdc(),m_pManager,m_rcImage,rcClient,m_strProperty,NULL);
}

CString CImagePreview::SetImageProperty(LPCTSTR pstrImage,CRect& rcDest,CRect& rcSource,CRect& rcCorner,COLORREF clrMask,int nFade,BOOL bHole)
{
	CString strImageSize = _T("0, 0");
	m_strImage=pstrImage;
	m_strProperty.Empty();
	m_rcImage.SetRectEmpty();
	if(!m_strImage.IsEmpty())
	{
		BOOL bFlag=FALSE;
		CString str;
		if(!rcDest.IsRectNull())
		{
			str.Format(_T(" dest='%d,%d,%d,%d'"),rcDest.left,rcDest.top,rcDest.right,rcDest.bottom);
			m_strProperty+=str;
			bFlag=TRUE;
		}
		if(!rcSource.IsRectNull())
		{
			str.Format(_T(" source='%d,%d,%d,%d'"),rcSource.left,rcSource.top,rcSource.right,rcSource.bottom);
			m_strProperty+=str;
			bFlag=TRUE;
		}
		if(!rcCorner.IsRectNull())
		{
			str.Format(_T(" corner='%d,%d,%d,%d'"),rcCorner.left,rcCorner.top,rcCorner.right,rcCorner.bottom);
			m_strProperty+=str;
			bFlag=TRUE;
		}
		if(clrMask!=0xFF000000)
		{
			str.Format(_T(" mask='0x%08x'"),clrMask);
			m_strProperty+=str;
			bFlag=TRUE;
		}
		if(nFade!=255)
		{
			str.Format(_T(" fade='%d'"),nFade);
			m_strProperty+=str;
			bFlag=TRUE;
		}
		if(bHole==TRUE)
		{
			str=_T(" hole='true'");
			m_strProperty+=str;
			bFlag=TRUE;
		}
		if(bFlag)
		{
			str.Format(_T("file='%s'"),pstrImage);
			m_strProperty=str+m_strProperty;
		}
		else
			m_strProperty=pstrImage;
	}
	else
		m_strProperty.Empty();


	STRINGorID type(pstrImage);
	TImageInfo* pImage = CRenderEngine::LoadImage(type, m_pManager, type, clrMask);
	if(pImage)
	{
		CRect rcClient;
		this->GetClientRect(&rcClient);
		int nX=(rcClient.Width()-pImage->nX)/2;
		int nY=(rcClient.Height()-pImage->nY)/2;

		m_rcImage.SetRect(nX,nY,nX+pImage->nX,nY+pImage->nY);

		TCHAR szBuf[MAX_PATH] = {0};
		_stprintf(szBuf, _T("%d, %d"), pImage->nX, pImage->nY);
		strImageSize = szBuf;

		::DeleteObject(pImage->hBitmap);
		delete pImage;
	}
	else
		m_strProperty.Empty();

	this->Invalidate(FALSE);

	return strImageSize;
}

void CImagePreview::SetManager(CPaintManagerUI* pManager)
{
	m_pManager=pManager;
}