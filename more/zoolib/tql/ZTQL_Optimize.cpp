/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZExpr_Logic_ValPred.h"

#include "zoolib/ZValPredCompound.h"

#include "zoolib/tql/ZTQL_Optimize.h"
#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZVisitor_Expr_Op_DoTransform_T.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"

namespace ZooLib {
using namespace ZRA;

// Turns a Select into a tree of Restrict and Union.

// =================================================================================================
#pragma mark -
#pragma mark * Local stuff

namespace { // anonymous

class Gather
:	public virtual ZVisitor_Do_T<ZValPredCompound>
,	public virtual ZVisitor_Expr_Logic_True
,	public virtual ZVisitor_Expr_Logic_False
,	public virtual ZVisitor_Expr_Logic_Not
,	public virtual ZVisitor_Expr_Logic_And
,	public virtual ZVisitor_Expr_Logic_Or
,	public virtual ZVisitor_Expr_Logic_ValPred
	{
public:
//	From ZVisitor_Expr_Logic_DoTransform
	virtual void Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iExpr);
	virtual void Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iExpr);
	virtual void Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr);
	virtual void Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iExpr);
	virtual void Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr);

// From ZVisitor_Expr_Logic_ValPred
	virtual void Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr);
	};

void Gather::Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iExpr)
	{ this->pSetResult(ZValPredCompound::sTrue()); }

void Gather::Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iExpr)
	{ this->pSetResult(ZValPredCompound::sFalse()); }

void Gather::Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iExpr)
	{ this->pSetResult(this->Do(iExpr->GetOp0()) & this->Do(iExpr->GetOp1())); }

void Gather::Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr)
	{ this->pSetResult(this->Do(iExpr->GetOp0()) | this->Do(iExpr->GetOp1())); }

void Gather::Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr)
	{ this->pSetResult(iExpr->GetValPred()); }

ZRef<Expr_Rel> spConvertSelect(ZRef<Expr_Rel> iRelation, ZRef<ZExpr_Logic> iLogical)
	{
	if (!iRelation)
		return ZRef<Expr_Rel>();

	const ZValPredCompound resultLogical = Gather().Do(iLogical);
	const ZValPredCompound::SectUnion& theSU = resultLogical.fSectUnion;

	ZRef<Expr_Rel> resultRelation;
	for (ZValPredCompound::SectUnion::const_iterator iterUnion = theSU.begin();
		iterUnion != theSU.end(); ++iterUnion)
		{
		ZRef<Expr_Rel> current = iRelation;
		for (ZValPredCompound::Sect::const_iterator iterSect = iterUnion->begin();
			iterSect != iterUnion->end(); ++iterSect)
			{
			current = new Expr_Rel_Restrict(current, *iterSect);
			}

		if (resultRelation)
			resultRelation = new Expr_Rel_Union(current, resultRelation);
		else
			resultRelation = current;
		}
	return resultRelation;
	}

class Optimize
:	public virtual ZVisitor_Expr_Op_DoTransform_T<Expr_Rel>
,	public virtual Visitor_Expr_Rel_Select
	{
public:
// From Visitor_Expr_Rel_Select
	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);
	};

void Optimize::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	ZRef<Expr_Rel> newExpr = this->Do(iExpr->GetOp0());

	this->pSetResult(spConvertSelect(newExpr, iExpr->GetExpr_Logic()));
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::sOptimize

namespace ZRA {

ZRef<Expr_Rel> sOptimize(ZRef<Expr_Rel> iExpr)
	{ return Optimize().Do(iExpr); }

} // namespace ZRA
} // namespace ZooLib
