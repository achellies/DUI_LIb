// eBook - Hanvon eBook Frame UI
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
#ifndef _DIRECTUI_LIST_H__
#define _DIRECTUI_LIST_H__

// basic list implemetion
typedef struct _DirectUIListitem
{
	CStdString m_sLeftTop;
	CStdString m_sRightTop;
	CStdString m_sLeftBottom;
	CStdString m_sRightBottom;
	CStdString m_sCenter;
}DirectUIListitem;

class CDirectUIList : public CVerticalLayoutUI
{
public:
	CDirectUIList()
		: m_nCurIndex(0)
	{}

	void RemoveAll()
	{
		m_nCurIndex = 0;
		__super::RemoveAll();
	}

	void Resize(int nSize)
	{
		RemoveAll();

		for (int i = 0; i < nSize; ++i)
		{
			CDialogBuilder builder;
			CContainerUI* pListItem = static_cast<CContainerUI*>(builder.Create(_T("BasicList.xml"), (UINT)0));
			if (pListItem != NULL)
			{
				this->Add(pListItem);
			}
		}
	}

	void AddItem(const DirectUIListitem& listItem)
	{
		if (GetCount())
		{
			CContainerUI* pContainer = GetClosestContainer(static_cast<CContainerUI*>(GetItemAt(m_nCurIndex)));
			if ((pContainer != NULL) && (pContainer->GetCount() > 0))
			{
				CLabelUI* pLabel = static_cast<CLabelUI*>(pContainer->GetItemAt(1));
				if (pLabel != NULL)
				{
					pLabel->SetAttribute(_T("text"), listItem.m_sLeftTop);
				}
			}
			++m_nCurIndex;
		}
	}
protected:
	CContainerUI* GetClosestContainer(const CContainerUI* pContainer)
	{
		if ((pContainer != NULL) && (pContainer->GetCount() > 0))
		{
			for (int i = 0; i < pContainer->GetCount(); ++i)
			{
				CControlUI* pControl = pContainer->GetItemAt(i);
				if ((pControl != NULL) 
					&& ((_tcsicmp(pControl->GetClass(), _T("HorizontalLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("VertialLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("DialogLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("TileLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("ContainerUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("TabLayoutUI")) == 0)))
				{
					if (_tcsicmp(pControl->GetName(), _T("listitem")) == 0)
					{
						return static_cast<CContainerUI*>(pControl);
					}

					return GetClosestContainer(static_cast<CContainerUI*>(pControl));
				}
				else
				{
					return NULL;
				}
			}
		}
		return NULL;
	}
protected:
	int m_nCurIndex;
};

class CDirectUIList2 : public CTileLayoutUI
{
public:
	CDirectUIList2()
		: m_nCurIndex(0)
	{}

	void RemoveAll()
	{
		m_nCurIndex = 0;
		__super::RemoveAll();
	}

	void Resize(int nSize)
	{
		RemoveAll();

		for (int i = 0; i < nSize; ++i)
		{
			CDialogBuilder builder;
			CContainerUI* pListItem = static_cast<CContainerUI*>(builder.Create(_T("BasicList2.xml"), (UINT)0));
			if (pListItem != NULL)
			{
				this->Add(pListItem);
			}
		}
	}

	void AddItem(const DirectUIListitem& listItem)
	{
		if (GetCount())
		{
			CContainerUI* pContainer = GetClosestContainer(static_cast<CContainerUI*>(GetItemAt(m_nCurIndex)));
			if ((pContainer != NULL) && (pContainer->GetCount() > 0))
			{
				CLabelUI* pLabel = static_cast<CLabelUI*>(pContainer->GetItemAt(1));
				if (pLabel != NULL)
				{
					pLabel->SetAttribute(_T("text"), listItem.m_sLeftTop);
				}
			}
			++m_nCurIndex;
		}
	}
protected:
	CContainerUI* GetClosestContainer(const CContainerUI* pContainer)
	{
		if ((pContainer != NULL) && (pContainer->GetCount() > 0))
		{
			for (int i = 0; i < pContainer->GetCount(); ++i)
			{
				CControlUI* pControl = pContainer->GetItemAt(i);
				if ((pControl != NULL) 
					&& ((_tcsicmp(pControl->GetClass(), _T("HorizontalLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("VertialLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("DialogLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("TileLayoutUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("ContainerUI")) == 0)
					|| (_tcsicmp(pControl->GetClass(), _T("TabLayoutUI")) == 0)))
				{
					if (_tcsicmp(pControl->GetName(), _T("listitem")) == 0)
					{
						return static_cast<CContainerUI*>(pControl);
					}

					return GetClosestContainer(static_cast<CContainerUI*>(pControl));
				}
				else
				{
					return NULL;
				}
			}
		}
		return NULL;
	}
protected:
	int m_nCurIndex;
};

#endif // #ifndef _DIRECTUI_LIST_H__