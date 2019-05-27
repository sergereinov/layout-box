/*
*****************************
Original code by Serge Reinov
*****************************
*/

#pragma once

#include <vector>
#include <string>
#include "../layout/layout-box.h"
#include "../layout/view/view-box.h"
#include "ViewDialog.h"
#include "DlgViewBoxItem.h"
#include <fonts-cache.h>

std::vector<std::string> split(const std::string& s, char delimiter); //see notes in .cpp

class DlgViewBox : public layout::view::ViewBox
{
	LINKED_OBJECT_DELETE_METHOD
public:
	DlgViewBox() {}
	DlgViewBox(layout::LinkedObject *pOwner) : layout::view::ViewBox(pOwner) {}

	virtual layout::BoxItem* CreateItem(
		const std::string &element, 
		const std::string &elemText, 
		const layout::view::Params &params);

protected:
	inline CViewDialog *GetHostDlg() { return (CViewDialog *)m_viewHost; }

	bool ApplyFontParams(DlgViewBoxItem* item, const layout::view::Params &params);
	DlgViewBoxItem* CreateLabel(const std::string &text, const layout::view::Params &params);
	DlgViewBoxItem* CreateEdit(const std::string &text, const layout::view::Params &params);
	DlgViewBoxItem* CreateGroup(const std::string &text, const layout::view::Params &params);
	DlgViewBoxItem* CreateList(const std::string &text, const layout::view::Params &params);
	DlgViewBoxItem* CreateButton(const std::string &text, const layout::view::Params &params);
	DlgViewBoxItem* CreateRadio(const std::string &text, const layout::view::Params &params);

	GDI::CFontsCache m_fontsCahce;

	static UINT LAST_ID;
};