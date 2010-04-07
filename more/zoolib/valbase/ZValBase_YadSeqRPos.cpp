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

#include "zoolib/ZYad_Any.h"
#include "zoolib/valbase/ZValBase_YadSeqRPos.h"
#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase_YadSeqRPos {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Concrete

ExprRep_Concrete::ExprRep_Concrete(ZRef<ZYadSeqRPos> iYadSeqRPos)
:	fYadSeqRPos(iYadSeqRPos)
	{}

ZRelHead ExprRep_Concrete::GetRelHead()
	{ return ZRelHead(true); }

ZRef<ZYadSeqRPos> ExprRep_Concrete::GetYadSeqRPos()
	{ return fYadSeqRPos; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Concrete_MakeIterator

bool Visitor_ExprRep_Concrete_MakeIterator::Visit_Concrete(ZRef<ZQL::ExprRep_Concrete> iRep)
	{
	if (ZRef<ExprRep_Concrete> theRep = iRep.DynamicCast<ExprRep_Concrete>())
		{
		fIterator = new Iterator(theRep->GetYadSeqRPos());
		return true;
		}
	return Visitor_ExprRep_Concrete::Visit_Concrete(iRep);
	}

bool Visitor_ExprRep_Concrete_MakeIterator::Visit_Restrict(ZRef<ZQL::ExprRep_Restrict> iRep)
	{
	// Could specialize here if we discover that theIterator is ours.
	if (ZRef<ZQE::Iterator> theIterator = this->MakeIterator(iRep->GetExprRep()))
		fIterator = new ZQE::Iterator_Any_Restrict(iRep->GetValCondition(), theIterator);

	return true;	
	}

bool Visitor_ExprRep_Concrete_MakeIterator::Visit_Select(ZRef<ZQL::ExprRep_Select> iRep)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->MakeIterator(iRep->GetExprRep_Relation()))
		fIterator = new ZQE::Iterator_Any_Select(iRep->GetExprRep_Logic(), theIterator);

	return true;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

Iterator::Iterator(const ZRef<ZYadSeqRPos>& iYadSeqRPos)
:	fYadSeqRPos_Model(iYadSeqRPos)
,	fYadSeqRPos(fYadSeqRPos_Model->Clone())
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Result> Iterator::ReadInc()
	{
	if (ZRef<ZYadR> theYadR = fYadSeqRPos->ReadInc())
		return new ZQE::Result_Any(sFromYadR(ZVal_Any(), theYadR));
	return ZRef<ZQE::Result>();
	}

void Iterator::Rewind()
	{ fYadSeqRPos = fYadSeqRPos_Model->Clone(); }

} // namespace ZValBase_YadSeqRPos
NAMESPACE_ZOOLIB_END
