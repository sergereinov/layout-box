/*
*****************************
Original code by Serge Reinov
*****************************
*/

#pragma once

#include <string>

class CViewController
{
public:
	virtual void BindViewModel(const std::string &element, const std::string &name, UINT id, CWnd *pCtrl) = 0;
};
