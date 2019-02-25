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

#ifndef __LAYOUT_VIEW_LIBRARY_H__
#define __LAYOUT_VIEW_LIBRARY_H__

#include <vector>
#include <map>
#include <string>

namespace layout { namespace view 
{

//typedef std::map<std::string, std::string> Params;

template<class _KT, class _VT>
class ParamsMap : public std::map<_KT, _VT>
{
public:
	bool find_value(const _KT &key, _VT &value) const
	{
		const_iterator it = find(key);
		if (it == end())
			return false;
		value = it->second; //copy value
		return true;
	}
};
typedef ParamsMap<std::string, std::string> Params;

struct Library
{
	struct Elem
	{
		std::string element;
		std::string text;
		Params params;
		std::vector<Elem> childs;

		Elem() : parent(0) {} //default

		//debug helpers
		Elem *parent;
		Elem(Elem *p) : parent(p) {}
		Elem(const char *el, const char *name = 0) : parent(0) { Set(el,name); }
		Elem& Set(const char *el, const char *name = 0) { element = el; if (name) {params["name"] = name;} return *this; }
		Elem& P(const char *pvar, const char *pval) { params[pvar] = pval; return *this; }
		Elem& Text(const char *tx) { text = tx; return *this; }
		Elem& Child(Elem &el) { childs.push_back(el); childs.rbegin()->parent = this; return *this; } //ugly copy branch
		Elem& NewChild() { childs.push_back(Elem(this)); return *childs.rbegin(); }
		Elem& Parent() { if (parent) return *parent; else return *this; } //ugly parent ref or broken branch
	};

	std::map<std::string,Elem> views;
};

}}

#endif
