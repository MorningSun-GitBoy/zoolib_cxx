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

#ifndef __ZCartesian_h__
#define __ZCartesian_h__ 1
#include "zconfig.h"

#include "zoolib/ZStdInt.h" // For int64
#include "zoolib/ZCompat_algorithm.h" // For std::min/max

namespace ZooLib {
namespace ZCartesian {

/*
Abbreviations
-------------
X = X coordinate (horizontal)
Y = Y coordinate (vertical)
Z = Z coordinate (not used here)

L = Left
T = Top
R = Right
B = Bottom
N = Near (not used here)
F = Far (not used here)

W = Width (distance on X axis)
H = Height (distance on Y axis)
D = Depth  (distance on Z axis, not used here)

LT = Left Top
RB = Right Bottom
LB = Left Bottom
RT = Right Top

WH = Width Height (extent or size)

C = Center
CX = aka CLR, Center X
CY = aka CTB, Center Y

Operations
----------
Contains
Aligned, Centered, Flipped, Insetted, Offsetted, With
*/

// =================================================================================================
// MARK: - PointTraits and accessors

template <class Type_p> struct PointTraits;

template <class Type_p>
typename PointTraits<Type_p>::XC_t X(const Type_p& iT) { return PointTraits<Type_p>::sX(iT); }
template <class Type_p>
typename PointTraits<Type_p>::X_t X(Type_p& iT) { return PointTraits<Type_p>::sX(iT); }


template <class Type_p>
typename PointTraits<Type_p>::YC_t Y(const Type_p& iT) { return PointTraits<Type_p>::sY(iT); }
template <class Type_p>
typename PointTraits<Type_p>::Y_t Y(Type_p& iT) { return PointTraits<Type_p>::sY(iT); }

#define ZMACRO_Cartesian(T) \
	template <> struct PointTraits<T> \
		{ \
		typedef T Ord_t; \
		typedef const Ord_t& XC_t; \
		inline static XC_t sX(const T& iT) { return iT; } \
		typedef Ord_t& X_t; \
		inline static X_t sX(T& ioT) { return ioT; } \
		typedef const Ord_t& YC_t; \
		inline static YC_t sY(const T& iT) { return iT; } \
		typedef Ord_t& Y_t; \
		inline static Y_t sY(T& ioT) { return ioT; } \
		};

ZMACRO_Cartesian(char)
ZMACRO_Cartesian(signed char)
ZMACRO_Cartesian(short)
ZMACRO_Cartesian(int)
ZMACRO_Cartesian(long)
ZMACRO_Cartesian(int64)
ZMACRO_Cartesian(float)
ZMACRO_Cartesian(double)
ZMACRO_Cartesian(long double)

#undef ZMACRO_Cartesian

// =================================================================================================
// MARK: - Fundamental Point Pseudo-ctor

template <class Point_p>
typename PointTraits<Point_p>::Point_t
sPoint
	(const typename PointTraits<Point_p>::Ord_t& iX,
	const typename PointTraits<Point_p>::Ord_t& iY)
	{ return PointTraits<Point_p>::sMake(iX, iY); }

// =================================================================================================
// MARK: - Other Pseudo-ctors.

template <class Point_p>
typename PointTraits<Point_p>::Point_t
sPoint()
	{ return PointTraits<Point_p>::sMake(0, 0); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
sPoint
	(const Other& iOther,
	const typename PointTraits<Other>::Dummy_t& dummy = null)
	{ return sPoint<Point_p>(X(iOther), Y(iOther)); }

// =================================================================================================
// MARK: - Point Comparison Operators

template <class Point_p>
typename PointTraits<Point_p>::Bool_t
operator==(const Point_p& iL, const Point_p& iR)
	{ return X(iL) == X(iR) && Y(iL) == Y(iR); }

template <class Point_p>
typename PointTraits<Point_p>::Bool_t
operator!=(const Point_p& iL, const Point_p& iR)
	{ return not (iL == iR); }

template <class Point_p>
typename PointTraits<Point_p>::Bool_t
operator<(const Point_p& iL, const Point_p& iR)
	{ return X(iL) < X(iR) || X(iL) == X(iR) && Y(iL) < Y(iR); }

template <class Point_p>
typename PointTraits<Point_p>::Bool_t
operator>(const Point_p& iL, const Point_p& iR)
	{ return iR < iL; }

template <class Point_p>
typename PointTraits<Point_p>::Bool_t
operator<=(const Point_p& iL, const Point_p& iR)
	{ return not (iR < iL); }

template <class Point_p>
typename PointTraits<Point_p>::Bool_t
operator>=(const Point_p& iL, const Point_p& iR)
	{ return not (iL < iR); }

// =================================================================================================
// MARK: - Point Manipulation Operators

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator+(const Point_p& iL, const Other& iR)
	{ return sPoint<Point_p>(X(iL) + X(iR), Y(iL) + Y(iR)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator+=(Point_p& ioL, const Other& iR)
	{
	X(ioL) += X(iR);
	Y(ioL) += Y(iR);
	return ioL;
	}

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator-(const Point_p& iL, const Other& iR)
	{ return sPoint<Point_p>(X(iL) - X(iR), Y(iL) - Y(iR)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator-=(Point_p& ioL, const Other& iR)
	{
	X(ioL) -= X(iR);
	Y(ioL) -= Y(iR);
	return ioL;
	}

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator*(const Point_p& iL, const Other& iR)
	{ return sPoint<Point_p>(X(iL) * X(iR), Y(iL) * Y(iR)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator*=(Point_p& ioL, const Other& iR)
	{
	X(ioL) *= X(iR);
	Y(ioL) *= Y(iR);
	return ioL;
	}

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator/(const Point_p& iL, const Other& iR)
	{ return sPoint<Point_p>(X(iL) / X(iR), Y(iL) / Y(iR)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator/=(Point_p& ioL, const Other& iR)
	{
	X(ioL) /= X(iR);
	Y(ioL) /= Y(iR);
	return ioL;
	}

// =================================================================================================
// MARK: - RectTraits and accessors

template <class Type_p> struct RectTraits;

template <class Type_p>
typename RectTraits<Type_p>::LC_t L(const Type_p& iT) { return RectTraits<Type_p>::sL(iT); }
template <class Type_p>
typename RectTraits<Type_p>::L_t L(Type_p& iT) { return RectTraits<Type_p>::sL(iT); }


template <class Type_p>
typename RectTraits<Type_p>::TC_t T(const Type_p& iT) { return RectTraits<Type_p>::sT(iT); }
template <class Type_p>
typename RectTraits<Type_p>::T_t T(Type_p& iT) { return RectTraits<Type_p>::sT(iT); }


template <class Type_p>
typename RectTraits<Type_p>::RC_t R(const Type_p& iT) { return RectTraits<Type_p>::sR(iT); }
template <class Type_p>
typename RectTraits<Type_p>::R_t R(Type_p& iT) { return RectTraits<Type_p>::sR(iT); }


template <class Type_p>
typename RectTraits<Type_p>::BC_t B(const Type_p& iT) { return RectTraits<Type_p>::sB(iT); }
template <class Type_p>
typename RectTraits<Type_p>::B_t B(Type_p& iT) { return RectTraits<Type_p>::sB(iT); }


template <class Type_p>
typename RectTraits<Type_p>::WC_t W(const Type_p& iT) { return RectTraits<Type_p>::sW(iT); }
template <class Type_p>
typename RectTraits<Type_p>::W_t W(Type_p& iT) { return RectTraits<Type_p>::sW(iT); }


template <class Type_p>
typename RectTraits<Type_p>::HC_t H(const Type_p& iT) { return RectTraits<Type_p>::sH(iT); }
template <class Type_p>
typename RectTraits<Type_p>::H_t H(Type_p& iT) { return RectTraits<Type_p>::sH(iT); }


template <class Type_p>
typename RectTraits<Type_p>::WHC_t WH(const Type_p& iT) { return RectTraits<Type_p>::sWH(iT); }
template <class Type_p>
typename RectTraits<Type_p>::WH_t WH(Type_p& iT) { return RectTraits<Type_p>::sWH(iT); }


template <class Type_p>
typename RectTraits<Type_p>::LTC_t LT(const Type_p& iT) { return RectTraits<Type_p>::sLT(iT); }
template <class Type_p>
typename RectTraits<Type_p>::LT_t LT(Type_p& iT) { return RectTraits<Type_p>::sLT(iT); }


template <class Type_p>
typename RectTraits<Type_p>::RBC_t RB(const Type_p& iT) { return RectTraits<Type_p>::sRB(iT); }
template <class Type_p>
typename RectTraits<Type_p>::RB_t RB(Type_p& iT) { return RectTraits<Type_p>::sRB(iT); }


template <class Type_p>
typename RectTraits<Type_p>::LBC_t LB(const Type_p& iT) { return RectTraits<Type_p>::sLB(iT); }
template <class Type_p>
typename RectTraits<Type_p>::LB_t LB(Type_p& iT) { return RectTraits<Type_p>::sLB(iT); }


template <class Type_p>
typename RectTraits<Type_p>::RTC_t RT(const Type_p& iT) { return RectTraits<Type_p>::sRT(iT); }
template <class Type_p>
typename RectTraits<Type_p>::RT_t RT(Type_p& iT) { return RectTraits<Type_p>::sRT(iT); }

// =================================================================================================
// MARK: - Fundamental Rect Pseudo-ctor.

template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t
sRect
	(const typename RectTraits<Rect_p>::Ord_t& iL,
	const typename RectTraits<Rect_p>::Ord_t& iT,
	const typename RectTraits<Rect_p>::Ord_t& iR,
	const typename RectTraits<Rect_p>::Ord_t& iB)
	{ return RectTraits<Rect_p>::sMake(iL, iT, iR, iB); }

// =================================================================================================
// MARK: - Derived accessors

template <class Rect_p>
typename RectTraits<Rect_p>::Ord_t
CX(const Rect_p& iRect)
	{ return (L(iRect) + R(iRect)) / 2; }

template <class Rect_p>
typename RectTraits<Rect_p>::Ord_t
CY(const Rect_p& iRect)
	{ return (T(iRect) + B(iRect)) / 2; }

template <class Rect_p>
typename RectTraits<Rect_p>::Point_t
C(const Rect_p& iRect)
	{ return sPoint<typename RectTraits<Rect_p>::Point_t>(CX(iRect), CY(iRect)); }

// =================================================================================================
// MARK: - Other Pseudo-ctors.

template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t
sRect()
	{ return sRect<Rect_p>(0, 0, 0, 0); }

template <class Point_p>
typename PointTraits<Point_p>::Rect_t
sRect(const Point_p& iPoint)
	{ return sRect<typename PointTraits<Point_p>::Rect_t>(0, 0, X(iPoint), Y(iPoint)); }

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
sRect
	(const Other& iOther,
	const typename RectTraits<Other>::Dummy_t& dummy = null)
	{ return sRect<Rect_p>(L(iOther), T(iOther), R(iOther), B(iOther)); }

template <class Point_p>
typename PointTraits<Point_p>::Rect_t
sRect
	(const Point_p& iLT,
	const Point_p& iRB)
	{ return sRect<typename PointTraits<Point_p>::Rect_t>(X(iLT), Y(iLT), X(iRB), Y(iRB)); }


template <class Rect_p, class OtherW, class OtherH>
typename RectTraits<Rect_p>::Rect_t
sRect
	(const OtherW& iW,
	const OtherH& iH)
	{ return sRect<Rect_p>(0, 0, iW, iH); }

// =================================================================================================
// MARK: - Rect Comparison Operators

template <class Rect_p>
typename RectTraits<Rect_p>::Bool_t
operator==(const Rect_p& iL, const Rect_p& iR)
	{ return LT(iL) == LT(iR) && RB(iL) == RB(iR); }

template <class Rect_p>
typename RectTraits<Rect_p>::Bool_t
operator!=(const Rect_p& iL, const Rect_p& iR)
	{ return not (iL == iR); }

template <class Rect_p>
typename RectTraits<Rect_p>::Bool_t
operator<(const Rect_p& iL, const Rect_p& iR)
	{ return LT(iL) < LT(iR) || LT(iL) == LT(iR) && RB(iL) < RB(iR); }

template <class Rect_p>
typename RectTraits<Rect_p>::Bool_t
operator>(const Rect_p& iL, const Rect_p& iR)
	{ return iR < iL; }

template <class Rect_p>
typename RectTraits<Rect_p>::Bool_t
operator<=(const Rect_p& iL, const Rect_p& iR)
	{ return not (iR < iL); }

template <class Rect_p>
typename RectTraits<Rect_p>::Bool_t
operator>=(const Rect_p& iL, const Rect_p& iR)
	{ return not (iL < iR); }

// =================================================================================================
// MARK: - Rect Manipulation Operators

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator+(const Rect_p& iL, const Other& iR)
	{
	return sRect<Rect_p>
		(L(iL) + X(iR),
		T(iL) + Y(iR),
		R(iL) + X(iR),
		B(iL) + Y(iR));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator+=(Rect_p& ioL, const Other& iR)
	{ return ioL = ioL + iR; }

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator-(const Rect_p& iL, const Other& iR)
	{
	return sRect<Rect_p>
		(L(iL) - X(iR),
		T(iL) - Y(iR),
		R(iL) - X(iR),
		B(iL) - Y(iR));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator-=(Rect_p& ioL, const Other& iR)
	{ return ioL = ioL - iR; }

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator*(const Rect_p& iL, const Other& iR)
	{
	return sRect<Rect_p>
		(L(iL) * X(iR),
		T(iL) * Y(iR),
		R(iL) * X(iR),
		B(iL) * Y(iR));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator*=(Rect_p& ioL, const Other& iR)
	{ return ioL = ioL * iR; }


template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator/(const Rect_p& iL, const Other& iR)
	{
	return sRect<Rect_p>
		(L(iL) / X(iR),
		T(iL) / Y(iR),
		R(iL) / X(iR),
		B(iL) / Y(iR));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator/=(Rect_p& ioL, const Other& iR)
	{ return ioL = ioL / iR; }

// =================================================================================================
// MARK: - sContains

template <class Rect_p, class OtherX>
bool sContainsX(const Rect_p& iRect, const OtherX& iOtherX)
	{ return X(iOtherX) >= L(iRect) && X(iOtherX) < R(iRect); }

template <class Rect_p, class OtherY>
bool sContainsY(const Rect_p& iRect, const OtherY& iOtherY)
	{ return Y(iOtherY) >= T(iRect) && Y(iOtherY) < B(iRect); }

template <class Rect_p, class OtherX_p, class OtherY_p>
bool sContains(const Rect_p& iRect, const OtherX_p& iOtherX, const OtherY_p& iOtherY)
	{
	return X(iOtherX) >= L(iRect) && X(iOtherX) < R(iRect)
		&& Y(iOtherY) >= T(iRect) && Y(iOtherY) < B(iRect);
	}

template <class Rect_p, class Other>
bool sContains(const Rect_p& iRect, const Other& iOther)
	{
	return X(iOther) >= L(iRect) && X(iOther) < R(iRect)
		&& Y(iOther) >= T(iRect) && Y(iOther) < B(iRect);
	}

// =================================================================================================
// MARK: - sOffsetted

template <class Rect_p, class OtherX, class OtherY>
typename RectTraits<Rect_p>::Rect_t
sOffsetted(const OtherX& iOtherX, const OtherY& iOtherY, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOtherX), T(iRect) + Y(iOtherY),
		R(iRect) + X(iOtherX), B(iRect) + Y(iOtherY));
	}

template <class Rect_p, class OtherX>
Rect_p sOffsettedX(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect) + X(iOther), T(iRect), R(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sOffsettedY(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iOther), R(iRect), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other>
Rect_p sOffsetted(const Other& iOther, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOther), T(iRect) + Y(iOther), R(iRect) + X(iOther), B(iRect) + Y(iOther));
	}

// =================================================================================================
// MARK: - sOffsetted

template <class Rect_p, class OtherX>
Rect_p sOffsettedL(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect) + X(iOther), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sOffsettedT(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX>
Rect_p sOffsettedR(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sOffsettedB(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), B(iRect) + Y(iOther)); }

// =================================================================================================
// MARK: - sAligned

template <class Rect_p, class OtherX>
Rect_p sAlignedL(const OtherX& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - L(iRect), 0, iRect); }

template <class Rect_p, class OtherY>
Rect_p sAlignedT(const OtherY& iOther, const Rect_p& iRect)
	{ return sOffsetted(0, Y(iOther) - T(iRect), iRect); }

template <class Rect_p, class OtherX>
Rect_p sAlignedR(const OtherX& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - R(iRect), 0, iRect); }

template <class Rect_p, class OtherY>
Rect_p sAlignedB(const OtherY& iOther, const Rect_p& iRect)
	{ return sOffsetted(0, Y(iOther) - B(iRect), iRect); }

// =================================================================================================
// MARK: - sAligned

template <class Rect_p, class Other_p>
Rect_p sAlignedLT(const Other_p& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - L(iRect), Y(iOther) - T(iRect), iRect); }

template <class Rect_p, class Other_p>
Rect_p sAlignedRB(const Other_p& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - R(iRect), Y(iOther) - B(iRect), iRect); }

template <class Rect_p, class Other_p>
Rect_p sAlignedLB(const Other_p& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - L(iRect), Y(iOther) - B(iRect), iRect); }

template <class Rect_p, class Other_p>
Rect_p sAlignedRT(const Other_p& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - R(iRect), Y(iOther) - T(iRect), iRect); }

// =================================================================================================
// MARK: - sAligned

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedLT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sOffsetted(X(iX) - L(iRect), Y(iY) - T(iRect), iRect); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedRB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sOffsetted(X(iX) - R(iRect), Y(iY) - B(iRect), iRect); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedLB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sOffsetted(X(iX) - L(iRect), Y(iY) - B(iRect), iRect); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedRT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sOffsetted(X(iX) - R(iRect), Y(iY) - T(iRect), iRect); }

// =================================================================================================
// MARK: - sCentered

template <class Rect_p, class OtherX>
Rect_p sCenteredX(const OtherX& iOtherX, const Rect_p& iRect)
	{ return sOffsetted(X(iOtherX) - CX(iRect), 0, iRect); }

template <class Rect_p, class OtherY>
Rect_p sCenteredY(const OtherY& iOtherY, const Rect_p& iRect)
	{ return sOffsetted(0, Y(iOtherY) - CY(iRect), iRect); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sCentered(const OtherX& iOtherX, const OtherY& iOtherY, const Rect_p& iRect)
	{ return sOffsetted(X(iOtherX) - CX(iRect), Y(iOtherY) - CY(iRect), iRect); }

template <class Rect_p, class Other>
Rect_p sCentered(const Other& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - CX(iRect), Y(iOther) - CY(iRect), iRect); }

// =================================================================================================
// MARK: - sFlippedY

template <class Rect_p, class Other_p>
Rect_p sFlippedY(const Other_p& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iY) - T(iRect), R(iRect), Y(iY) - B(iRect)); }

// =================================================================================================
// MARK: - sInsetted

template <class Rect_p, class OtherX>
Rect_p sInsettedX(const OtherX& iOtherX, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect) + X(iOtherX), T(iRect), R(iRect) - X(iOtherX), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sInsettedY(const OtherY& iOtherY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iOtherY), R(iRect), B(iRect) - Y(iOtherY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sInsetted(const OtherX& iOtherX, const OtherY& iOtherY, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOtherX), T(iRect) + Y(iOtherY),
		R(iRect) - X(iOtherX), B(iRect) - Y(iOtherY));
	}

template <class Rect_p, class Other>
Rect_p sInsetted(const Other& iOther, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOther), T(iRect) + Y(iOther), R(iRect) - X(iOther), B(iRect) - Y(iOther));
	}

// =================================================================================================
// MARK: - sWith

template <class Rect_p, class OtherX>
Rect_p sWithL(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iOther), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sWithT(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX>
Rect_p sWithR(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iOther), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sWithB(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), Y(iOther)); }

// =================================================================================================
// MARK: - sWith

template <class Rect_p, class Other_p>
Rect_p sWithLT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iOther), Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sWithRB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iOther), Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithLB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iOther), T(iRect), R(iRect), Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithRT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iOther), Y(iOther), X(iOther), B(iRect)); }

// =================================================================================================
// MARK: - sWith

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithLT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iX), Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithRB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iX), Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithLB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iX), T(iRect), R(iRect), Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithRT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iY), X(iX), B(iRect)); }

template <class Rect_p, class OtherL, class OtherR>
Rect_p sWithLR(const OtherL& iL, const OtherR& iR, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iL), T(iRect), X(iR), B(iRect)); }

template <class Rect_p, class OtherT, class OtherB>
Rect_p sWithTB(const OtherT& iT, const OtherB& iB, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iT), R(iRect), Y(iB)); }

// =================================================================================================
// MARK: - sWithW, with Width by moving L or R

template <class Rect_p, class OtherX>
Rect_p sWithWL(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX>
Rect_p sWithWR(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iOther), B(iRect)); }

// =================================================================================================
// MARK: - sWithH, with Height by moving T or B

template <class Rect_p, class OtherY>
Rect_p sWithHT(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sWithHB(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), T(iRect) + Y(iOther)); }

// =================================================================================================
// MARK: - sWithWH, with Width and Height by moving LT, RB, LB, RT

template <class Rect_p, class Other_p>
Rect_p sWithWHLT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), B(iRect) - Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHRB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iOther), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHLB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), T(iRect), R(iRect), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHRT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iOther), L(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHLT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iX), B(iRect) - Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHRB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iX), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHLB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iX), T(iRect), R(iRect), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHRT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iY), L(iRect) + X(iX), B(iRect)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // __ZCartesian_h__