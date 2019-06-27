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

#include "layout-box.h"
#include <set>

#ifdef _DEBUG
#include <windows.h>
#include <tchar.h>
#endif

//TODO
#ifdef NO_VIRTUAL_GET_ITEM_ARRANGE
#else
#endif


namespace layout
{

// -- Box -- abstract class --------------------------------------------------------------------

double Box::GetWeightsSum(int dimensionIndex, std::vector<BoxItem *> &items)
{
	double sum = 0.0;
	for(size_t i=0, ic=items.size(); i<ic; i++)
		sum += GetWeightForItem(items[i], dimensionIndex);
	return sum; //warn: it can be 0.0
}

void Box::AddSorted(std::deque<BoxItem *> &v, BoxItem *p, const int dimensionIndex)
{
	if (v.empty()) { v.push_back(p); return; }

	const double item_sz = GetSizeForItem(p, dimensionIndex);
	const size_t v_size = v.size();
	size_t d = v_size / 2;
	size_t off = d;
	for(;;)
	{
		d /= 2;
		if (item_sz < GetSizeForItem(v[off], dimensionIndex))
		{
			if (d) off -= d;
			else
			{
				v.insert(v.begin() + off, p);
				return;
			}
		}
		else
		{
			if (d) off += d;
			else
			{
				v.insert(v.begin() + off + 1, p);
				return;
			}
		}
	}
}

void Box::ArrangeItems(std::vector<BoxItem *> &items, bool hasAnyMin, bool hasAnyMax, int di) //di = dimensionIndex, like x or y
{
	const double quantFactor = 0.5; //if it too gross, then see&use Point::DimensionsQuantizationFactor
	const double area_D_di = m_margins.GetClientSize(GetArea()).D[di];
	
	std::vector<BoxItem *> other = items; //clone
	std::deque<BoxItem *> fitted_min, fitted_max;
	std::set<void *> fitted_min_dictionary, fitted_max_dictionary;
	double fitted_total = 0.0;

	if (hasAnyMin || hasAnyMax)
	{
		for(;!other.empty();) //fit min-max (when sz < min_dim or sz > max_dim)
		{
			bool had_changes = false;
			
			double weights_sum = GetWeightsSum(di, other);
			double sum_w = (weights_sum > 0.0) ? weights_sum : 1.0;
			double total = area_D_di - fitted_total;

			for(size_t i=0, ic=other.size(); i<ic; i++)
			{
				BoxItem *p = other[i];
				const double min_dim = GetMinForItem(p, di);
				const double max_dim = GetMaxForItem(p, di);
				const double sz = total * GetWeightForItem(p, di) / sum_w;
				if (min_dim > sz)
				{
					fitted_total += min_dim;
					SetSizeForItem(p, di, min_dim);
					
					if (fitted_min_dictionary.end() == fitted_min_dictionary.find(p))
					{
						AddSorted(fitted_min, p, di);
						fitted_min_dictionary.insert(p);
					}
#ifdef _DEBUG
					//else: second time there with the same value => just set min and forget
					else
						OutputDebugString(_T("[Box::ArrangeItems] second time there with the same value => just set min and forget\n"));
#endif

					had_changes = true;					
					other.erase(other.begin()+i);
					break; //start over
				}
				else
				if (max_dim < sz)
				{
					fitted_total += max_dim;
					SetSizeForItem(p, di, max_dim);

					if (fitted_max_dictionary.end() == fitted_max_dictionary.find(p))
					{
						AddSorted(fitted_max, p, di);
						fitted_max_dictionary.insert(p);
					}
#ifdef _DEBUG
					//else: second time there with the same value => just set max and forget
					else
						OutputDebugString(_T("[Box::ArrangeItems] second time there with the same value => just set max and forget\n"));
#endif

					had_changes = true;
					other.erase(other.begin()+i);
					break; //start over
				}
			}			

			if (had_changes)
			{
				if (!other.empty())
					continue; //start over with new 'other'

				//update and go on to fitted
				weights_sum = GetWeightsSum(di, other);
				sum_w = (weights_sum > 0.0) ? weights_sum : 1.0;
				total = area_D_di - fitted_total;
			}
			//else: no changes, so 'weights_sum', 'sum_w' and 'total' stayed the same
			
			//try rollback fitted_min
			if(!fitted_min.empty())
			{
				//get lowest min
				BoxItem *p = *fitted_min.begin();
				const double min_dim = GetMinForItem(p, di);
				const double sz_dim = min_dim; //GetSizeForItem(p, di);
				const double w_dim = GetWeightForItem(p, di);
				const double sw = (weights_sum + w_dim) > 0.0 ? (weights_sum + w_dim) : 1.0;
				const double sz = (total + sz_dim) * w_dim / sw;
				if (min_dim < sz - quantFactor)
				{					
					fitted_total -= sz_dim;
					fitted_min.pop_front();
					other.push_back(p); //put item back to other
					continue; //had_changes, start over
				}
			}

			//try rollback fitted_max
			if(!fitted_max.empty())
			{
				//get greatest max
				BoxItem *p = *fitted_max.rbegin();
				const double max_dim = GetMaxForItem(p, di);
				const double sz_dim = max_dim; //GetSizeForItem(p, di);
				const double w_dim = GetWeightForItem(p, di);
				const double sw = (weights_sum + w_dim) > 0.0 ? (weights_sum + w_dim) : 1.0;
				const double sz = (total + sz_dim) * w_dim / sw;
				if (sz + quantFactor < max_dim)
				{
					fitted_total -= sz_dim;
					fitted_max.pop_back();
					other.push_back(p); //put item back to other
					continue; //had_changes, start over
				}
			}

			break;
		} //for(;!other.empty();)
	} //if (hasAnyMin || hasAnyMax)

	if (!other.empty())
	{
		//catch all unlimited items

		const double weights_sum = GetWeightsSum(di, other);
		const double sum_w = (weights_sum > 0.0) ? weights_sum : 1.0;
		const double total = area_D_di - fitted_total;
		double sum_sz = 0.0;
		for(size_t i=0, ic=other.size(); i<ic; i++)
		{
			BoxItem *p = other[i];
			const double sz = total * GetWeightForItem(p, di) / sum_w;
			if (i < ic - 1)
				SetSizeForItem(p, di, sz);
			else
				SetSizeForItem(p, di, total - sum_sz); //fit last item
			sum_sz += sz;
		}
	}

	//end of Box::ArrangeItems
}


// -- HBox ----------------------------------------------------------------------------------

void HBox::UpdateMinMax()
{
	m_hasAnyMin_on_MainDimension = false;
	m_hasAnyMax_on_MainDimension = false;

	bool allElemsHasMax_on_MainDimension = true;
	
	double sum_min_m_dim = 0.0;
	double sum_max_m_dim = 0.0;

	m_minArea.Set(m_margins.GetOuterBounds());
	m_maxArea.Set(Point::Maximum);

	for(size_t i=0; i<m_Items.size(); i++)
	{
		BoxItem *p = m_Items[i];

		p->UpdateMinMax();

		m_hasAnyMin_on_MainDimension |= p->HasMinimum(m_mainDimension);
		bool hasMax_on_MainDimension = p->HasMaximum(m_mainDimension);
		m_hasAnyMax_on_MainDimension |= hasMax_on_MainDimension;
		allElemsHasMax_on_MainDimension &= hasMax_on_MainDimension;

		sum_min_m_dim += p->GetMinArea().D[m_mainDimension];
		if (allElemsHasMax_on_MainDimension)
			sum_max_m_dim += p->GetMaxArea().D[m_mainDimension];
		
		SetBiggestMin(m_secondDimension, p->GetMinArea());
		SetLowestMax(m_secondDimension, p->GetMaxArea());
	}

	m_minArea.D[m_mainDimension] = m_margins.GetOuterBounds(m_mainDimension) + sum_min_m_dim;
	if (allElemsHasMax_on_MainDimension && (sum_max_m_dim > 0.0))
		m_maxArea.D[m_mainDimension] = m_margins.GetOuterBounds(m_mainDimension) + sum_max_m_dim;
	else
		m_maxArea.D[m_mainDimension] = Point::Maximum;
}

double HBox::GetMinForItem(BoxItem *item, int dimensionIndex)
{
	if (m_mainDimension == dimensionIndex)
		return item->GetMinArea().D[m_mainDimension];
	return m_minArea.D[m_secondDimension];
}

double HBox::GetMaxForItem(BoxItem *item, int dimensionIndex)
{
	if (m_mainDimension == dimensionIndex)
		return item->GetMaxArea().D[m_mainDimension];
	return m_maxArea.D[m_secondDimension];
}

void HBox::Add(BoxItem *item)
{
	m_Items.push_back(item);
	if (AutoUpdateMinMax)
		UpdateMinMax();
}

void HBox::UpdateGeometry()
{
	ArrangeItems(m_Items, m_hasAnyMin_on_MainDimension, m_hasAnyMax_on_MainDimension, m_mainDimension);
	
	double sdsz = m_margins.GetClientSize(GetArea()).D[m_secondDimension];
	for(size_t i=0; i<m_Items.size(); i++)
	{
		BoxItem *p = m_Items[i];
		p->SetArea(m_secondDimension, sdsz);
		p->UpdateGeometry();
	}
}

void HBox::InvalidateAt(const Point &offset)
{
	Point pt = m_margins.GetClientOffset(offset);
	for(size_t i=0; i<m_Items.size(); i++)
	{
		BoxItem *p = m_Items[i];
		p->InvalidateAt(pt);

		pt.Inc(m_mainDimension, p->GetArea().D[m_mainDimension]);
	}
}

// -- GridBox ----------------------------------------------------------------------------------

void GridBox::InitGrid()
{
	m_rowDimension = Point::Y_Dim;
	m_columnDimension = Point::X_Dim;

	//init 1x1
	m_Items.push_back(0L);
	m_Ruler.push_back(new BoxItem(this));
	m_rowsCount = 1;
	m_columnsCount = 1;
}

void GridBox::Set(BoxItem *item, size_t row, size_t column, size_t rowSpan, size_t columnSpan)
{
	//check for max-row-col
	if (!rowSpan || !columnSpan ||
		(row + rowSpan >= MaxRowColumnsCount) || (column + columnSpan >= MaxRowColumnsCount))
		return;

	size_t pos = PrepareIndex(row, column, rowSpan, columnSpan);
	m_Items[pos] = item; //replace prev. item
	if (item)
	{
		item->SetSpan(m_rowDimension, rowSpan);
		item->SetSpan(m_columnDimension, columnSpan);
	}

	if (AutoUpdateMinMax)
		UpdateMinMax();
}

size_t GridBox::PrepareIndex(size_t row, size_t column, size_t rowSpan, size_t columnSpan)
{
	size_t needRow = row + rowSpan - 1;
	while(m_rowsCount <= needRow)
	{
		if (m_Ruler.size() <= needRow)
			m_Ruler.push_back(new BoxItem(this)); //enlarge ruler

		m_Items.insert(m_Items.end(), m_columnsCount, 0L); //add row
		m_rowsCount++;
	}

	size_t needColumn = column + columnSpan - 1;
	while(m_columnsCount <= needColumn)
	{
		if (m_Ruler.size() <= needColumn)
			m_Ruler.push_back(new BoxItem(this)); //enlarge ruler

		//add column
		for(size_t i=0; i<m_rowsCount; i++)
		{
			int idx = i * (m_columnsCount + 1) + m_columnsCount;
			m_Items.insert(m_Items.begin() + idx, 0L);
		}
		m_columnsCount++;
	}

	return IndexOf(row, column);
}

bool GridBox::HasItem(size_t row, size_t column) const
{
	size_t pos = IndexOf(row, column);
	return ((pos < m_Items.size()) && (0L != m_Items[pos]));
}

void GridBox::SetRulerWeight(size_t rulerIndex, int dimensionIndex, double value)
{
	while(m_Ruler.size() <= rulerIndex)
		m_Ruler.push_back(new BoxItem(this)); //enlarge ruler

	m_Ruler[rulerIndex]->SetWeight(dimensionIndex, value);
}

void GridBox::UpdateMinMax()
{
	m_minArea.Set(m_margins.GetOuterBounds());
	m_maxArea.Set(Point::Maximum);

	//calc min-max for m_Ruler's dimensions
	for(size_t r=0; r<m_rowsCount; r++)
	{
		BoxItem *pR = m_Ruler[r];

		for(size_t c=0; c<m_columnsCount; c++)
		{
			BoxItem *pC = m_Ruler[c];

			if (r == 0) pC->ResetMinMax(m_columnDimension); //reset ruler for all columns on first row
			if (c == 0) pR->ResetMinMax(m_rowDimension); //reser ruler for all rows on first column

			size_t idx = IndexOf(r, c);
			BoxItem *p = m_Items[idx];
			if (!p)
				continue;

			p->UpdateMinMax();

			pR->MergeMinMax(m_rowDimension, *p);
			pC->MergeMinMax(m_columnDimension, *p);
		}
	}

	//and calc min-max for GridBox itself
	m_hasAnyMin_on_Rows = false;
	m_hasAnyMax_on_Rows = false;
	m_hasAnyMin_on_Columns = false;
	m_hasAnyMax_on_Columns = false;

	bool allElemsHasMax_on_Rows = true;
	bool allElemsHasMax_on_Columns = true;

	double sum_min_rows = 0.0;
	double sum_max_rows = 0.0;
	double sum_min_columns = 0.0;
	double sum_max_columns = 0.0;

	for(size_t i=0; i<m_Ruler.size(); i++)
	{
		BoxItem *pRuler = m_Ruler[i];

		m_hasAnyMin_on_Rows |= pRuler->HasMinimum(m_rowDimension);
		bool hasMax_on_Rows =  pRuler->HasMaximum(m_rowDimension);
		m_hasAnyMax_on_Rows |= hasMax_on_Rows;
		allElemsHasMax_on_Rows &= hasMax_on_Rows;

		m_hasAnyMin_on_Columns |= pRuler->HasMinimum(m_columnDimension);
		bool hasMax_on_Columns =  pRuler->HasMaximum(m_columnDimension);
		m_hasAnyMax_on_Columns |= hasMax_on_Columns;
		allElemsHasMax_on_Columns &= hasMax_on_Columns;

		sum_min_rows += pRuler->GetMinArea().D[m_rowDimension];
		if (allElemsHasMax_on_Rows)
			sum_max_rows += pRuler->GetMaxArea().D[m_rowDimension];

		sum_min_columns += pRuler->GetMinArea().D[m_columnDimension];
		if (allElemsHasMax_on_Columns)
			sum_max_columns += pRuler->GetMaxArea().D[m_columnDimension];
	}

	m_minArea.D[m_rowDimension] = m_margins.GetOuterBounds(m_rowDimension) + sum_min_rows;
	if (allElemsHasMax_on_Rows && (sum_max_rows > 0.0))
		m_maxArea.D[m_rowDimension] = m_margins.GetOuterBounds(m_rowDimension) + sum_max_rows;
	else
		m_maxArea.D[m_rowDimension] = Point::Maximum;

	m_minArea.D[m_columnDimension] = m_margins.GetOuterBounds(m_columnDimension) + sum_min_columns;
	if (allElemsHasMax_on_Columns && (sum_max_columns > 0.0))
		m_maxArea.D[m_columnDimension] = m_margins.GetOuterBounds(m_columnDimension) + sum_max_columns;
	else
		m_maxArea.D[m_columnDimension] = Point::Maximum;
}

void GridBox::UpdateGeometry()
{
	std::vector<BoxItem *> rowsRuler(m_Ruler.begin(), m_Ruler.begin() + m_rowsCount);
	ArrangeItems(rowsRuler, m_hasAnyMin_on_Rows, m_hasAnyMax_on_Rows, m_rowDimension);

	std::vector<BoxItem *> columnsRuler(m_Ruler.begin(), m_Ruler.begin() + m_columnsCount);
	ArrangeItems(columnsRuler, m_hasAnyMin_on_Columns, m_hasAnyMax_on_Columns, m_columnDimension);

	//apply size and span
	for(size_t r=0; r<m_rowsCount; r++)
	{
		for(size_t c=0; c<m_columnsCount; c++)
		{
			size_t idx = IndexOf(r, c);
			BoxItem *p = m_Items[idx];
			if (!p)
				continue;

			double rowSize = 0.0;
			size_t rowSpan = p->GetSpan().S[m_rowDimension];
			for(size_t rs=0; (rs < rowSpan) && (r + rs < m_Ruler.size()); rs++)
				rowSize += m_Ruler[r + rs]->GetArea().D[m_rowDimension];

			double columnSize = 0.0;
			size_t columnSpan = p->GetSpan().S[m_columnDimension];
			for(size_t cs=0; (cs < columnSpan) && (c + cs < m_Ruler.size()); cs++)
				columnSize += m_Ruler[c + cs]->GetArea().D[m_columnDimension];

			p->SetArea(m_rowDimension, rowSize);
			p->SetArea(m_columnDimension, columnSize);			
			p->UpdateGeometry();
		}
	}
}

void GridBox::InvalidateAt(const Point &offset)
{
	Point pt_r = m_margins.GetClientOffset(offset);
	for(size_t r=0; r<m_rowsCount; r++)
	{
		Point pt_c = pt_r;
		for(size_t c=0; c<m_columnsCount; c++)
		{
			const Point &pt = pt_c;

			size_t idx = IndexOf(r, c);
			BoxItem *p = m_Items[idx];
			if (p)
				p->InvalidateAt(pt);

			pt_c.Inc(m_columnDimension, m_Ruler[c]->GetArea().D[m_columnDimension]);
		}
		pt_r.Inc(m_rowDimension, m_Ruler[r]->GetArea().D[m_rowDimension]);
	}
}

// -- StackedBox -------------------------------------------------------------------------------

void StackedBox::Add(BoxItem *item, bool AutoUpdateMinMax)
{
	m_Items.push_back(item);
	if (AutoUpdateMinMax)
		UpdateMinMax();
}
void StackedBox::UpdateMinMax()
{
	m_minArea.Set(m_margins.GetOuterBounds());
	m_maxArea.Set(Point::Maximum);
	for(size_t i=0; i<m_Items.size(); i++)
	{
		BoxItem *p = m_Items[i];
		p->UpdateMinMax();
		for(int di=0; di<Point::DimensionsCount; di++)
			MergeMinMax(di, *p);
	}
}
void StackedBox::UpdateGeometry()
{
	Size clientSize = m_margins.GetClientSize(GetArea());
	for(size_t i=0; i<m_Items.size(); i++)
	{
		BoxItem *p = m_Items[i];
		p->SetArea(clientSize);
		p->UpdateGeometry();
	}
}
void StackedBox::InvalidateAt(const Point &offset)
{
	Point clientOffset = m_margins.GetClientOffset(offset);
	for(size_t i=0; i<m_Items.size(); i++)
		m_Items[i]->InvalidateAt(clientOffset);
}


}