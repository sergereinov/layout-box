#pragma once

#include "mfc/ViewDialog.h"

// CTestViewDlg dialog

class CTestViewDlg : public CViewDialog
{
	DECLARE_DYNAMIC(CTestViewDlg)

public:
	CTestViewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestViewDlg();

// Dialog Data
	enum { IDD = IDD_TESTVIEWDLG };

	virtual void BindViewModel(const std::string &elem, const std::string &name, UINT id, CWnd *pCtrl);
	virtual BOOL OnInitDialog();

protected:
	struct ViewModel
	{
		CEdit *pEdit;
		CListBox *pList;
		UINT idButtonFour;
		UINT idButtonFive;
		UINT idButtonSix;
		UINT idRadioReady;
		UINT idRadioNone;
		UINT idOk;
		UINT idCancel;

		ViewModel() { memset(this, 0x0, sizeof(ViewModel)); }
	} m_vm;

	BOOL OnButtonClicked(UINT id);
	void SelectInEdit(const CString text);
	void SelectAllInList(BOOL bSelect);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
