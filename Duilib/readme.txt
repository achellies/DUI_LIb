AIM:
1. all control is windowless
2. separates interface development from the business logic completely
3. support xul
4. write in pure c++
5. support lua script binding
6. cross platform(windows, wince mobile, linux, embeded device)


TodoList:
1. use tinyxml to parse the skin config file
2. Calendar\Clock etc. widget.
3. lua script binding
4.

Beware of bugs:
1. multiple line text edit is not a directui component.

消息对应关系
Combo 需要相应的消息是：itemselect，通过按enter键或者鼠标点击combox中的列表项都会触发该消息;
button 需要相应的消息是click
option 需要相应的消息是selectchanged

list中的每一列表项对应的消息是: itemselect itemactivate itemclick
其中itemselect 消息是有缺陷的，itemselect 消息只有在该列表项没有被选中的情况下被触发了选中操作才会发出，此消息的pSender是list控件本身；
itemactivate 消息是列表项在选中的状态下用户按了enter键时触发的，此消息的pSender是CListElementUI的子类
itemclick消息是列表项被用户通过鼠标点击后发出的，此消息的pSender是CListElementUI的子类


改动：
1. List中ITEM（包括CListContainerElementUI CListLabelElementUI等）
标准库中是先itemclick然后发itemselect, 目前改为先进行选择, 即发itemselect消息，然后发itemclick消息，这样方便处理listitem的鼠标点击效果;

void CListLabelElementUI::DoEvent(TEventUI& event)
标准：
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
            m_pManager->SendNotify(this, _T("itemclick"));
            Select();
            Invalidate();
        }
        return;
    }
改动：
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
            Select();
            Invalidate();
            m_pManager->SendNotify(this, _T("itemclick"));
        }
        return;
    }

2. 标准库中 如果CListContainerElementUI 中包括像button option combox 等控件，如果用户之间点击该控件的位置，如果该CListContainerElementUI当前状态没有被选中，则这次点击是进行CListContainerElementUI的选中操作，然后得再次点击才能触发button/option等控件的点击消息，
目前改动的是在点击选中过程中(UIEVENT_BUTTONDOWN)同时把消息发送给CListContainerElementUI内部的子控件，然后在UIEVENT_BUTTONUP的消息也发送给CListContainerElementUI内部的控件；

void CListContainerElementUI::DoEvent(TEventUI& event)
标准：

    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
		if( IsEnabled() ){
			m_pManager->SendNotify(this, _T("itemclick"));
			Select();
			Invalidate();			

		}
		return;
    }
    if( event.Type == UIEVENT_BUTTONUP ) 
    {
        return;
    }

改动：
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
		if( IsEnabled() ){
			Select();
			Invalidate();
			m_pManager->SendNotify(this, _T("itemclick"));

			CControlUI* pControl = GetManager()->FindControl(this, event.ptMouse);
			if ((pControl != NULL) && (pControl->GetInterface(_T("Button")) != NULL))
				pControl->Event(event);
		}
		return;
    }
    if( event.Type == UIEVENT_BUTTONUP ) 
    {
		CControlUI* pControl = GetManager()->FindControl(this, event.ptMouse);
		if ((pControl != NULL) && (pControl->GetInterface(_T("Button")) != NULL))
			pControl->Event(event);
        return;
    }

3. 标准库中Combo的列表是通过List来实现的，列表的每一项有可能是CListContainerElementUI CListLabelElementUI等，下面以CListLabelElementUI为例子，CListLabelElementUI处理button Click的代码如下：

void CListLabelElementUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else CListElementUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
		if( IsEnabled() ) {
			Select();
			Invalidate();
			m_pManager->SendNotify(this, _T("itemclick"));
		}
        return;
    }

此处的m_pManager是CComboWnd, 而m_pManager 内的m_aNotifiers是空的，该消息不回发给任何控件，如果此列表项是处于选中的状态，外面也不回收到itemselect的消息，所以combo的消息处理有缺陷。所以把CListContainerElementUI CListLabelElementUI的实现改成如下所示：

    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
		if( IsEnabled() ) {
			if (m_bSelected)
			{
				m_bSelected = false;
				if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
			}
			Select();
			Invalidate();
			m_pManager->SendNotify(this, _T("itemclick"));
		}
        return;
    }

