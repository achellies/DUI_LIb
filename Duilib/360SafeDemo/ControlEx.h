#pragma once

class ComputerExamineUI : public CContainerUI
{
public:
	ComputerExamineUI(CPaintManagerUI* pManager)
	{
		CDialogBuilder builder;
		CContainerUI* pComputerExamine = static_cast<CContainerUI*>(builder.Create(_T("ComputerExamine.xml"), (UINT)0, NULL, pManager));
		if( pComputerExamine ) {
			this->Add(pComputerExamine);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};