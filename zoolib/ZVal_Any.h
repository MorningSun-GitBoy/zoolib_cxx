/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZVal_Any__
#define __ZVal_Any__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors.h"

#include <map>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

using std::map;
using std::pair;
using std::string;
using std::vector;

class ZVal_Any;
class ZList_Any;
class ZMap_Any;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_Any

class ZVal_Any
:	public ZAny
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_Any,
		operator_bool_generator_type, operator_bool_type);

	typedef ZAny inherited;

public:
	ZVal_Any AsVal_Any(const ZVal_Any& iDefault);

	operator operator_bool_type() const;

	ZVal_Any();
	ZVal_Any(const ZVal_Any& iOther);
	~ZVal_Any();
	ZVal_Any& operator=(const ZVal_Any& iOther);

	ZVal_Any(const ZAny& iVal);

	ZVal_Any& operator=(const ZAny& iVal);

	template <class S>
	ZVal_Any(const S& iVal)
	:	inherited(iVal)
		{}

	template <class S>
	ZVal_Any& operator=(const S& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oVal) const
		{
		if (const S* theVal = ZAnyCast<S>(this))
			{
			oVal = *theVal;
			return true;
			}
		return false;
		}

	template <class S>
	S DGet_T(const S& iDefault) const
		{
		if (const S* theVal = ZAnyCast<S>(this))
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get_T() const
		{
		if (const S* theVal = ZAnyCast<S>(this))
			return *theVal;
		return S();
		}

	template <class S>
	void Set_T(const S& iVal)
		{
		ZAny::operator=(iVal);
		}

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Data, ZData_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, List, ZList_Any)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_Any, Map, ZMap_Any)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZList_Any

class ZList_Any
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMap_Any,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	ZList_Any AsList_Any(const ZVal_Any& iDefault);

	operator operator_bool_type() const;

	ZList_Any();
	ZList_Any(const ZList_Any& iOther);
	~ZList_Any();
	ZList_Any& operator=(const ZList_Any& iOther);

	ZList_Any(vector<ZAny>& iOther);

	ZList_Any& operator=(vector<ZAny>& iOther);

// ZList protocol
	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_Any& oVal) const;
	ZVal_Any DGet(const ZVal_Any& iDefault, size_t iIndex) const;
	ZVal_Any Get(size_t iIndex) const;

	void Set(size_t iIndex, const ZVal_Any& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const ZVal_Any& iVal);

	void Append(const ZVal_Any& iVal);

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any

class ZMap_Any
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMap_Any,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	typedef map<string, ZAny>::iterator Index_t;

	ZMap_Any AsMap_Any(const ZVal_Any& iDefault);

	operator operator_bool_type() const;

	ZMap_Any();
	ZMap_Any(const ZMap_Any& iOther);
	~ZMap_Any();
	ZMap_Any& operator=(const ZMap_Any& iOther);

	ZMap_Any(const map<string, ZAny>& iOther);
	ZMap_Any& operator=(map<string, ZAny>& iOther);

	template <class T>
	ZMap_Any(const vector<pair<string, T> >& iOther);

	template <class T>
	ZMap_Any(const map<string, T>& iOther);

// ZMap protocol
	void Clear();

	bool QGet(const string8& iName, ZVal_Any& oVal) const;
	bool QGet(const Index_t& iIndex, ZVal_Any& oVal) const;

	ZVal_Any DGet(const ZVal_Any& iDefault, const string8& iName) const;
	ZVal_Any DGet(const ZVal_Any& iDefault, const Index_t& iIndex) const;

	ZVal_Any Get(const string8& iName) const;
	ZVal_Any Get(const Index_t& iIndex) const;

	void Set(const string8& iName, const ZVal_Any& iVal);
	void Set(const Index_t& iIndex, const ZVal_Any& iVal);

	void Erase(const string8& iName);
	void Erase(const Index_t& iIndex);

// Our protocol
	Index_t Begin() const;
	Index_t End() const;

	string8 NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const string8& iName) const;

	Index_t IndexOf(const ZMap_Any& iOther, const Index_t& iOtherIndex) const;

private:
	void pTouch();
	map<string, ZAny>::iterator pTouch(const Index_t& iIndex);

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any::Rep

class ZMap_Any::Rep
:	public ZRefCounted
	{
public:
	Rep();
	virtual ~Rep();
	
	Rep(const map<string, ZAny>& iMap);

	template <class Iterator>
	Rep(Iterator begin, Iterator end)
	:	fMap(begin, end)
		{}

private:
	map<string, ZAny> fMap;
	friend class ZMap_Any;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_Any, inline templated constructors

template <class T>
ZMap_Any::ZMap_Any(const vector<pair<string, T> >& iOther)
:	fRep(new Rep(iOther.begin(), iOther.end()))
	{}

template <class T>
ZMap_Any::ZMap_Any(const map<string, T>& iOther)
:	fRep(new Rep(iOther.begin(), iOther.end()))
	{}

NAMESPACE_ZOOLIB_END

#endif // __ZVal_Any__
