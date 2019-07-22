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

#include "view-box.h"

namespace layout { namespace view 
{

//static
int ViewBox::ParseSequence(std::vector<int> &result, const std::string &text)
{
	result.clear();

	//seq.: "10" or ",6,,6" or "75,23" or ",2" or "160,"
	size_t comma = 0, pos = 0;
	for(;;)
	{
		comma = text.find(',', pos);
		size_t len = (comma != std::string::npos) ? (comma - pos) : (text.length() - pos);
		result.push_back(atoi(text.substr(pos, len).c_str()));
		if (comma == std::string::npos)
			break;
		pos = comma + 1;
	}
	return result.size();
}

BoxItem* ViewBox::ApplyDefaultParams(BoxItem *item, const Params &params)
{
	std::vector<int> sequence;
	Params::const_iterator it;

	it = params.find("margins");
	if (it != params.end())
	{
		//margins: "10" or ",6,,6" etc
		ParseSequence(sequence, it->second);
		if (sequence.size() == 1)
			item->MarginsRef().Set(sequence[0]); //set all
		else
		{
			if (sequence.size() > 0) item->MarginsRef().LeftTop.X(sequence[0]);
			if (sequence.size() > 1) item->MarginsRef().LeftTop.Y(sequence[1]);
			if (sequence.size() > 2) item->MarginsRef().RightBottom.X(sequence[2]);
			if (sequence.size() > 3) item->MarginsRef().RightBottom.Y(sequence[3]);
		}
	}
	it = params.find("size");
	if (it != params.end())
	{
		//size: "75,23" or ",2" etc
		ParseSequence(sequence, it->second);
		if ((sequence.size() > 0) && (sequence[0] > 0)) { item->MinRef().X(sequence[0]); item->MaxRef().X(sequence[0]); }
		if ((sequence.size() > 1) && (sequence[1] > 0)) { item->MinRef().Y(sequence[1]); item->MaxRef().Y(sequence[1]); }
	}
	it = params.find("min");
	if (it != params.end())
	{
		//min: "30,40" or "160," etc
		ParseSequence(sequence, it->second);
		if ((sequence.size() > 0) && (sequence[0] >= 0)) { item->MinRef().X(sequence[0]); }
		if ((sequence.size() > 1) && (sequence[1] >= 0)) { item->MinRef().Y(sequence[1]); }
	}
	it = params.find("max");
	if (it != params.end())
	{
		//max: "60,40" or ",50" etc
		ParseSequence(sequence, it->second);
		if ((sequence.size() > 0) && (sequence[0] > 0)) { item->MaxRef().X(sequence[0]); }
		if ((sequence.size() > 1) && (sequence[1] > 0)) { item->MaxRef().Y(sequence[1]); }
	}

	return item;
}

void ViewBox::Implement(Library &lib, const std::string &viewName, void *viewHost)
{
	m_viewHost = viewHost;

	std::deque<std::string> nestedViews;
	std::string topViewName = viewName;

	for(;;)
	{
		Library::Elem& elem = lib.views[topViewName];
		Params::iterator it = elem.params.find("parent");
		if ((it == elem.params.end()) || (it->second.empty()))
			break;

		nestedViews.push_back(topViewName);
		topViewName = it->second;
	}

	BoxItem* view = ImplementView(Params(), lib.views[topViewName], lib, nestedViews);
	Add(view);
}

void ViewBox::MergeParams(Params &target, const Params &params, bool inheritableOnly)
{
	for(Params::const_iterator it = params.begin();
		it != params.end(); it++)
	{
		if (!inheritableOnly)
			target[it->first] = it->second; //copy all
		else
		{
			//only inheritable
			if (0 == it->first.compare(0, 4, "font"))
				target[it->first] = it->second; 
		}
	}
}

BoxItem* ViewBox::ImplementView(const Params &inheritedParams, Library::Elem &elem,
								Library &lib, std::deque<std::string> &nestedViews)
{
	Params viewParams;
	MergeParams(viewParams, inheritedParams, true);
	MergeParams(viewParams, elem.params);

	//impl. elem-box & attach children
	if (elem.element == "hbox")
	{
		HBox *hbox = (HBox *)ApplyDefaultParams(new HBox(this), viewParams);
		for(size_t i=0; i<elem.children.size(); i++)
			hbox->Add(ImplementView(viewParams, elem.children[i], lib, nestedViews));
		return hbox;
	}
	if (elem.element == "vbox")
	{
		VBox *vbox = (VBox *)ApplyDefaultParams(new VBox(this), viewParams);
		for(size_t i=0; i<elem.children.size(); i++)
			vbox->Add(ImplementView(viewParams, elem.children[i], lib, nestedViews));
		return vbox;
	}
	if (elem.element == "sbox")
	{
		StackedBox *sbox = (StackedBox *)ApplyDefaultParams(new StackedBox(this), viewParams);
		for(size_t i=0; i<elem.children.size(); i++)
			sbox->Add(ImplementView(viewParams, elem.children[i], lib, nestedViews));
		return sbox;
	}
	if (elem.element == "gbox")
	{
		GridBox *gbox = (GridBox *)ApplyDefaultParams(new GridBox(this), viewParams);
		for(size_t i=0; i<elem.children.size(); i++)
		{
			Library::Elem &child = elem.children[i];

			//get grid-pos & grid-span
			std::vector<int> seqPos, seqSpan;
			Params::const_iterator it;
			if (child.params.end() != (it=child.params.find("gpos")))
				ParseSequence(seqPos, it->second);
			if (child.params.end() != (it=child.params.find("gspan")))
				ParseSequence(seqSpan, it->second);
			size_t row = ((seqPos.size() > 0) && (seqPos[0] >= 0)) ? (size_t)seqPos[0] : 0;
			size_t col = ((seqPos.size() > 1) && (seqPos[1] >= 0)) ? (size_t)seqPos[1] : 0;
			size_t rowSpan = ((seqSpan.size() > 0) && (seqSpan[0] > 0)) ? (size_t)seqSpan[0] : 1;
			size_t colSpan = ((seqSpan.size() > 1) && (seqSpan[1] > 0)) ? (size_t)seqSpan[1] : 1;
			
			//set child into grid
			gbox->Set(
				ImplementView(viewParams, child, lib, nestedViews),
				row, col, rowSpan, colSpan);
		}
		return gbox;
	}

	if (elem.element == "child")
	{
		//embedding child view with viewParams inheritance
		if (!nestedViews.empty())
		{
			std::string childName = *nestedViews.rbegin();
			nestedViews.pop_back();
			BoxItem *child = ImplementView(viewParams, lib.views[childName], lib, nestedViews);
			return child;
		}
	}

	//not a container OR unknown container

	if (elem.element != "space") //is it not a space-item?
	{
		//try create virtual item
		BoxItem *item = CreateItem(elem.element, elem.text, viewParams);
		if (item)
			return item;
	}

	//create (empty, default or as unknown) space-item
	BoxItem *space = ApplyDefaultParams(new BoxItem(this), viewParams);
	return space;
}


}}
