/*
*****************************
Original code by Serge Reinov
*****************************
*/

#include <stdafx.h>
#include "ViewModel.h"

void CViewModel::SetText(DlgViewBoxItem *pItem, LPCTSTR lpszText)
{
	if (pItem && pItem->m_pCtrl)
		pItem->m_pCtrl->SetWindowText(lpszText);
}
CString CViewModel::GetText(DlgViewBoxItem *pItem)
{
	if (pItem && pItem->m_pCtrl)
	{
		CString result;
		pItem->m_pCtrl->GetWindowText(result);
		return result;
	}
	return _T("");
}
void CViewModel::HideItem(DlgViewBoxItem *pItem)
{
	if (pItem && pItem->m_pCtrl)
		pItem->m_pCtrl->ShowWindow(SW_HIDE);
}
void CViewModel::ShowItem(DlgViewBoxItem *pItem, LPCTSTR lpszText)
{
	if (pItem && pItem->m_pCtrl)
	{
		if (lpszText)
			pItem->m_pCtrl->SetWindowText(lpszText);
		pItem->m_pCtrl->ShowWindow(SW_SHOW);
	}
}
void CViewModel::EnableItem(DlgViewBoxItem *pItem, BOOL bEnable)
{
	if (pItem && pItem->m_pCtrl)
		pItem->m_pCtrl->EnableWindow(bEnable);
}
void CViewModel::SetFocus(DlgViewBoxItem *pItem)
{
	if (pItem && pItem->m_pCtrl)
		pItem->m_pCtrl->SetFocus();
}

void CViewModelAuto::Bind(DlgViewBoxItem *item)
{
	if (!item || item->m_Name.empty())
		return;

	//we have the item with name so bind it

	m_elements.push_back(item);
	size_t index = m_elements.size()-1;
	m_id2elems[item->m_nID] = index;
	m_name2elems[item->m_Name] = index;

	//if button track on_button
	//if <event> track <on_event>

	if (item->m_Element == "button")
		m_state.hasButtons = TRUE;
	else if (item->m_Element == "list")
		m_state.hasLists = TRUE;
}

DlgViewBoxItem *CViewModelAuto::GetItemByName(const std::string &name)
{
	std::map<std::string, size_t>::iterator it = m_name2elems.find(name);
	if (it == m_name2elems.end())
		return NULL;

	size_t index = it->second;
	if (index < m_elements.size())
		return m_elements[index];

	return NULL;
}

DlgViewBoxItem *CViewModelAuto::GetItemByID(UINT nID)
{
	std::map<UINT, size_t>::iterator it = m_id2elems.find(nID);
	if (it == m_id2elems.end())
		return NULL;

	size_t index = it->second;
	if (index < m_elements.size())
		return m_elements[index];

	return NULL;
}

CWnd* CViewModelAuto::UnpackItemCtrl(const std::string &name)
{
	DlgViewBoxItem *item = GetItemByName(name);
	if (item)
		return item->m_pCtrl;
	return NULL;
}
