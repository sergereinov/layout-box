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
	virtual void AfterInit() {;}

	//public metrics
	virtual BOOL HasButtons() { return FALSE; }
	virtual BOOL HasLists() { return FALSE; }

	//handlers
	virtual BOOL OnClickButton(UINT nID) { return FALSE; }
	virtual BOOL OnListSelChange(UINT nID) { return FALSE; }
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
