// TestViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test-layoutDlg.h"
#include "TestViewDlg.h"

#include <mfc/DlgViewBox.h>

namespace lv = layout::view;
using namespace layout;


// CTestViewDlg dialog

IMPLEMENT_DYNAMIC(CTestViewDlg, CViewDialog)

CTestViewDlg::CTestViewDlg(CWnd* pParent /*=NULL*/)
	: CViewDialog(CTestViewDlg::IDD, pParent)
{

}

CTestViewDlg::~CTestViewDlg()
{
}

void CTestViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CViewDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestViewDlg, CViewDialog)
	//ON_WM_GETMINMAXINFO()
	//ON_WM_SIZE()
END_MESSAGE_MAP()


// CTestViewDlg message handlers

BOOL CTestViewDlg::OnInitDialog()
{
	CViewDialog::OnInitDialog();
	SetDefID(-1);

	lv::Library lib;
	//..fill lib..
	typedef lv::Library::Elem Elem;
	Elem& main_box = lib.views["test"];
	main_box.Set("vbox", "test").P("margins", "10")
		//.Child(Elem("label", "title").P("size", ",44").P("border", "sunken").Text(
		.Child(Elem("edit", "title").P("size", ",44").P("style", "readonly;multiline;nohidesel").Text(
			"It is a period of civil war. Rebel spaceships, striking from a hidden base, have won their first victory against the evil Galactic Empire.")
		).Child(Elem("sbox", "body")
			.Child(Elem("group", "group_context").Text("Context"))
			.Child(Elem("hbox", "context").P("margins", "10,18,10,12")
				.Child(Elem("list", "list").P("margins", ",1,,").P("min", "160,").P("style", "multiplesel").Text(
					"Rebel spaceships\n" "Galactic Empire\n" "Rebel spies\n" "steal secret plans\n"
					"ultimate weapon\n" "the DEATH STAR\n" "Princess Leia\n"
					"restore freedom to the galaxy\n" "A New Hope\n")
				).Child(Elem("space").P("size", "4,"))
				.Child(Elem("vbox", "side_controls")
					.Child(Elem("vbox", "buttons")
						.Child(Elem("button", "four").P("size", "75,23").Text("Four"))
						.Child(Elem("button", "five").P("size", "75,23").Text("Five"))
						.Child(Elem("button", "six").P("size", "75,23").Text("Six"))
					).Child(Elem("space"))
					.Child(Elem("gbox", "radio").P("margins", ",6,,6")
						.Child(Elem("label", "selection").P("size", "54,16").P("gpos", "0,1").Text("Selection:"))
						.Child(Elem("radio", "ready").P("size", "54,16").P("gpos", "1,1").Text("Ready"))
						.Child(Elem("radio", "none").P("size", "54,16").P("gpos", "2,1").Text("None"))
						.Child(Elem("space").P("size", ",6").P("gpos", "3,2"))
					)
				)
			)
		).Child(Elem("hbox", "dialog_buttons").P("margins", ",10,,")
			.Child(Elem("space"))
			.Child(Elem("button", "ok").P("size", "75,23").Text("OK"))
			.Child(Elem("space").P("size", "2,"))
			.Child(Elem("button", "cancel").P("size", "75,23").Text("Cancel"))
		)
		;


	//create & implement view
	DlgViewBox *vb = new DlgViewBox(&m_box);
	vb->Implement(lib, "test", this);

	//set view
	m_box.Add(vb, true);

	//first update geom. & move/size controls
	AfterInitView();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTestViewDlg::BindViewModel(DlgViewBoxItem *item)
{
	if (!item)
		return;

	const std::string &elem = item->m_Element;
	const std::string &name = item->m_Name;
	const UINT id = item->m_nID;
	const CWnd *pCtrl = item->m_pCtrl;

	if (elem == "edit")
	{
		if (name == "title")
			m_vm.pEdit = (CEdit*)pCtrl;
		return;
	}
	if (elem == "list")
	{
		if (name == "list")
			m_vm.pList = (CListBox*)pCtrl;
		return;
	}
	if (elem == "button")
	{
		if (name == "four")			m_vm.idButtonFour = id;
		else if (name == "five")	m_vm.idButtonFive = id;
		else if (name == "six")		m_vm.idButtonSix = id;
		else if (name == "ok")		m_vm.idOk = id;
		else if (name == "cancel")	m_vm.idCancel = id;
		return;
	}
	if (elem == "radio")
	{
		if (name == "ready")		m_vm.idRadioReady = id;
		else if (name == "none")	m_vm.idRadioNone = id;
		return;
	}
	return;
}


LRESULT CTestViewDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COMMAND)
	{
		WORD wCode = HIWORD(wParam);
		WORD wID = LOWORD(wParam);
		//HWND hWnd = (HWND)lParam;
		if (wCode == BN_CLICKED)
		{
			if (OnButtonClicked((UINT)wID))
				return 0;
		}
	}

	return CViewDialog::WindowProc(message, wParam, lParam);
}
void CTestViewDlg::SelectInEdit(const CString text)
{
	if (m_vm.pEdit)
	{
		CString str;
		m_vm.pEdit->GetWindowText(str);
		int pos = str.Find(text);
		if (pos >= 0)
			m_vm.pEdit->SetSel(pos, pos + text.GetLength());
	}
}
void CTestViewDlg::SelectAllInList(BOOL bSelect)
{
	if (m_vm.pList)
	{
		for(int i=0, ic=m_vm.pList->GetCount(); i<ic; i++)
			m_vm.pList->SetSel(i, bSelect);
	}
}
BOOL CTestViewDlg::OnButtonClicked(UINT id)
{
	if (id == m_vm.idButtonFour)
	{
		SelectInEdit("Rebel spaceships");
		return TRUE;
	}
	else if (id == m_vm.idButtonFive)
	{
		SelectInEdit("hidden base");
		return TRUE;
	}
	else if (id == m_vm.idButtonSix)
	{
		SelectInEdit("Empire");
		return TRUE;
	}
	else if (id == m_vm.idOk)
	{
		EndDialog(IDOK);
		return TRUE;
	}
	else if (id == m_vm.idCancel)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}
	else if (id == m_vm.idRadioReady)
	{
		SelectAllInList(TRUE);
		return TRUE;
	}
	else if (id == m_vm.idRadioNone)
	{
		SelectAllInList(FALSE);
		return TRUE;
	}
	return FALSE;
}
