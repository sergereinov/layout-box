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

#ifndef __LAYOUT_TYPES_H__
#define __LAYOUT_TYPES_H__


#if !defined(LAYOUT_POINT_DIMENSIONS_COUNT)
#	define LAYOUT_POINT_DIMENSIONS_COUNT 2
#elif LAYOUT_POINT_DIMENSIONS_COUNT < 2
#	undef LAYOUT_POINT_DIMENSIONS_COUNT
#	define LAYOUT_POINT_DIMENSIONS_COUNT 2
#endif

namespace layout
{

class Point
{
public:
	static const int DimensionsCount = LAYOUT_POINT_DIMENSIONS_COUNT;
	static const int X_Dim = 0;
	static const int Y_Dim = 1;
#if LAYOUT_POINT_DIMENSIONS_COUNT > 2
	static const int Z_Dim = 2;
	//and so on..
#endif

	double D[DimensionsCount];

	Point() { Set(0.0); }
	Point(double all) { Set(all); }
	static inline bool IsValidDimension(int dimensionIndex) { return (dimensionIndex >= 0) && (dimensionIndex < DimensionsCount); }
	static inline bool IsMaximum(const double val) { return (val > Point::MaximumCompare); }

	inline void Set(double value) { for(int i=0; i<DimensionsCount; i++) D[i] = value; }
	inline void Set(const Point &src) { for(int i=0; i<DimensionsCount; i++) D[i] = src.D[i]; }
	inline void Set(int dimensionIndex, double value) { if (IsValidDimension(dimensionIndex)) D[dimensionIndex] = value; }
	inline void Inc(int dimensionIndex, double value) { if (IsValidDimension(dimensionIndex)) D[dimensionIndex] += value; }
	inline void Add(const Point &src) { for(int i=0; i<DimensionsCount; i++) D[i] += src.D[i]; }

	void operator= (const Point &src) { Set(src); }
	Point& operator+= (const Point &rhs) { Add(rhs); return *this; }
	friend Point operator+ (Point lhs, const Point &rhs) { lhs += rhs; return lhs; }

	inline double X() const { return D[X_Dim]; }
	inline Point& X(double val) { D[X_Dim] = val; return *this; }

	inline double Y() const { return D[Y_Dim]; }
	inline Point& Y(double val) { D[Y_Dim] = val; return *this; }

#if LAYOUT_POINT_DIMENSIONS_COUNT > 2
	inline double Z() const { return D[Z_Dim]; }
	inline Point& Z(double val) { D[Z_Dim] = val; return *this; }
	//and so on..
#endif

public:
	static const double Maximum;
protected:
	static const double MaximumCompare;
	//static const double DimensionsQuantizationFactor; // size = round(size / DimensionsQuantizationFactor) * DimensionsQuantizationFactor;
};

class Span
{
public:
	size_t S[Point::DimensionsCount];
	Span() { for(int i=0; i<Point::DimensionsCount; i++) S[i] = 1; }
};

class Size : public Point
{
public:
	Size() : Point() {}
	Size(double all) : Point(all) {}
	Size(const Point& src) : Point(src) {}
};

class Margins
{
public:
	Point LeftTop; //near zero-point
	Point RightBottom; //near size-point

	void Set(double all) { LeftTop.Set(all); RightBottom.Set(all); }
	void Set(double lt, double rb) { LeftTop.Set(lt); RightBottom.Set(rb); }
	void Set(const Point &all) { LeftTop.Set(all); RightBottom.Set(all); }
	void Set(const Point &lt, const Point &rb) { LeftTop.Set(lt); RightBottom.Set(rb); }

	inline Size GetClientSize(const Size &area)
	{
		Size result;
		for(int i=0; i<Point::DimensionsCount; i++)
		{
			result.D[i] = area.D[i] - LeftTop.D[i] - RightBottom.D[i];
			if (result.D[i] < 0.0) result.D[i] = 0.0;
		}
		return result;
	}
	inline Point GetClientOffset(const Point &offset)
	{
		Point result;
		for(int i=0; i<Point::DimensionsCount; i++)
		{
			result.D[i] = offset.D[i] + LeftTop.D[i];
			if (result.D[i] < 0.0) result.D[i] = 0.0;
		}
		return result;
	}
	inline double GetOuterBounds(int dimensionIndex) const { return LeftTop.D[dimensionIndex] + RightBottom.D[dimensionIndex]; }
	Size GetOuterBounds() const { return LeftTop + RightBottom; }
};

class Area
{
public:
	Area() : m_maxArea(Point::Maximum), m_weight(1.0) {}

	inline const Size& GetArea() const { return m_area; }
	inline const Size& GetMinArea() const { return m_minArea; }
	inline const Size& GetMaxArea() const { return m_maxArea; }
	inline void SetArea(int dimensionIndex, double value)
	{
		if (Point::IsValidDimension(dimensionIndex))
			m_area.D[dimensionIndex] = value;
	}
	inline void SetArea(const Size& size) {	m_area = size; }
	inline const Size& GetWeight() const { return m_weight; }
	inline void SetWeight(int dimensionIndex, double value)
	{
		if (Point::IsValidDimension(dimensionIndex))
			m_weight.D[dimensionIndex] = value;
	}
	inline const Span& GetSpan() const { return m_span; }
	inline void SetSpan(int dimensionIndex, size_t value)
	{
		if (Point::IsValidDimension(dimensionIndex))
			m_span.S[dimensionIndex] = value;
	}
	
	inline bool HasMinimum(int dimensionIndex) const { return (m_minArea.D[dimensionIndex] > 0.0); }
	inline bool HasMaximum(int dimensionIndex) const { return !Point::IsMaximum(m_maxArea.D[dimensionIndex]); }

	inline void ResetMinMax(int dimensionIndex) { m_minArea.Set(dimensionIndex, 0.0); m_maxArea.Set(dimensionIndex, Point::Maximum); }
	inline void SetBiggestMin(int dimensionIndex, const Size& other)
	{
		//select biggest min (for 'not-less-than-min' ops.)
		double val = m_margins.GetOuterBounds(dimensionIndex) + other.D[dimensionIndex];
		if ((val > 0.0) && (m_minArea.D[dimensionIndex] < val))
			m_minArea.D[dimensionIndex] = val;
	}
	inline void SetLowestMax(int dimensionIndex, const Size& other)
	{
		//select lowest max (for 'not-more-than-max' ops.)
		double val = m_margins.GetOuterBounds(dimensionIndex) + other.D[dimensionIndex];
		if ((val < Point::Maximum) && (m_maxArea.D[dimensionIndex] > val))
			m_maxArea.D[dimensionIndex] = val;
	}
	inline void MergeMinMax(int dimensionIndex, const Area& other)
	{
		SetBiggestMin(dimensionIndex, other.GetMinArea());
		SetLowestMax(dimensionIndex, other.GetMaxArea());
	}

	inline const Margins& GetMargins() const { return m_margins; }
	
	//shortcuts (omg)
	inline Margins& MarginsRef() { return m_margins; }
	inline Size& MinRef() { return m_minArea; }
	inline Size& MaxRef() { return m_maxArea; }

protected:
	Size m_area;
	Size m_weight;
	Span m_span;
	Margins m_margins;

	Size m_minArea;
	Size m_maxArea;
};

class Item : public Area
{
public:

#ifdef _DEBUG
	int ItemDebugID;
	Item() : ItemDebugID(0) {}
#endif

};

}

#endif