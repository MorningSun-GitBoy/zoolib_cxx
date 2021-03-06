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

#include "zoolib/RelationalAlgebra/Expr_Rel_Difference.h"

#include "zoolib/Compare_Ref.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompare_T

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Difference& iL,
	const RelationalAlgebra::Expr_Rel_Difference& iR)
	{
	if (int compare = sCompare_T(iL.GetOp0(), iR.GetOp0()))
		return compare;

	return sCompare_T(iL.GetOp1(), iR.GetOp1());
	}

ZMACRO_CompareRegistration_T(RelationalAlgebra::Expr_Rel_Difference)

namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Difference

Expr_Rel_Difference::Expr_Rel_Difference(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Rel_Difference::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Difference* theVisitor =
		sDynNonConst<Visitor_Expr_Rel_Difference>(&iVisitor))
		{ this->Accept_Expr_Rel_Difference(*theVisitor); }
	else
		{ inherited::Accept_Expr_Op2(iVisitor); }
	}

ZP<Expr_Rel> Expr_Rel_Difference::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Difference::Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
	{ return new Expr_Rel_Difference(iOp0, iOp1); }

void Expr_Rel_Difference::Accept_Expr_Rel_Difference(Visitor_Expr_Rel_Difference& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Difference(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Difference

void Visitor_Expr_Rel_Difference::Visit_Expr_Rel_Difference(const ZP<Expr_Rel_Difference>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

} // namespace RelationalAlgebra
} // namespace ZooLib
