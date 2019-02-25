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

#ifndef __LAYOUT_OBJECT_H__
#define __LAYOUT_OBJECT_H__

#include <vector>

namespace layout
{

//avoiding virtual destructor for no reason actually
#define LINKED_OBJECT_DELETE_METHOD virtual void DeleteThis() { delete this; }

class LinkedObject
{
public:
	LinkedObject(){}
	LinkedObject(LinkedObject *pOwner)
	{ 
		if (pOwner)
			pOwner->AttachMe(this);  //register in upper object as owned
	};

	~LinkedObject()
	{
		if (!m_OwnedObjects.empty())
		{
			//delete all lower objects
			for(int i=0; i<(int)m_OwnedObjects.size(); i++)
				m_OwnedObjects[i]->DeleteThis();
			
			m_OwnedObjects.clear();
		}
	}

protected:
	virtual void DeleteThis() = 0;	//- must be implemented in the last child (sic)
									//- or change all classes chains to virtual destructors (too sic or not too sic)
									//btw anyway:
									//  we are also need some place to collect our Qt-style new objects pointers
									//  so using LINKED_OBJECT_DELETE_METHOD not such a big price

	int Find(const LinkedObject *pObj)
	{
		for(int i=0; i<(int)m_OwnedObjects.size(); i++)
			if (m_OwnedObjects[i] == pObj)
				return i;
		return -1;
	}
	void AttachMe(LinkedObject *pObj)
	{
		if (Find(pObj) < 0)
			m_OwnedObjects.push_back(pObj);

	}
	void DetachMe(LinkedObject *pObj)
	{
		int index = Find(pObj);
		if (index < 0)
			return;
		m_OwnedObjects.erase(m_OwnedObjects.begin() + index);
	}

private:
	std::vector<LinkedObject *> m_OwnedObjects;
};

}

#endif
