/*
*****************************
Original code by Serge Reinov
*****************************
*/

#pragma once

#include "../layout/layout-box.h"
#include "../layout/view/library.h"
#include "DlgViewBoxItem.h"
#include "ViewModel.h"

class CViewDialog : public CDialog
{
	DECLARE_DYNAMIC(CViewDialog)

public:
	explicit CViewDialog(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CViewDialog();

	//pre-init setup
	void SetupLayout(layout::view::Library *pLibrary, const char *pszLayoutName);
	void SetModel(CViewModel *pModel);

	//at init step
	virtual BOOL OnInitDialog();
	virtual void BindViewModel(DlgViewBoxItem *item);

	//others
	afx_msg void OnSize(UINT nType, int cx, int cy);
#ifndef WINCE
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
#endif

protected:
	DECLARE_MESSAGE_MAP()

	//items container
	layout::StackedBox m_box;

	//library & model references
	layout::view::Library *m_pLibrary;
	std::string m_LayoutName;
	CViewModel *m_pModel;

	//implementers & helpers
	void ImplementLayout();
	void AfterInitView(); //call in OnInitDialog after ImplementLayout with m_box.Add(item, true)
	void OnSizeBox(int cx, int cy);

	//other
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
};
