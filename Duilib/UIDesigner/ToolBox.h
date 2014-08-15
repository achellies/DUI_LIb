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

#ifndef ToolBox_H_
#define ToolBox_H_

using DuiLib::CControlUI;
using DuiLib::INotifyUI;
using DuiLib::IDialogBuilderCallback;

class CDesignerCanvas;
class Node;

class CToolBox : public CWindowWnd, public INotifyUI, public IDialogBuilderCallback
{
friend class CDesignerCanvas;
public:
	CToolBox(){}
	LPCTSTR GetWindowClassName() const { return _T("CToolBox"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND hWnd);

	void Init();

	void OnPrepare();

	void SetCanvas(CDesignerCanvas* pDesignerCanvas);

	void Notify(TNotifyUI& msg);

	virtual CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager);

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

	void ResetControlSelection();

	void ResetTab();

	void ContructTree();

	void ContructTree(CControlUI* pControl, Node* parent);

private:
	CPaintManagerUI paint_manager_;
	CDesignerCanvas*	m_pDesignerCanvas;
};


struct NodeData
{
	int level_;
	bool child_visible_;
	bool has_child_;
	std::wstring text_;
	CControlUI* control_;
	CListLabelElementUI* list_elment_;
};

class Node
{
public:
	Node();
	explicit Node(NodeData t);
	Node(NodeData t, Node* parent);
	~Node();
	NodeData& data();
	int num_children() const;
	Node* child(int i);
	Node* parent();
	bool has_children() const;
	void add_child(Node* child);
	void remove_child(Node* child);
	Node* get_last_child();

private:
	void set_parent(Node* parent);

private:
	typedef std::vector <Node*>	Children;

	Children	children_;
	Node*		parent_;
	NodeData    data_;
};

class ObjectTreeListUI : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	ObjectTreeListUI();

	~ObjectTreeListUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(CControlUI* pControl, Node* parent = NULL);

	void RemoveNode(Node* node);

	void SetChildVisible(Node* node, bool visible);

	SIZE GetExpanderSizeX(Node* node) const;

private:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	std::wstring text_padding;
	std::wstring level_text_start_pos;
	std::wstring level_expand_image;
	std::wstring level_collapse_image;
};

#endif // #ifndef ToolBox_H_