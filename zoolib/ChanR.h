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

#ifndef __ZooLib_ChanR_h__
#define __ZooLib_ChanR_h__ 1
#include "zconfig.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZStdInt.h" // For size_t, uint64

namespace ZooLib {

// =================================================================================================
// MARK: - ChanR

template <class Val>
class ChanR
	{
public:
	virtual size_t Read(Val* iDest, size_t iCount)
		{ return 0; }

	virtual uint64 Skip(uint64 iCount)
		{
		const uint64 kBufSize = 4096 / sizeof(Val);
		Val scratch[kBufSize];
		return this->Read(&scratch, std::min(iCount, kBufSize));
		}

	virtual uint64 Readable()
		{ return 0; }

// For a golang-style select mechanism we'll need an API whereby we can efficiently
// wait for readability on multiple channels at once.

//	virtual bool WaitReadable(double iTimeout) // ??
//		{
//		ZThread::sSleep(iTimeout);
//		return false;
//		}
	};

// =================================================================================================
// MARK: -

template <class Val>
size_t sRead(const ChanR<Val>& iChanR, Val* iDest, size_t iCount)
	{ return const_cast<ChanR<Val>&>(iChanR).Read(iDest, iCount); }

template <class Val>
uint64 sSkip(const ChanR<Val>& iChanR, uint64 iCount)
	{ return const_cast<ChanR<Val>&>(iChanR).Skip(iCount); }

template <class Val>
uint64 sReadable(const ChanR<Val>& iChanR)
	{ return const_cast<ChanR<Val>&>(iChanR).Readable(); }

// =================================================================================================
// MARK: -

template <class Val>
ZQ<Val> sQRead(const ChanR<Val>& iChanR)
	{
	Val buf;
	if (not sRead(iChanR, &buf, 1))
		return null;
	return buf;
	}

template <class Val>
size_t sReadFully(const ChanR<Val>& iChanR, Val* iDest, size_t iCount)
	{
	Val* localDest = iDest;
	while (iCount)
		{
		if (const size_t countRead = sRead(iChanR, localDest, iCount))
			{
			iCount -= countRead;
			localDest += countRead;
			}
		else
			{ break; }
		}
	return localDest - iDest;
	}

template <class Val>
uint64 sSkipFully(const ChanR<Val>& iChanR, uint64 iCount)
	{
	uint64 countRemaining = iCount;
	while (countRemaining)
		{
		if (const size_t countSkipped = sSkip(iChanR, countRemaining))
			{ countRemaining -= countSkipped; }
		else
			{ break; }
		}
	return iCount - countRemaining;
	}

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__