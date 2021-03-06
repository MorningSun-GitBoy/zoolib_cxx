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

#include "zoolib/Stringf.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/Log.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_ToStrim.h"
#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"
#include "zoolib/ZValPred_Any.h"

#include "zoolib/RelationalAlgebra/AsSQL.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"
#include "zoolib/RelationalAlgebra/Util_Rel_Operators.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {

using std::map;
using namespace Util_STL;

// =================================================================================================
#pragma mark - Analysis (anonymous)

namespace { // anonymous

struct Analysis
	{
	Map_ZZ fConstValues;
	RelHead fRelHead_Physical;
	Rename fRename;
	Rename fRename_Inverse;
	ZP<ZExpr_Bool> fCondition;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - Analyzer (anonymous)

namespace { // anonymous

class Analyzer
:	public virtual ZVisitor_Do_T<Analysis>
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Dee
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
	{
public:
	Analyzer(const map<string8,RelHead>& iTables);

	virtual void Visit(const ZP<ZVisitee>& iRep);

	virtual void Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr);

	const map<string8,RelHead>& fTables;
	map<string8,int> fTablesUsed;
	};

Analyzer::Analyzer(const map<string8,RelHead>& iTables)
:	fTables(iTables)
	{}

void Analyzer::Visit(const ZP<ZVisitee>& iRep)
	{ ZUnimplemented(); }

void Analyzer::Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr)
	{
	// Identify the table.
	RelHead theRH_Required;
	RelHead theRH_Optional;
	sRelHeads(iExpr->GetConcreteHead(), theRH_Required, theRH_Optional);

	bool allOK = true;
	Analysis resultAnalysis;
	resultAnalysis.fCondition = sTrue();
//##	ZAssertCompile(false); // NDY
	ZQ<map<string8,RelHead>::const_iterator> found;
	RelHead theDefault;
	foreachi (iter, fTables)
		{
		const RelHead theRH_Table = sPrefixInserted(iter->first + "_", iter->second);
		const RelHead theRH_Overlap = theRH_Table & theRH_Required;
		if (theRH_Overlap.size() >= theRH_Required.size())
			{
			// Required columns are all present in the table.
			const RelHead theRH_WO_Required_Or_Optional =
				(theRH_Table - theRH_Required) - theRH_Optional;

			if (theRH_WO_Required_Or_Optional.empty())
				{
				// All columns in the table are in required or optional.
				const RelHead theRH_Extra = theRH_Optional - theRH_Table;

				const string8 realTableName = found.Get()->first;
				const string8 realTableNameUnderscore = realTableName + "_";
				const int numericSuffix = fTablesUsed[realTableName]++;
				const string8 usedTableName = realTableName + sStringf("%d", numericSuffix);
				const string8 usedTableNameDot = usedTableName + ".";

				Analysis theAnalysis;
				theAnalysis.fCondition = sTrue();
				foreachi (iter, theRH_Table)
					{
					const string8 attrName = *iter;
					const string8 fieldName = sPrefixErased(realTableNameUnderscore, attrName);
					const string8 physicalFieldName = usedTableNameDot + fieldName;
					theAnalysis.fRelHead_Physical |= physicalFieldName;
					sInsertMust(theAnalysis.fRename, attrName, physicalFieldName);
					sInsertMust(theAnalysis.fRename_Inverse, physicalFieldName, attrName);
					}

//##				uuuuurrrrrggggghhhhh ... with optional and required columns it's now
//##				possible for more than one table to satisfy the concrete, so we'll need
//##				to construct a term *here*, and union it with others.

				}


			if ((sPrefixInserted(iter->first + "_", iter->second) & theRH_Table).size() == theRH_Table.size())
				{
				found = iter;
				break;
				}
			}
		}
	if (not found)
		throw std::runtime_error("Couldn't find table");

//##SELECT a, b, c from (a0, NULL, something) Union (a1) where

	this->pSetResult(resultAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr)
	{
	Analysis theAnalysis;
	theAnalysis.fCondition = sTrue();
	theAnalysis.fConstValues.Set(iExpr->GetColName(), iExpr->GetVal());
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr)
	{
	Analysis theAnalysis;
	theAnalysis.fCondition = sTrue();
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr)
	{
	Analysis analysis0 = this->Do(iExpr->GetOp0());
	const Analysis analysis1 = this->Do(iExpr->GetOp1());

	for (Map_ZZ::Index_t i = analysis1.fConstValues.Begin();
		i != analysis1.fConstValues.End(); ++i)
		{
		analysis0.fConstValues.Set(analysis1.fConstValues.NameOf(i), analysis1.fConstValues.Get(i));
		}
	analysis0.fRelHead_Physical |= analysis1.fRelHead_Physical;
	analysis0.fRename.insert(analysis1.fRename.begin(), analysis1.fRename.end());
	analysis0.fRename_Inverse.insert(
		analysis1.fRename_Inverse.begin(), analysis1.fRename_Inverse.end());
	analysis0.fCondition &= analysis1.fCondition;

	this->pSetResult(analysis0);
	}

void Analyzer::Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr)
	{
	Analysis theAnalysis = this->Do(iExpr->GetOp0());
	const RelHead& theRH = iExpr->GetProjectRelHead();
	RelHead newRelHead;
	foreachi (ii, theAnalysis.fRelHead_Physical)
		{
		const string8& theString1 = *ii;
		const string8 theString2 = sGetMust(theAnalysis.fRename_Inverse, theString1);
		if (sContains(theRH, theString2))
			newRelHead.insert(theString1);
		}
	theAnalysis.fRelHead_Physical.swap(newRelHead);
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr)
	{
	Analysis theAnalysis = this->Do(iExpr->GetOp0());
	theAnalysis.fCondition &= Util_Expr_Bool::sRenamed(theAnalysis.fRename, iExpr->GetExpr_Bool());
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr)
	{
	Analysis theAnalysis = this->Do(iExpr->GetOp0());
	const ColName& oldName = iExpr->GetOld();
	const ColName& newName = iExpr->GetNew();
	if (ZQ<ColName> theQ = sQGetErase(theAnalysis.fRename, oldName))
		{
		const ColName orgName = *theQ;
		const ColName orgNameInverse = sGetEraseMust(theAnalysis.fRename_Inverse, orgName);
		ZAssert(orgNameInverse == oldName);
		sInsertMust(theAnalysis.fRename, newName, orgName);
		sInsertMust(theAnalysis.fRename_Inverse, orgName, newName);
		}
	else
		{
		Map_ZZ::Index_t theIndex = theAnalysis.fConstValues.IndexOf(oldName);
		ZAssert(theIndex != theAnalysis.fConstValues.End());
		const Val_ZZ theVal = theAnalysis.fConstValues.Get(theIndex);
		theAnalysis.fConstValues.Erase(theIndex);
		theAnalysis.fConstValues.Set(newName, theVal);
		}

	this->pSetResult(theAnalysis);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - ToStrim_SQL (anonymous)

namespace { // anonymous

class ToStrim_SQL
:	public virtual ZVisitor_ToStrim
,	public virtual ZVisitor_Expr_Bool_True
,	public virtual ZVisitor_Expr_Bool_False
,	public virtual ZVisitor_Expr_Bool_Not
,	public virtual ZVisitor_Expr_Bool_And
,	public virtual ZVisitor_Expr_Bool_Or
,	public virtual ZVisitor_Expr_Bool_ValPred
	{
public:
	virtual void Visit_Expr_Bool_True(const ZP<ZExpr_Bool_True>& iRep);
	virtual void Visit_Expr_Bool_False(const ZP<ZExpr_Bool_False>& iRep);
	virtual void Visit_Expr_Bool_Not(const ZP<ZExpr_Bool_Not>& iRep);
	virtual void Visit_Expr_Bool_And(const ZP<ZExpr_Bool_And>& iRep);
	virtual void Visit_Expr_Bool_Or(const ZP<ZExpr_Bool_Or>& iRep);
	virtual void Visit_Expr_Bool_ValPred(const ZP<ZExpr_Bool_ValPred>& iRep);
	};

void ToStrim_SQL::Visit_Expr_Bool_True(const ZP<ZExpr_Bool_True>& iRep)
	{ pStrimW() << "1"; }

void ToStrim_SQL::Visit_Expr_Bool_False(const ZP<ZExpr_Bool_False>& iRep)
	{ pStrimW() << "0"; }

void ToStrim_SQL::Visit_Expr_Bool_Not(const ZP<ZExpr_Bool_Not>& iRep)
	{
	pStrimW() << " NOT (";
	this->pToStrim(iRep->GetOp0());
	pStrimW() << ")";
	}

void ToStrim_SQL::Visit_Expr_Bool_And(const ZP<ZExpr_Bool_And>& iRep)
	{
	ZP<ZExpr_Bool> theFalse = sFalse();
	ZP<ZExpr_Bool> theTrue = sTrue();

	ZP<ZExpr_Bool> theOp0 = iRep->GetOp0();
	ZP<ZExpr_Bool> theOp1 = iRep->GetOp1();
	if (theOp0 == theFalse || theOp1 == theFalse)
		{
		this->pToStrim(theFalse);
		}
	else if (theOp0 == theTrue)
		{
		this->pToStrim(theOp1);
		}
	else if (theOp1 == theTrue)
		{
		this->pToStrim(theOp0);
		}
	else
		{
		pStrimW() << "(";
		this->pToStrim(iRep->GetOp0());
		pStrimW() << " AND ";
		this->pToStrim(iRep->GetOp1());
		pStrimW() << ")";
		}
	}

void ToStrim_SQL::Visit_Expr_Bool_Or(const ZP<ZExpr_Bool_Or>& iRep)
	{
	ZP<ZExpr_Bool> theFalse = sFalse();
	ZP<ZExpr_Bool> theTrue = sTrue();

	ZP<ZExpr_Bool> theOp0 = iRep->GetOp0();
	ZP<ZExpr_Bool> theOp1 = iRep->GetOp1();
	if (theOp0 == theTrue || theOp1 == theTrue)
		{
		this->pToStrim(theTrue);
		}
	else if (theOp0 == theFalse)
		{
		this->pToStrim(theOp1);
		}
	else if (theOp1 == theFalse)
		{
		this->pToStrim(theOp0);
		}
	else
		{
		pStrimW() << "(";
		this->pToStrim(iRep->GetOp0());
		pStrimW() << " OR ";
		this->pToStrim(iRep->GetOp1());
		pStrimW() << ")";
		}
	}

static void spToStrim_SimpleValue(const ChanW_UTF& s, const Any& iAny)
	{
	if (false)
		{}
	else if (iAny.IsNull())
		{
		s << "null";
		}
	else if (const string8* theValue = iAny.PGet<string8>())
		{
		ZStrimW_Escaped::Options theOptions;
		theOptions.fQuoteQuotes = true;
		theOptions.fEscapeHighUnicode = false;
		s << "'";
		ZStrimW_Escaped(theOptions, s) << *theValue;
		s << "'";
		}
	else if (const bool* theValue = iAny.PGet<bool>())
		{
		if (*theValue)
			s << "1";
		else
			s << "0";
		}
	else if (ZQ<int64> theQ = sQCoerceInt(iAny))
		{
		sEWritef(s, "%lld", *theQ);
		}
	else if (const float* asFloat = iAny.PGet<float>())
		{
		Util_Chan::sWriteExact(*asFloat, s);
		}
	else if (const double* asDouble = iAny.PGet<double>())
		{
		Util_Chan::sWriteExact(*asDouble, s);
		}
	else if (const ZTime* asTime = iAny.PGet<ZTime>())
		{
		Util_Chan::sWriteExact(asTime->fVal, s);
		}
	else
		{
		s << "null /*!!Unhandled: " << iAny.Type().name() << "!!*/";
		}
	}

static void spWrite_PropName(const string8& iName, const ChanW_UTF& s)
	{ s << iName; }

static void spToStrim(const ZP<ZValComparand>& iComparand, const ChanW_UTF& s)
	{
	if (not iComparand)
		{
		s << "/*Null Comparand*/";
		}
	else if (ZP<ZValComparand_Name> asName = iComparand.DynamicCast<ZValComparand_Name>())
		{
		spWrite_PropName(asName->GetName(), s);
		}
	else if (ZP<ZValComparand_Const_Any> asConst =
		iComparand.DynamicCast<ZValComparand_Const_Any>())
		{
		spToStrim_SimpleValue(s, asConst->GetVal());
		}
	else
		{
		s << "/*Unknown Comparand*/";
		}
	}

void spToStrim(const ZValPred& iValPred, const ChanW_UTF& s)
	{
	if (ZP<ZValComparator_Simple> asSimple =
		iValPred.GetComparator().DynamicCast<ZValComparator_Simple>())
		{
		spToStrim(iValPred.GetLHS(), s);
		switch (asSimple->GetEComparator())
			{
			case ZValComparator_Simple::eLT:
				{
				s << " < ";
				break;
				}
			case ZValComparator_Simple::eLE:
				{
				s << " <= ";
				break;
				}
			case ZValComparator_Simple::eEQ:
				{
				s << " = ";
				break;
				}
			case ZValComparator_Simple::eNE:
				{
				s << " != ";
				break;
				}
			case ZValComparator_Simple::eGE:
				{
				s << " >= ";
				break;
				}
			case ZValComparator_Simple::eGT:
				{
				s << " > ";
				break;
				}
			}
		spToStrim(iValPred.GetRHS(), s);
		}
	else if (ZP<ZValComparator_StringContains> asStringContains =
		iValPred.GetComparator().DynamicCast<ZValComparator_StringContains>())
		{
		if (ZP<ZValComparand_Name> asName = iValPred.GetLHS().DynamicCast<ZValComparand_Name>())
			{
			if (ZP<ZValComparand_Const_Any> asConst =
				iValPred.GetRHS().DynamicCast<ZValComparand_Const_Any>())
				{
				if (const string8* asString = asConst->GetVal().PGet<string8>())
					{
					spWrite_PropName(asName->GetName(), s);
					s << " LIKE ";
					ZStrimW_Escaped::Options theOptions;
					theOptions.fQuoteQuotes = true;
					theOptions.fEscapeHighUnicode = false;
					s << "'%";
					ZStrimW_Escaped(theOptions, s) << *asString;
					s << "%'";
					return;
					}
				}
			}
		s << "/*Unhandled StringContains*/";
		}
	else
		{
		s << "/*Unhandled Comparator*/";
		}
	}

void ToStrim_SQL::Visit_Expr_Bool_ValPred(const ZP<ZExpr_Bool_ValPred>& iRep)
	{ spToStrim(iRep->GetValPred(), pStrimW()); }

} // anonymous namespace

// =================================================================================================
#pragma mark - RelationalAlgebra::sWriteAsSQL

bool sWriteAsSQL(const map<string8,RelHead>& iTables, ZP<Expr_Rel> iRel, const ZStrimW& s)
	{
	try
		{
		Analyzer theAnalyzer(iTables);
		const Analysis theAnalysis = theAnalyzer.Do(iRel);

		s << "SELECT DISTINCT ";

		{
		RelHead theRHLogical;
		foreachi (ii, theAnalysis.fRelHead_Physical)
			theRHLogical |= sGetMust(theAnalysis.fRename_Inverse, *ii);

		for (Map_ZZ::Index_t ii = theAnalysis.fConstValues.Begin();
			ii != theAnalysis.fConstValues.End(); ++ii)
			{
			theRHLogical |= ColName(theAnalysis.fConstValues.NameOf(ii));
			}

		bool isFirst = true;
		foreachi (ii, theRHLogical)
			{
			if (not sGetSet(isFirst, false))
				s << ",";

			if (ZQ<string8> theQ = sQGet(theAnalysis.fRename, *ii))
				s << *theQ;
			else
				spToStrim_SimpleValue(s, theAnalysis.fConstValues.Get(*ii));
			}
		}

		s << " FROM ";

		{
		bool isFirst = true;

		foreachi (ii, theAnalyzer.fTablesUsed)
			{
			for (int xx = 0; xx < ii->second; ++xx)
				{
				if (not sGetSet(isFirst, false))
					s << ",";
				s << ii->first << " AS " << ii->first << sStringf("%d", xx);
				}
			}
		}

		s << " WHERE ";

		ToStrim_SQL().ToStrim(ToStrim_SQL::Options(), s, theAnalysis.fCondition);

		s << ";";
		return true;
		}
	catch (...)
		{}

	return false;
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
