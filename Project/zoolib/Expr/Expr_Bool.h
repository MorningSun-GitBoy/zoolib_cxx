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

#ifndef __ZooLib_Expr_Expr_Bool_h__
#define __ZooLib_Expr_Expr_Bool_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"

#include "zoolib/Expr/Expr_Op_T.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Expr_Bool

class Expr_Bool
:	public virtual Expr
	{
protected:
	Expr_Bool();
	};

// =================================================================================================
#pragma mark - Expr_Bool_True

class Visitor_Expr_Bool_True;

class Expr_Bool_True
:	public virtual Expr_Bool
,	public virtual Expr_Op0_T<Expr_Bool>
	{
	typedef Expr_Op0_T<Expr_Bool> inherited;
protected:
	Expr_Bool_True();

public:
	static ZP<Expr_Bool> sTrue();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0_T
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor);

	virtual ZP<Expr_Bool> Self();
	virtual ZP<Expr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_True(Visitor_Expr_Bool_True& iVisitor);
	};

template <>
int sCompare_T(const Expr_Bool_True& iL, const Expr_Bool_True& iR);

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_True

class Visitor_Expr_Bool_True
:	public virtual Visitor_Expr_Op0_T<Expr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_True(const ZP<Expr_Bool_True>& iExpr);
	};

// =================================================================================================
#pragma mark - Expr_Bool_False

class Visitor_Expr_Bool_False;

class Expr_Bool_False
:	public virtual Expr_Bool
,	public virtual Expr_Op0_T<Expr_Bool>
	{
	typedef Expr_Op0_T<Expr_Bool> inherited;

protected:
	Expr_Bool_False();

public:
	static ZP<Expr_Bool> sFalse();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0_T
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor);

	virtual ZP<Expr_Bool> Self();
	virtual ZP<Expr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_False(Visitor_Expr_Bool_False& iVisitor);
	};

template <>
int sCompare_T(const Expr_Bool_False& iL, const Expr_Bool_False& iR);

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_False

class Visitor_Expr_Bool_False
:	public virtual Visitor_Expr_Op0_T<Expr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_False(const ZP<Expr_Bool_False>& iExpr);
	};

// =================================================================================================
#pragma mark - Expr_Bool_Not

class Visitor_Expr_Bool_Not;

class Expr_Bool_Not
:	public virtual Expr_Bool
,	public virtual Expr_Op1_T<Expr_Bool>
	{
	typedef Expr_Op1_T<Expr_Bool> inherited;
public:
	Expr_Bool_Not(const ZP<Expr_Bool>& iOp0);

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op1_T
	virtual void Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Bool>& iVisitor);

	virtual ZP<Expr_Bool> Self();
	virtual ZP<Expr_Bool> Clone(const ZP<Expr_Bool>& iOp0);

// Our protocol
	virtual void Accept_Expr_Bool_Not(Visitor_Expr_Bool_Not& iVisitor);
	};

template <>
int sCompare_T(const Expr_Bool_Not& iL, const Expr_Bool_Not& iR);

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Not

class Visitor_Expr_Bool_Not
:	public virtual Visitor_Expr_Op1_T<Expr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_Not(const ZP<Expr_Bool_Not>& iExpr);
	};

// =================================================================================================
#pragma mark - Expr_Bool_And

class Visitor_Expr_Bool_And;

class Expr_Bool_And
:	public virtual Expr_Bool
,	public virtual Expr_Op2_T<Expr_Bool>
	{
	typedef Expr_Op2_T<Expr_Bool> inherited;
public:
	Expr_Bool_And(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1);

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op2_T
	virtual void Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Bool>& iVisitor);

	virtual ZP<Expr_Bool> Self();
	virtual ZP<Expr_Bool> Clone(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1);

// Our protocol
	virtual void Accept_Expr_Bool_And(Visitor_Expr_Bool_And& iVisitor);
	};

template <>
int sCompare_T(const Expr_Bool_And& iL, const Expr_Bool_And& iR);

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_And

class Visitor_Expr_Bool_And
:	public virtual Visitor_Expr_Op2_T<Expr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_And(const ZP<Expr_Bool_And>& iExpr);
	};

// =================================================================================================
#pragma mark - Expr_Bool_Or

class Visitor_Expr_Bool_Or;

class Expr_Bool_Or
:	public virtual Expr_Bool
,	public virtual Expr_Op2_T<Expr_Bool>
	{
	typedef Expr_Op2_T<Expr_Bool> inherited;
public:
	Expr_Bool_Or(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1);

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op2_T
	virtual void Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Bool>& iVisitor);

	virtual ZP<Expr_Bool> Self();
	virtual ZP<Expr_Bool> Clone(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1);

// Our protocol
	virtual void Accept_Expr_Bool_Or(Visitor_Expr_Bool_Or& iVisitor);
	};

template <>
int sCompare_T(const Expr_Bool_Or& iL, const Expr_Bool_Or& iR);

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Or

class Visitor_Expr_Bool_Or
:	public virtual Visitor_Expr_Op2_T<Expr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_Or(const ZP<Expr_Bool_Or>& iExpr);
	};

// =================================================================================================
#pragma mark - Operators

ZP<Expr_Bool> sTrue();
ZP<Expr_Bool> sFalse();

ZP<Expr_Bool> sNot(const ZP<Expr_Bool>& iExpr_Bool);

ZP<Expr_Bool> sAnd(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS);
ZP<Expr_Bool> sOr(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS);

ZP<Expr_Bool> operator~(const ZP<Expr_Bool>& iExpr_Bool);

ZP<Expr_Bool> operator&(bool iBool, const ZP<Expr_Bool>& iExpr_Bool);
ZP<Expr_Bool> operator&(const ZP<Expr_Bool>& iExpr_Bool, bool iBool);

ZP<Expr_Bool> operator|(bool iBool, const ZP<Expr_Bool>& iExpr_Bool);
ZP<Expr_Bool> operator|(const ZP<Expr_Bool>& iExpr_Bool, bool iBool);

ZP<Expr_Bool> operator&(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS);
ZP<Expr_Bool>& operator&=(ZP<Expr_Bool>& ioLHS, const ZP<Expr_Bool>& iRHS);

ZP<Expr_Bool> operator|(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS);
ZP<Expr_Bool>& operator|=(ZP<Expr_Bool>& ioLHS, const ZP<Expr_Bool>& iRHS);

} // namespace ZooLib

#endif // __ZooLib_Expr_Expr_Bool_h__
