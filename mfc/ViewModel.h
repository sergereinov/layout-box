/*
*****************************
Original code by Serge Reinov
*****************************
*/

#pragma once

#include <vector>
#include "DlgViewBoxItem.h"

class CViewModel
{
public:
	virtual void Bind(DlgViewBoxItem *item) = 0;
	virtual void AfterInit() {;} //after implement items
	virtual void EndOfInit() {;} //custom child init handler (not used by ViewDialog)
	virtual void BeforeDestroy() {;}

	//public metrics
	virtual BOOL HasButtons() { return FALSE; }
	virtual BOOL HasLists() { return FALSE; }
	virtual BOOL HasColors() { return FALSE; }
	virtual BOOL HasKeyDown() { return FALSE; }

	//handlers
	virtual BOOL OnClickButton(UINT nID) { return FALSE; }
	virtual BOOL OnListSelChange(UINT nID) { return FALSE; }
	virtual HBRUSH OnCtlColor(HBRUSH hDefaultBrush, CDC* pDC, CWnd* pWnd, UINT nCtlColor) { return hDefaultBrush; }
	virtual BOOL OnKeyDown(WORD nVK, BYTE nScanCode) { return FALSE; } // VK see: https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
	virtual void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {;}

	//special handlers
	virtual BOOL On_WM_USER(const UINT message, const WPARAM wParam, const LPARAM lParam, LRESULT &lResult) { return FALSE; }
	virtual BOOL On_WM_APP(const UINT message, const WPARAM wParam, const LPARAM lParam, LRESULT &lResult) { return FALSE; }
	virtual BOOL On_RegisteredWindowMessage(const UINT message, const WPARAM wParam, const LPARAM lParam, LRESULT &lResult) { return FALSE; }
	virtual BOOL WindowProc(const UINT message, const WPARAM wParam, const LPARAM lParam, LRESULT &lResult) { return FALSE; }

	//common helpers
	static void SetText(DlgViewBoxItem *pItem, LPCTSTR lpszText);
	static CString GetText(DlgViewBoxItem *pItem);
	static void HideItem(DlgViewBoxItem *pItem);
	static void ShowItem(DlgViewBoxItem *pItem, LPCTSTR lpszText = NULL);
	static void EnableItem(DlgViewBoxItem *pItem, BOOL bEnable);
	static void SetFocus(DlgViewBoxItem *pItem);
};

class CViewModelAuto : public CViewModel
{
public:
	virtual void Bind(DlgViewBoxItem *item);
	virtual BOOL HasButtons() { return m_state.hasButtons; }
	virtual BOOL HasLists() { return m_state.hasLists; }

protected:
	std::vector<DlgViewBoxItem*> m_elements;
	std::map<UINT, size_t> m_id2elems;
	std::map<std::string, size_t> m_name2elems;

	struct State
	{
		BOOL hasButtons;
		BOOL hasLists;

		//init state
		State() { Clear(); }
		void Clear() { memset(this, 0x0, sizeof(State)); }
	};
	State m_state;

	DlgViewBoxItem *GetItemByName(const std::string &name);
	DlgViewBoxItem *GetItemByID(UINT nID);

	CWnd* UnpackItemCtrl(const std::string &name);
};
