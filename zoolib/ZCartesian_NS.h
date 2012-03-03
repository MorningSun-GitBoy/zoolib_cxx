/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCartesian_NS_h__
#define __ZCartesian_NS_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZCartesian.h"

#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX)

#if defined(__OBJC__)
	#import <Foundation/NSGeometry.h>
	#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME \
		(defined(NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES) \
		&& NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES)
#else
	#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 \
		&& (__LP64__ || NS_BUILD_32_LIKE_64)
		#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME 1
	#else
		typedef struct _NSPoint { float x; float y; } NSPoint;
		typedef struct _NSSize { float width; float height; } NSSize;
		typedef struct _NSRect { NSPoint origin; NSSize size; } NSRect;
	#endif
#endif

#ifndef ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
	#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME 0
#endif

#if ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
	#include "zoolib/ZCartesian_CG.h"
#endif

#if not ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - NSPoint

template <>
struct Traits<NSPoint>
	{
	typedef null_t Dummy_t;
	typedef bool Bool_t;

	typedef float Ord_t;
	typedef NSPoint Point_t;
	typedef NSRect Rect_t;

	typedef const Ord_t& XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint.x; }

	typedef Ord_t& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint.x; }

	typedef const Ord_t& YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint.y; }

	typedef Ord_t& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint.y; }

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const NSPoint result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
// MARK: - NSSize

template <>
struct Traits<NSSize>
	{
	typedef null_t Dummy_t;
	typedef bool Bool_t;

	typedef float Ord_t;
	typedef NSSize Point_t;
	typedef NSRect Rect_t;

	typedef const Ord_t& XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint.width; }

	typedef Ord_t& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint.width; }

	typedef const Ord_t& YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint.height; }

	typedef Ord_t& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint.height; }

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const NSSize result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
// MARK: - NSRect

template <>
struct Traits<NSRect>
	{
	typedef null_t Dummy_t;
	typedef bool Bool_t;

	typedef float Ord_t;
	typedef NSPoint Point_t;
	typedef NSRect Rect_t;

	typedef const Ord_t& LC_t;
	static LC_t sL(const Rect_t& iRect) { return iRect.origin.x; }

	typedef Ord_t& L_t;
	static L_t sL(Rect_t& ioRect) { return ioRect.origin.x; }

	typedef const Ord_t& TC_t;
	static TC_t sT(const Rect_t& iRect) { return iRect.origin.y; }

	typedef Ord_t& T_t;
	static T_t sT(Rect_t& ioRect) { return ioRect.origin.y; }

	typedef const Ord_t RC_t;
	static RC_t sR(const Rect_t& iRect) { return iRect.origin.x + iRect.size.width; }

	typedef const Ord_t R_t;
	static R_t sR(Rect_t& ioRect) { return ioRect.origin.x + ioRect.size.width; }

	typedef const Ord_t BC_t;
	static BC_t sB(const Rect_t& iRect) { return iRect.origin.y + iRect.size.height; }

	typedef const Ord_t B_t;
	static B_t sB(Rect_t& ioRect) { return ioRect.origin.y + ioRect.size.height; }

	typedef const Ord_t& WC_t;
	static WC_t sW(const Rect_t& iRect) { return iRect.size.width; }

	typedef Ord_t& W_t;
	static W_t sW(Rect_t& ioRect) { return ioRect.size.width; }

	typedef const Ord_t& HC_t;
	static HC_t sH(const Rect_t& iRect) { return iRect.size.height; }

	typedef Ord_t& H_t;
	static H_t sH(Rect_t& ioRect) { return ioRect.size.height; }

	typedef const NSSize& WHC_t;
	static WHC_t sWH(const Rect_t& iRect) { return iRect.size; }

	typedef NSSize& WH_t;
	static WH_t sWH(Rect_t& ioRect) { return ioRect.size; }

	typedef const Point_t& LTC_t;
	static LTC_t sLT(const Rect_t& iRect) { return iRect.origin; }

	typedef Point_t& LT_t;
	static LT_t sLT(Rect_t& ioRect) { return ioRect.origin; }

	typedef const Point_t RBC_t;
	static RBC_t sRB(const Rect_t& iRect) { return sPoint<Point_t>(R(iRect), B(iRect)); }

	typedef const Point_t RB_t;
	static RB_t sRB(Rect_t& ioRect) { return sPoint<Point_t>(R(ioRect), B(ioRect)); }

	typedef const Point_t LBC_t;
	static LBC_t sLB(const Rect_t& iRect) { return sPoint<Point_t>(L(iRect), B(iRect)); }

	typedef const Point_t LB_t;
	static LB_t sLB(Rect_t& ioRect) { return sPoint<Point_t>(L(ioRect), B(ioRect)); }

	typedef const Point_t RTC_t;
	static RTC_t sRT(const Rect_t& iRect) { return sPoint<Point_t>(R(iRect), T(iRect)); }

	typedef const Point_t RT_t;
	static RT_t sRT(Rect_t& ioRect) { return sPoint<Point_t>(R(ioRect), T(ioRect)); }

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		const NSRect result = { {iL, iT}, {iR - iL, iB - iT} };
		return result;
		}
	};

} // namespace ZCartesian
} // namespace ZooLib

#endif // not ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
#endif // ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX)
#endif // __ZCartesian_NS_h__