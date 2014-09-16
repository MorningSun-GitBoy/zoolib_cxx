/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/ChanW_UTF.h"
#include "zoolib/Unicode.h"

#include "zoolib/ZDebug.h"

#include <stdexcept> // for std::range_error

namespace ZooLib {

// =================================================================================================
// MARK: -

static
void spWriteMust(const UTF32* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{
	const size_t countWritten = sWriteFully(iSource, iCountCU, iChanW);
	if (countWritten != iCountCU)
		ChanW_UTF::sThrow_Exhausted();
	}

static
void spWriteMust(const UTF16* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{
	if (iCountCU)
		{
		const size_t cuConsumed = sWriteFully(iSource, iCountCU, iChanW);
		if (cuConsumed != iCountCU)
			{
			// See comments in the UTF-8 variant.
			if (0 != Unicode::sCUToCP(iSource + cuConsumed, iSource + iCountCU))
				ChanW_UTF::sThrow_Exhausted();
			}
		}
	}

static
void spWriteMust(const UTF8* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{
	if (iCountCU)
		{
		const size_t cuConsumed = sWriteFully(iSource, iCountCU, iChanW);
		if (cuConsumed != iCountCU)
			{
			// We weren't able to write the whole string.
			// It may be that the final code units are a valid prefix and thus
			// were not consumed by Imp_WriteUTF8 because it's expecting
			// that the caller will pass the prefix again, this time with enough
			// following code units to make up a complete code point,
			// or with invalid code units which will be consumed and ignored.
			// For string8 (and string16 and string32) we know there's
			// nothing more to come. So if the string contains no more
			// complete valid code points between source + cuConsumed and the
			// end then we can just return. If there are one or more valid
			// code points then the short write must be because we've reached
			// our end, and we throw the end of strim exception.
			if (0 != Unicode::sCUToCP(iSource + cuConsumed, iSource + iCountCU))
				ChanW_UTF::sThrow_Exhausted();
			}
		}
	}

// =================================================================================================
// MARK: -

/** Write the UTF-32 string starting at \a iSource and continuing to \a iSource + \a iCountCU.
Do not write more code units than those required to represent \a iCountCP code points.
Report the number of code units and code points successfully written in the optional output
parameters \a oCountCU and \a oCountCP.
*/
void sWrite(const UTF32* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	size_t actualCU;
	if (iCountCP >= iCountCU)
		{
		// iCountCP is the same or larger than iCountCU, and thus the limit
		// on what can be written is simply iCountCU because a single code unit
		// maps to at most one code point.
		actualCU = iCountCU;
		}
	else
		{
		// We're being asked to write fewer CPs than we have CUs. So we have to
		// map from CPs to CUs.
		actualCU = std::min(iCountCU, Unicode::sCPToCU(iSource, iCountCU, iCountCP, nullptr));
		}

	const size_t cuWritten = sWrite(iSource, actualCU, iChanW);
	if (oCountCU)
		*oCountCU = cuWritten;
	if (oCountCP)
		*oCountCP = Unicode::sCUToCP(iSource, cuWritten);
	}

/** Write the UTF-16 string starting at \a iSource and continuing to \a iSource + \a iCountCU.
Do not write more code units than those required to represent \a iCountCP code points.
Report the number of code units and code points successfully written in the optional output
parameters \a oCountCU and \a oCountCP.
*/
void sWrite(const UTF16* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	size_t actualCU;
	if (iCountCP >= iCountCU)
		{
		// iCountCP is the same or larger than iCountCU, and thus the limit
		// on what can be written is simply iCountCU because a single code unit
		// maps to at most one code point.
		actualCU = iCountCU;
		}
	else
		{
		// We're being asked to write fewer CPs than we have CUs. So we have to
		// map from CPs to CUs.
		actualCU = std::min(iCountCU, Unicode::sCPToCU(iSource, iCountCU, iCountCP, nullptr));
		}

	const size_t cuWritten = sWrite(iSource, actualCU, iChanW);
	if (oCountCU)
		*oCountCU = cuWritten;
	if (oCountCP)
		*oCountCP = Unicode::sCUToCP(iSource, cuWritten);
	}

/** Write the UTF-8 string starting at \a iSource and continuing to \a iSource + \a iCountCU.
Do not write more code units than those required to represent \a iCountCP code points.
Report the number of code units and code points successfully written in the optional output
parameters \a oCountCU and \a oCountCP.
*/
void sWrite(const UTF8* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	size_t actualCU;
	if (iCountCP >= iCountCU)
		{
		// iCountCP is the same or larger than iCountCU, and thus the limit
		// on what can be written is simply iCountCU because a single code unit
		// maps to at most one code point.
		actualCU = iCountCU;
		}
	else
		{
		// We're being asked to write fewer CPs than we have CUs. So we have to
		// map from CPs to CUs.
		actualCU = std::min(iCountCU, Unicode::sCPToCU(iSource, iCountCU, iCountCP, nullptr));
		}

	const size_t cuWritten = sWrite(iSource, actualCU, iChanW);
	if (oCountCU)
		*oCountCU = cuWritten;
	if (oCountCP)
		*oCountCP = Unicode::sCUToCP(iSource, cuWritten);
	}

// ---

void sWrite(const UTF32* iString, const ChanW_UTF& iChanW)
	{
	if (size_t countCU = Unicode::sCountCU(iString))
		spWriteMust(iString, countCU, iChanW);
	}

void sWrite(const UTF16* iString, const ChanW_UTF& iChanW)
	{
	if (size_t countCU = Unicode::sCountCU(iString))
		spWriteMust(iString, countCU, iChanW);
	}

void sWrite(const UTF8* iString, const ChanW_UTF& iChanW)
	{
	if (size_t countCU = Unicode::sCountCU(iString))
		spWriteMust(iString, countCU, iChanW);
	}

// ---

void sWrite(const string32& iString, const ChanW_UTF& iChanW)
	{
	// Some non-conformant string implementations (MS) hate it if you access the buffer underlying
	// an empty string. They don't even return null, they crash or throw an exception.
	if (const size_t countCU = iString.size())
		spWriteMust(iString.data(), countCU, iChanW);
	}

void sWrite(const string16& iString, const ChanW_UTF& iChanW)
	{
	if (const size_t countCU = iString.size())
		spWriteMust(iString.data(), countCU, iChanW);
	}

void sWrite(const string8& iString, const ChanW_UTF& iChanW)
	{
	if (const size_t countCU = iString.size())
		spWriteMust(iString.data(), countCU, iChanW);
	}

// -----

/** Write the zero-terminated UTF-8 string starting at \a iString. Standard printf-style parameter
substitution is applied to the string before writing.
*/
void sWritef(const ChanW_UTF& iChanW,
	const UTF8* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	size_t countCU_Produced, count_CUWritten;
	sWritev(iChanW, &countCU_Produced, &count_CUWritten, iString, args);
	va_end(args);
	if (count_CUWritten != countCU_Produced)
		ChanWBase::sThrow_Exhausted();
	}

/** Write the zero-terminated UTF-8 string starting at \a iString. Standard printf-style parameter
substitution is applied to the string before writing. The number of UTF-8 code units produced
is returned in \a oCount_CUProduced. The number of UTF-8 code units successfully written is
returned in \a oCount_CUWritten.
*/
void sWritef(const ChanW_UTF& iChanW, size_t* oCount_CUProduced, size_t* oCount_CUWritten,
	const UTF8* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	sWritev(iChanW, oCount_CUProduced, oCount_CUWritten, iString, args);
	va_end(args);
	}

void sWritev(const ChanW_UTF& iChanW,
	const UTF8* iString, va_list iArgs)
	{
	size_t countCU_Produced, count_CUWritten;
	sWritev(iChanW, &countCU_Produced, &count_CUWritten, iString, iArgs);
	if (count_CUWritten != countCU_Produced)
		ChanWBase::sThrow_Exhausted();
	}

void sWritev(const ChanW_UTF& iChanW, size_t* oCount_CUProduced, size_t* oCount_CUWritten,
	const UTF8* iString, va_list iArgs)
	{
	string8 buffer(512, ' ');
	for (;;)
		{
		#if ZCONFIG(Compiler, MSVC)
			va_list args = iArgs;
			int count = _vsnprintf(const_cast<char*>(buffer.data()), buffer.size(), iString, args);
		#else
			va_list args;
			#ifdef __va_copy
				__va_copy(args, iArgs);
			#else
				va_copy(args, iArgs);
			#endif
			int count = vsnprintf(const_cast<char*>(buffer.data()), buffer.size(), iString, args);
		#endif

		if (count < 0)
			{
			// Handle -ve result from glibc prior to version 2.1 by growing the string.
			buffer.resize(buffer.size() * 2);
			}
		else if (size_t(count) > buffer.size())
			{
			// Handle C99 standard, where count indicates how much space would have been needed.
			buffer.resize(count);
			}
		else
			{
			// The string fitted, we can now write it out.
			const size_t countWritten = sWriteFully(buffer.data(), count, iChanW);
			if (oCount_CUProduced)
				*oCount_CUProduced = count;
			if (oCount_CUWritten)
				*oCount_CUWritten = countWritten;
			break;
			}
		}
	}

} // namespace ZooLib
