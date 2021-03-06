/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZooLib_ChanW_Bin_HexStrim_h__
#define __ZooLib_ChanW_Bin_HexStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanW_UTF_InsertSeparator.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_Bin_HexStrim_Real

class ChanW_Bin_HexStrim_Real
:	public ChanW_Bin
	{
public:
	ChanW_Bin_HexStrim_Real(bool iLowercaseHex, const ChanW_UTF& iChanW_UTF);

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	const ChanW_UTF& fChanW_UTF;
	const char* fHexDigits;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_HexStrim

class ChanW_Bin_HexStrim
:	public ChanW_Bin
	{
public:
	ChanW_Bin_HexStrim(const std::string& iByteSeparator,
		const std::string& iChunkSeparator, size_t iChunkSize,
		bool iLowercaseHex,
		const ChanW_UTF& iChanW_UTF);

	ChanW_Bin_HexStrim(const std::string& iByteSeparator,
		const std::string& iChunkSeparator, size_t iChunkSize,
		const ChanW_UTF& iStrimSink);

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	ChanW_UTF_InsertSeparator fChanW_UTF;
	ChanW_Bin_HexStrim_Real fChanW_Bin;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_HexStrim_h__
