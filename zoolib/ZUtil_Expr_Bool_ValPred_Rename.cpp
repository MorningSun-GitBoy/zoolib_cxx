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

#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/ZUtil_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ZValPred_Rename.h"
#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"

using std::map;
using std::string;

namespace ZooLib {
namespace Util_Expr_Bool {

// =================================================================================================
#pragma mark -
#pragma mark * DoRename (anonymous)

namespace { // anonymous

class DoRename
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<ZExpr_Bool>
,	public virtual ZVisitor_Expr_Bool_ValPred
	{
public:
	DoRename(const map<string,string>& iRename);

	virtual void Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iExpr);
private:
	const map<string,string>& fRename;
	};

DoRename::DoRename(const map<string,string>& iRename)
:	fRename(iRename)
	{}

void DoRename::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iExpr)
	{ this->pSetResult(new ZExpr_Bool_ValPred(sRenamed(fRename, iExpr->GetValPred()))); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Util_Expr_Bool

ZRef<ZExpr_Bool> sRenamed(const std::map<std::string,std::string>& iRename,
	const ZRef<ZExpr_Bool>& iExpr)
	{ return DoRename(iRename).Do(iExpr); }

} // namespace Util_Expr_Bool
} // namespace ZooLib
