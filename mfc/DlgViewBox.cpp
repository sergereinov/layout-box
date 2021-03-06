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
layout::BoxItem* DlgViewBox::CreateItem(const std::string &element, const std::string &elemText, const layout::view::Params &params)
{
	DlgViewBoxItem *item = NULL;

	if (element == "label")			item = CreateLabel(elemText, params);
	else if (element == "edit")		item = CreateEdit(elemText, params);
	else if (element == "group")	item = CreateGroup(elemText, params);
	else if (element == "list")		item = CreateList(elemText, params);
	else if (element == "table")	item = CreateTable(elemText, params);
	else if (element == "button")	item = CreateButton(elemText, params);
	else if (element == "radio")	item = CreateRadio(elemText, params);
	else if (element == "check")	item = CreateCheck(elemText, params);
	else if (element == "bitmap")	item = CreateBitmap(elemText, params);
	else if (element == "progress")	item = CreateProgress(params);


	if (item)
	{
		CViewDialog *pDlg = GetHostDlg();

		ApplyDefaultParams(item, params);
		if (!ApplyFontParams(item, params))
		{
			if (pDlg)
			{
				CFont *pFont = pDlg->GetFont();
				item->m_pCtrl->SetFont(pFont, FALSE);
			}
		}

		item->m_Element = element;
		item->m_Params = params;
		params.find_value("name", item->m_Name);

		if (pDlg)
			pDlg->BindViewModel(item);
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

DlgViewBoxItem* DlgViewBox::CreateLabel(const std::string &text, const lv::Params &params)
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
	std::string textalign;
	if (params.find_value("align", textalign))
	{
		if (std::string::npos != textalign.find("center"))
			dwStyle |= SS_CENTER;
		else if (std::string::npos != textalign.find("right"))
			dwStyle |= SS_RIGHT;

		if (std::string::npos != textalign.find("mid"))
			dwStyle |= SS_CENTERIMAGE;
	}

	MAKE_TCHAR(ptext, text.c_str())

	UINT id = DlgViewBox::LAST_ID++;
	CStatic *label = new CStatic();
	label->Create(ptext, dwStyle, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, label, id);
}

DlgViewBoxItem* DlgViewBox::CreateEdit(const std::string &text, const layout::view::Params &params)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL;
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

	MAKE_TCHAR(ptext, text.c_str())

	UINT id = DlgViewBox::LAST_ID++;
	CEdit *edit = new CEdit();
	edit->Create(dwStyle, CRect(0,0,1,1), GetHostDlg(), id);
	edit->SetWindowText(ptext);
	return new DlgViewBoxItem(this, edit, id);
}

DlgViewBoxItem* DlgViewBox::CreateGroup(const std::string &text, const layout::view::Params &params)
{
	MAKE_TCHAR(ptext, text.c_str())

	UINT id = DlgViewBox::LAST_ID++;
	CButton *group = new CButton();
	group->Create(ptext, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, group, id);
}

DlgViewBoxItem* DlgViewBox::CreateList(const std::string &text, const layout::view::Params &params)
{
	DWORD dwStyle = 
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | WS_TABSTOP |
		LBS_NOTIFY | LBS_NOINTEGRALHEIGHT;
	std::string style;
	if (params.find_value("style", style))
	{
		if (std::string::npos != style.find("multiplesel"))
			dwStyle |= LBS_MULTIPLESEL;
	}

	UINT id = DlgViewBox::LAST_ID++;
	CListBox *list = new CListBox();
	list->Create(dwStyle, CRect(0,0,1,1), GetHostDlg(), id);
	std::vector<std::string> lines = split(text, '\n');
	for(size_t i=0; i<lines.size(); i++)
	{
		MAKE_TCHAR(ptext, lines[i].c_str())
		list->AddString(ptext);
	}	
	return new DlgViewBoxItem(this, list, id);
}

DlgViewBoxItem* DlgViewBox::CreateTable(const std::string &text, const layout::view::Params &params)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | WS_TABSTOP;
	UINT id = DlgViewBox::LAST_ID++;
	CListCtrl *table = new CListCtrl();
	table->Create(dwStyle, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, table, id);
}

DlgViewBoxItem* DlgViewBox::CreateButton(const std::string &text, const layout::view::Params &params)
{
	MAKE_TCHAR(ptext, text.c_str())

	UINT id = DlgViewBox::LAST_ID++;
	CButton *button = new CButton();
	button->Create(ptext, WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, button, id);
}

DlgViewBoxItem* DlgViewBox::CreateRadio(const std::string &text, const layout::view::Params &params)
{
	MAKE_TCHAR(ptext, text.c_str())

	UINT id = DlgViewBox::LAST_ID++;
	CButton *radio = new CButton();
	radio->Create(ptext, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, radio, id);
}

DlgViewBoxItem* DlgViewBox::CreateCheck(const std::string &text, const layout::view::Params &params)
{
	MAKE_TCHAR(ptext, text.c_str())

	UINT id = DlgViewBox::LAST_ID++;
	CButton *check = new CButton();
	check->Create(ptext, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, check, id);
}

DlgViewBoxItem* DlgViewBox::CreateBitmap(const std::string &text, const layout::view::Params &params)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | SS_BITMAP;
	std::string border;
	if (params.find_value("border", border))
	{
		if (0 == border.compare("sunken"))
			dwStyle |= SS_SUNKEN;
		else
			dwStyle |= WS_BORDER;
	}

	std::string textalign;
	if (params.find_value("align", textalign))
	{
		if (std::string::npos != textalign.find("mid"))
			dwStyle |= SS_CENTERIMAGE;
	}


	MAKE_TCHAR(ptext, text.c_str())

	UINT id = DlgViewBox::LAST_ID++;
	CStatic *label = new CStatic();
	label->Create(ptext, dwStyle, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, label, id);
}

DlgViewBoxItem* DlgViewBox::CreateProgress(const layout::view::Params &params)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	DWORD dwExStyle = 0;
	std::string style;
	if (params.find_value("style", style))
	{
		if (std::string::npos != style.find("smooth"))
			dwStyle |= PBS_SMOOTH;
	}
	std::string border;
	if (params.find_value("border", border))
	{
		if (std::string::npos != border.find("modal"))
			dwExStyle |= WS_EX_DLGMODALFRAME;

		dwStyle |= WS_BORDER;
	}

	UINT id = DlgViewBox::LAST_ID++;
	CProgressCtrl *progress = new CProgressCtrl();
	progress->CreateEx(dwExStyle, dwStyle, CRect(0,0,1,1), GetHostDlg(), id);
	return new DlgViewBoxItem(this, progress, id);
}
