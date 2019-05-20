#pragma once

#include "mfc/ViewDialog.h"
#include "mfc/ViewController.h"

// CTestXMLDlg dialog

class CTestXMLDlg : public CViewDialog, public CViewController
{
	DECLARE_DYNAMIC(CTestXMLDlg)

public:
	CTestXMLDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestXMLDlg();

// Dialog Data
	enum { IDD = IDD_TESTXML };

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
