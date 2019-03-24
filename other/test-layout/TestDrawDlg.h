#pragma once

//-----------------------------------------------------------------------------------------
#include <layout/layout-box.h>
//-----------------------------------------------------------------------------------------


// CTestDrawDlg dialog

class CTestDrawDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDrawDlg)

public:
	CTestDrawDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestDrawDlg();

// Dialog Data
	enum { IDD = IDD_TESTDRAWDLG };

protected:
	layout::GridBox m_box;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
