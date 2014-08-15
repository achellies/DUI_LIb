// UIDesigner
// Version 1.0.0
// Released: 24th January, 2011 by:
//
//      achellies
//      email: achellies@163.com
//      url: http://directui.googlecode.com/
//
//
// Copyright (c) 2010-2011  achellies
//
// Permission is hereby granted, free of charge, to
// any person obtaining a copy of this software and
// associated documentation files (the "Software"),
// to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice
// shall be included in all copies or substantial portions
// of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
//
// Acknowledgements :
//
//
// Beware of bugs.
//
//
//
////////////////////////////////////////////////////////
#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>

#include "tinyxml.h"

#ifndef Designer_Canvas_Wnd_H_
#define Designer_Canvas_Wnd_H_

using DuiLib::CControlUI;
using DuiLib::INotifyUI;

enum eAdjustment
{
	eAdjustmentUnknown,

	// 左对齐
	eAdjustmentAlign2Left,

	// 右对齐
	eAdjustmentAlign2Right,

	// 顶端对齐
	eAdjustmentAlign2Top,

	// 底端对齐
	eAdjustmentAlign2Bottom,

	// 左右居中
	eAdjustmentAlign2VCenter,

	// 上下居中
	eAdjustmentAlign2HCenter,

	// 横向分布
	eAdjustmentTransverseDistribution,

	// 纵向分布
	eAdjustmentLongitudinalDistribution,

	// 单一控件水平居中
	eAdjustmentHorizontalCenter,

	// 单一控件垂直居中
	eAdjustmentVerticalLCenter,

	// 使控件宽度相等
	eAdjustmentSameWidth,

	// 使控件高度相等
	eAdjustmentSameHeight,

	// 使控件大小相等
	eAdjustmentSameSize,

	// 控件属性窗口
	eAdjustmentProperties
};

class CToolBox;
class CToolbar;
class CPropertiesSheet;

struct _WindowFormInfo
{
	_WindowFormInfo()
		: bOpacity(true)
		, dwTransparency(0)
	{
		szCanvasSize.cx = 600;
		szCanvasSize.cy = 800;
	}

	bool bOpacity;
	DWORD dwTransparency;
	SIZE szCanvasSize;
};

class CDesignerCanvas : public CWindowWnd, public INotifyUI
{
friend class CToolBox;
public:
	CDesignerCanvas(CToolBox* pToolBoxWnd);
	~CDesignerCanvas();
	LPCTSTR GetWindowClassName() const { return _T("CDesignerCanvas"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND hWnd);

	void Init();

	void OnPrepare();

	void Notify(TNotifyUI& msg);

	LPCTSTR GetItemText(CControlUI* pControl, int iIndex, int iSubItem);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#endif

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool PreExit();

	void SetToolbarPointer(CToolbar* pToolbar);
	void Layout();

	_WindowFormInfo GetFormInfo();
	void SetFormInfo(_WindowFormInfo info);
	void SetFormSize(SIZE szSize);

	void DeleteControl();
	void CopyControl();
	void CutControl();
	void PasteControl();

	void Undo();
	void Redo();

	void Refresh();

	void AdjustControls(eAdjustment adjust);

	void ShowPropertiesSheet();

	CPropertiesSheet* GetPropertiesSheet();

	void OpenSkinFile();
	void NewSkinFile();
	void SaveSkinFile();

	CPaintManagerUI* GetPaintManager();

	CString GetCurrentSkinPath();

private:
	void Save_Properties(CControlUI* pControlUI, TiXmlElement* pParentNode);
	void Save_SingleProperties(CControlUI* pControlUI, TiXmlElement* pParentNode);
	void Save_ContainerProperties(CControlUI* pControlUI, TiXmlElement* pParentNode);
	void Save_CControlUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CLabelUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CButtonUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_COptionUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CProgressUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CSliderUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CEditUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CScrollbarUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CListUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CComboUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CListHeaderItemUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CListElementUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);
	void Save_CContainerUI_Properties(CControlUI* pControlUI, TiXmlElement* pNode);

	void DeleteControl(CControlUI *&pCopiedOrCutControl, bool bDeleteLeafControl);
	void CopyControl(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);

	void Copy_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_SingleProperties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_ContainerProperties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CControlUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CLabelUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CButtonUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_COptionUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CProgressUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CSliderUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CEditUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CScrollbarUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CListUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CComboUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CListHeaderItemUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CListElementUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);
	void Copy_CContainerUI_Properties(CControlUI* pBasedControlUI, CControlUI *&pCopiedOrCutControl);

private:
	bool				m_bMouseCaptured;
	bool				m_bSkinFileChanged;
	CPoint				m_point;

	_WindowFormInfo		m_formInfo;
	CToolBox*			m_pToolBox;
	CToolbar*			m_pToolbar;
	CPropertiesSheet*	m_pPropertiesSheet;
	CControlUI*			m_pCanvasContainer;
	CControlUI*			m_pCopyControl;
	CControlUI*			m_pCutControl;
	CPaintManagerUI		paint_manager_;
	tString				m_tstrSkinFile;
	tString				m_tstrSkinPath;
	tString				m_tstrDefaultFeaturesSkinPath;
};

#endif // #ifndef Designer_Canvas_Wnd_H_