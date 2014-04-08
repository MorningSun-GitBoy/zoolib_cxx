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

#ifndef __ZooLib_QueryEngine_Walker_h__
#define __ZooLib_QueryEngine_Walker_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_Any.h"

#include <set>

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
// MARK: - Walker

class Walker : public ZCounted
	{
protected:
	Walker();

public:
	virtual ~Walker();

// Our protocol
	virtual void Rewind() = 0;

	virtual ZRef<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset) = 0;

	virtual bool QReadInc(ZVal_Any* ioResults) = 0;
	};

// =================================================================================================
// MARK: - Walker_Unary

class Walker_Unary : public Walker
	{
protected:
	Walker_Unary(const ZRef<Walker>& iWalker);

public:
	virtual ~Walker_Unary();

// From QueryEngine::Walker
	virtual void Rewind();

protected:
	ZRef<Walker> fWalker;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_h__