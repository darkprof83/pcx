/*
 * osgdb_pcx is plugin for OpenSceneGraph engine
 * Copyright (C) 2015 by darkprof
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301  USA
 */
 
#ifndef PCX_CLASS_H
#define PCX_CLASS_H

#include <osg/Image>
#include <osg/Notify>
#include <osg/Image>
#include <osg/GL>
#include <osg/Endian>

#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

#include <vector>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <tr1/memory>

#include "../global.hpp"

//#define PCX_RGB 3

/*
 * PCX header
 */
#pragma pack(push, 1)
struct PCXHEADER { 
  BYTE Identifier; /* PCX Id Number (Always 0x0A) */ 
  BYTE Version; /* Version Number */ 
  BYTE Encoding; /* Encoding Format */ 
  BYTE BitsPerPixel; /* Bits per Pixel */ 
  WORD XStart; /* Left of image */ 
  WORD YStart; /* Top of Image */ 
  WORD XEnd; /* Right of Image */
  WORD YEnd; /* Bottom of image */ 
  WORD HorzRes; /* Horizontal Resolution */ 
  WORD VertRes; /* Vertical Resolution */ 
  BYTE Palette[48]; /* 16-Color EGA Palette */ 
  BYTE Reserved1; /* Reserved (Always 0) */ 
  BYTE NumBitPlanes; /* Number of Bit Planes */ 
  WORD BytesPerLine; /* Bytes per Scan-line */ 
  WORD PaletteType; /* Palette Type */ 
  WORD HorzScreenSize; /* Horizontal Screen Size */ 
  WORD VertScreenSize; /* Vertical Screen Size */ 
  BYTE Reserved2[54]; /* Reserved (Always 0) */ 
};
#pragma pack(pop)

#pragma pack(push, 1)
struct COLORS {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};
#pragma pack(pop)

class ReaderWriterPCX : public osgDB::ReaderWriter
{
  public:
    ReaderWriterPCX();
    const char* className() const;
    ReadResult readObject(std::istream& fin, const Options* options = 0) const;
    ReadResult readObject(const std::string& file, const Options* options = 0) const;
    ReadResult readImage(std::istream& fin, const Options* options = 0) const;
    ReadResult readImage(const std::string& file, const Options* options = 0) const;
    WriteResult writeImage(const osg::Image& image, std::ostream& fout, const Options* = 0) const;
    WriteResult writeImage(const osg::Image& img, const std::string& fileName, const Options* options = 0) const;
  
  private:
    static ReadResult readPCXStream(std::istream& fin);
    
};

#endif
