
// test-layoutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test-layout.h"
#include "test-layoutDlg.h"

#include "TestDrawDlg.h"
#include "TestMFCControls.h"
#include "TestViewDlg.h"
#include "TestXmlDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestLayoutDlg dialog




CTestLayoutDlg::CTestLayoutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestLayoutDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestLayoutDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_TEST_DRAW, &CTestLayoutDlg::OnBnClickedButtonTestDraw)
	ON_BN_CLICKED(IDC_BUTTON_TEST_WITH_MFC, &CTestLayoutDlg::OnBnClickedButtonTestWithMfc)
	ON_BN_CLICKED(IDC_BUTTON_TEST_VIEW_AND_LIB, &CTestLayoutDlg::OnBnClickedButtonTestViewAndLib)
	ON_BN_CLICKED(IDC_BUTTON_TEST_FROM_XML, &CTestLayoutDlg::OnBnClickedButtonTestFromXml)
END_MESSAGE_MAP()


// CTestLayoutDlg message handlers

BOOL CTestLayoutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestLayoutDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestLayoutDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestLayoutDlg::OnBnClickedButtonTestDraw()
{
	CTestDrawDlg dlg;
	dlg.DoModal();
}

void CTestLayoutDlg::OnBnClickedButtonTestWithMfc()
{
	CTestMFCControls dlg;
	dlg.DoModal();
}

void CTestLayoutDlg::OnBnClickedButtonTestViewAndLib()
{
	CTestViewDlg dlg;
	dlg.DoModal();
}

void CTestLayoutDlg::OnBnClickedButtonTestFromXml()
{
	CTestXMLDlg dlg;
	dlg.DoModal();
}
