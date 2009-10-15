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

#ifndef __ZWinRegistry_Val__
#define __ZWinRegistry_Val__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZWinHeader.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZRef_WinHANDLE.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZVal_Any.h"

#include <string>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

namespace ZWinRegistry {

using namespace std;

// =================================================================================================
#pragma mark -
#pragma mark * ZWinRegistry::Tagged_T

template <class Tag, class Value>
class Tagged_T
	{
	Value fValue;
public:
	Tagged_T() {}
	Tagged_T(const Tagged_T& iOther) : fValue(iOther.fValue) {}
	~Tagged_T() {}
	Tagged_T& operator=(const Tagged_T& iOther) { fValue = iOther.fValue; return *this; }

	Tagged_T(const Value& iValue) : fValue(iValue) {}
	Tagged_T& operator=(const Value& iValue) { fValue = iValue; return *this; }

	const Value& Get() const { return fValue; }
	};

typedef Tagged_T<struct Env, string16> String_Env;
typedef Tagged_T<struct Link, string16> String_Link;
typedef vector<string16> StringList;
typedef ZData_Any Data;
class KeyRef;

// =================================================================================================
#pragma mark -
#pragma mark * ZWinRegistry::Val

class Val : public ZAny
	{
	typedef ZAny inherited;

public:
	ZAny AsAny() const;

	Val();
	Val(const Val& iOther);
	~Val();
	Val& operator=(const Val& iOther);

	Val(const string8& iVal);
	Val(const string16& iVal);
	Val(const vector<string16>& iVal);
	Val(const String_Env& iVal);
	Val(const String_Link& iVal);
	Val(const int32& iVal);
	Val(const int64& iVal);
	Val(const Data& iVal);
	Val(const KeyRef& iVal);

	ZMACRO_ZValAccessors_Decl_Get(Val, String16, string16)
	ZMACRO_ZValAccessors_Decl_Get(Val, StringList, vector<string16>)
	ZMACRO_ZValAccessors_Decl_Get(Val, String_Env, String_Env)
	ZMACRO_ZValAccessors_Decl_Get(Val, String_Link, String_Link)
	ZMACRO_ZValAccessors_Decl_Get(Val, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Get(Val, Int64, int64)
	ZMACRO_ZValAccessors_Decl_Get(Val, Data, Data)
	ZMACRO_ZValAccessors_Decl_Get(Val, KeyRef, KeyRef)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWinRegistry::KeyRef

class KeyRef
:	public ZRef<HKEY>
	{
	typedef ZRef<HKEY> inherited;
public:
	static KeyRef sHKCR();
	static KeyRef sHKCU();
	static KeyRef sHKLM();
	static KeyRef sHKU();

	void swap(KeyRef& rhs);

	typedef size_t Index_t;

	KeyRef();
	KeyRef(const KeyRef& iOther);
	~KeyRef();
	KeyRef& operator=(const KeyRef& iOther);

	KeyRef(ZRef<HKEY> iOther);
	KeyRef& operator=(ZRef<HKEY> iOther);

// ZMap protocol
	bool QGet(const string16& iName, Val& oVal) const;
	bool QGet(const string8& iName, Val& oVal) const;
	bool QGet(const Index_t& iIndex, Val& oVal) const;

	Val DGet(const Val& iDefault, const string16& iName) const;
	Val DGet(const Val& iDefault, const string8& iName) const;
	Val DGet(const Val& iDefault, const Index_t& iIndex) const;

	Val Get(const string16& iName) const;
	Val Get(const string8& iName) const;
	Val Get(const Index_t& iIndex) const;

	void Set(const string16& iName, const Val& iVal);
	void Set(const string8& iName, const Val& iVal);
	void Set(const Index_t& iIndex, const Val& iVal);

	void Erase(const string16& iName);
	void Erase(const string8& iName);
	void Erase(const Index_t& iIndex);

// Extended ZMap protocol
	Index_t Begin() const;
	Index_t End() const;

	string8 NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const string8& iName) const;
	Index_t IndexOf(const KeyRef& iOther, const Index_t& iOtherIndex) const;

// Our protocol
	HKEY& OParam();
	};

} // namespace ZWinRegistry

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * std::swap

namespace std {

inline void swap(ZOOLIB_PREFIX::ZWinRegistry::KeyRef& a,
	ZOOLIB_PREFIX::ZWinRegistry::KeyRef& b)
	{ a.swap(b); }

} // namespace std

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinRegistry_Val__
