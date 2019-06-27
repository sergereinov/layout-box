/*
*****************************
Original code by Serge Reinov
*****************************
*/

#pragma once
#include <string>
#include <map>
#include <sstream>

#include <WinGDI.h>


#ifdef UNICODE
//TODO: make it CStringW-like result, inline func etc
#define MAKE_TCHAR(ptr, psz) CA2W _CW_##ptr(psz); const TCHAR *ptr = _CW_##ptr;
#else
#define MAKE_TCHAR(ptr, psz) const TCHAR *ptr = psz;
#endif



/*allow strncpy*/
#pragma warning(disable:4996)

namespace GDI
{
	typedef std::string FontAlias;
	typedef std::map<GDI::FontAlias, HFONT> FontsMap;

	class CFontsCache
	{
	public:
		CFontsCache() {}
		~CFontsCache() { clear(); }

		void clear()
		{
			for(FontsMap::iterator it=fonts.begin();
				it != fonts.end(); it++)
			{
				HFONT h = it->second;
				if (h != NULL)
					DeleteObject(h);
			}
			fonts.clear();
		}

		HFONT GetFont(int dpi, const char *lpszName, int size, 
			bool isBold, bool isItalic, bool isUnderline, bool isStrikeout, 
			int angle10, int charSet)
		{
			std::string alias = BuildFontAlias(dpi, lpszName, size, 
				isBold, isItalic, isUnderline, isStrikeout, angle10, charSet);

			//check cache
			FontsMap::const_iterator it = fonts.find(alias);
			if (it != fonts.end())
				return it->second; //already created
			
			LONG lfHeight = (size*(100*dpi/72))/100;

			//create new font
			LOGFONT lf =
			{ 
				//-MulDiv(size, dpi, 72), //too precise
				-lfHeight,				
				0,
				angle10, angle10,
				(isBold ? FW_BOLD : FW_NORMAL),
				(isItalic ? TRUE : FALSE),
				(isUnderline ? TRUE : FALSE),
				(isStrikeout ? TRUE : FALSE),
				charSet,
#ifndef WINCE
				OUT_TT_PRECIS,
#else
				OUT_DEFAULT_PRECIS,
#endif
				CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, _T("")
			};
			//strncpy(lf.lfFaceName, lpszName, sizeof(lf.lfFaceName));
			{
				MAKE_TCHAR(ptext, lpszName);
				_tcsncpy(lf.lfFaceName, ptext, sizeof(lf.lfFaceName)/sizeof(TCHAR));
			}

			HFONT handle = CreateFontIndirect(&lf);
			if (0 != handle)
				fonts[alias] = handle;
			
			return handle;
		}

	private:
		inline std::string BuildFontAlias(int dpi, const char *lpszName, int size, 
			bool isBold, bool isItalic, bool isUnderline, bool isStrikeout, 
			int angle10, int charSet)
		{
			std::ostringstream alias;
			alias << dpi << ":" <<lpszName << size << (isBold ? "b" : "_") << (isItalic ? "i" : "_")
				<< (isUnderline ? "u" : "_") << (isStrikeout ? "s" : "_")
				<< ":a=" << angle10 << ":cs=" << charSet;
			return alias.str();
		}

		FontsMap fonts;
	};
}
