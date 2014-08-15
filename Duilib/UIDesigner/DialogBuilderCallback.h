#ifndef DialogBuilderCallback_H_
#define DialogBuilderCallback_H_
#include <windows.h>
#include <objbase.h>
#include "UIlib.h"
using DuiLib::CControlUI;
using DuiLib::IDialogBuilderCallback;
using DuiLib::CPaintManagerUI;

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager) 
	{
		return NULL;
	}
};

#endif //DialogBuilderCallback_H_