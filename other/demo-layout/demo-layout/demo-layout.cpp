// demo-layout.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <layout/layout-box.h>
using namespace layout;

// No need a GUI.
// No framework required (use your favorite one or none at all).
// Any OS.
// Any platform.

void demo_hbox1();
void demo_vbox1();
void demo_gridbox1();
void demo_stackedbox1();

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "--- demo_hbox1 ---" << std::endl;
	demo_hbox1();

	std::cout << "--- demo_vbox1 ---" << std::endl;
	demo_vbox1();

	std::cout << "--- demo_gridbox1 ---" << std::endl;
	demo_gridbox1();

	std::cout << "--- demo_stackedbox1 ---" << std::endl;
	demo_stackedbox1();

	return 0;
}

void demo_hbox1()
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

void demo_vbox1()
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

	const double test_sizes[] = {100.0, 40.0};

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

class MyBoxItem : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD
public:
	MyBoxItem(LinkedObject *pOwner) : BoxItem(pOwner) {}

	virtual void InvalidateAt(const Point &offset)
	{
		//print result
		std::cout << "MyBoxItem " 
			<< "pos(x,y) = (" 
			<< offset.X() << "," 
			<< offset.Y() << ") "
			<< "size(x,y) = (" 
			<< m_area.X() << "," 
			<< m_area.Y() << ") "
			<< std::endl;
	}
};

void demo_gridbox1()
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

void demo_stackedbox1()
{
	//create container layout::GridBox
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

	delete stacked; //delete grid and all linked items
}
