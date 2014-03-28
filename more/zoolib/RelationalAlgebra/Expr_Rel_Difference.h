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

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Difference_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Difference_h__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Difference;

// =================================================================================================
// MARK: - Expr_Rel_Difference

class Expr_Rel_Difference
:	public virtual Expr_Rel
,	public virtual ZExpr_Op2_T<Expr_Rel>
	{
	typedef ZExpr_Op2_T<Expr_Rel> inherited;
public:
	Expr_Rel_Difference(const ZRef<Expr_Rel>& iOp0, const ZRef<Expr_Rel>& iOp1);

// From ZExpr_Op2_T
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(const ZRef<Expr_Rel>& iOp0, const ZRef<Expr_Rel>& iOp1);

// Our protocol
	virtual void Accept_Expr_Rel_Difference(Visitor_Expr_Rel_Difference& iVisitor);
	};

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Difference

class Visitor_Expr_Rel_Difference
:	public virtual ZVisitor_Expr_Op2_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr);
	};

} // namespace RelationalAlgebra

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Difference& iL, const RelationalAlgebra::Expr_Rel_Difference& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_h__
