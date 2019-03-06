/*
*****************************
Original code by Serge Reinov
*****************************
*/

#include <stdafx.h>
#include "DlgViewBox.h"

UINT DlgViewBox::LAST_ID = 1000;

namespace lv = layout::view;
using namespace layout;


//////////////////////
//thx2 Jonathan Boccara's blog https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
#include <sstream>
std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}
//////////////////////

//virtual
layout::BoxItem* DlgViewBox::CreateItem(const std::string &element, const std::string &elemText, const layout::view::Params &params, void *userParam)
{
	CViewDialog *pDlg = (CViewDialog*)userParam;
	DlgViewBoxItem *item = NULL;


	if (element == "label")
	{
		item = CreateLabel(elemText, params, pDlg);
	}
	else if (element == "edit")
	{
		item = CreateEdit(elemText, params, pDlg);
	}
	else if (element == "group")
	{
		item = CreateGroup(elemText, params, pDlg);
	}
	else if (element == "list")
	{
		item = CreateList(elemText, params, pDlg);
	}
	else if (element == "button")
	{
		item = CreateButton(elemText, params, pDlg);
	}
	else if (element == "radio")
	{
		item = CreateRadio(elemText, params, pDlg);
	}


	if (item)
	{
		ApplyDefaultParams(item, params);
		if (!ApplyFontParams(item, params))
		{
			CFont *pFont = pDlg->GetFont();
			item->m_pCtrl->SetFont(pFont, FALSE);
		}

		std::string name;
		params.find_value("name", name);
		pDlg->BindViewModel(element, name, item->m_nID, item->m_pCtrl);
	}

	return item; //item or NULL for unknown element
}

bool DlgViewBox::ApplyFontParams(DlgViewBoxItem* item, const layout::view::Params &params)
{
	if (!item || !item->m_pCtrl)
		return false;

	//font='Arial'
	//font-size='16'
	//font-style='bold;italic;strikeout;underline'
	std::string font;
	if (params.find_value("font", font))
	{
		std::string font_size;
		int size = 0;
		if (params.find_value("font-size", font_size))
			size = atoi(font_size.c_str());
		if (size <= 0)
			size = 10;

		std::string font_style;
		bool isBold = false;
		bool isItalic = false;
		bool isUnderline = false;
		bool isStrikeout = false;
		if (params.find_value("font-style", font_style))
		{
			isBold = (std::string::npos != font_style.find("bold"));
			isItalic = (std::string::npos != font_style.find("italic"));
			isUnderline = (std::string::npos != font_style.find("strikeout"));
			isStrikeout = (std::string::npos != font_style.find("underline"));
		}

		const int dpi = 72;
		const int angle10 = 0;
		const int charSet = DEFAULT_CHARSET;

		HFONT hFont = m_fontsCahce.GetFont(dpi, font.c_str(), size, isBold, isItalic, isUnderline, isStrikeout, angle10, charSet);
		item->m_pCtrl->SetFont(CFont::FromHandle(hFont), FALSE);
		return true;
	}

	return false;
}

DlgViewBoxItem* DlgViewBox::CreateLabel(const std::string &text, const lv::Params &params, CViewDialog *pHostDlg)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	std::string border;
	if (params.find_value("border", border))
	{
		if (0 == border.compare("sunken"))
			dwStyle |= SS_SUNKEN;
		else
			dwStyle |= WS_BORDER;
	}

	UINT id = DlgViewBox::LAST_ID++;
	CStatic *label = new CStatic();
	label->Create(text.c_str(), dwStyle, CRect(0,0,1,1), pHostDlg, id);
	return new DlgViewBoxItem(this, label, id);
}

DlgViewBoxItem* DlgViewBox::CreateEdit(const std::string &text, const layout::view::Params &params, CViewDialog *pHostDlg)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER;
	std::string style;
	if (params.find_value("style", style))
	{
		if (std::string::npos != style.find("readonly"))
			dwStyle |= ES_READONLY;
		if (std::string::npos != style.find("multiline"))
			dwStyle |= ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL;
		if (std::string::npos != style.find("nohidesel"))
			dwStyle |= ES_NOHIDESEL;
	}
	
	UINT id = DlgViewBox::LAST_ID++;
	CEdit *edit = new CEdit();
	edit->Create(dwStyle, CRect(0,0,1,1), pHostDlg, id);
	edit->SetWindowText(text.c_str());
	return new DlgViewBoxItem(this, edit, id);
}

DlgViewBoxItem* DlgViewBox::CreateGroup(const std::string &text, const layout::view::Params &params, CViewDialog *pHostDlg)
{
	UINT id = DlgViewBox::LAST_ID++;
	CButton *group = new CButton();
	group->Create(text.c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, CRect(0,0,1,1), pHostDlg, id);
	return new DlgViewBoxItem(this, group, id);
}

DlgViewBoxItem* DlgViewBox::CreateList(const std::string &text, const layout::view::Params &params, CViewDialog *pHostDlg)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT;
	std::string style;
	if (params.find_value("style", style))
	{
		if (std::string::npos != style.find("multiplesel"))
			dwStyle |= LBS_MULTIPLESEL;
	}

	UINT id = DlgViewBox::LAST_ID++;
	CListBox *list = new CListBox();
	list->Create(dwStyle, CRect(0,0,1,1), pHostDlg, id);
	std::vector<std::string> lines = split(text, '\n');
	for(size_t i=0; i<lines.size(); i++)
		list->AddString(lines[i].c_str());
	return new DlgViewBoxItem(this, list, id);
}

DlgViewBoxItem* DlgViewBox::CreateButton(const std::string &text, const layout::view::Params &params, CViewDialog *pHostDlg)
{
	UINT id = DlgViewBox::LAST_ID++;
	CButton *button = new CButton();
	button->Create(text.c_str(), WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), pHostDlg, id);
	return new DlgViewBoxItem(this, button, id);
}

DlgViewBoxItem* DlgViewBox::CreateRadio(const std::string &text, const layout::view::Params &params, CViewDialog *pHostDlg)
{
	UINT id = DlgViewBox::LAST_ID++;
	CButton *radio = new CButton();
	radio->Create(text.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, CRect(0,0,1,1), pHostDlg, id);
	return new DlgViewBoxItem(this, radio, id);
}

