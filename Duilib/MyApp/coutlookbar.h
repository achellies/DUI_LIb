#ifndef _OUTLOOKBAR_H_
#define _OUTLOOKBAR_H_


#include <vector>
enum NodeType
{
	NoType	= 0,
	Tree_GameType	= 1,
	Tree_GameName,
	Tree_GameArea,
	Tree_GameRoom,
	Tree_GameLink,
	OutLookBar_Root,
	OutLookBar_Node
};

class COutlookbarNode;


struct NodeData		//节点
{
	NodeType		nodetype;			// 1 = tree_gaametype,2 = tree_game_name,3 = tree_game_area,4 = tree_room,5= tree_game_link ,6 = outlookbar_root ,7 = outlookbar_node 		
	int				_level;				//深度
	CStdString		_text;				//字符串
	COutlookbarNode* _pListElement;	//子节点指针
	int				netspeed;			//网络状况	快,中,慢
	bool			bIsInstall;			//如果nodetype是表示游戏,哪么此值表示该游戏是否安装
	bool			_child_visible;		//子节点是否可见

};	

class Node									//每一个节点的操作
{
public:
	typedef std::vector <Node*>	Children;	
	Children	_children;					//子节点数组
	Node*		_parent;					//父节点
	NodeData    _data;						//节点携带的数据

private:
	void set_parent(Node* parent) { _parent = parent; }	//设置父节点

public:
	Node() : _parent (NULL) {}									//构造函数
	explicit Node(NodeData t) : _data (t), _parent (NULL) {}	//构造函数
	Node(NodeData t, Node* parent)	: _data (t), _parent (parent) {}
	~Node() 
	{
		for (int i = 0; i < num_children(); i++)
			delete _children[i]; 
	}
	NodeData& data() { return _data; }						//获得数据
	int num_children() const { return _children.size(); }	//子节点的数量
	Node* child(int i)	{ return _children[i]; }			//获得指定的子节点
	Node* parent() { return ( _parent);	}					//设置父节点
	bool has_children() const {	return num_children() > 0; }//判断是否有子节点
	void add_child(Node* child)								//添加子节点
	{
		child->set_parent(this); 
		_children.push_back(child); 
	}

	void remove_child(Node* child)							//移除子节点
	{
		Children::iterator iter = _children.begin();
		for( ; iter < _children.end(); ++iter )
		{
			if( *iter == child ) 
			{
				_children.erase(iter);
				return;
			}
		}
	}

	Node* get_last_child()									//获取最底层的最后一个节点
	{
		if( has_children() )
		{
			return child(num_children() - 1)->get_last_child();
		}
		else 
			return this;
	}
};

class COutlookbarNode:public CListLabelElementUI
{
public:
	CStdString	m_hotImageStr;
	CStdString	m_normalImageStr;
	CStdString	m_pushImageStr;
	RECT		m_rcImage;
	RECT		m_rcText;
	SIZE		m_szImage;
public:
	COutlookbarNode()
	{
		m_hotImageStr.Empty();
		m_normalImageStr.Empty();
		m_pushImageStr.Empty();
		memset(&m_rcImage,0,sizeof(RECT));
		memset(&m_rcText,0,sizeof(RECT));
		m_szImage.cx	= m_szImage.cy	= 0;
	}
	
	LPCTSTR GetClass() const
	{
		return _T("OutlookbarNode");
	}

	LPVOID GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("OutlookbarNode")) == 0 )
			return static_cast<COutlookbarNode*>(this);

		return CListElementUI::GetInterface(pstrName);
	}

	inline SIZE	GetImageSize() const 
	{
		return m_szImage;
	}
	bool  SetImageSize(SIZE sz)
	{
		if (sz.cx == 0 || sz.cy == 0)
		{
			return false;
		}
		else
		{
			m_szImage			= sz;
			return true;	
		}
	}

	inline CStdString	GetHotImage() const 
	{
		return m_hotImageStr;
	}
	bool  SetHotImage(CStdString str)
	{
		if (str.IsEmpty())
		{
			return false;
		}
		else
		{
			m_hotImageStr = str;
			return true;
		}
	}
	inline CStdString	GetNorImage() const 
	{
		return m_normalImageStr;
	}
	bool  SetNorImage(CStdString str)
	{
		if (str.IsEmpty())
		{
			return false;
		}
		else
		{
			m_normalImageStr = str;
			return true;
		}
	}
	inline CStdString	GetPushImage() const 
	{
		return m_pushImageStr;
	}
	bool  SetPushImage(CStdString str)
	{
		if (str.IsEmpty())
		{
			return false;
		}
		else
		{
			m_pushImageStr = str;
			return true;
		}
	}

	void Event(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
		{
			if( m_pOwner != NULL ) 
				m_pOwner->DoEvent(event);
			else 
				CListLabelElementUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN )
		{
			if( IsEnabled() ) 
			{
				m_pManager->SendNotify(this, _T("itemclick"));
				Select();
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) 
			{
				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
				if (IsRectEmpty(&m_rcImage) || IsRectEmpty(&m_rcText))
				{
					m_uButtonState |= UISTATE_HOT;
					Invalidate();
				}
				else 
				{
					if (PtInRect(&m_rcImage,pt) || PtInRect(&m_rcText,pt))
					{
						m_uButtonState |= UISTATE_HOT;
						Invalidate();
					}	
				}
				
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) 
			{
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CListLabelElementUI::DoEvent(event);
	}

	SIZE EstimateSize(SIZE szAvailable)
	{
		if( m_pOwner == NULL ) 
			return CSize(0, 0);

		Node* pNode	= reinterpret_cast<Node*>(GetTag());
		if (pNode == NULL)
		{
			return CSize(0, 0);
		}

		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		SIZE cXY = m_cxyFixed;
		if( cXY.cy == 0 && m_pManager != NULL ) 
		{
			cXY.cy = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
			cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		if( cXY.cx == 0 && m_pManager != NULL ) 
		{

			RECT rcText = { 0, 0, 9999, cXY.cy };
			if( pInfo->bShowHtml ) 
			{
				int nLinks = 0;
				CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), this, m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
			}
			else 
			{
				CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
			}
			cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;        
		}

		return cXY;
	}

	void DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) 
			return;
		Node* pNode	= reinterpret_cast<Node*>(GetTag());
		if (pNode == NULL)
		{
			return ;
		}

		if (pNode->_data.nodetype != OutLookBar_Node)
		{
			DrawItemBk(hDC, m_rcItem);
		}
		
		DrawItemText(hDC, m_rcItem);
	}
	
	void CalcClientRect(RECT rcItem)
	{
		int width		= rcItem.right - rcItem.left;
		m_rcImage.left	= ( width- m_szImage.cx)/2 + rcItem.left;
		m_rcImage.top	= rcItem.top +1;
		m_rcImage.right	= m_rcImage.left + m_szImage.cx;
		m_rcImage.bottom= m_rcImage.top + m_szImage.cy;
	}

	void DrawItemText(HDC hDC, const RECT& rcItem)
	{
		if( m_sText.IsEmpty() ) 
			return;

		if( m_pOwner == NULL ) 
			return;
		Node* pNode	= reinterpret_cast<Node*>(GetTag());
		if (pNode == NULL)
		{
			return ;
		}
		int nLinks = 0;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;
		CStdString	imageStr	= m_normalImageStr;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) 
		{
			iTextColor = pInfo->dwHotTextColor;
			imageStr	= m_hotImageStr;
		}
		if( IsSelected() ) 
		{
			iTextColor = pInfo->dwSelectedTextColor;
			imageStr	= m_pushImageStr;
		}
		if( !IsEnabled() ) 
		{
			iTextColor = pInfo->dwDisabledTextColor;
		}

		//画outlookbarNode
		if(pNode->_data.nodetype == OutLookBar_Node)
		{
			SIZE	szText;
			
			int width		= rcItem.right - rcItem.left;
			m_rcImage.left	= ( width- m_szImage.cx)/2 + rcItem.left;
			m_rcImage.top	= rcItem.top +1;
			m_rcImage.right	= m_rcImage.left + m_szImage.cx;
			m_rcImage.bottom= m_rcImage.top + m_szImage.cy;
			
			if(GetTextExtentPoint(hDC,m_sText,_tcslen(m_sText),&szText) == FALSE)
			{
				szText.cx	= m_rcImage.right - m_rcImage.left;
			}
			m_rcText		= m_rcImage;
			m_rcText.top	= m_rcImage.bottom +1;
			m_rcText.bottom	= rcItem.bottom -1;
			m_rcText.left	= (width - szText.cx)/2 + rcItem.left;
			m_rcText.right	= m_rcText.left + szText.cx;
			CRenderEngine::DrawImageString(hDC, this, m_pManager,m_rcImage,m_rcImage,imageStr);
			CRenderEngine::DrawHtmlText(hDC, this, m_pManager,m_rcText,m_sText, iTextColor,NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
		}
		//画树
		else
		{
			SIZE	szText	= {0};
			RECT	rcText	= rcItem;
			if (pNode->_data._level == 0)		//根节点居中显示
			{
				ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
				::SetBkMode(hDC, TRANSPARENT);
				::SetTextColor(hDC, RGB(GetBValue(iTextColor), GetGValue(iTextColor), GetRValue(iTextColor)));
				HFONT hOldFont = (HFONT)::SelectObject(hDC, m_pManager->GetFont(pInfo->nFont));
				GetTextExtentPoint(hDC,m_sText,_tcslen(m_sText),&szText);
				rcText.top		= (rcItem.bottom -rcItem.top - szText.cy)/2 + rcItem.top;
				rcText.bottom	= rcText.top + szText.cy;
				::DrawText(hDC, m_sText, -1, &rcText, DT_CENTER);
				::SelectObject(hDC, hOldFont);
			}
			else if( pInfo->bShowHtml)
			{
				if (pNode->_data.nodetype != Tree_GameType)
				{
					CRenderEngine::DrawHtmlText(hDC, this, m_pManager, rcText, m_sText, iTextColor,NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
				}
				else
				{
					CRenderEngine::DrawHtmlText(hDC, this, m_pManager, rcText, m_sText, iTextColor,NULL, NULL, nLinks, DT_VCENTER | pInfo->uTextStyle);
				}
				
			}
			else
			{
				if (pNode->_data.nodetype != Tree_GameType)
				{
					CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor,pInfo->nFont, DT_VCENTER | pInfo->uTextStyle);
				}
				else
				{
					CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor,pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
				}
			}
		}
	}
};

class COutlookbar : public CListUI
{
public:
	
	COutlookbar() : _root(NULL),m_pLastVisible(NULL)								//构造函数
	{
		SetItemShowHtml(true);
		_root = new Node;
		_root->data()._level = -1;
		_root->data()._child_visible = true;
		_root->data()._pListElement = NULL;
		_root->data().bIsInstall	= false;
		_root->data().netspeed		= 0;
		_root->data().nodetype		= NoType;
	}

	~COutlookbar() { if(_root) delete _root; }				//析构函数

	bool Add(CControlUI* pControl)							//添加节点
	{
		if( !pControl ) 
			return false;

		if( _tcsicmp(pControl->GetClass(), _T("OutlookbarNode")) == 0 ) 
			return false;

		return CListUI::Add(pControl);						
	}

	bool AddAt(CControlUI* pControl, int iIndex)			//添加节点到指定的位置
	{
		if( !pControl ) 
			return false;

		if( _tcsicmp(pControl->GetClass(), _T("OutlookbarNode")) == 0 ) 
			return false;

		return CListUI::AddAt(pControl, iIndex);
	}

	bool Remove(CControlUI* pControl)						//移除节点
	{
		if( !pControl ) 
			return false;

		if( _tcsicmp(pControl->GetClass(), _T("OutlookbarNode")) == 0 ) 
			return false;

		return CListUI::Remove(pControl);
	}

	bool RemoveAt(int iIndex)								//移除指定的节点
	{
		CControlUI* pControl = GetItemAt(iIndex);

		if( !pControl ) 
			return false;

		if( _tcsicmp(pControl->GetClass(), _T("OutlookbarNode")) == 0 ) 
			return false;

		return CListUI::RemoveAt(iIndex);
	}

	void RemoveAll()									//移除所有节点
	{
		CListUI::RemoveAll();
		delete _root;
		_root = new Node;
		_root->data()._level = -1;
		_root->data()._child_visible = true;
		_root->data()._pListElement = NULL;
	}

	Node* GetRoot() { return _root; }					//获得父节点

	Node* AddNode(LPCTSTR text, Node* parent = NULL,NodeType nodetype = NoType,bool bIsInstall = false,int netspeed = 0)	//添加节点
	{
		if( parent == NULL ) 
			parent = _root;


		COutlookbarNode* pListElement = new COutlookbarNode;

		Node* node = new Node;

		node->data()._level = parent->data()._level + 1;
		node->data()._child_visible = false;
		node->data()._text = text;
		node->data()._pListElement = pListElement;
		node->data().nodetype	= nodetype;
		node->data().bIsInstall = bIsInstall;
		node->data().netspeed	= netspeed;
		
		if( !parent->data()._child_visible )
		{
			pListElement->SetVisible(false);
		}
		if (OutLookBar_Node == nodetype)
		{
			pListElement->SetMinHeight(80);
			pListElement->SetNorImage(_T("file='items.bmp' source='0,0,56,56' mask='#FF00FFFF'"));
			pListElement->SetHotImage(_T("file='items.bmp' source='56,0,112,56' mask='#FF00FFFF'"));
			pListElement->SetPushImage(_T("file='items.bmp' source='112,0,168,56' mask='#FF00FFFF'"));
			pListElement->SetImageSize(CSize(56,56));
		}
		if( parent != _root && !parent->data()._pListElement->IsVisible() )
		{
			pListElement->SetVisible(false);
		}

		CStdString html_text;
		if (nodetype != OutLookBar_Node && nodetype != OutLookBar_Root)
		{
			if (node->_data._level != 0)
			{
				html_text += _T("<x 6>");
			}
			for( int i = 1; i < node->data()._level; ++i )
			{
				html_text += _T("<x 24>");
			}
			if( node->data()._level < 3 && node->data()._level != 0) 
			{
				if( node->data()._child_visible ) 
					html_text += _T("<a><i tree_expand.png 1></a>");
				else 
					html_text += _T("<a><i tree_expand.png 0></a>");
			}
		}
		html_text += node->data()._text;

		pListElement->SetText(html_text);
		pListElement->SetTag((UINT_PTR)node);
		if( node->data()._level == 0 ) 
		{
			pListElement->SetBkImage(_T("file='tree_top.png' fade='100'"));
		}

		int index = 0;
		if( parent->has_children() )
		{
			Node* prev = parent->get_last_child();
			index = prev->data()._pListElement->GetIndex() + 1;
		}
		else 
		{
			if( parent == _root ) index = 0;
			else index = parent->data()._pListElement->GetIndex() + 1;
		}
		if( !CListUI::AddAt(pListElement, index) ) 
		{
			delete pListElement;
			delete node;
			node = NULL;
		}

		parent->add_child(node);
		return node;
	}

	void RemoveNode(Node* node)
	{
		if( !node || node == _root ) return;

		for( int i = 0; i < node->num_children(); ++i )
		{
			Node* child = node->child(i);
			RemoveNode(child);
		}

		CListUI::Remove(node->data()._pListElement);
		delete node->data()._pListElement;
		node->parent()->remove_child(node);
		delete node;
	}

	
	void SetChildVisible(Node* node, bool visible)
	{
		if( !node || node == _root ) return;

		if( node->data()._child_visible == visible ) 
			return;

		node->data()._child_visible = visible;

		CStdString html_text;
		if(node->data().nodetype != OutLookBar_Node && node->data().nodetype != OutLookBar_Root)
		{
			if( node->data()._level != 0 ) 
			{
				html_text += _T("<x 6>");
			}
			for( int i = 1; i < node->data()._level; ++i )
			{
				html_text += _T("<x 24>");
			}

			if( node->data()._level < 3 && node->data()._level != 0) 
			{
				if( node->data()._child_visible ) 
					html_text += _T("<a><i tree_expand.png 1></a>");
				else 
					html_text += _T("<a><i tree_expand.png 0></a>");
			}
		}

		html_text += node->data()._text;
		node->data()._pListElement->SetText(html_text);

		if( !node->data()._pListElement->IsVisible() ) 
			return;
		if( !node->has_children() ) 
			return;

		Node* begin = node->child(0);
		Node* end = node->get_last_child();
		for( int i = begin->data()._pListElement->GetIndex(); i <= end->data()._pListElement->GetIndex(); ++i )
		{
			CControlUI* control = GetItemAt(i);
			if( _tcsicmp(control->GetClass(), _T("OutlookbarNode")) == 0 )
			{
				if( !visible ) 
				{
					control->SetVisible(false);
				}
				else
				{
					Node* local_parent = ((Node*)control->GetTag())->parent();
					if( local_parent->data()._child_visible && local_parent->data()._pListElement->IsVisible() )
					{
						control->SetVisible(true);
					}
				}
			}
		}
	}

	SIZE GetExpanderSizeX(Node* node) const
	{
		if( !node || node == _root ) 
			return CSize();
		
		//位置计算
		SIZE szExpander = {0};
		szExpander.cx = 6 + 24 * node->data()._level - 4/*适当放大一点*/;
		szExpander.cy = szExpander.cx + 16 + 8/*适当放大一点*/;
		return szExpander;
	}

private:
	Node*	_root;
public:
	Node*	m_pLastVisible;
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager) 
	{
		if( _tcsicmp(pstrClass, _T("OutLookBar")) == 0 ) 
			return new COutlookbar;

		return NULL;
	}
};

#endif