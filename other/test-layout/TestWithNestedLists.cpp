#include <stdafx.h>

#include <string>
#include <vector>
#include <deque>

#include <layout/view/library.h>
#include <layout/view/xml-reader.h>
#include <mfc/ViewDialog.h>
#include <mfc/ViewModel.h>

//////////////////////////////////
// model

class CNestedListsModel
{
public:
	const char* GetLayoutXML()
	{
		return
			"<layouts>"

			"  <vbox name='lists'>"
			"    <list name='list' max=',200' />"
			"    <hbox>"
			"      <button name='prev' size='90,32'>Prev</button>"
			"      <button name='next' size='90,32'>Next</button>"
			"    </hbox>"
			"  </vbox>"

			"</layouts>"
			;
	}

	const char* GetLayoutName() { return "lists"; }


	std::vector<std::string> GetListFor(const std::string &item)
	{
		if (item.empty())
		{
			const char *r[] = {"1", "2"};
			return std::vector<std::string>(r, r + sizeof(r)/sizeof(r[0]));
		}
		if (item == "1")
		{
			const char *r[] = {"11", "12", "13"};
			return std::vector<std::string>(r, r + sizeof(r)/sizeof(r[0]));
		}
		if (item == "13")
		{
			const char *r[] = {"131", "132", "133"};
			return std::vector<std::string>(r, r + sizeof(r)/sizeof(r[0]));
		}
		if (item == "2")
		{
			const char *r[] = {"21", "22", "23"};
			return std::vector<std::string>(r, r + sizeof(r)/sizeof(r[0]));
		}
		return std::vector<std::string>();
	}

	class State
	{
		std::deque<std::string> selectedItems;

	public:
		inline void AddItem(const std::string &item) { selectedItems.push_back(item); }
		inline void RemoveLastItem() { selectedItems.pop_back(); }
		std::string GetLastItem() { return selectedItems.empty() ? "" : *selectedItems.rbegin(); }
		inline BOOL IsEmpty() { return selectedItems.empty() ? TRUE : FALSE; }
	};

	State state;

} NestedListsModel;

class CListsViewModel : public CViewModelAuto
{
public:
	CListsViewModel() 
		: m_list(NULL)
		, m_prev(NULL)
		, m_next(NULL)
	{}

	virtual void AfterInit()
	{
		m_list = GetItemByName("list");
		m_prev = GetItemByName("prev");
		m_next = GetItemByName("next");

		UpdateList("", "");
		OnListSelChange(m_list->m_nID);
	}

	virtual BOOL OnClickButton(UINT nID)
	{
		if (m_prev && (m_prev->m_nID == nID))
		{
			if (!NestedListsModel.state.IsEmpty())
			{
				std::string sel = NestedListsModel.state.GetLastItem();
				NestedListsModel.state.RemoveLastItem();
				std::string list = NestedListsModel.state.GetLastItem();
				UpdateList(list, sel);
				OnListSelChange(m_list->m_nID);
			}
			return TRUE;
		}

		if (m_next && (m_next->m_nID == nID))
		{
			std::string selectedText;
			GetSelectedInList(selectedText);
			if (UpdateList(selectedText, ""))
				NestedListsModel.state.AddItem(selectedText);
		
			OnListSelChange(m_list->m_nID);
			return TRUE;
		}

		return FALSE;
	}

	virtual BOOL OnListSelChange(UINT nID)
	{
		if (m_list && (m_list->m_nID == nID))
		{
			std::string selectedText;
			GetSelectedInList(selectedText);
			BOOL bHasData = (NestedListsModel.GetListFor(selectedText).size() > 0);
			if (m_next && m_next->m_pCtrl)
				m_next->m_pCtrl->EnableWindow(bHasData);

			BOOL bHasPrev = !NestedListsModel.state.IsEmpty();
			if (m_prev && m_prev->m_pCtrl)
				m_prev->m_pCtrl->EnableWindow(bHasPrev);
		}
		return FALSE;
	}

private:
	BOOL GetSelectedInList(std::string &selectedText)
	{
		if (m_list && m_list->m_pCtrl)
		{
			CListBox *lb = (CListBox*)m_list->m_pCtrl;
			int sel = lb->GetCurSel();
			if (sel >= 0)
			{
				CString str;
				lb->GetText(sel, str);
				selectedText = str;
			}
			else
				selectedText.clear();
			return TRUE;
		}
		return FALSE;
	}

	BOOL UpdateList(const std::string &listName, const std::string &selectText)
	{
		std::vector<std::string> lv = NestedListsModel.GetListFor(listName);
		if (m_list && m_list->m_pCtrl)
		{
			CListBox *lb = (CListBox*)m_list->m_pCtrl;
			if (!lv.empty())
			{
				lb->ResetContent();
				for(size_t i=0; i<lv.size(); i++)
					lb->AddString(lv[i].c_str());

				if (!selectText.empty())
				{
					int sel = lb->FindStringExact(0, selectText.c_str());
					lb->SetCurSel((sel >= 0) ? sel : 0);
				}
				else
					lb->SetCurSel(0);
			}
		}
		return lv.empty() ? FALSE : TRUE;
	}

private:
	DlgViewBoxItem* m_list;
	DlgViewBoxItem* m_prev;
	DlgViewBoxItem* m_next;
};

void TestNestedLists()
{
	layout::view::Library lib;
	layout::view::appendXml(lib, NestedListsModel.GetLayoutXML());

	CListsViewModel vm;

	CViewDialog v(IDD_TEMPLATE_A14);
	v.SetupLayout(&lib, NestedListsModel.GetLayoutName());
	v.SetModel(&vm);

	v.DoModal();
}
