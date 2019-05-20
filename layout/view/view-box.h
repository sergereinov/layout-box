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

#ifndef __LAYOUT_VIEW_BOX_H__
#define __LAYOUT_VIEW_BOX_H__

#include <string>
#include <map>
#include <vector>

#include "library.h"
#include "../layout-box.h"


namespace layout { namespace view 
{

class ViewBox : public StackedBox
{
	LINKED_OBJECT_DELETE_METHOD
public:
	ViewBox() : m_viewHost(0L), m_viewController(0L) {}
	ViewBox(LinkedObject *pOwner) : StackedBox(pOwner), m_viewHost(0L), m_viewController(0L) {}

	virtual BoxItem* CreateItem(
		const std::string &element, 
		const std::string &text, 
		const Params &params)
	{ return 0; }

	void Implement(Library &lib, const std::string &viewName, void *viewHost, void *viewController);

protected:
	void *m_viewHost;
	void *m_viewController;

	BoxItem* ApplyDefaultParams(BoxItem *item, const Params &params);
	void MergeParams(Params &target, const Params &params, bool inheritableOnly = false);

	BoxItem* ImplementView(
		const Params &mergedParentsParams, 
		Library::Elem &elem, 
		Library &lib, std::deque<std::string> &nestedViews);

	static int ParseSequence(std::vector<int> &result, const std::string &text);
};


}}

#endif
