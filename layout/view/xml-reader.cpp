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

#include "xml-reader.h"

//using tinyxml2 from www.grinninglizard.com written by Lee Thomason, see notice in tinyxml2.h & tinyxml2.cpp
#include <tinyxml/tinyxml2.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace tinyxml2;

namespace layout { namespace view 
{

void setElementXml(Library::Elem &el, const XMLElement *xmlElem)
{
	const char *pElem = xmlElem->Name();
	el.Set(pElem);

	const char *pText = xmlElem->GetText();
	if (pText && pText[0])
		el.Text(pText);

	for(const XMLAttribute *xmlAttr = xmlElem->FirstAttribute();
		xmlAttr != 0; xmlAttr = xmlAttr->Next())
	{
		el.P(xmlAttr->Name(), xmlAttr->Value());
	}

	for(const XMLElement *child = xmlElem->FirstChildElement();
		child != 0; child = child->NextSiblingElement())
	{
		setElementXml(el.NewChild(), child);
	}
}

bool appendXml(Library &lib, const char *xml)
{
	XMLDocument doc;
	XMLError err = doc.Parse(xml);
	if (err != tinyxml2::XML_SUCCESS)
		return false;

	XMLElement *root = doc.RootElement(); //probably better use Find("layouts")
	if (0 != stricmp(root->Name(), "layouts"))
		return true; //not layouts

	const XMLElement *xmlElem = root->FirstChildElement();
	while(xmlElem)
	{
		const XMLAttribute *xmlAttr = xmlElem->FindAttribute("name");
		const char *name = 0;
		if (xmlAttr)
			name = xmlAttr->Value();
		if (name && name[0])
		{
			//it has a name
			Library::Elem& topElem = lib.views[name];
			setElementXml(topElem, xmlElem);
		}

		xmlElem = xmlElem->NextSiblingElement();
	}

	return true;
}

}}
