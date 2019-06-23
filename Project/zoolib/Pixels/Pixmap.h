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

#ifndef __ZooLib_Pixels_Pixmap_h__
#define __ZooLib_Pixels_Pixmap_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_operator_bool.h"

#include "zoolib/Pixels/Geom.h"
#include "zoolib/Pixels/PixelDesc.h"
#include "zoolib/Pixels/Raster.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixmapRep

class PixmapRep
:	public Counted
	{
public:
	PixmapRep(const ZP<Raster>& iRaster, const RectPOD& iBounds, const PixelDesc& iPixelDesc);

	PointPOD GetSize();

	const RectPOD& GetBounds();

	const PixelDesc& GetPixelDesc();

	const ZP<Raster>& GetRaster();

	ZP<PixmapRep> Touch();

protected:
	ZP<Raster> fRaster;
	RectPOD fBounds;
	PixelDesc fPixelDesc;
	};

ZP<PixmapRep> sPixmapRep(const RasterDesc& iRasterDesc,
	const PointPOD& iSize,
	const PixelDesc& iPixelDesc);

// =================================================================================================
#pragma mark - Pixmap

class Pixmap
	{
public:
	Pixmap();
	Pixmap(const Pixmap& iOther);
	~Pixmap();
	Pixmap& operator=(const Pixmap& iOther);

	Pixmap(const ZP<PixmapRep>& iRep);
	Pixmap& operator=(const ZP<PixmapRep>& iOther);

	Pixmap(const null_t&);
	Pixmap& operator=(const null_t&);

	ZMACRO_operator_bool(Pixmap, operator_bool) const
		{ return operator_bool_gen::translate(!!fRep); }

	PointPOD Size() const;
	Ord Width() const;
	Ord Height() const;

	void Touch();

	const ZP<PixmapRep>& GetRep() const;

	const ZP<Raster>& GetRaster() const;
	const RasterDesc& GetRasterDesc() const;
	const void* GetBaseAddress() const;
	void* GetBaseAddress();
	const RectPOD& GetBounds() const;
	const PixelDesc& GetPixelDesc() const;

protected:
	ZP<PixmapRep> fRep;
	};

Pixmap sPixmap(const RasterDesc& iRasterDesc, PointPOD iSize, const PixelDesc& iPixelDesc);

// =================================================================================================
#pragma mark - Pixmap

void sMunge(Pixmap& ioPixmap, MungeProc iMungeProc, void* iRefcon);

void sBlit(const Pixmap& iSource, const RectPOD& iSourceBounds,
	Pixmap& ioDest, PointPOD iDestLoc);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Pixmap_h__
