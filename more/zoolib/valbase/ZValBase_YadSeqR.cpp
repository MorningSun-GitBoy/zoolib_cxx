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

#include "zoolib/ZThread.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/valbase/ZValBase.h"
#include "zoolib/valbase/ZValBase_YadSeqR.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase_YadSeqR {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Concrete declaration

class ExprRep_Concrete : public ZValBase::ExprRep_Concrete
	{
public:
	ExprRep_Concrete(ZRef<ZYadSeqR> iYadSeqR);

// From ZValBase::ExprRep_Concrete
	virtual ZRef<ZQE::Iterator> MakeIterator();

// Our protocol
	ZRef<ZQE::Result> ReadInc(size_t& ioIndex);

private:
	ZMtx fMtx;
	ZRef<ZYadSeqR> fYadSeqR;
	ZSeq_Any fSeq;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(ZRef<ExprRep_Concrete> iExprRep, size_t iIndex);

	virtual ~Iterator();
	
	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

protected:
	ZRef<ExprRep_Concrete> fExprRep;
	size_t fIndex;
	};

Iterator::Iterator(ZRef<ExprRep_Concrete> iExprRep, size_t iIndex)
:	fExprRep(iExprRep)
,	fIndex(0)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fExprRep, fIndex); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{ return fExprRep->ReadInc(fIndex); }

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Concrete definition

ExprRep_Concrete::ExprRep_Concrete(ZRef<ZYadSeqR> iYadSeqR)
:	fYadSeqR(iYadSeqR)
	{}

ZRef<ZQE::Iterator> ExprRep_Concrete::MakeIterator()
	{ return new Iterator(this, 0); }

ZRef<ZQE::Result> ExprRep_Concrete::ReadInc(size_t& ioIndex)
	{
	ZAcqMtx acq(fMtx);

	if (ioIndex < fSeq.Count())
		return new ZQE::Result_Any(fSeq.Get(ioIndex++));

	if (ZRef<ZYadR> theYadR = fYadSeqR->ReadInc())
		{
		const ZVal_Any theVal = sFromYadR(ZVal_Any(), theYadR);
		++ioIndex;
		if (this->GetRefCount() > 1)
			{
			fSeq.Append(theVal);
			ZAssert(ioIndex == fSeq.Count());
			}
		return new ZQE::Result_Any(theVal);
		}

	return ZRef<ZQE::Result>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_YadSeqR pseudo constructors

ZQL::Expr_Concrete sConcrete(ZRef<ZYadSeqR> iYadSeqR)
	{
	// Could do a dynamic cast on iYadSeqR to see if it's really a ZYadSeqRPos,
	// in which case returning a ZValBase_YadSeqRPos::Iterator would be a win.
	return ZQL::Expr_Concrete(new ExprRep_Concrete(iYadSeqR));
	}

} // namespace ZValBase_YadSeqR
NAMESPACE_ZOOLIB_END
