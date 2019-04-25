/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#ifndef __ZooLib_Pixels_Blit_h__
#define __ZooLib_Pixels_Blit_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Geom.h"
#include "zoolib/Pixels/PixelDesc.h"
#include "zoolib/Pixels/Raster.h"

namespace ZooLib {
namespace Pixels {

/**
There are a lot of parameters to many of the methods in ZDCPixmapBlit. In
order to reduce the visual load we use the following abbreviations:
RD = RasterDesc
PD = PixelDesc
B = Bounds
*/

typedef RasterDesc RD;
typedef PixelDesc PD;

// =================================================================================================
#pragma mark -

void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, uint32 iPixval);
void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, const RectPOD& iBounds, uint32 iPixval);

// =================================================================================================
#pragma mark -

void sMunge(void* iBaseAddress,
	const RasterDesc& iRasterDesc, const PixelDesc& iPixelDesc, const RectPOD& iBounds,
	MungeProc iMungeProc, void* iRefcon);

// =================================================================================================
#pragma mark -

void sBlitPixvals(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc,
	const RectPOD& iSourceBounds, PointPOD iDestLocation);

void sBlit(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const PixelDesc& iSourcePixelDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const PixelDesc& iDestPixelDesc,
	const RectPOD& iSourceBounds, PointPOD iDestLocation);

void sBlitRowPixvals(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, int32 iDestH,
	int32 iCount);

void sBlitRow(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const PixelDesc& iSourcePixelDesc,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const PixelDesc& iDestPixelDesc,
	int32 iDestH,
	int32 iCount);

// =================================================================================================


enum EOp
	{
	eOp_Copy,
	eOp_Over,
	eOp_In,
	eOp_Plus
	};

/** Replicate iSourceB over iDestB, aligning iSourceOrigin with iDestB.TopLeft(). */
void sBlit(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	EOp iOperation);

/** Copy source to iDestB without replication. The actual rectangle drawn
will be smaller than iDestB if iSourceB is smaller. */
void sBlit(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const PD& iSourcePD,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	EOp iOperation);

/** Replicate iSourceB, with replicated matte. */
void sBlit(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Replicate iSourceB, matted. */
void sBlit(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const PD& iMattePD,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Draw iSourceB into iDestB, with replicated matte. */
void sBlit(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const PD& iSourcePD,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Draw iSourceB masked by iMatteB into iDestB. */
void sBlit(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const PD& iSourcePD,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const PD& iMattePD,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Fill iDestB with iColor. */
void sColor(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOperation);

/** Fill iDestB with iColor, matted. */
void sColor(
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const PD& iMattePD,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOperation);

/** Fill iDestB with iColor, with replicated matte. */
void sColor(
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOperation);

/** Invert, replacing each pixel with white minus that pixel. */
void sInvert(void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD);

/** Multiply r,g, b & alpha by iAmount/65535. */
void sOpaque(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD, uint16 iAmount);

/** Multiply r, g, b by iAmount/65535, leaving alpha alone. */
void sDarken(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD, uint16 iAmount);

/** Multiply alpha by iAmount/65535, leaving r,g,b alone. */
void sFade(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD, uint16 iAmount);

/** Take the alpha channel of matte, store it in alpha channel of dest,
pre-multiplying r,g,b of dest as it does so. */
void sApplyMatte(
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const PD& iMattePD,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const PD& iDestPD);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Blit_h__
