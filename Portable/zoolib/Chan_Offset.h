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

#ifndef __ZooLib_Chan_Offset_h__
#define __ZooLib_Chan_Offset_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanPos_Offset

template <class Chan_p>
class ChanPos_Offset
:	public ChanFilter<Chan_P>
	{
	typedef ChanFilter<Chan_P> inherited;
public:
	ChanPos_Offset(const Chan_p& iChan, uint64 iOffset)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	ChanPos_Offset(uint64 iOffset, const Chan_p& iChan)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	virtual uint64 Pos()
		{ return sPos(inherited::pGetChan()) - fOffset; }

	virtual void PosSet(uint64 iPos)
		{ sPosSet(inherited::pGetChan(), iPos + fOffset); }

protected:
	const uint64 fOffset;
	};

// =================================================================================================
#pragma mark - ChanSize_Offset

template <class Chan_p>
class ChanSize_Offset
:	public ChanFilter<Chan_P>
	{
	typedef ChanFilter<Chan_P> inherited;
public:
	ChanSize_Offset(const Chan_p& iChan, uint64 iOffset)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	ChanSize_Offset(uint64 iOffset, const Chan_p& iChan)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

// From ChanSize
	virtual uint64 Size()
		{ return sSize(inherited::pGetChan()) - fOffset; }

protected:
	const uint64 fOffset;
	};

// =================================================================================================
#pragma mark - ChanSizeSet_Offset

template <class Chan_p>
class ChanSizeSet_Offset
:	public ChanFilter<Chan_P>
	{
	typedef ChanFilter<Chan_P> inherited;
public:
	ChanSizeSet_Offset(const Chan_p& iChan, uint64 iOffset)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

	ChanSizeSet_Offset(uint64 iOffset, const Chan_p& iChan)
	:	inherited(iChan)
	,	fOffset(iOffset)
		{}

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize)
		{ sSizeSet(inherited::pGetChan(), iSize + fOffset); }

protected:
	const uint64 fOffset;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Offset_h__
