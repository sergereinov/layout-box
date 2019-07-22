# layout-box - simple & abstract layout manager

* No need a GUI.
* No framework required (use your favorite one or none at all).
* Any OS.
* Any platform.

<br/>
<details>
  <summary>Click to show demo GIF</summary>
  
  ![TestDrawDlg](/other/images/TestDrawDlg.gif)
</details>
<br/>

Examples:
- [layout::HBox](#layouthbox)
- [layout::VBox](#layoutvbox)
- [layout::GridBox](#layoutgridbox)
- [layout::StackedBox](#layoutstackedbox)
- [Linked objects](#linked-objects)
- [Example with composition](#example-with-composition)

Other examples: see the `other/test-layout` project for more complex examples. 

[Dependencies](#dependencies) 

## layout::HBox
```c++
#include <iostream>

#include <layout/layout-box.h>
using namespace layout;

void main()
{
	//create container layout::HBox
	HBox *hbox = new HBox();
	
	//create items
	BoxItem *item1 = new BoxItem(hbox);
	BoxItem *item2 = new BoxItem(hbox);
	item2->SetWeight(Point::X_Dim, 2.0); //make item2 2 times bigger than item1

	//add items into container
	hbox->Add(item1);
	hbox->Add(item2);

	//set container size and update geometry
	hbox->SetArea(Size(100.0));
	hbox->UpdateGeometry();

	//print result
	std::cout << "item1 size x = " << item1->GetArea().X() << std::endl;
	std::cout << "item2 size x = " << item2->GetArea().X() << std::endl;

	delete hbox; //delete hbox and all linked items
}
```
Prints
```
item1 size x = 33.3333
item2 size x = 66.6667
```


## layout::VBox
```c++
#include <iostream>

#include <layout/layout-box.h>
using namespace layout;

void main()
{
	//create container layout::VBox
	VBox *vbox = new VBox();
	
	//create items
	BoxItem *item1 = new BoxItem(vbox);
	BoxItem *item2 = new BoxItem(vbox);
	BoxItem *item3 = new BoxItem(vbox);

	item1->SetMinMax(Point::Y_Dim, 10.0, 10.0);
	item3->SetMinMax(Point::Y_Dim, 20.0, Point::Maximum);

	//add items into container
	vbox->Add(item1);
	vbox->Add(item2);
	vbox->Add(item3);
	
	//update vbox minmax by its elements
	vbox->UpdateMinMax();

	const double test_sizes[] = {100.0, 43.0};

	for(int i = 0; i < sizeof(test_sizes)/sizeof(double); i++)
	{
		double sz = test_sizes[i];
		std::cout << "set size = " << sz << std::endl;

		//set container size and update geometry
		vbox->SetArea(Size(sz));
		vbox->UpdateGeometry();

		//print result
		std::cout << "item1 size y is " << item1->GetArea().Y() << std::endl;
		std::cout << "item2 size y is " << item2->GetArea().Y() << std::endl;
		std::cout << "item3 size y is " << item3->GetArea().Y() << std::endl;
	}

	delete vbox; //delete vbox and all linked items
}
```
Prints
```
set size = 100
item1 size y is 10
item2 size y is 45
item3 size y is 45
set size = 43
item1 size y is 10
item2 size y is 13
item3 size y is 20
```


## layout::GridBox
```c++
#include <iostream>

#include <layout/layout-box.h>
using namespace layout;

class MyBoxItem : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD
public:
	MyBoxItem(LinkedObject *pOwner) : BoxItem(pOwner) {}

	virtual void InvalidateAt(const Point &offset)
	{
		//print result
		std::cout << "MyBoxItem " 
			<< "pos(x,y) = (" << offset.X() << "," 	<< offset.Y() << ") "
			<< "size(x,y) = (" << m_area.X() << "," << m_area.Y() << ") "
			<< std::endl;
	}
};

void main()
{
	//create container layout::GridBox
	GridBox *grid = new GridBox();

	//create items
	MyBoxItem *item1 = new MyBoxItem(grid);
	MyBoxItem *item2 = new MyBoxItem(grid);
	MyBoxItem *item3 = new MyBoxItem(grid);

	//add items into container
	grid->Set(item1, 0, 0);
	grid->Set(item2, 1, 1);
	grid->Set(item3, 2, 0, 1, 2); //spanning on two columns

	//set container size and update geometry
	grid->SetArea(Size(100.0));
	grid->UpdateGeometry();

	std::cout << "grid size(x,y) = (" << grid->GetArea().X() << "," << grid->GetArea().Y() << ")" << std::endl;

	//position the container to the point (0.0, 0.0)
	// and print results via MyBoxItem::InvalidateAt
	grid->InvalidateAt(Point(0.0));

	//inspect grid metric
	std::cout << "grid items:" << std::endl;
	for(size_t r = 0, rc = grid->GetRowsCount(); r < rc; r++)
	{
		for(size_t c = 0, cc = grid->GetColumnsCount(); c < cc; c++)
			std::cout << (grid->HasItem(r, c) ? "item\t" : "null\t");
		std::cout << std::endl;
	}

	delete grid; //delete grid and all linked items
}
```
Prints
```
grid size(x,y) = (100,100)
MyBoxItem pos(x,y) = (0,0) size(x,y) = (50,33.3333)
MyBoxItem pos(x,y) = (50,33.3333) size(x,y) = (50,33.3333)
MyBoxItem pos(x,y) = (0,66.6667) size(x,y) = (100,33.3333)
grid items:
item    null
null    item
item    null
```


## layout::StackedBox
```c++
#include <iostream>

#include <layout/layout-box.h>
using namespace layout;

class MyBoxItem : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD
public:
	MyBoxItem(LinkedObject *pOwner) : BoxItem(pOwner) {}

	virtual void InvalidateAt(const Point &offset)
	{
		//print result
		std::cout << "MyBoxItem " 
			<< "pos(x,y) = (" << offset.X() << "," 	<< offset.Y() << ") "
			<< "size(x,y) = (" << m_area.X() << "," << m_area.Y() << ") "
			<< std::endl;
	}
};

void main()
{
	//create container layout::StackedBox
	StackedBox *stacked = new StackedBox();

	//create items
	MyBoxItem *item1 = new MyBoxItem(stacked);
	MyBoxItem *item2 = new MyBoxItem(stacked);

	//add items into container
	stacked->Add(item1);
	stacked->Add(item2);

	//set container size and update geometry
	stacked->SetArea(Size(100.0));
	stacked->UpdateGeometry();

	//position the container to the point (0.0, 0.0)
	// and print results via MyBoxItem::InvalidateAt
	stacked->InvalidateAt(Point(0.0));

	delete stacked; //delete stacked and all linked items
}
```
Prints
```
MyBoxItem pos(x,y) = (0,0) size(x,y) = (100,100)
MyBoxItem pos(x,y) = (0,0) size(x,y) = (100,100)
```


## Linked objects
```c++
#include <iostream>

#include <layout/layout-box.h>
using namespace layout;

class MyObject : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD
public:
	MyObject(LinkedObject *pOwner) : BoxItem(pOwner)
	{
		std::cout << "created MyObject" << std::endl;
	}
	~MyObject()
	{
		std::cout << "deleting MyObject" << std::endl;
	}
};

void main()
{
	//create container
	HBox *hbox = new HBox();

	//create items
	new MyObject(hbox);
	new MyObject(hbox);
	new MyObject(hbox);

	//delete container and all linked items
	delete hbox; 
}
```
Prints
```
created MyObject
created MyObject
created MyObject
deleting MyObject
deleting MyObject
deleting MyObject
```

## Example with composition

<details>
  <summary>Click to show</summary>
  
```c++
#include <iostream>
#include <string>

#include <layout/layout-box.h>
using namespace layout;

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

void main()
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
	hbox_buttons->Add(new layout::BoxItem(view)); //middle space filler
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
	view->InvalidateAt(Point(0)); //at (0.0, 0.0) with size of view (screen_sx, screen_sy)

	//show screen_buffer
	for(int pos = 0; pos < screen_size; pos += screen_sx)
		std::cout << std::string(screen_buffer + pos, screen_sx) << std::endl;

	//delete view and all linked layout items
	delete view;
}
```
Prints
```
+---------------------------------+
|                                 |
|  Message box with two options.  |
|                                 |
+---------------------------------+
+-----+                      +----+
| YES |                      | NO |
+-----+                      +----+
```

</details>


## Other examples
See the `other/test-layout` project for more complex examples.

## Dependencies

Tested under Visual Studio 2008. 

Using C++98/2003. 

Dependencies:
- for layout: `std::set`, `std::vector`, `std::deque`
- for layout/view, library and xml-reader: `std::map`, `std::string`, tinyxml2
- for mfc, ViewDialog: MFC, Windows
