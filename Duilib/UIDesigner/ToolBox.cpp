#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include <math.h>
#include "ToolBox.h"
#include "Resource.h"
#include "ControlType.h"
#include "DesignerCanvas.h"
#include "PropertiesSheet.h"
#include "UIDesignerDlg.h"
#include "DialogBuilderCallback.h"
#include "UIManager.h"

eControlType g_eControlType = eControl_Unkonwn;

void CToolBox::SetCanvas(CDesignerCanvas* pDesignerCanvas)
{
	m_pDesignerCanvas = pDesignerCanvas;
}

void CToolBox::Init()
{}

void CToolBox::OnPrepare()
{}

void CToolBox::Notify(TNotifyUI& msg)
{
	if ( _tcsicmp(msg.sType, _T("windowinit")) == 0 ) OnPrepare();
	else if( _tcsicmp(msg.sType, _T("click")) == 0 )
	{
	}
	else if( _tcsicmp(msg.sType, _T("itemclick")) == 0 )
	{
		ObjectTreeListUI* object_tree = static_cast<ObjectTreeListUI*>(paint_manager_.FindControl(_T("object_list")));
		if (object_tree != NULL)
		{
			if (_tcsicmp(msg.pSender->GetClass(), DuiLib::kListLabelElementUIClassName) == 0)
			{
				Node* node = (Node*)msg.pSender->GetTag();

				POINT pt = {0};
				::GetCursorPos(&pt);
				::ScreenToClient(paint_manager_.GetPaintWindow(), &pt);
				pt.x -= msg.pSender->GetX();
				SIZE sz = object_tree->GetExpanderSizeX(node);
				if (pt.x >= sz.cx && pt.x < sz.cy)
				{
					object_tree->SetChildVisible(node, !node->data().child_visible_);
				}
				else
				{
					m_pDesignerCanvas->paint_manager_.RemoveAllPitchUpon();
					node->data().control_->SetPitchUpon();
					node->data().control_->Invalidate();
					if (m_pDesignerCanvas->m_pPropertiesSheet != NULL)
						m_pDesignerCanvas->m_pPropertiesSheet->AttachControlUI(node->data().control_);
				}
			}
		}
	}
	else if( _tcsicmp(msg.sType , _T("itemactivate")) == 0 )
	{
	}
	else if( _tcsicmp(msg.sType, _T("selectchanged")) == 0 )
	{
		CStdString name = msg.pSender->GetName();
		if (_tcsicmp(name, _T("Pointer")) == 0 )
			g_eControlType = eControl_Unkonwn;
		else if (_tcsicmp(name, _T("Label")) == 0)
			g_eControlType = eControl_Label;
		else if (_tcsicmp(name, _T("Edit")) == 0)
			g_eControlType = eControl_Edit;
		else if (_tcsicmp(name, _T("Button")) == 0)
			g_eControlType = eControl_Button;
		else if (_tcsicmp(name, _T("CheckBox")) == 0)
			g_eControlType = eControl_CheckBox;
		else if (_tcsicmp(name, _T("RadioBox")) == 0)
			g_eControlType = eControl_RadioBox;
		else if (_tcsicmp(name, _T("HSlider")) == 0)
			g_eControlType = eControl_HSlider;
		else if (_tcsicmp(name, _T("VSlider")) == 0)
			g_eControlType = eControl_VSlider;
		else if (_tcsicmp(name, _T("Combox")) == 0)
			g_eControlType = eControl_Combox;
		else if (_tcsicmp(name, _T("HProgress")) == 0)
			g_eControlType = eControl_HProgress;
		else if (_tcsicmp(name, _T("VProgress")) == 0)
			g_eControlType = eControl_VProgress;
		else if (_tcsicmp(name, _T("List")) == 0)
			g_eControlType = eControl_List;
		else if (_tcsicmp(name, _T("HScrollbar")) == 0)
			g_eControlType = eControl_HScrollbar;
		else if (_tcsicmp(name, _T("VScrollbar")) == 0)
			g_eControlType = eControl_VScrollbar;
		else if (_tcsicmp(name, _T("Tree")) == 0)
			g_eControlType = eControl_Tree;
		else if (_tcsicmp(name, _T("TabControl")) == 0)
			g_eControlType = eControl_TabControl;
		else if (_tcsicmp(name, _T("TabLayout")) == 0)
			g_eControlType = eControl_TabLayout;
		else if (_tcsicmp(name, _T("Container")) == 0)
			g_eControlType = eControl_Container;
		else if (_tcsicmp(name, _T("HorizontalLayout")) == 0)
			g_eControlType = eControl_HorizontalLayout;
		else if (_tcsicmp(name, _T("VerticalLayout")) == 0)
			g_eControlType = eControl_VerticalLayout;
		else if (_tcsicmp(name, _T("RichEdit")) == 0)
			g_eControlType = eControl_RichEdit;
		else if (_tcsicmp(name, _T("ActiveX")) == 0)
			g_eControlType = eControl_ActiveX;
		else if (_tcsicmp(name, _T("toolbox_btn")) == 0)
		{
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("Tab")));
			if ( pControl && pControl->GetCurSel() != 0 )
				pControl->SelectItem(0);
		}
		else if (_tcsicmp(name, _T("object_view_btn")) == 0)
		{
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("Tab")));
			if (pControl && pControl->GetCurSel() != 1 )
			{
				pControl->SelectItem(1);
				ContructTree();
			}
		}
	}
}

void CToolBox::ResetTab()
{
	CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(_T("Tab")));
	if ( pControl && pControl->GetCurSel() != 0 )
		pControl->SelectItem(0);
}

void CToolBox::ResetControlSelection()
{
	ResetTab();
	COptionUI* pRadioBox = static_cast<COptionUI*>(paint_manager_.FindControl(_T("Pointer")));
	if (pRadioBox != NULL)
	{
		pRadioBox->Selected(true);
		g_eControlType = eControl_Unkonwn;
		pRadioBox->NeedParentUpdate();
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
}

LRESULT CToolBox::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	paint_manager_.Init(m_hWnd);
	CDialogBuilder builder;

	tString tstrSkin = CPaintManagerUI::GetInstancePath() + lpszSkinPath;
	paint_manager_.SetResourcePath(tstrSkin.c_str());

#if defined(USING_ZIP_SKIN)
	paint_manager_.SetResourceZip(lpszZipSkin);
	CControlUI* pRoot = builder.Create(_T("uidesigner_toolbox.xml"), (UINT)0,  this, &paint_manager_);
#else
	tstrSkin = paint_manager_.GetResourcePath() + _T("uidesigner_toolbox.xml");
	CControlUI* pRoot = builder.Create(tstrSkin.c_str(), (UINT)0,  this, &paint_manager_);
#endif
	ASSERT(pRoot && _T("Failed to parse XML"));
	paint_manager_.AttachDialog(pRoot);
	paint_manager_.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CToolBox::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void CToolBox::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

LRESULT CToolBox::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CToolBox::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CToolBox::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CToolBox::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CToolBox::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = paint_manager_.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}
#endif
LRESULT CToolBox::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = paint_manager_.GetRoundCorner();
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	if( !::IsIconic(*this) && !::IsIconic(GetParent(*this)) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
#if 0
		RECT rc = { rcClient.left, rcClient.top + szRoundCorner.cx, rcClient.right, rcClient.bottom };
		HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
		HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
		::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
		::SetWindowRgn(*this, hRgn1, TRUE);
		::DeleteObject(hRgn1);
		::DeleteObject(hRgn2);
#else
		HRGN hRGN = CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this,hRGN,TRUE);
		::DeleteObject(hRGN);
#endif
	}
#endif
	bHandled = FALSE;
	return 0;
}
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
LRESULT CToolBox::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	DuiLib::CRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}
#endif
LRESULT CToolBox::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) {
		bHandled = TRUE;
		return 0;
	}		
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CToolBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
		case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
		case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( paint_manager_.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

CControlUI* CToolBox::CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager)
{
	if (_tcsicmp(pstrClass, _T("ObjectList")) == 0)
		return new ObjectTreeListUI();

	return NULL;
}

void CToolBox::ContructTree()
{
	ObjectTreeListUI* object_tree = static_cast<ObjectTreeListUI*>(paint_manager_.FindControl(_T("object_list")));
	if ((object_tree != NULL) && (m_pDesignerCanvas != NULL))
	{
		object_tree->RemoveAll();		
		ContructTree(m_pDesignerCanvas->paint_manager_.GetRoot(), NULL);
	}
}

void CToolBox::ContructTree(CControlUI* pControl, Node* parent)
{
	ObjectTreeListUI* object_tree = static_cast<ObjectTreeListUI*>(paint_manager_.FindControl(_T("object_list")));
	if ((object_tree != NULL) && (m_pDesignerCanvas != NULL))
	{
		if ((static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pControl->GetInterface(_T("Container"))) != NULL))
		{
			Node* node = object_tree->AddNode(pControl, parent);
			for (int index = 0; index < static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer")))->GetCount(); ++index)
				ContructTree(static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer")))->GetItemAt(index), node);
		}
		else
		{
			object_tree->AddNode(pControl, parent);
		}
	}
}

inline double CalculateDelay(double state)
{
    return pow(state, 2);
}

void Node::set_parent(Node* parent)
{
	parent_ = parent;
}

Node::Node()
	: parent_(NULL)
{}

Node::Node(NodeData t)
	: data_(t)
	, parent_(NULL)
{}

Node::Node(NodeData t, Node* parent)
: data_ (t)
, parent_ (parent)
{}

Node::~Node() 
{
	for (int i = 0; i < num_children(); ++i)
		delete children_[i]; 
}

NodeData& Node::data()
{
	return data_;
}

int Node::num_children() const
{
	return static_cast<int>(children_.size());
}

Node* Node::child(int i)
{
	return children_[i];
}

Node* Node::parent()
{
	return ( parent_);
}

bool Node::has_children() const
{
	return num_children() > 0;
}

void Node::add_child(Node* child)
{
	child->set_parent(this); 
	children_.push_back(child); 
}

void Node::remove_child(Node* child)
{
	Children::iterator iter = children_.begin();
	for (; iter < children_.end(); ++iter)
	{
		if (*iter == child) 
		{
			children_.erase(iter);
			return;
		}
	}
}

Node* Node::get_last_child()
{
	if (has_children())
	{
		return child(num_children() - 1)->get_last_child();
	}
	return this;
}

ObjectTreeListUI::ObjectTreeListUI()
	: root_node_(NULL)
	, delay_deltaY_(0)
	, delay_number_(0)
	, delay_left_(0)
	, level_expand_image(_T("<a><i tree_expand.png 2 1></a>"))
	, level_collapse_image(_T("<a><i tree_expand.png 2 0></a>"))
	, level_text_start_pos(_T("<x 45>"))
	, text_padding(_T("<x 10>"))
{
	SetItemShowHtml(true);

	root_node_ = new Node;
	root_node_->data().level_ = -1;
	root_node_->data().child_visible_ = true;
	root_node_->data().has_child_ = true;
	root_node_->data().list_elment_ = NULL;
}

ObjectTreeListUI::~ObjectTreeListUI()
{
	if (root_node_)
		delete root_node_;

	root_node_ = NULL;
}

bool ObjectTreeListUI::Add(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), DuiLib::kListLabelElementUIClassName) == 0)
		return false;

	return CListUI::Add(pControl);
}

bool ObjectTreeListUI::AddAt(CControlUI* pControl, int iIndex)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), DuiLib::kListLabelElementUIClassName) == 0)
		return false;

	return CListUI::AddAt(pControl, iIndex);
}

bool ObjectTreeListUI::Remove(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), DuiLib::kListLabelElementUIClassName) == 0)
		return false;

	return CListUI::Remove(pControl);
}

bool ObjectTreeListUI::RemoveAt(int iIndex)
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), DuiLib::kListLabelElementUIClassName) == 0)
		return false;

	return CListUI::RemoveAt(iIndex);
}

void ObjectTreeListUI::RemoveAll()
{
	CListUI::RemoveAll();
	delete root_node_;

	root_node_ = new Node;
	root_node_->data().level_ = -1;
	root_node_->data().child_visible_ = true;
	root_node_->data().has_child_ = true;
	root_node_->data().list_elment_ = NULL;
}

void ObjectTreeListUI::DoEvent(TEventUI& event) 
{
	if (!IsMouseEnabled() && event.Type > DuiLib::UIEVENT__MOUSEBEGIN && event.Type < DuiLib::UIEVENT__MOUSEEND)
	{
		if (m_pParent != NULL)
			m_pParent->DoEvent(event);
		else
			CVerticalLayoutUI::DoEvent(event);
		return;
	}

	if (event.Type == DuiLib::UIEVENT_TIMER && event.wParam == SCROLL_TIMERID)
	{
		if (delay_left_ > 0)
		{
			--delay_left_;
			SIZE sz = GetScrollPos();
			LONG lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
			if ((lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ))
			{
				sz.cy -= lDeltaY;
				SetScrollPos(sz);
				return;
			}
		}
		delay_deltaY_ = 0;
		delay_number_ = 0;
		delay_left_ = 0;
		m_pManager->KillTimer(this, SCROLL_TIMERID);
		return;
	}
	if (event.Type == DuiLib::UIEVENT_SCROLLWHEEL)
	{
		LONG lDeltaY = 0;
		if (delay_number_ > 0)
			lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
		switch (LOWORD(event.wParam))
		{
		case SB_LINEUP:
			if (delay_deltaY_ >= 0)
				delay_deltaY_ = lDeltaY + 8;
			else
				delay_deltaY_ = lDeltaY + 12;
			break;
		case SB_LINEDOWN:
			if (delay_deltaY_ <= 0)
				delay_deltaY_ = lDeltaY - 8;
			else
				delay_deltaY_ = lDeltaY - 12;
			break;
		}
		if
			(delay_deltaY_ > 100) delay_deltaY_ = 100;
		else if
			(delay_deltaY_ < -100) delay_deltaY_ = -100;

		delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
		delay_left_ = delay_number_;
		m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
		return;
	}

	CListUI::DoEvent(event);
}

Node* ObjectTreeListUI::GetRoot()
{
	return root_node_;
}

Node* ObjectTreeListUI::AddNode(CControlUI* pControl, Node* parent)
{
	if (!parent)
		parent = root_node_;

	CListLabelElementUI* pListElement = new CListLabelElementUI;
	Node* node = new Node;

	node->data().level_ = parent->data().level_ + 1;
	node->data().has_child_ = (pControl->GetInterface(DuiLib::kIContainerUIInterfaceName) != NULL) ? true : false;
	node->data().control_ = pControl;

	node->data().child_visible_ = (node->data().level_ == 0);

	CString class_name = pControl->GetClass();
	class_name = class_name.Mid(0, class_name.GetLength() - 2);

	node->data().text_ = class_name;
	bool add_tail = false;
	bool add_head = false;
	if (_tcslen(pControl->GetName()) > 0)
	{
		add_head = true;
		add_tail = true;
		node->data().text_ += _T(" (name:");
		node->data().text_ += pControl->GetName();
		node->data().text_ += _T("  ");
	}

	if (_tcslen(pControl->GetText()) > 0)
	{
		add_tail = true;
		if (!add_head)
			node->data().text_ += _T(" (");

		add_head = true;
		node->data().text_ += _T("text:");
		node->data().text_ += pControl->GetText();
		node->data().text_ += _T("  ");
	}
	if (add_tail)
		node->data().text_ +=_T(")");

	node->data().list_elment_ = pListElement;

	if (!parent->data().child_visible_)
		pListElement->SetVisible(false);

	if (parent != root_node_ && !parent->data().list_elment_->IsVisible())
		pListElement->SetVisible(false);

	std::wstring html_text = text_padding;

	for (int i = 0; i < node->data().level_; ++i)
		html_text += level_text_start_pos;

	if (node->data().has_child_) 
	{
		if (node->data().child_visible_)
			html_text += level_expand_image;
		else
			html_text += level_collapse_image;
	}

	html_text += node->data().text_;
	pListElement->SetText(html_text.c_str());
	pListElement->SetFixedHeight(32);
	pListElement->SetTag((UINT_PTR)node);
	int index = 0;
	if (parent->has_children())
	{
		Node* prev = parent->get_last_child();
		index = prev->data().list_elment_->GetIndex() + 1;
	}
	else 
	{
		if (parent == root_node_)
			index = 0;
		else
			index = parent->data().list_elment_->GetIndex() + 1;
	}
	if (!CListUI::AddAt(pListElement, index))
	{
		delete pListElement;
		delete node;
		node = NULL;
	}

	parent->add_child(node);
	return node;
}

void ObjectTreeListUI::RemoveNode(Node* node)
{
	if (!node || node == root_node_) return;

	for (int i = 0; i < node->num_children(); ++i)
	{
		Node* child = node->child(i);
		RemoveNode(child);
	}

	CListUI::Remove(node->data().list_elment_);
	delete node->data().list_elment_;
	node->parent()->remove_child(node);
	delete node;
}

void ObjectTreeListUI::SetChildVisible(Node* node, bool visible)
{
	if (!node || node == root_node_)
		return;

	if (node->data().child_visible_ == visible)
		return;

	node->data().child_visible_ = visible;

	std::wstring html_text;
	html_text += text_padding;
	for (int i = 0; i < node->data().level_; ++i)
		html_text += level_text_start_pos;

	if (node->data().has_child_)
	{
		if (node->data().child_visible_)
			html_text += level_expand_image;
		else
			html_text += level_collapse_image;
	}

	html_text += node->data().text_;
	node->data().list_elment_->SetText(html_text.c_str());

	if (!node->data().list_elment_->IsVisible())
		return;

	if (!node->has_children())
		return;

	Node* begin = node->child(0);
	Node* end = node->get_last_child();
	for (int i = begin->data().list_elment_->GetIndex(); i <= end->data().list_elment_->GetIndex(); ++i)
	{
		CControlUI* control = GetItemAt(i);
		if (_tcsicmp(control->GetClass(), DuiLib::kListLabelElementUIClassName) == 0)
		{
			if (visible) 
			{
				Node* local_parent = ((Node*)control->GetTag())->parent();
				if (local_parent->data().child_visible_ && local_parent->data().list_elment_->IsVisible())
					control->SetVisible(true);
			}
			else
				control->SetVisible(false);
		}
	}
}

SIZE ObjectTreeListUI::GetExpanderSizeX(Node* node) const
{
	if (!node || node == root_node_) return CSize();

	SIZE szExpander = {0};
	szExpander.cx = 6 + 45 * node->data().level_ - 4;
	szExpander.cy = szExpander.cx + 24 + 8;
	return szExpander;
}