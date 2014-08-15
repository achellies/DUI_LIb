#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

#include <vector>

// category(0)->game(1)->server(2)->room(3)
class GameListUI : public CListUI
{
public:
    struct NodeData
    {
        int _level;
        bool _child_visible;
        CStdString _text;
        CListLabelElementUI* _pListElement;
    };

    class Node
    {
        typedef std::vector <Node*>	Children;
        Children	_children;
        Node*		_parent;
        NodeData    _data;

    private:
        void set_parent(Node* parent) { _parent = parent; }

    public:
        Node() : _parent (NULL) {}
        explicit Node(NodeData t) : _data (t), _parent (NULL) {}
        Node(NodeData t, Node* parent)	: _data (t), _parent (parent) {}
        ~Node() 
        {
            for (int i = 0; i < num_children(); i++)
                delete _children[i]; 
        }
        NodeData& data() { return _data; }	
        int num_children() const { return _children.size(); }
        Node* child(int i)	{ return _children[i]; }
        Node* parent() { return ( _parent);	}
        bool has_children() const {	return num_children() > 0; }
        void add_child(Node* child) 
        {
            child->set_parent(this); 
            _children.push_back(child); 
        }
        void remove_child(Node* child)
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
        Node* get_last_child()
        {
            if( has_children() )
            {
                return child(num_children() - 1)->get_last_child();
            }
            else return this;
        }
    };	

    GameListUI() : _root(NULL)
    {
        SetItemShowHtml(true);

        _root = new Node;
        _root->data()._level = -1;
        _root->data()._child_visible = true;
        _root->data()._pListElement = NULL;
    }

    ~GameListUI() { if(_root) delete _root; }

    bool Add(CControlUI* pControl)
    {
        if( !pControl ) return false;
        if( _tcsicmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::Add(pControl);
    }

    bool AddAt(CControlUI* pControl, int iIndex)
    {
        if( !pControl ) return false;
        if( _tcsicmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::AddAt(pControl, iIndex);
    }

    bool Remove(CControlUI* pControl)
    {
        if( !pControl ) return false;
        if( _tcsicmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::Remove(pControl);
    }

    bool RemoveAt(int iIndex)
    {
        CControlUI* pControl = GetItemAt(iIndex);
        if( !pControl ) return false;
        if( _tcsicmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::RemoveAt(iIndex);
    }

    void RemoveAll()
    {
        CListUI::RemoveAll();
        delete _root;
        _root = new Node;
        _root->data()._level = -1;
        _root->data()._child_visible = true;
        _root->data()._pListElement = NULL;
    }

    Node* GetRoot() { return _root; }

    Node* AddNode(LPCTSTR text, Node* parent = NULL)
    {
        if( !parent ) parent = _root;

        CListLabelElementUI* pListElement = new CListLabelElementUI;
        Node* node = new Node;
        node->data()._level = parent->data()._level + 1;
        if( node->data()._level == 0 ) node->data()._child_visible = true;
        else node->data()._child_visible = false;
        node->data()._text = text;
        node->data()._pListElement = pListElement;

        if( !parent->data()._child_visible )
        {
            pListElement->SetVisible(false);
        }
        if( parent != _root && !parent->data()._pListElement->IsVisible() )
        {
            pListElement->SetVisible(false);
        }

        CStdString html_text;
        html_text += _T("<x 6>");
        for( int i = 0; i < node->data()._level; ++i )
        {
            html_text += _T("<x 24>");
        }
        if( node->data()._level < 3 ) {
            if( node->data()._child_visible ) html_text += _T("<a><i icon_c.png></a>");
            else html_text += _T("<a><i icon_c.png></a>");
        }
        html_text += node->data()._text;
        pListElement->SetText(html_text);
      
        pListElement->SetFixedHeight(24);
        pListElement->SetTag((UINT_PTR)node);
        if( node->data()._level == 0 ) {
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
        if( !CListUI::AddAt(pListElement, index) ) {
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

        if( node->data()._child_visible == visible ) return;
        node->data()._child_visible = visible;

        CStdString html_text;
        html_text += _T("<x 6>");
        for( int i = 0; i < node->data()._level; ++i )
        {
            html_text += _T("<x 24>");
        }
        if( node->data()._level < 3 ) {
            if( node->data()._child_visible ) html_text += _T("<i icon_c.png>");
            else html_text += _T("<a><i icon_c.png></a>");
        }
        html_text += node->data()._text;
        node->data()._pListElement->SetText(html_text);

        if( !node->data()._pListElement->IsVisible() ) return;
        if( !node->has_children() ) return;

        Node* begin = node->child(0);
        Node* end = node->get_last_child();
        for( int i = begin->data()._pListElement->GetIndex(); i <= end->data()._pListElement->GetIndex(); ++i )
        {
            CControlUI* control = GetItemAt(i);
            if( _tcsicmp(control->GetClass(), _T("ListLabelElementUI")) == 0 )
            {
                if( !visible ) 
                {
                    control->SetVisible(false);
                }
                else
                {
                    Node* local_parent = ((GameListUI::Node*)control->GetTag())->parent();
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
        if( !node || node == _root ) return CSize();
        if( node->data()._level >= 3 ) return CSize();

        SIZE szExpander = {0};
        szExpander.cx = 6 + 24 * node->data()._level - 4/*适当放大一点*/;
        szExpander.cy = szExpander.cx + 16 + 8/*适当放大一点*/;
        return szExpander;
    }

private:
    Node* _root;
};



#endif __CONTROLEX_H__
