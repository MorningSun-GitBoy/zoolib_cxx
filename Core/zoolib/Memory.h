/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_Memory_h__
#define __ZooLib_Memory_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include <cstring> // For std::memcmp, std::memcpy, std::memmove and std::memset

#if defined(__ANDROID__)
	// On Android we get bzero from strings.h.
	#include <strings.h>
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - memcmp, memcpy, memmove, memset, bzero wrappers.

inline int sMemCompare(const void* iLHS, const void* iRHS, size_t iCount)
	{ return std::memcmp(iLHS, iRHS, iCount); }

int sMemCompare(const void* iL, size_t iCountL, const void* iR, size_t iCountR);

inline void sMemCopy(void* oDest, const void* iSource, size_t iCount)
	{ std::memcpy(oDest, iSource, iCount); }

inline void sMemMove(void* oDest, const void* iSource, size_t iCount)
	{ std::memmove(oDest, iSource, iCount); }

inline void sMemSet(void* oDest, unsigned char iValue, size_t iCount)
	{ std::memset(oDest, iValue, iCount); }

inline void sMemZero(void* oDest, size_t iCount)
	{
	#if ZCONFIG_SPI_Enabled(POSIX)
		// bzero is not always a function, it may be a macro, so we must not namespace-qualify it.
		bzero(oDest, iCount);
	#else
		sMemSet(oDest, 0, iCount);
	#endif
	}

template <class T>
void sMemZero_T(T& oT)
	{ sMemZero(&oT, sizeof(oT)); }

} // namespace ZooLib

#endif // __ZooLib_Memory_h__
