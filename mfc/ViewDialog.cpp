/*
*****************************
Original code by Serge Reinov
*****************************
*/

#include <stdafx.h>
#include "ViewDialog.h"
#include "DlgViewBox.h"

using namespace layout;


IMPLEMENT_DYNAMIC(CViewDialog, CDialog)

CViewDialog::CViewDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
	, m_pLibrary(NULL)
	, m_pModel(NULL)
{

}

CViewDialog::~CViewDialog()
{
}

BEGIN_MESSAGE_MAP(CViewDialog, CDialog)
#ifndef WINCE
	ON_WM_GETMINMAXINFO()
#endif
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CViewDialog::OnSize(UINT nType, int cx, int cy)
{
	OnSizeBox(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

#ifndef WINCE
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
#endif

void CViewDialog::AfterInitView()
{
	//first update geom. & move/size controls
	CRect r;
	GetWindowRect(r);

	//force update min-max via resize window
	SetWindowPos(0, 0, 0, r.Width(), r.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

	//force update box container size
	CRect c;
	GetClientRect(c);
	OnSizeBox(c.Width(), c.Height());

	if(m_pModel)
		m_pModel->AfterInit();
}

void CViewDialog::OnSizeBox(int cx, int cy)
{
	m_box.SetArea(Point::X_Dim, cx);
	m_box.SetArea(Point::Y_Dim, cy);
	m_box.UpdateGeometry();
	m_box.InvalidateAt(0.0);
}

void CViewDialog::ImplementLayout()
{
	//implement view chain
	DlgViewBox *vb = new DlgViewBox(&m_box);
	vb->Implement(*m_pLibrary, m_LayoutName, this);

	//set view
	m_box.Add(vb, true);
}

BOOL CViewDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pLibrary && !m_LayoutName.empty())
	{
		ImplementLayout();
		AfterInitView(); //first update geom. & move/size controls
	}

	return TRUE;
}

void CViewDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	if (m_pModel) m_pModel->OnActivate(nState, pWndOther, bMinimized);
}
void CViewDialog::OnDestroy()
{
	if (m_pModel) m_pModel->BeforeDestroy();

	CDialog::OnDestroy();
}


void CViewDialog::BindViewModel(DlgViewBoxItem *item)
{
	if(m_pModel)
	{
		m_pModel->Bind(item);
	}
}

void CViewDialog::SetupLayout(layout::view::Library *pLibrary, const char *pszLayoutName)
{
	m_pLibrary = pLibrary;
	m_LayoutName = pszLayoutName;
}
void CViewDialog::SetModel(CViewModel *pModel)
{
	m_pModel = pModel;
}


LRESULT CViewDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pModel != NULL)
	{
		if (message == WM_COMMAND)
		{
			WORD wCode = HIWORD(wParam);
			WORD wID = LOWORD(wParam);
			//HWND hWnd = (HWND)lParam;
			if (wCode == BN_CLICKED)
			{
				if (m_pModel->HasButtons() &&
					m_pModel->OnClickButton((UINT)wID))
					return 0;
			}
			else if (wCode == LBN_SELCHANGE)
			{
				if (m_pModel->HasLists() &&
					m_pModel->OnListSelChange((UINT)wID))
					return 0;
			}
		}

		LRESULT lResult = 0;
		if ((message >= 0x0400) && (message <= 0x7FFF))
		{
			//WM_USER + n
			if (m_pModel->On_WM_USER(message, wParam, lParam, lResult))
				return lResult;
		}
		else
		if ((message >= 0x8000) && (message <= 0xBFFF))
		{
			//WM_APP + n
			if (m_pModel->On_WM_APP(message, wParam, lParam, lResult))
				return lResult;
		}
		else
		if ((message >= 0xC000) && (message <= 0xFFFF))
		{
			//got via RegisterWindowMessage
			if (m_pModel->On_RegisteredWindowMessage(message, wParam, lParam, lResult))
				return lResult;
		}

		if (m_pModel->WindowProc(message, wParam, lParam, lResult))
			return lResult;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CViewDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (m_pModel && m_pModel->HasKeyDown())
		{
			WORD nVK = (WORD)pMsg->wParam;
			BYTE nScanCode = (BYTE)(pMsg->lParam >> 16);
			if (m_pModel->OnKeyDown(nVK, nScanCode))
				return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH CViewDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if ((m_pModel != NULL) &&
		(m_pModel->HasColors()))
		return m_pModel->OnCtlColor(hbr, pDC, pWnd, nCtlColor);

	return hbr;
}

/*
//TODO? or is it better to use CWnd base?
void CViewDialog::ShowModal()
{
	//ref: https://docs.microsoft.com/en-us/windows/desktop/winmsg/window-styles
	//  +: https://docs.microsoft.com/en-us/windows/desktop/dlgbox/dialog-box-styles
	const DWORD dwStyle = 
		WS_POPUP | WS_CAPTION | WS_BORDER | WS_VISIBLE |
		DS_CENTER | DS_MODALFRAME
		;

	//ref: https://docs.microsoft.com/en-us/windows/desktop/winmsg/extended-window-styles
	const DWORD dwExtStyle = 0; //WS_EX_DLGMODALFRAME;

	DLGTEMPLATE t;
	t.style = dwStyle;
	t.dwExtendedStyle = dwExtStyle;
	t.cdit = 0;
	t.x = t.y = 0;
	t.cx = t.cy = 200;

	if (InitModalIndirect(&t))
		DoModal();


	//ref: https://devblogs.microsoft.com/oldnewthing/20070521-00/?p=26783

	// ??
	//use: CreateIndirect (?)
	// then: CWnd::RunModalLoop
	return;
}
*/

