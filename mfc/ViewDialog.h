/*
*****************************
Original code by Serge Reinov
*****************************
*/

#pragma once

#include "../layout/layout-box.h"

class CViewDialog : public CDialog
{
	DECLARE_DYNAMIC(CViewDialog)

public:
	explicit CViewDialog(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CViewDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

protected:
	DECLARE_MESSAGE_MAP()

	layout::StackedBox m_box;

	void AfterInitView(); //call in OnInitDialog after m_box.Add(item, true)
	void OnSizeBox(int cx, int cy);
};
