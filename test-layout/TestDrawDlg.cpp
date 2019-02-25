// TestDrawDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test-layout.h"
#include "TestDrawDlg.h"


//-----------------------------------------------------------------------------------------

using namespace layout;

static int RectBoxItem_counter = 0;

class RectBoxItem : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD

public:
	RectBoxItem(CWnd *pDrawSurface, LPCSTR lpszText, LinkedObject *pOwner, Size &minSize = Size(0.0), Size &maxSize = Size(Point::Maximum)) 
		: m_pDrawSurface(pDrawSurface), m_strText(lpszText), BoxItem(pOwner)
	{
		m_minArea = minSize;
		m_maxArea = maxSize;

		RectBoxItem_counter++;
		CString str;
		str.Format("[CREATE '%s'] items created: RectBoxItem_counter = %i\n", m_strText, RectBoxItem_counter);
		OutputDebugString(str);
	}
	~RectBoxItem()
	{
		RectBoxItem_counter--;
		CString str;
		str.Format("[DESTROY '%s'] items left: RectBoxItem_counter = %i\n", m_strText, RectBoxItem_counter);
		OutputDebugString(str);
	}

	static CRect DrawRect(HDC hDC, const Point &offset, const Size &size)
	{
		CRect r;
		r.left = (int)(offset.D[Point::X_Dim] + 0.5);
		r.top = (int)(offset.D[Point::Y_Dim] + 0.5);
		r.right = (int)(offset.D[Point::X_Dim] + size.D[Point::X_Dim] + 0.5);
		r.bottom = (int)(offset.D[Point::Y_Dim] + size.D[Point::Y_Dim] + 0.5);

		r.right--;
		r.bottom--;

		MoveToEx(hDC, r.left, r.top, NULL);
		LineTo(hDC, r.right, r.top);
		LineTo(hDC, r.right, r.bottom);
		LineTo(hDC, r.left, r.bottom);
		LineTo(hDC, r.left, r.top);

		return r;
	}
	virtual void InvalidateAt(const Point &offset)
	{
		HWND hDrawSurface = m_pDrawSurface->m_hWnd;
		HDC hDC = GetDC(hDrawSurface);
		
		SetBkMode(hDC, TRANSPARENT);

		HPEN hPen = CreatePen(PS_SOLID, 1, COLORREF(0x000000));
		HPEN hPenGray = CreatePen(PS_DASH, 1, COLORREF(0x808080));

		char t1 = m_strText[0];

		HPEN hOldPen = (HPEN)SelectObject(hDC, hPenGray);
		DrawRect(hDC, MarginsRef().GetClientOffset(offset), MarginsRef().GetClientSize(GetArea()));

		SelectObject(hDC, hPen);
		CRect r = DrawRect(hDC, offset, GetArea());

		DrawText(hDC, m_strText, m_strText.GetLength(), r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		SelectObject(hDC, hOldPen);
		ReleaseDC(hDrawSurface, hDC);
		DeleteObject(hPen);
		DeleteObject(hPenGray);
	}

	RectBoxItem* SetMin(int sx, int sy)
	{
		m_minArea.D[Point::X_Dim] = sx;
		m_minArea.D[Point::Y_Dim] = sy;
		return this;
	}
	RectBoxItem* SetMax(int sx, int sy)
	{
		m_maxArea.D[Point::X_Dim] = sx;
		m_maxArea.D[Point::Y_Dim] = sy;
		return this;
	}

protected:
	CWnd *m_pDrawSurface;
	CString m_strText;
};



//-----------------------------------------------------------------------------------------


// CTestDrawDlg dialog

IMPLEMENT_DYNAMIC(CTestDrawDlg, CDialog)

CTestDrawDlg::CTestDrawDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDrawDlg::IDD, pParent)
{
	// -- 0,0 -- (row,col) ----------------------------------------------------
	RectBoxItem *r1 = new RectBoxItem(this, "r1", &m_box);
	r1->MarginsRef().Set(10.0); //just show margins
	r1->SetMin(100, 100)->SetMax(200, 200);
	RectBoxItem *r2 = new RectBoxItem(this, "r2", &m_box);
	RectBoxItem *r3 = new RectBoxItem(this, "r3", &m_box);

	RectBoxItem *r4 = new RectBoxItem(this, "r4", &m_box);
	r4->SetWeight(Point::X_Dim, 2.0);
	RectBoxItem *r5 = new RectBoxItem(this, "r5", &m_box);

	RectBoxItem *r6 = new RectBoxItem(this, "r6", &m_box);

	HBox *h1 = new HBox(&m_box);
	h1->Add(r1);
	h1->Add(r2);
	h1->Add(r3);

	HBox *h2 = new HBox(&m_box);
	h2->Add(r4);
	h2->Add(r5);

	HBox *h3 = new HBox(&m_box);
	h3->SetWeight(Point::Y_Dim, 0.5);
	h3->Add(r6);

	VBox *v1 = new VBox(&m_box);
	v1->Add(h1);
	v1->Add(h2);
	v1->Add(h3);
	
	// -- 1,0 -- (row,col) ----------------------------------------------------
	GridBox *g1 = new GridBox(&m_box);

	RectBoxItem *gr1 = new RectBoxItem(this, "gr1", &m_box);
	gr1->SetMax(50, 50);
	RectBoxItem *gr2 = new RectBoxItem(this, "gr2", &m_box);
	RectBoxItem *gr3 = new RectBoxItem(this, "gr3", &m_box);
	gr3->SetMax(100, 100);
	RectBoxItem *gr4 = new RectBoxItem(this, "gr4", &m_box);
	//gr4->SetWeight(Point::Y_Dim, 0.5); //wrong!
	g1->SetRowWeight(3, 0.5); //correct

	g1->Set(gr1, 2, 0);
	g1->Set(gr2, 1, 1);
	g1->Set(gr3, 0, 2);
	g1->Set(gr4, 3, 1, 1, 2);

	// -- 0,1 -- (row,col) ----------------------------------------------------
	StackedBox *s1 = new StackedBox(&m_box);
	RectBoxItem *sr1 = new RectBoxItem(this, "sr1________", &m_box);
	RectBoxItem *sr2 = new RectBoxItem(this, "____sr2____", &m_box);
	RectBoxItem *sr3 = new RectBoxItem(this, "________sr3", &m_box);
	s1->Add(sr1);
	s1->Add(sr2);
	s1->Add(sr3);

	// -- 1,1 -- (row,col) ----------------------------------------------------
	StackedBox *s2 = new StackedBox(&m_box);

	RectBoxItem *srg = new RectBoxItem(this, "group", &m_box);

	GridBox *g2 = new GridBox(&m_box);
	g2->MarginsRef().Set(20.0);
	
	int x=0, y=0, dx=1, dy=0;
	const int side_count = 7;
	for(int num=0; num<=4*(side_count-1); num++)
	{
		CString str;
		str.Format("n%i", num);
		RectBoxItem *ri = new RectBoxItem(this, str, &m_box); //create
		ri->SetMin(30, 20); //setup
		g2->Set(ri, y, x); //add

		x += dx; y += dy;
		if (x > side_count-2) { dx = 0; dy = 1; }
		if (y > side_count-2) { dx = -1; dy = 0; }
		if (x < 0) { x = 0; dx = 0; dy = -1; }
		if (y < 0) break;
	}

	s2->Add(srg);
	s2->Add(g2);

	// -- fill box ------------------------------------------------------------
	m_box.Set(v1, 0, 0);
	m_box.Set(g1, 1, 0);
	m_box.Set(s1, 0, 1);
	m_box.Set(s2, 1, 1);

	m_box.UpdateMinMax();
}

CTestDrawDlg::~CTestDrawDlg()
{
}

void CTestDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestDrawDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTestDrawDlg message handlers

BOOL CTestDrawDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTestDrawDlg::OnEraseBkgnd(CDC* pDC)
{
	//return CDialog::OnEraseBkgnd(pDC);

	CRect r;
	GetClientRect(r);
	pDC->FillSolidRect(r, COLORREF(0xFFFFFF));
	return TRUE;
}

void CTestDrawDlg::OnPaint()
{
	{
		CPaintDC dc(this); // device context for painting
		// TODO: Add your message handler code here
		// Do not call CDialog::OnPaint() for painting messages
	}

	m_box.InvalidateAt(Point(0.0));
}

void CTestDrawDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CRect rc, rw;
	GetClientRect(rc);
	GetWindowRect(rw);
	int w0 = rw.Width() - rc.Width();
	int h0 = rw.Height() - rc.Height();

	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = w0 + (int)(m_box.GetMinArea().D[Point::X_Dim] + 0.5);
	lpMMI->ptMinTrackSize.y = h0 + (int)(m_box.GetMinArea().D[Point::Y_Dim] + 0.5);
	if (!Point::IsMaximum(m_box.GetMaxArea().D[Point::X_Dim]))
		lpMMI->ptMaxTrackSize.x = w0 + (int)(m_box.GetMaxArea().D[Point::X_Dim] + 0.5);
	if (!Point::IsMaximum(m_box.GetMaxArea().D[Point::Y_Dim]))
		lpMMI->ptMaxTrackSize.y = h0 + (int)(m_box.GetMaxArea().D[Point::Y_Dim] + 0.5);

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CTestDrawDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_box.SetArea(Point::X_Dim, cx);
	m_box.SetArea(Point::Y_Dim, cy);
	m_box.UpdateGeometry();
	Invalidate();

	CString str;
	str.Format("%i x %i", cx, cy);
	SetWindowText(str);
}
