/*
-----------------------------------------------------------------------------
ICGUtils.h

For [CS482] Interactive Computer Graphics practice class
Computer Graphics and Visualization Lab @ KAIST
-----------------------------------------------------------------------------
*/

#ifndef __ICGUtils_h_
#define __ICGUtils_h_

#include "ICGDef.h"

/** Brought some D3D utility macros here */
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

/** 
 * Make formatted string when you write "%s, %d" stuff. 
 * The parameters of the function which use this macro will be [const TCHAR*, ...] 
 * @param BufferSize : The size of the result string buffer 
 * @param CharCount : It Could be BufferSize-1, or just _TRUNCATE
 */
#define IMPLEMENT_FORMATTED_STRING(_ResultString, _BufferSize, _CharCount, _FormattedString) \
	va_list __Args; \
	va_start(__Args, _FormattedString); \
	_vsnprintf_s(_ResultString, _BufferSize, _CharCount, _FormattedString, __Args); \
	va_end(__Args);

/** For debug logging to output window. 
 * See if Ogre::LogManager::getSingleton().logMessage() can also do this.. */
inline void DebugLogf(const char* FormattedString, ...)
{
	char ResultString[1024];
	IMPLEMENT_FORMATTED_STRING(ResultString, ARRAYSIZE(ResultString), ARRAYSIZE(ResultString)-1, FormattedString);
	strcat_s(ResultString, ARRAYSIZE(ResultString), TEXT("\n"));
	::OutputDebugString(ResultString);
}

#endif // #ifndef __ICGUtils_h_