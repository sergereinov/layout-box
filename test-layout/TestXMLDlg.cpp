// TestXMLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test-layoutDlg.h"
#include "TestXMLDlg.h"

#include "../mfc/DlgViewBox.h"
#include "../layout/view/xml-reader.h"

namespace lv = layout::view;
using namespace layout;

// CTestXMLDlg dialog

IMPLEMENT_DYNAMIC(CTestXMLDlg, CViewDialog)

CTestXMLDlg::CTestXMLDlg(CWnd* pParent /*=NULL*/)
	: CViewDialog(CTestXMLDlg::IDD, pParent)
{

}

CTestXMLDlg::~CTestXMLDlg()
{
}

void CTestXMLDlg::DoDataExchange(CDataExchange* pDX)
{
	CViewDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestXMLDlg, CViewDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTestXMLDlg message handlers

BOOL CTestXMLDlg::OnInitDialog()
{
	CViewDialog::OnInitDialog();
	SetDefID(-1);

	//layouts collection
	const char cXML[] =
		"<layouts>"

		//'main' view template with <child /> slot
		"	<vbox name='main' margins='10'>"
		"		<edit name='title' size=',68' style='readonly;multiline;nohidesel' font='Consolas' font-size='16' font-style='bold;italic'>"
		""			"It is a period of civil war. Rebel spaceships, striking from a hidden base, have won their first victory against the evil Galactic Empire."
		""		"</edit>"
		"		<sbox name='body'>"
		"			<group>Context</group>"
		"			<child font='Arial' font-size='14' />"
		"		</sbox>"
		"		<hbox name='dialog_buttons' margins=',10,,'>"
		"			<space />"
		"			<button name='ok' size='75,23'>OK</button>"
		"			<space size='2,' />"
		"			<button name='cancel' size='75,23'>Cancel</button>"
		"		</hbox>"
		"	</vbox>"
		""

		//child context with reference to template: parent='main'
		"	<hbox name='selectors' parent='main' margins='10,18,10,12'>"
		"		<list name='list' margins=',1,,' min='160,' style='multiplesel'>"
		""			"Rebel spaceships\n"
		""			"Galactic Empire\n"
		""			"Rebel spies\n"
		""			"steal secret plans\n"
		""			"ultimate weapon\n"
		""			"the DEATH STAR\n"
		""			"Princess Leia\n"
		""			"restore freedom to the galaxy\n"
		""			"A New Hope\n"
		""		"</list>"
		"		<space size='4,' />"
		"		<vbox name='side_controls'>"
		"			<vbox name='buttons'>"
		"				<button name='four' size='75,23'>Four</button>"
		"				<button name='five' size='75,23'>Five</button>"
		"				<button name='six' size='75,23'>Six</button>"
		"			</vbox>"
		"			<space />"
		"			<gbox name='radio' margins=',6,,6'>"
		"				<label name='selection' size='64,20' gpos='0,1'>Selection:</label>"
		"				<radio name='ready' size='64,20' gpos='1,1'>Ready</radio>"
		"				<radio name='none' size='64,20' gpos='2,1'>None</radio>"
		"				<space size=',6' gpos='3,2' />"
		"			</gbox>"
		"		</vbox>"
		"	</hbox>"

		"</layouts>"
		;

	lv::Library lib;
	lv::appendXml(lib, cXML);

	//create & implement view
	DlgViewBox *vb = new DlgViewBox(&m_box);
	vb->Implement(lib, "selectors", this); //implement entire view via child-to-parent reference

	//set view
	m_box.Add(vb, true);

	//first update geom. & move/size controls
	AfterInitView();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTestXMLDlg::BindViewModel(const std::string &elem, const std::string &name, UINT id, CWnd *pCtrl)
{
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

LRESULT CTestXMLDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
void CTestXMLDlg::SelectInEdit(const CString text)
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
void CTestXMLDlg::SelectAllInList(BOOL bSelect)
{
	if (m_vm.pList)
	{
		for(int i=0, ic=m_vm.pList->GetCount(); i<ic; i++)
			m_vm.pList->SetSel(i, bSelect);
	}
}
BOOL CTestXMLDlg::OnButtonClicked(UINT id)
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
