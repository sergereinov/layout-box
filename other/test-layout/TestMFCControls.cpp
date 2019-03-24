// TestMFCControls.cpp : implementation file
//

#include "stdafx.h"
#include "test-layoutDlg.h"
#include "TestMFCControls.h"

using namespace layout;

class DlgBoxItem : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD

public:
	DlgBoxItem(LinkedObject *pOwner, CDialog *pDlg, UINT id, BOOL fixHeight = FALSE, BOOL fixWidth = FALSE)
		: BoxItem(pOwner), m_pDlg(pDlg), m_ID(id)
	{
		CRect r;
		m_pDlg->GetDlgItem(m_ID)->GetWindowRect(r);
		if (fixHeight)
		{
			m_minArea.Y(r.Height());
			m_maxArea.Y(r.Height());
		}
		if (fixWidth)
		{
			m_minArea.X(r.Width());
			m_maxArea.X(r.Width());
		}
	}
	DlgBoxItem(LinkedObject *pOwner, int fixedHeight, int fixedWidth)
		: BoxItem(pOwner), m_pDlg(0), m_ID(0)
	{
		if (fixedHeight)
		{
			m_minArea.Y(fixedHeight);
			m_maxArea.Y(fixedHeight);
		}
		if (fixedWidth)
		{
			m_minArea.X(fixedWidth);
			m_maxArea.X(fixedWidth);
		}
	}

	inline CRect GetRect(const Point &offset, const Size& size)
	{
		CRect r(
			(int)(offset.D[Point::X_Dim] + 0.5),
			(int)(offset.D[Point::Y_Dim] + 0.5),
			(int)(offset.D[Point::X_Dim] + size.D[Point::X_Dim] + 0.5) - 1,
			(int)(offset.D[Point::Y_Dim] + size.D[Point::Y_Dim] + 0.5) - 1
			);
		return r;
	}
	virtual void InvalidateAt(const Point &offset)
	{
		if (!m_pDlg || !m_ID)
			return;

		CRect r = GetRect(
			m_margins.GetClientOffset(offset),
			m_margins.GetClientSize(m_area)
			);
		CWnd *w = m_pDlg->GetDlgItem(m_ID);
		w->MoveWindow(r);
		w->Invalidate();
	}
	void SetMin(int sx, int sy)
	{
		m_minArea.D[Point::X_Dim] = sx;
		m_minArea.D[Point::Y_Dim] = sy;
	}

protected:
	CDialog *m_pDlg;
	UINT m_ID;

};



// CTestMFCControls dialog

IMPLEMENT_DYNAMIC(CTestMFCControls, CDialog)

CTestMFCControls::CTestMFCControls(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMFCControls::IDD, pParent)
{

}

CTestMFCControls::~CTestMFCControls()
{
}

void CTestMFCControls::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestMFCControls, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTestMFCControls message handlers

BOOL CTestMFCControls::OnInitDialog()
{
	CDialog::OnInitDialog();

	//sample data
	CListBox *pList = (CListBox *)GetDlgItem(IDC_LIST);
	pList->AddString("Rebel spaceships");
	pList->AddString("Galactic Empire");
	pList->AddString("Rebel spies ");
	pList->AddString("steal secret plans");
	pList->AddString("ultimate weapon");
	pList->AddString("the DEATH STAR");
	pList->AddString("Princess Leia");
	pList->AddString("restore freedom to the galaxy");
	pList->AddString("A New Hope");

	//create layout rules
	m_box.MarginsRef().Set(10.0);

	VBox *main_box = new VBox(&m_box);
	DlgBoxItem *title = new DlgBoxItem(&m_box, this, IDC_STATIC_TITLE, TRUE);
	
	StackedBox *body = new StackedBox(&m_box);
	DlgBoxItem *group = new DlgBoxItem(&m_box, this, IDC_STATIC_CONTEXT);
	
	HBox *context = new HBox(&m_box);
	//context->MarginsRef().Set(10.0);
	context->MarginsRef().LeftTop.X(10);
	context->MarginsRef().LeftTop.Y(18);
	context->MarginsRef().RightBottom.X(10);
	context->MarginsRef().RightBottom.Y(12);

	DlgBoxItem *list = new DlgBoxItem(&m_box, this, IDC_LIST);
	list->MarginsRef().LeftTop.Y(1); //wrap to buttons block
	list->SetMin(160, 0);
	list->ItemDebugID = 1;

	VBox *side_controls = new VBox(&m_box);
	
	VBox *buttons = new VBox(&m_box);
	DlgBoxItem *four = new DlgBoxItem(&m_box, this, IDC_BUTTON_FOUR, TRUE, TRUE);
	DlgBoxItem *five = new DlgBoxItem(&m_box, this, IDC_BUTTON_FIVE, TRUE, TRUE);
	DlgBoxItem *six = new DlgBoxItem(&m_box, this, IDC_BUTTON_SIX, TRUE, TRUE);
	buttons->Add(four);
	buttons->Add(five);
	buttons->Add(six);

	GridBox *radio = new GridBox(&m_box);
	radio->MarginsRef().LeftTop.Y(6); //top gap 6px, for example
	DlgBoxItem *selection = new DlgBoxItem(&m_box, this, IDC_STATIC_SELECTION, TRUE, TRUE);
	DlgBoxItem *ready = new DlgBoxItem(&m_box, this, IDC_RADIO_READY, TRUE, TRUE);
	DlgBoxItem *none = new DlgBoxItem(&m_box, this, IDC_RADIO_NONE, TRUE, TRUE);
	radio->Set(selection, 0, 1);
	radio->Set(ready, 1, 1);
	radio->Set(none, 2, 1);
	radio->Set(new DlgBoxItem(&m_box, 6, 0), 3, 2); //make grid 3x4 with gap 6px below


	side_controls->Add(buttons);
	side_controls->Add(new BoxItem(&m_box));
	side_controls->Add(radio);


	context->Add(list);
	context->Add(new DlgBoxItem(&m_box, 0, 4));
	context->Add(side_controls);

	body->Add(group);
	body->Add(context);

	HBox *dialog_buttons = new HBox(&m_box);
	dialog_buttons->MarginsRef().LeftTop.D[Point::Y_Dim] = 10;
	DlgBoxItem *ok = new DlgBoxItem(&m_box, this, IDOK, TRUE, TRUE);
	DlgBoxItem *cancel = new DlgBoxItem(&m_box, this, IDCANCEL, TRUE, TRUE);

	dialog_buttons->Add(new BoxItem(&m_box)); //greedy free space filler
	dialog_buttons->Add(ok);
	dialog_buttons->Add(new DlgBoxItem(&m_box, 0, 2)); //gap between buttons
	dialog_buttons->Add(cancel);
	
	main_box->Add(title);
	main_box->Add(body);
	main_box->Add(dialog_buttons);

	m_box.Add(main_box, true);

	CRect r;
	GetClientRect(r);
	m_box.SetArea(Point::X_Dim, r.Width());
	m_box.SetArea(Point::Y_Dim, r.Height());
	m_box.UpdateGeometry();
	m_box.InvalidateAt(0.0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTestMFCControls::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CRect rc, rw;
	GetClientRect(rc);
	GetWindowRect(rw);
	int w0 = rw.Width() - rc.Width();
	int h0 = rw.Height() - rc.Height();

	lpMMI->ptMinTrackSize.x = w0 + (int)(m_box.GetMinArea().D[Point::X_Dim] + 0.5);
	lpMMI->ptMinTrackSize.y = h0 + (int)(m_box.GetMinArea().D[Point::Y_Dim] + 0.5);
	if (!Point::IsMaximum(m_box.GetMaxArea().D[Point::X_Dim]))
		lpMMI->ptMaxTrackSize.x = w0 + (int)(m_box.GetMaxArea().D[Point::X_Dim] + 0.5);
	if (!Point::IsMaximum(m_box.GetMaxArea().D[Point::Y_Dim]))
		lpMMI->ptMaxTrackSize.y = h0 + (int)(m_box.GetMaxArea().D[Point::Y_Dim] + 0.5);

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CTestMFCControls::OnSize(UINT nType, int cx, int cy)
{
	m_box.SetArea(Point::X_Dim, cx);
	m_box.SetArea(Point::Y_Dim, cy);
	m_box.UpdateGeometry();
	m_box.InvalidateAt(0.0);

	CDialog::OnSize(nType, cx, cy);
}
