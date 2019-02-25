/*
*****************************
Original code by Serge Reinov
*****************************
*/

#include <stdafx.h>
#include "ViewDialog.h"


using namespace layout;


IMPLEMENT_DYNAMIC(CViewDialog, CDialog)

CViewDialog::CViewDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{

}

CViewDialog::~CViewDialog()
{
}

BEGIN_MESSAGE_MAP(CViewDialog, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CViewDialog::OnSize(UINT nType, int cx, int cy)
{
	OnSizeBox(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

void CViewDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	//get min/max info from m_box
	CRect rc, rw;
	GetClientRect(rc);
	GetWindowRect(rw);
	int w0 = rw.Width() - rc.Width();
	int h0 = rw.Height() - rc.Height();

	lpMMI->ptMinTrackSize.x = w0 + (int)(m_box.GetMinArea().X() + 0.5);
	lpMMI->ptMinTrackSize.y = h0 + (int)(m_box.GetMinArea().Y() + 0.5);
	if (!Point::IsMaximum(m_box.GetMaxArea().X()))
		lpMMI->ptMaxTrackSize.x = w0 + (int)(m_box.GetMaxArea().X() + 0.5);
	if (!Point::IsMaximum(m_box.GetMaxArea().Y()))
		lpMMI->ptMaxTrackSize.y = h0 + (int)(m_box.GetMaxArea().Y() + 0.5);

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CViewDialog::AfterInitView()
{
	//first update geom. & move/size controls
	CRect r;
	GetClientRect(r);
	OnSizeBox(r.Width(), r.Height());
}

void CViewDialog::OnSizeBox(int cx, int cy)
{
	m_box.SetArea(Point::X_Dim, cx);
	m_box.SetArea(Point::Y_Dim, cy);
	m_box.UpdateGeometry();
	m_box.InvalidateAt(0.0);
}
