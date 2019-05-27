/*
*****************************
Original code by Serge Reinov
*****************************
*/

#pragma once

#include "../layout/layout-box.h"
#include "../layout/view/library.h"

class DlgViewBoxItem : public layout::BoxItem
{
	LINKED_OBJECT_DELETE_METHOD

public:
	DlgViewBoxItem(layout::LinkedObject *pOwner, CWnd *pCtrl, UINT nID)
		: layout::BoxItem(pOwner)
		, m_pCtrl(pCtrl)
		, m_nID(nID)
	{}

	~DlgViewBoxItem()
	{
		if (m_pCtrl)
		{
			delete m_pCtrl; //delete C<mfc-controls> with CWnd virtual destructor
			m_pCtrl = NULL;
		}
	}

	//virtual void UpdateMinMax() {} //calc_rect for some ctrls types

	virtual void InvalidateAt(const layout::Point &offset)
	{
		if (!m_pCtrl || !::IsWindow(m_pCtrl->m_hWnd))
			return;

		CRect r = GetRect(
			m_margins.GetClientOffset(offset),
			m_margins.GetClientSize(m_area));
		m_pCtrl->MoveWindow(r);
		m_pCtrl->Invalidate();
	}


	CRect GetRect(const layout::Point &offset, const layout::Size& size)
	{
		CRect r(
			(int)(offset.X() + 0.5),
			(int)(offset.Y() + 0.5),
			(int)(offset.X() + size.X() + 0.5) - 1,
			(int)(offset.Y() + size.Y() + 0.5) - 1);
		return r;
	}

	std::string m_Element;
	std::string m_Name;
	layout::view::Params m_Params;
	CWnd *m_pCtrl;
	UINT m_nID;
};
