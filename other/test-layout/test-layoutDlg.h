
// test-layoutDlg.h : header file
//

#pragma once


// CTestLayoutDlg dialog
class CTestLayoutDlg : public CDialog
{
// Construction
public:
	CTestLayoutDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTLAYOUT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTestDraw();
	afx_msg void OnBnClickedButtonTestWithMfc();
	afx_msg void OnBnClickedButtonTestViewAndLib();
	afx_msg void OnBnClickedButtonTestFromXml();
	afx_msg void OnBnClickedButtonTestWithNestedLists();
};
