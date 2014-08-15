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

#ifndef Properties_Sheet_H_
#define Properties_Sheet_H_

using DuiLib::CControlUI;
using DuiLib::INotifyUI;

class CDesignerCanvas;

class CPropertiesSheet : public CWindowWnd, public INotifyUI
{
public:
	CPropertiesSheet(CControlUI* pHostControlUI, CDesignerCanvas* pDesignerCanvas);
	LPCTSTR GetWindowClassName() const { return _T("CPropertiesSheet"); };
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

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void DetachControlUI();
	void AttachControlUI(CControlUI* pHostControlUI);

protected:

	void UpdateLayout();

	void ApplySettings();

	void SetEditUIReadOnly(CControlUI* pControlUI, bool bReadOnly);

	void ApplyCommonProperties();
	void ApplySpecialProperties();

	void LoadCommonProperties();
	void LoadSpecialProperties();

	void LoadSpecialPropertiesTab(CTabLayoutUI* pTabLayoutUI);

private:
	void LoadLabelUIProperties();
	void ApplyLabelUIProperties();

	void LoadButtonUIProperties();
	void ApplyButtonUIProperties();

	void LoadOptionUIProperties();
	void ApplyOptionUIProperties();

	void LoadProgressUIProperties();
	void ApplyProgressUIProperties();

	void LoadSliderUIProperties();
	void ApplySliderUIProperties();

	void LoadEditUIProperties();
	void ApplyEditUIProperties();

	void LoadScrollbarUIProperties();
	void ApplyScrollbarUIProperties();

	void LoadComboUIProperties();
	void ApplyComboUIProperties();

	void LoadListUIProperties();
	void ApplyListUIProperties();

	void ChangeColor(CControlUI* pSender);
	void ChangeImage(CControlUI* pSender);

private:
	CControlUI*			m_pHostControlUI;
	CControlUI*			m_pSpecialPropertiesTab;
	CDesignerCanvas*	m_pDesignerCanvas;
	CPaintManagerUI		paint_manager_;
};

#endif // #ifndef Properties_Sheet_H_