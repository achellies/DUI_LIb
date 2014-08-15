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
#ifndef _CONTROLEX_H__
#define _CONTROLEX_H__
#include "DUI_List.h"
#include "DUI_Tree.h"

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager) 
	{
		if( _tcsicmp(pstrClass, _T("DirectUIList")) == 0 ) return new CDirectUIList;
		else if( _tcsicmp(pstrClass, _T("DirectUIList2")) == 0 ) return new CDirectUIList2;
		else if( _tcsicmp(pstrClass, _T("DirectUITree")) == 0 ) return new CDirectUITree;
		return NULL;
	}
};

#endif // #ifndef _CONTROLEX_H__