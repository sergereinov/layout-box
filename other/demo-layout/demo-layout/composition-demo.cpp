#include "stdafx.h"

#include <iostream>
#include <string>

#include <layout/layout-box.h>
using namespace layout;


//
// example with composition
//

// --------------------------------------------------------------
// some useful defs

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define inrange(val, min_val, max_val) max(min_val, min(max_val, val))

// --------------------------------------------------------------
// character screen buffer

const int screen_sx = 35;
const int screen_sy = 8;
const int screen_size = screen_sx * screen_sy;
char screen_buffer[screen_size] = {0x0};


// --------------------------------------------------------------
// let's say we have UI controls defined somewhere
// and that draw themselves into the global screen_buffer

namespace UI
{
	class TextControl
	{
	public:
		void MoveControl(int x, int y, int sx, int sy) 
		{ 
			m_x = inrange(x, 0, screen_sx);
			m_y = inrange(y, 0, screen_sy);
			m_sx = inrange(sx, 0, screen_sx - m_x);
			m_sy = inrange(sy, 0, screen_sy - m_y);
		}

		virtual void OnDraw()
		{
			//draw frame rect
			for(int i = m_x, i_last = m_x + m_sx - 1; i <= i_last; i++)
				for(int j = m_y, j_last = m_y + m_sy - 1; j <= j_last; j++)
				{
					const int pos = i + j * screen_sx;
					if ((i == m_x) || (i == i_last))
						screen_buffer[pos] = ((j > m_y) && (j < j_last)) ? '|' : '+';
					else if ((j == m_y) || (j == j_last))
						screen_buffer[pos] = ((i > m_x) && (i < i_last)) ? '-' : '+';
					else
						screen_buffer[pos] = ' ';
				}
		}

		void DrawText(const std::string &text)
		{
			int len = ((int)text.length() < m_sx) ? (int)text.length() : m_sx;
			int pos_x = m_x + (m_sx - len) / 2;
			int pos_y = m_y + m_sy / 2;
			int pos = pos_x + pos_y * screen_sx;

			if (pos >= screen_size)
				pos = screen_size - 1;
			if (pos + len >= screen_size)
				len = screen_size - pos - 1;

			memcpy(screen_buffer + pos, text.c_str(), len);
		}

		int m_x;
		int m_y;
		int m_sx;
		int m_sy;
	};

	class Label : public TextControl
	{
	public:
		Label(const char *text) : m_text(text) {}

		virtual void OnDraw()
		{
			TextControl::OnDraw();

			DrawText(m_text);
		}

		std::string m_text;
	};

	class Button : public TextControl
	{
	public:
		Button(const char *name) : m_name(name) {}

		virtual void OnDraw()
		{
			TextControl::OnDraw();

			DrawText(m_name);
		}

		std::string m_name;
	};
}

// --------------------------------------------------------------
// define class of text item based on layout box item
// and use it to move & draw the linked UI control

class TextItem : public layout::BoxItem
{
	LINKED_OBJECT_DELETE_METHOD
public:
	TextItem(LinkedObject *pOwner, UI::TextControl *pControl) 
		: BoxItem(pOwner)
		, m_pControl(pControl)
	{}

	virtual void InvalidateAt(const Point &offset)
	{
		if (m_pControl)
		{
			const int x = (int)(offset.X() + 0.5);
			const int y = (int)(offset.Y() + 0.5);
			const int sx = (int)(m_area.X() + 0.5);
			const int sy = (int)(m_area.Y() + 0.5);

			m_pControl->MoveControl(x, y, sx, sy);
			m_pControl->OnDraw();
		}
	}

	UI::TextControl *m_pControl;
};

// --------------------------------------------------------------

void demo_with_composition()
{
	//create some controls
	UI::Label label_message("Message box with two options.");
	UI::Button button_yes("YES");
	UI::Button button_no("NO");


	//define main layout view
	StackedBox *view = new layout::StackedBox();


	//create layout items with link to appropriate controls
	TextItem *msg = new TextItem(view, &label_message);
	TextItem *yes = new TextItem(view, &button_yes);
	TextItem *no = new TextItem(view, &button_no);

	yes->SetMinMax(Point::X_Dim, 5, 7); //minmax for X-axis
	yes->SetMinMax(Point::Y_Dim, 3, 3); //minmax for Y-axis
	no->SetMinMax(Point::X_Dim, 4, 6);
	no->SetMinMax(Point::Y_Dim, 3, 3);


	//horisontal layout box with buttons
	HBox *hbox_buttons = new layout::HBox(view);
	hbox_buttons->Add(yes);
	hbox_buttons->Add(new layout::BoxItem(view)); //middle greedy filler
	hbox_buttons->Add(no);


	//vertical layout box with message and buttons box
	VBox *vbox_messagebox = new layout::VBox(view);
	vbox_messagebox->Add(msg);
	vbox_messagebox->Add(hbox_buttons);


	//setup main view
	view->Add(vbox_messagebox);

	//update minmax from all nested items
	view->UpdateMinMax();

	//set size & update geometry
	view->SetArea(Size().X(screen_sx).Y(screen_sy));
	view->UpdateGeometry();
	
	//draw view into screen_buffer (via TextItem::InvalidateAt)
	view->InvalidateAt(Point(0)); //at (0.0, 0.0) with size of view (scr_sx, scr_sy)

	//show screen_buffer
	for(int pos = 0; pos < screen_size; pos += screen_sx)
		std::cout << std::string(screen_buffer + pos, screen_sx) << std::endl;

	//delete view and all linked layout items
	delete view;
}
