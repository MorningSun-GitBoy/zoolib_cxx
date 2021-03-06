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

#include "zoolib/QueryEngine/Walker_Calc.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark - Walker_Calc

Walker_Calc::Walker_Calc(const ZP<Walker>& iWalker,
	const string8& iColName,
	const ZP<Callable_t>& iCallable)
:	Walker_Unary(iWalker)
,	fColName(iColName)
,	fCallable(iCallable)
	{}

Walker_Calc::~Walker_Calc()
	{}

ZP<Walker> Walker_Calc::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker = fWalker->Prime(iOffsets, fBindings, ioBaseOffset);
	oOffsets.insert(fBindings.begin(), fBindings.end());
	fOutputOffset = ioBaseOffset++;
	oOffsets[fColName] = fOutputOffset;

	if (not fWalker)
		return null;

	return this;
	}

bool Walker_Calc::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();

	if (not fWalker->QReadInc(ioResults))
		return false;

	ioResults[fOutputOffset] = fCallable->Call(PseudoMap(&fBindings, ioResults));

	return true;
	}

} // namespace QueryEngine
} // namespace ZooLib
