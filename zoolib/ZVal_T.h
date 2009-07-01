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

#ifndef __ZVal_T__
#define __ZVal_T__ 1
#include "zconfig.h"

#include "zoolib/ZTypes.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Placement construction/destruction

template <class T>
inline T* sConstruct_T(void* iBytes)
	{
	new(iBytes) T();
	return static_cast<T*>(iBytes);
	}

template <class T>
inline T* sConstruct_T(void* iBytes, const T& iOther)
	{
	new(iBytes) T(iOther);
	return static_cast<T*>(iBytes);
	}

template <class T, class P0>
inline T* sConstruct_T(void* iBytes, const P0& iP0)
	{
	new(iBytes) T(iP0);
	return static_cast<T*>(iBytes);
	}

template <class T, class P0, class P1>
inline T* sConstruct_T(void* iBytes, const P0& iP0, const P1& iP1)
	{
	new(iBytes) T(iP0, iP1);
	return static_cast<T*>(iBytes);
	}

template <class T>
inline T* sCopyConstruct_T(const void* iSource, void* iBytes)
	{
	new(iBytes) T(*static_cast<const T*>(iSource));
	return static_cast<T*>(iBytes);
	}

template <class T>
inline void sDestroy_T(void* iBytes)
	{ static_cast<T*>(iBytes)->~T(); }

template <class T>
inline const T* sFetch_T(const void* iBytes)
	{ return static_cast<const T*>(iBytes); }

template <class T>
inline T* sFetch_T(void* iBytes)
	{ return static_cast<T*>(iBytes); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValR_T

template <class T>
class ZValR_T
	{
protected:
	ZValR_T() {}
	ZValR_T(const ZValR_T&) {}
	~ZValR_T() {}
	ZValR_T& operator=(ZValR_T&) { return *this; }
	
public:
	template <class S>
	S DGet_T(const S& iDefault) const
		{
		S result;
		if (static_cast<const T*>(this)->QGet_T(result))
			return result;
		return iDefault;
		}

	template <class S>
	S Get_T() const
		{ return static_cast<const T*>(this)->DGet_T(S()); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValListR_T

template <class T, class Val_t>
class ZValListR_T
	{
protected:
	ZValListR_T() {}
	ZValListR_T(const ZValListR_T&) {}
	~ZValListR_T() {}
	ZValListR_T& operator=(ZValListR_T&) { return *this; }
	
public:
	Val_t DGet(size_t iIndex, const Val_t& iDefault) const
		{
		Val_t theVal;
		if (static_cast<const T*>(this)->QGet(iIndex, theVal))
			return theVal;
		return iDefault;
		}

	Val_t Get(size_t iIndex) const
		{ return this->DGet(iIndex, Val_t()); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMapIterator

class ZValMapIterator
	{
public:
	ZValMapIterator() : fVal(0) {}
	ZValMapIterator(const ZValMapIterator& iOther) : fVal(iOther.fVal) {}
	~ZValMapIterator() {}
	ZValMapIterator& operator=(const ZValMapIterator& iOther)
		{
		fVal = iOther.fVal;
		return *this;
		}

	explicit ZValMapIterator(size_t iVal) : fVal(iVal) {}

	ZValMapIterator& operator++()
		{
		++fVal;
		return *this;
		}

	bool operator==(const ZValMapIterator& iOther) const { return fVal == iOther.fVal; }
	bool operator!=(const ZValMapIterator& iOther) const { return fVal != iOther.fVal; }

	size_t GetIndex() const { return fVal; }

private:
	size_t fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMapR_T

template <class T, class Name_t, class Val_t>
class ZValMapR_T
	{
protected:
	ZValMapR_T() {}
	ZValMapR_T(const ZValMapR_T&) {}
	~ZValMapR_T() {}
	ZValMapR_T& operator=(ZValMapR_T&) { return *this; }
	
public:
	Val_t DGet(Name_t iName, const Val_t& iDefault) const
		{
		Val_t theVal;
		if (static_cast<const T*>(this)->QGet(iName, theVal))
			return theVal;
		return iDefault;
		}

	Val_t Get(Name_t iName) const
		{ return this->DGet(iName, Val_t()); }

	template <class S>
	bool QGet_T(Name_t iName, S& oVal) const
		{
		Val_t theVal;
		if (static_cast<const T*>(this)->QGet(iName, theVal))
			return theVal.QGet_T<S>(oVal);
		return false;
		}

	template <class S>
	S DGet_T(Name_t iName, const S& iDefault) const
		{
		Val_t theVal;
		if (static_cast<const T*>(this)->QGet(iName, theVal))
			return theVal.DGet_T(iDefault);
		return iDefault;
		}

	template <class S>
	S Get_T(Name_t iName) const
		{ return static_cast<const T*>(this)->DGet_T(iName, S()); }
	};

NAMESPACE_ZOOLIB_END

#endif // __ZVal_T__
