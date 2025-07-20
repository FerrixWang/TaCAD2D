// Copyright [2025] TaCAD

#ifndef OCCT_OCCTHELPER_PIXMAPHELPER_H_
#define OCCT_OCCTHELPER_PIXMAPHELPER_H_

// Qt includes
#include <QImage>

// Occt includes
#include <Image_PixMap.hxx>

class PixMapHelper
{
public:
	static Handle(Image_PixMap) convertFromBitmap(const QImage& object);
};

#endif  // OCCT_OCCTHELPER_PIXMAPHELPER_H_
