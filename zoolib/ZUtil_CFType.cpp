/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/ZUtil_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZTime.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFData.h)
#include ZMACINCLUDE2(CoreFoundation,CFDate.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_CFType {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CFType

static CFStringRef sEmptyCFString = CFSTR("");

ZRef<CFStringRef> sString()
	{ return sEmptyCFString; }

ZRef<CFStringRef> sString(const string8& iString8)
	{
	if (CFIndex sourceSize = iString8.size())
		{
		return Adopt(::CFStringCreateWithBytes(nullptr,
			reinterpret_cast<const UInt8*>(iString8.data()), sourceSize,
			kCFStringEncodingUTF8, false));
		}

	return sEmptyCFString;
	}

ZRef<CFStringRef> sString(const string16& iString16)
	{
	if (CFIndex sourceSize = iString16.size())
		{
		return Adopt(::CFStringCreateWithCharacters(nullptr,
			reinterpret_cast<const UniChar*>(iString16.data()), sourceSize));
		}

	return sEmptyCFString;
	}

ZRef<CFMutableStringRef> sStringMutable()
	{ return Adopt(::CFStringCreateMutable(kCFAllocatorDefault, 0)); }

ZRef<CFMutableStringRef> sStringMutable(const string8& iString8)
	{ return sStringMutable(sString(iString8)); }

ZRef<CFMutableStringRef> sStringMutable(const string16& iString16)
	{ return sStringMutable(sString(iString16)); }

ZRef<CFMutableStringRef> sStringMutable(const ZRef<CFStringRef>& iCFString)
	{ return Adopt(::CFStringCreateMutableCopy(kCFAllocatorDefault, 0, iCFString)); }

ZRef<CFDictionaryRef> sDictionary()
	{
	return Adopt(::CFDictionaryCreate(kCFAllocatorDefault, nullptr, nullptr, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
	}

ZRef<CFMutableDictionaryRef> sDictionaryMutable()
	{
	return Adopt(::CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
	}

ZRef<CFMutableDictionaryRef> sDictionaryMutable(const ZRef<CFDictionaryRef>& iCFDictionary)
	{
	return Adopt(::CFDictionaryCreateMutableCopy(kCFAllocatorDefault,
		::CFDictionaryGetCount(iCFDictionary), iCFDictionary));
	}

ZRef<CFArrayRef> sArray()
	{ return Adopt(::CFArrayCreate(kCFAllocatorDefault, nullptr, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable()
	{ return Adopt(::CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable(const ZRef<CFArrayRef>& iCFArray)
	{
	return Adopt(::CFArrayCreateMutableCopy(kCFAllocatorDefault,
		::CFArrayGetCount(iCFArray), iCFArray));
	}

ZRef<CFDataRef> sData()
	{ return Adopt(::CFDataCreate(kCFAllocatorDefault, 0, 0)); }

ZRef<CFDataRef> sData(const void* iSource, size_t iSize)
	{
	return Adopt(::CFDataCreate(kCFAllocatorDefault,
		static_cast<const UInt8*>(iSource), iSize));
	}

ZRef<CFMutableDataRef> sDataMutable()
	{ return Adopt(::CFDataCreateMutable(kCFAllocatorDefault, 0)); }

ZRef<CFMutableDataRef> sDataMutable(size_t iSize)
	{
	ZRef<CFMutableDataRef> theData = Adopt(::CFDataCreateMutable(kCFAllocatorDefault, 0));
	::CFDataSetLength(theData, iSize);
	return theData;
	}

ZRef<CFMutableDataRef> sDataMutable(const ZRef<CFDataRef>& iCFData)
	{ return Adopt(::CFDataCreateMutableCopy(kCFAllocatorDefault, 0, iCFData)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CFType, conversions

string8 sAsUTF8(CFStringRef iCFString)
	{
	if (const char *s = ::CFStringGetCStringPtr(iCFString, kCFStringEncodingUTF8))
		return string8(s);

	const CFIndex sourceCU = ::CFStringGetLength(iCFString);
	if (sourceCU == 0)
		return string8();

	// Worst case is six bytes per code unit.
	const size_t bufferSize = sourceCU * 6;
	string8 result(bufferSize, 0);

	UInt8* buffer = reinterpret_cast<UInt8*>(const_cast<char*>(result.data()));

	CFIndex bufferUsed;
	::CFStringGetBytes(iCFString, CFRangeMake(0, sourceCU),
		kCFStringEncodingUTF8, 1, false,
		buffer, bufferSize, &bufferUsed);

	result.resize(bufferUsed);

	return result;
	}

string16 sAsUTF16(CFStringRef iCFString)
	{
	const CFIndex sourceCU = ::CFStringGetLength(iCFString);
	if (sourceCU == 0)
		return string16();

	if (const UniChar* s = ::CFStringGetCharactersPtr(iCFString))
		return string16(reinterpret_cast<const UTF16*>(s), sourceCU);

	string16 result(sourceCU, 0);

	UniChar* buffer = reinterpret_cast<UniChar*>(const_cast<UTF16*>(result.data()));

	::CFStringGetCharacters(iCFString, CFRangeMake(0, sourceCU), buffer);
	return result;
	}

static ZData_Any spAsData_Any(const ZRef<CFDataRef>& iCFData)
	{
	if (size_t theLength = ::CFDataGetLength(iCFData))
		return ZData_Any(::CFDataGetBytePtr(iCFData), theLength);
	return ZData_Any();
	}

static ZSeq_Any spAsSeq_Any(const ZAny& iDefault, const ZRef<CFArrayRef>& iCFArray)
	{
	ZSeq_Any theSeq;

	for (size_t x = 0, theCount = ::CFArrayGetCount(iCFArray); x < theCount; ++x)
		theSeq.Append(sAsAny(iDefault, ::CFArrayGetValueAtIndex(iCFArray, x)));

	return theSeq;
	}

static void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	CFStringRef theKey = static_cast<CFStringRef>(iKey);
	CFTypeRef theValue = static_cast<CFTypeRef>(iValue);

	pair<ZMap_Any*, const ZAny*>* thePair =
		static_cast<pair<ZMap_Any*, const ZAny*>*>(iRefcon);

	thePair->first->Set(sAsUTF8(theKey), sAsAny(*thePair->second, theValue));
	}

static ZMap_Any spAsMap_Any(const ZAny& iDefault, const ZRef<CFDictionaryRef>& iCFDictionary)
	{
	ZMap_Any theMap;
	pair<ZMap_Any*, const ZAny*> thePair(&theMap, &iDefault);
	::CFDictionaryApplyFunction(iCFDictionary, spGatherContents, &thePair);
	return theMap;
	}

ZAny sAsAny(const ZAny& iDefault, ZRef<CFTypeRef> iVal)
	{
	if (!iVal)
		return ZAny();

	const CFTypeID theTypeID = ::CFGetTypeID(iVal);

	#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
		&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
		if (theTypeID == ::CFNullGetTypeID())
			return ZAny();
	#endif

	if (theTypeID == ::CFStringGetTypeID())
		return ZAny(sAsUTF8(iVal.StaticCast<CFStringRef>()));

	if (theTypeID == ::CFDictionaryGetTypeID())
		return ZAny(spAsMap_Any(iDefault, iVal.StaticCast<CFDictionaryRef>()));

	if (theTypeID == ::CFArrayGetTypeID())
		return ZAny(spAsSeq_Any(iDefault, iVal.StaticCast<CFArrayRef>()));

	if (theTypeID == ::CFBooleanGetTypeID())
		return ZAny(bool(::CFBooleanGetValue(iVal.StaticCast<CFBooleanRef>())));

	if (theTypeID == ::CFDateGetTypeID())
		{
		return ZAny(ZTime(kCFAbsoluteTimeIntervalSince1970
			+ ::CFDateGetAbsoluteTime(iVal.StaticCast<CFDateRef>())));
		}
	
	if (theTypeID == ::CFDataGetTypeID())
		return ZAny(spAsData_Any(iVal.StaticCast<CFDataRef>()));
	
	if (theTypeID == ::CFNumberGetTypeID())
		{
		const CFNumberRef theNumberRef = iVal.StaticCast<CFNumberRef>();
		switch (::CFNumberGetType(theNumberRef))
			{
			case kCFNumberSInt8Type:
			case kCFNumberCharType:
				{
				int8 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt8Type, &theValue);
				return ZAny(theValue);
				}
			case kCFNumberSInt16Type:
			case kCFNumberShortType:
				{
				int16 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt16Type, &theValue);
				return ZAny(theValue);
				}
			case kCFNumberSInt32Type:
			case kCFNumberIntType:
				{
				int32 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt32Type, &theValue);
				return ZAny(theValue);
				}
			case kCFNumberSInt64Type:
			case kCFNumberLongLongType:
				{
				int64 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt64Type, &theValue);
				return ZAny(theValue);
				}
			case kCFNumberFloat32Type:
			case kCFNumberFloatType:
				{
				float theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat32Type, &theValue);
				return ZAny(theValue);
				}
			case kCFNumberFloat64Type:
			case kCFNumberDoubleType:
				{
				double theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat64Type, &theValue);
				return ZAny(theValue);
				}
			}
		}

	return iDefault;
	}

ZAny sAsAny(ZRef<CFTypeRef> iVal)
	{ return sAsAny(ZAny(), iVal); }

static ZRef<CFTypeRef> spMakeNumber(CFNumberType iType, const void* iVal)
	{ return Adopt_T<CFTypeRef>(::CFNumberCreate( kCFAllocatorDefault, iType, iVal)); }

ZRef<CFTypeRef> sAsCFType(const ZRef<CFTypeRef>& iDefault, const ZAny& iVal)
	{
	if (false)
		{}
	else if (!iVal)
		{
		#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
			&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
			return kCFNull;
		#else
			return ZRef<CFTypeRef>();
		#endif
		}
	else if (const string8* theValue = iVal.PGet_T<string8>())
		{
		return sString(*theValue);
		}
	else if (const vector<char>* theValue = iVal.PGet_T<vector<char> >())
		{
		if (size_t theSize = theValue->size())
			return sData(&(*theValue)[0], theSize);
		else
			return sData();
		}
	else if (const ZData_Any* theValue = iVal.PGet_T<ZData_Any>())
		{
		if (size_t theSize = theValue->GetSize())
			return sData(theValue->GetData(), theSize);
		else
			return sData();
		}
	else if (const ZSeq_Any* theValue = iVal.PGet_T<ZSeq_Any>())
		{
		ZRef<CFMutableArrayRef> theArray;
		for (size_t x = 0, count = theValue->Count(); x < count; ++x)
			::CFArrayAppendValue(theArray, sAsCFType(iDefault, theValue->Get(x)));
		return theArray;
		}
	else if (const ZMap_Any* theValue = iVal.PGet_T<ZMap_Any>())
		{
		ZRef<CFMutableDictionaryRef> theDictionary = sDictionaryMutable();
		for (ZMap_Any::Index_t i = theValue->Begin(), end = theValue->End();
			i != end; ++i)
			{
			::CFDictionarySetValue(theDictionary,
				sString(theValue->NameOf(i)), sAsCFType(iDefault, theValue->Get(i)));
			}
		return theDictionary;
		}
	else if (const bool* theValue = iVal.PGet_T<bool>())
		{
		if (*theValue)
			return kCFBooleanTrue;
		else
			return kCFBooleanFalse;
		}
	else if (const ZTime* theValue = iVal.PGet_T<ZTime>())
		{
		return Adopt_T<CFTypeRef>(::CFDateCreate(kCFAllocatorDefault,
			theValue->fVal - kCFAbsoluteTimeIntervalSince1970));
		}
	else if (const char* theValue = iVal.PGet_T<char>())
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const unsigned char* theValue = iVal.PGet_T<unsigned char>())
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const signed char* theValue = iVal.PGet_T<signed char>())
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const short* theValue = iVal.PGet_T<short>())
		{
		return spMakeNumber(kCFNumberSInt16Type, theValue);
		}
	else if (const unsigned short* theValue = iVal.PGet_T<unsigned short>())
		{
		return spMakeNumber(kCFNumberSInt16Type, theValue);
		}
	else if (const int* theValue = iVal.PGet_T<int>())
		{
		if (ZIntIs32Bit)	
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const unsigned int* theValue = iVal.PGet_T<unsigned int>())
		{
		if (ZIntIs32Bit)	
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const long* theValue = iVal.PGet_T<long>())
		{
		if (ZLongIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const unsigned long* theValue = iVal.PGet_T<unsigned long>())
		{
		if (ZLongIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const int64* theValue = iVal.PGet_T<int64>())
		{
		return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const uint64* theValue = iVal.PGet_T<uint64>())
		{
		return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const float* theValue = iVal.PGet_T<float>())
		{
		return spMakeNumber(kCFNumberFloatType, theValue);
		}
	else if (const double* theValue = iVal.PGet_T<double>())
		{
		return spMakeNumber(kCFNumberDoubleType, theValue);
		}

	return iDefault;
	}

ZRef<CFTypeRef> sAsCFType(const ZAny& iVal)
	{ return sAsCFType(ZRef<CFTypeRef>(), iVal); }

} // namespace ZUtil_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
