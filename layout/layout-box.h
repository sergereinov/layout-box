/*
******************************************************
Original code by Serge Reinov
https://github.com/sergereinov/layout-box
******************************************************

The MIT License (MIT)

Copyright (c) 2019 Serge Reinov https://github.com/sergereinov/layout-box

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************
*/

#ifndef __LAYOUT_BOX_H__
#define __LAYOUT_BOX_H__


#include "layout-types.h"
#include "layout-object.h"
#include <vector>
#include <deque>

namespace layout
{

// -- BoxItem ----------------------------------------------------------------------------------

class BoxItem : public Item, public LinkedObject
{
	LINKED_OBJECT_DELETE_METHOD

public:
	BoxItem() {}
	BoxItem(LinkedObject *pOwner) : LinkedObject(pOwner) {}

	virtual void UpdateMinMax() {}
	virtual void UpdateGeometry() {}
	virtual void InvalidateAt(const Point &offset) {}
};


// -- Box -- abstract class --------------------------------------------------------------------

class Box : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD

public:
	Box() : AutoUpdateMinMax(false) {}
	Box(LinkedObject *pOwner) : BoxItem(pOwner), AutoUpdateMinMax(false){}

	bool AutoUpdateMinMax;

protected:
	double GetNormalizedWeightsCoef(int dimensionIndex, std::vector<BoxItem *> &items);
	double GetWeightsSum(int dimensionIndex, std::vector<BoxItem *> &items);
	void AddSorted(std::deque<BoxItem *> &v, BoxItem *p, const int dimensionIndex);
	void ArrangeItems(std::vector<BoxItem *> &items, bool hasAnyMin, bool hasAnyMax, int dimensionIndex);

	//virtual interface for ArrangeItems
	virtual double GetMinForItem(BoxItem *item, int dimensionIndex) = 0;
	virtual double GetMaxForItem(BoxItem *item, int dimensionIndex) = 0;
	virtual double GetSizeForItem(BoxItem *item, int dimensionIndex) = 0;
	virtual void SetSizeForItem(BoxItem *item, int dimensionIndex, double value) = 0;
	virtual double GetWeightForItem(BoxItem *item, int dimensionIndex) = 0;
};

// -- HBox -------------------------------------------------------------------------------------

class HBox : public Box
{
	LINKED_OBJECT_DELETE_METHOD

public:
	HBox() { InitDimensions(); }
	HBox(LinkedObject *pOwner) : Box(pOwner) { InitDimensions(); }

	void Add(BoxItem *item);
	virtual void UpdateMinMax();
	virtual void UpdateGeometry();
	virtual void InvalidateAt(const Point &offset);

protected:
	inline void InitDimensions() { m_mainDimension = Point::X_Dim; m_secondDimension = Point::Y_Dim; }

	virtual double GetMinForItem(BoxItem *item, int dimensionIndex);
	virtual double GetMaxForItem(BoxItem *item, int dimensionIndex);
	virtual double GetSizeForItem(BoxItem *item, int dimensionIndex) { return item->GetArea().D[dimensionIndex]; }
	virtual void SetSizeForItem(BoxItem *item, int dimensionIndex, double value) { item->SetArea(dimensionIndex, value); }
	virtual double GetWeightForItem(BoxItem *item, int dimensionIndex) { return item->GetWeight().D[dimensionIndex]; }

	std::vector<BoxItem *> m_Items;
	int m_mainDimension;
	int m_secondDimension;
	bool m_hasAnyMin_on_MainDimension;
	bool m_hasAnyMax_on_MainDimension;
};

// -- VBox -------------------------------------------------------------------------------------

class VBox : public HBox
{
	LINKED_OBJECT_DELETE_METHOD
public:
	VBox() { InitDimensions(); }
	VBox(LinkedObject *pOwner) : HBox(pOwner) { InitDimensions(); }
protected:
	inline void InitDimensions() { m_mainDimension = Point::Y_Dim; m_secondDimension = Point::X_Dim; }
};

// -- GridBox ----------------------------------------------------------------------------------

class GridBox : public Box
{
	LINKED_OBJECT_DELETE_METHOD

public:
	static const int MaxRowColumnsCount = 100;

	GridBox() { InitGrid(); }
	GridBox(LinkedObject *pOwner) : Box(pOwner) { InitGrid(); }

	void Set(BoxItem *item, size_t row, size_t column, size_t rowSpan = 1, size_t columnSpan = 1);
	virtual void UpdateMinMax();
	virtual void UpdateGeometry();
	virtual void InvalidateAt(const Point &offset);

	inline void SetRowWeight(size_t row, double value) { SetRulerWeight(row, m_rowDimension, value); }
	inline void SetColumnWeight(size_t column, double value) { SetRulerWeight(column, m_columnDimension, value); }

protected:
	void InitGrid();

	virtual double GetMinForItem(BoxItem *item, int dimensionIndex) { return item->GetMinArea().D[dimensionIndex]; }
	virtual double GetMaxForItem(BoxItem *item, int dimensionIndex) { return item->GetMaxArea().D[dimensionIndex]; }
	virtual double GetSizeForItem(BoxItem *item, int dimensionIndex) { return item->GetArea().D[dimensionIndex]; }
	virtual void SetSizeForItem(BoxItem *item, int dimensionIndex, double value) { item->SetArea(dimensionIndex, value); }
	virtual double GetWeightForItem(BoxItem *item, int dimensionIndex) { return item->GetWeight().D[dimensionIndex]; }

	inline size_t IndexOf(size_t row, size_t column) { return row * m_columnsCount + column; }
	size_t PrepareIndex(size_t row, size_t column, size_t rowSpan, size_t columnSpan);

	void SetRulerWeight(size_t rulerIndex, int dimensionIndex, double value);

	std::vector<BoxItem *> m_Items;
	size_t m_rowsCount;
	size_t m_columnsCount;
	std::vector<BoxItem *> m_Ruler; //diagonal ruler: 'x' is column-side ruler & 'y' is row-side ruler; ruler.size = max(x,y)
	int m_rowDimension;
	int m_columnDimension;
	bool m_hasAnyMin_on_Rows;
	bool m_hasAnyMax_on_Rows;
	bool m_hasAnyMin_on_Columns;
	bool m_hasAnyMax_on_Columns;
};

// -- StackedBox -------------------------------------------------------------------------------

class StackedBox : public BoxItem
{
	LINKED_OBJECT_DELETE_METHOD

public:
	StackedBox() {}
	StackedBox(LinkedObject *pOwner) : BoxItem(pOwner) {}

	void Add(BoxItem *item, bool AutoUpdateMinMax = false);

	virtual void UpdateMinMax();
	virtual void UpdateGeometry();
	virtual void InvalidateAt(const Point &offset);
protected:
	std::vector<BoxItem *> m_Items;
};

}

#endif
