#pragma once

//-----------------------------------------------------------------------------------------
#include "../layout/layout-box.h"
//-----------------------------------------------------------------------------------------

// CTestMFCControls dialog

class CTestMFCControls : public CDialog
{
	DECLARE_DYNAMIC(CTestMFCControls)

public:
	CTestMFCControls(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestMFCControls();

// Dialog Data
	enum { IDD = IDD_TEST_MFC_CONTROLS };

protected:
	layout::StackedBox m_box;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
