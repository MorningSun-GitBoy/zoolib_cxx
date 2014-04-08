/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZooLib_RelationalAlgebra_Transform_ConsolidateRenames_h__
#define __ZooLib_RelationalAlgebra_Transform_ConsolidateRenames_h__
#include "zconfig.h"

#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
// MARK: - RelationalAlgebra::Transform_ConsolidateRenames

class Transform_ConsolidateRenames
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<Expr_Rel>
,	public virtual Visitor_Expr_Rel_Rename
	{
	typedef ZVisitor_Expr_Op_Do_Transform_T<Expr_Rel> inherited;
public:
// From ZVisitor_Expr_OpX_T via ZVisitor_Expr_Op_DoTransform_T
	virtual void Visit_Expr_Op0(const ZRef<ZExpr_Op0_T<Expr_Rel> >& iExpr);
	virtual void Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<Expr_Rel> >& iExpr);
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr);

// From Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);

	Rename fRename;
	};

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Transform_ConsolidateRenames_h__