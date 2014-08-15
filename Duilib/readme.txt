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

��Ϣ��Ӧ��ϵ
Combo ��Ҫ��Ӧ����Ϣ�ǣ�itemselect��ͨ����enter�����������combox�е��б���ᴥ������Ϣ;
button ��Ҫ��Ӧ����Ϣ��click
option ��Ҫ��Ӧ����Ϣ��selectchanged

list�е�ÿһ�б����Ӧ����Ϣ��: itemselect itemactivate itemclick
����itemselect ��Ϣ����ȱ�ݵģ�itemselect ��Ϣֻ���ڸ��б���û�б�ѡ�е�����±�������ѡ�в����Żᷢ��������Ϣ��pSender��list�ؼ�����
itemactivate ��Ϣ���б�����ѡ�е�״̬���û�����enter��ʱ�����ģ�����Ϣ��pSender��CListElementUI������
itemclick��Ϣ���б���û�ͨ��������󷢳��ģ�����Ϣ��pSender��CListElementUI������


�Ķ���
1. List��ITEM������CListContainerElementUI CListLabelElementUI�ȣ�
��׼��������itemclickȻ��itemselect, Ŀǰ��Ϊ�Ƚ���ѡ��, ����itemselect��Ϣ��Ȼ��itemclick��Ϣ���������㴦��listitem�������Ч��;

void CListLabelElementUI::DoEvent(TEventUI& event)
��׼��
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
            m_pManager->SendNotify(this, _T("itemclick"));
            Select();
            Invalidate();
        }
        return;
    }
�Ķ���
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
            Select();
            Invalidate();
            m_pManager->SendNotify(this, _T("itemclick"));
        }
        return;
    }

2. ��׼���� ���CListContainerElementUI �а�����button option combox �ȿؼ�������û�֮�����ÿؼ���λ�ã������CListContainerElementUI��ǰ״̬û�б�ѡ�У�����ε���ǽ���CListContainerElementUI��ѡ�в�����Ȼ����ٴε�����ܴ���button/option�ȿؼ��ĵ����Ϣ��
Ŀǰ�Ķ������ڵ��ѡ�й�����(UIEVENT_BUTTONDOWN)ͬʱ����Ϣ���͸�CListContainerElementUI�ڲ����ӿؼ���Ȼ����UIEVENT_BUTTONUP����ϢҲ���͸�CListContainerElementUI�ڲ��Ŀؼ���

void CListContainerElementUI::DoEvent(TEventUI& event)
��׼��

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

�Ķ���
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

3. ��׼����Combo���б���ͨ��List��ʵ�ֵģ��б��ÿһ���п�����CListContainerElementUI CListLabelElementUI�ȣ�������CListLabelElementUIΪ���ӣ�CListLabelElementUI����button Click�Ĵ������£�

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

�˴���m_pManager��CComboWnd, ��m_pManager �ڵ�m_aNotifiers�ǿյģ�����Ϣ���ط����κοؼ���������б����Ǵ���ѡ�е�״̬������Ҳ�����յ�itemselect����Ϣ������combo����Ϣ������ȱ�ݡ����԰�CListContainerElementUI CListLabelElementUI��ʵ�ָĳ�������ʾ��

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

