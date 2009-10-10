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

#ifndef __ZJavaScriptCore__
#define __ZJavaScriptCore__ 1
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZTypes.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZValAccessors.h"

#include <JavaScriptCore/JavaScriptCore.h>

#include <string>

NAMESPACE_ZOOLIB_BEGIN

namespace ZJavaScriptCore {

class ObjectImp;
class ObjectRef;

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ContextRefSetter

JSContextRef sCurrentContextRef();

class ContextRefSetter
	{
public:
	ContextRefSetter(JSContextRef iJSContextRef);
	~ContextRefSetter();

private:
	JSContextRef fJSContextRef_Prior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ContextRef

class ContextRef
	{
public:
	ContextRef();
	ContextRef(const ContextRef& iOther);
	~ContextRef();
	ContextRef& operator=(const ContextRef& iOther);

	ContextRef(JSContextRef iJSContextRef);
	ContextRef(JSGlobalContextRef iJSGlobalContextRef);

	operator JSGlobalContextRef() const;

	bool operator==(const ContextRef& iOther) const;
	bool operator!=(const ContextRef& iOther) const;

	ObjectRef GetGlobalObjectRef() const;

private:
	class Rep;
	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::String

class String
	{
public:
	String();
	String(const String& iOther);
	~String();
	String& operator=(const String& iOther);

	String(JSStringRef iJSStringRef);
	String(const ZRef<JSStringRef>& iJSStringRef);
	String(const Adopt_T<JSStringRef>& iJSStringRef);
	String(const string8&);
	String(const string16&);

	operator JSStringRef() const;

	string8 AsString8() const;
	string16 AsString16() const;

private:
	ZRef<JSStringRef> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::Value

class Value
:	private ZRef<JSValueRef>
	{
	typedef ZRef<JSValueRef> inherited;
public:
	static bool sQFromAny(const ZAny& iAny, Value& oVal);
	static Value sDFromAny(const Value& iDefault, const ZAny& iAny);
	static Value sFromAny(const ZAny& iAny);

	ZAny AsAny() const;

	Value();
	Value(const Value& iOther);
	~Value();
	Value& operator=(const Value& iOther);

	Value(ObjectImp* iOther);
	Value(const ZRef<ObjectImp>& iOther);
	Value(JSObjectRef iOther);
	Value(JSValueRef iOther);
	explicit Value(bool iValue);
	Value(double iValue);
	Value(const String& iValue);
	Value(const ObjectRef& iValue);

	Value& operator=(JSObjectRef iOther);
	Value& operator=(JSValueRef iOther);
	Value& operator=(bool iValue);
	Value& operator=(double iValue);
	Value& operator=(const String& iValue);
	Value& operator=(const ObjectRef& iValue);

	String ToString() const;

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Value,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const;
	operator bool() const;
	operator JSValueRef() const;
	JSValueRef& OParam();

// ZVal Get protocol
	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	S DGet_T(const S& iDefault) const
		{
		S result;
		if (this->QGet_T(result))
			return result;
		return iDefault;
		}

	template <class S>
	S Get_T() const
		{ return this->DGet_T(S()); }

	ZMACRO_ZValAccessors_Decl_Get(Value, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Get(Value, Double, double)
	ZMACRO_ZValAccessors_Decl_Get(Value, String, String)
	ZMACRO_ZValAccessors_Decl_Get(Value, String16, string16)
	ZMACRO_ZValAccessors_Decl_Get(Value, String8, string8)
	ZMACRO_ZValAccessors_Decl_Get(Value, ObjectRef, ObjectRef)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ObjectRef

class ObjectRef
:	public ZRef<JSObjectRef>
	{
	typedef ZRef<JSObjectRef> inherited;
public:
	ZAny AsAny() const;

	ObjectRef();
	ObjectRef(const ObjectRef& iOther);
	~ObjectRef();
	ObjectRef& operator=(const ObjectRef& iOther);

	ObjectRef(const ZRef<JSObjectRef>& iOther);
	ObjectRef& operator=(const ZRef<JSObjectRef>& iOther);

// ZMap protocol (ish)
	bool QGet(const string8& iName, Value& oVal) const;
	Value DGet(const Value& iDefault, const string8& iName) const;
	Value Get(const string8& iName) const;
	bool Set(const string8& iName, const Value& iValue);
	bool Erase(const string8& iName);

// ZList protocol (ish)
	bool IsList() const;
	size_t Count() const;
	bool QGet(size_t iIndex, Value& oVal) const;
	Value DGet(const Value& iDefault, size_t iIndex) const;
	Value Get(size_t iIndex) const;
	bool Set(size_t iIndex, const Value& iValue);
	bool Erase(size_t iIndex);
	void Insert(size_t iIndex, const Value& iVal);
	void Append(const Value& iVal);

// Our protocol
	Value CallAsFunction(const ObjectRef& iThis,
		const Value* iArgs, size_t iArgCount,
		Value& oEx);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ObjectImp

class ObjectImp
:	public ZRefCountedWithFinalize
	{
public:
	ObjectImp();
	virtual ~ObjectImp();

// From ZRefCountedWithFinalize
	virtual void Finalize();

// Our protocol
	JSObjectRef GetJSObjectRef();

	virtual void Initialize(JSContextRef iJSContextRef, JSObjectRef iJSObjectRef);
	virtual void Finalize(JSObjectRef iJSObjectRef);

	virtual bool HasProperty(const ObjectRef& iThis, const String& iPropName);

	virtual Value GetProperty(
		const ObjectRef& iThis, const String& iPropName, Value& oEx);

	virtual bool SetProperty(
		const ObjectRef& iThis, const String& iPropName, const Value& iVal, Value& oEx);

	virtual void GetPropertyNames(
		const ObjectRef& iThis, JSPropertyNameAccumulatorRef propertyNames);

	virtual Value CallAsFunction(const ObjectRef& iFunction, const ObjectRef& iThis,
		const Value* iArgs, size_t iArgCount,
		Value& oEx);

private:
	static void spInitialize(JSContextRef ctx, JSObjectRef object);
	static void spFinalize(JSObjectRef object);

	static bool spHasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName);

	static JSValueRef spGetProperty(JSContextRef ctx,
		JSObjectRef object, JSStringRef propertyName, JSValueRef* exception);

	static bool spSetProperty(JSContextRef ctx,
		JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception);

	static void spGetPropertyNames(JSContextRef ctx,
		JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames);

	static JSValueRef spCallAsFunction(JSContextRef ctx,
		JSObjectRef function, JSObjectRef thisObject,
		size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

	static ZRef<ObjectImp> spFromRef(JSObjectRef object);

	static JSClassRef spGetJSClass();

private:
	void pTossIfAppropriate();

	JSObjectRef fJSObjectRef;
	};

} // namespace ZJavaScriptCore

NAMESPACE_ZOOLIB_END

#endif // __ZJavaScriptCore__
