#ifndef Control_Type_H_
#define Control_Type_H_

enum eControlType
{
	eControl_Unkonwn,
	eControl_Label,
	eControl_Edit,
	eControl_Button,
	eControl_CheckBox,
	eControl_RadioBox,
	eControl_HSlider,
	eControl_VSlider,
	eControl_Combox,
	eControl_HProgress,
	eControl_VProgress,
	eControl_List,
	eControl_HScrollbar,
	eControl_VScrollbar,
	eControl_Tree,
	eControl_TabControl,
	eControl_TabLayout,
	eControl_Container,
	eControl_HorizontalLayout,
	eControl_VerticalLayout,
	eControl_RichEdit,
	eControl_ActiveX
};

extern eControlType g_eControlType;

#endif // Control_Type_H_