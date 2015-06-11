/*
 * $Id: ReaderWriterPCX.cpp 12579 2015-05-26 09:32:40Z darkprof $
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
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

#include "ReaderWriterPCX.hpp"
#include "../global.hpp"
//#include "pcx.hpp"

static unsigned char* pcx_load(
  std::istream& fin,
  int& width_ret, 
  int& height_ret, 
  int& numComponents_ret)
{
  shared_ptr(PCXHEADER) pcx (new PCXHEADER);
  uint8_t tmp, count;
  bool isHavePalette = true;
  bool isStandartPalette = false;
  bool isHave16Colors = false;
  uint MaxNumberOfColors, index;
  shared_ptr(COLORS[]) colorPalette;// (new  uint8_t(768));
  //shared_ptr(std::vector<unsigned char>) srcBuffer;
  //shared_ptr() 
  //bool swap;
  
  // actual file size
  fin.seekg(0, std::ios::end);
  size_t actFileSize = fin.tellg();
  fin.seekg(0, std::ios::beg);
  
  if (actFileSize < 128) {
    OSG_WARN << "Invalid PCX file size\n";
    return 0;
  }

  fin.read((char*) &pcx->Identifier, sizeof(pcx->Identifier));
  if (pcx->Identifier != 0x0a) {
    OSG_WARN << "Invalid PCX Identifier\n";
    return 0;
  }
  
  fin.read((char*) &pcx->Version, sizeof(pcx->Version));
  if ((pcx->Version == 1) || (pcx->Version > 5)) {
    OSG_WARN << "Invalid PCX Version\n";
    return 0;
  }
  
  fin.read((char*) &pcx->Encoding, sizeof(pcx->Encoding));
  if ((pcx->Encoding != 0) && (pcx->Encoding != 1)) {
    OSG_WARN << "Invalid PCX Encoding\n";
    return 0;
  }
  
  fin.read((char*) &pcx->BitsPerPixel, sizeof(pcx->BitsPerPixel));
  if ((pcx->BitsPerPixel != 8) &&
    (pcx->BitsPerPixel != 1) &&
    (pcx->BitsPerPixel != 2) &&
    (pcx->BitsPerPixel != 4)) {
      
    OSG_WARN << "Invalid PCX BitsPerPixel\n";
    return 0;
  }
  
  /*
   * Image size
   */
  fin.read((char*) &pcx->XStart, sizeof(pcx->XStart));
  fin.read((char*) &pcx->YStart, sizeof(pcx->YStart));
  fin.read((char*) &pcx->XEnd, sizeof(pcx->XEnd));
  fin.read((char*) &pcx->YEnd, sizeof(pcx->YEnd));
  fin.read((char*) &pcx->HorzRes, sizeof(pcx->HorzRes));
  fin.read((char*) &pcx->VertRes, sizeof(pcx->VertRes));
  
  width_ret = pcx->XEnd - pcx->XStart + 1; /* Width of image in pixels */ 
  height_ret = pcx->YEnd - pcx->YStart + 1; /* Length of image in scan lines */
  if ((width_ret <= 0) || (height_ret <= 0)) {
    OSG_WARN << "Invalid PCX image size\n";
    return 0;
  }
  
  fin.read((char*) &pcx->Palette, sizeof(pcx->Palette));
  fin.read((char*) &pcx->Reserved1, sizeof(pcx->Reserved1));
  fin.read((char*) &pcx->NumBitPlanes, sizeof(pcx->NumBitPlanes));
  fin.read((char*) &pcx->BytesPerLine, sizeof(pcx->BytesPerLine));
  fin.read((char*) &pcx->PaletteType, sizeof(pcx->PaletteType));
  fin.read((char*) &pcx->HorzScreenSize, sizeof(pcx->HorzScreenSize));
  fin.read((char*) &pcx->VertScreenSize, sizeof(pcx->VertScreenSize));
  fin.read((char*) &pcx->Reserved2, sizeof(pcx->Reserved2));
  
  /*
   * Settings
   */
  isHavePalette = (pcx->BitsPerPixel * pcx->NumBitPlanes) < 24;
  MaxNumberOfColors = (1L << (pcx->BitsPerPixel * pcx->NumBitPlanes));
  isStandartPalette = (MaxNumberOfColors < 16);
  isHave16Colors = (MaxNumberOfColors == 16);
  
  /*
   * color palette
   */
  // 2, 4, 8 colors FIX
  if (isStandartPalette) {
    OSG_WARN << "Sorry, PCX image palittre 2, 4 or 8 colors not support\n";
    return 0;
  }
  // 16 colors FIX
  if (isStandartPalette) {
    OSG_WARN << "Sorry, PCX image palittre 16 colors not support\n";
    return 0;
  }
  // if 256 colors read palette from end file
  if (MaxNumberOfColors == 256) {
    fin.seekg(-769, std::ios::end);
    
    fin.read((char*) &tmp, sizeof(tmp));
    if (tmp != 0x0c) {
      OSG_WARN << "Invalid PCX image 256 color palette\n";
      return 0;
    }
    
    numComponents_ret = sizeof(COLORS);
    
    //colorPalette->resize (PCX_RGB * MaxNumberOfColors);
    colorPalette.reset( new COLORS [MaxNumberOfColors] );
    fin.read((char*) colorPalette.get(), sizeof(COLORS) * MaxNumberOfColors);
    
  }
   
  /*
   * image data
   */
  //const unsigned int imageBytesPerRow = dib.width * imageBytesPerPixel;
  const unsigned int imageBufferSize = width_ret * height_ret;
  //srcSize = 8 * pcx->NumBitPlanes; // work only if equalent or above 256 colors
  //srcBuffer->resize (srcSize);
  COLORS* imageBuffer = new COLORS[imageBufferSize];
  
  /* 
  ** Decode a PCX scan line. 
  ** 
  ** In this example the size of Buffer[] and the value of BufferSize 
  ** is equal to the scan line length. Data is read from the FILE 
  ** stream fpIn and written to Buffer[]. 
  */ 
  fin.seekg(sizeof (PCXHEADER), std::ios::beg);
  index = 0; count = 0;
  // #### pcx->Encoding, if ##################################################
  if (pcx->Encoding) { 
    if (!isHavePalette) {
      OSG_WARN << "Sorry, packed PCX image with no palette not support\n";
      return 0;
    }
      
    for (int h = height_ret - 1; h >= 0; --h) { 
      for (int w = 0; w < width_ret; ++w) {
        if(!count) {
          if(!fin.read((char*) &tmp, sizeof(tmp))) {
            OSG_WARN << "file truncated\n";
            return 0;        
          }
        
          if( (tmp & 0xc0) == 0xc0) {
            count = tmp & 0x3f;
            if(!fin.read((char*) &tmp, sizeof(tmp))) {
              OSG_WARN << "file truncated\n";
              return 0;
            }
          } else {
            count = 1;
          }
        }
      
        index = h * width_ret + w;
        imageBuffer[index].red = colorPalette[tmp].red;
        imageBuffer[index].green = colorPalette[tmp].green;
        imageBuffer[index].blue = colorPalette[tmp].blue;
        --count;
      }
    }
  // ##### pcx->Encoding, else ###############################################
  } else {
    // if no palette
    if (!isHavePalette) {
      //while (index < imageBufferSize) {
      for (int h = height_ret - 1; h >= 0; --h) { 
        for (int w = 0; w < width_ret; ++w) {
          index = h * width_ret + w;
          if ( !fin.read((char*) &imageBuffer[index], sizeof(COLORS)) ) {
            OSG_WARN << "file truncated\n";
            return 0;
          }
        }
      }
    } else {
      for (int h = height_ret - 1; h >= 0; --h) { 
        for (int w = 0; w < width_ret; ++w) {
          if ( !fin.read((char*) &tmp, sizeof(tmp)) ) {
            OSG_WARN << "file truncated\n";
            return 0;
          }
        
          index = h * width_ret + w;
          imageBuffer[index].red = colorPalette[tmp].red;
          imageBuffer[index].green = colorPalette[tmp].green;
          imageBuffer[index].blue = colorPalette[tmp].blue;
        }
      }
    }
  } // #### pcx->Encoding, end if ############################################
  
  return (uint8_t*)imageBuffer;
}

static bool pcx_save(const osg::Image& img, std::ostream& fout)
{
  
}

ReaderWriterPCX::ReaderWriterPCX()
{
  supportsExtension("pcx","PCX Image format");
}

const char* ReaderWriterPCX::className() const 
{ 
  return "PCX Image Reader"; 
}

osgDB::ReaderWriter::ReadResult ReaderWriterPCX::readObject(
  std::istream& fin, const Options* options) const
{
  return readImage(fin, options);
}

osgDB::ReaderWriter::ReadResult ReaderWriterPCX::readObject(
  const std::string& file, const Options* options) const
{
  return readImage(file, options);
}


osgDB::ReaderWriter::ReadResult ReaderWriterPCX::readImage(
  std::istream& fin, const Options* options) const
{
  return readPCXStream(fin);
}

osgDB::ReaderWriter::ReadResult ReaderWriterPCX::readImage(
  const std::string& file, const Options* options) const
{
  std::string ext = osgDB::getLowerCaseFileExtension(file);
  if (!acceptsExtension(ext)) 
    return osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED;

  std::string fileName = osgDB::findDataFile(file, options);
  if (fileName.empty()) 
    return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;

  osgDB::ifstream istream(fileName.c_str(), std::ios::in | std::ios::binary);
  if(!istream) 
    return osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED;

  osgDB::ReaderWriter::ReadResult rr = readPCXStream(istream);
  if(rr.validImage()) 
    rr.getImage()->setFileName(file);

  return rr;
}


osgDB::ReaderWriter::WriteResult ReaderWriterPCX::writeImage(
  const osg::Image& image, std::ostream& fout, const Options* options) const
{
  if (pcx_save(image, fout))
    return osgDB::ReaderWriter::WriteResult::FILE_SAVED;
  else
    return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
}

osgDB::ReaderWriter::WriteResult ReaderWriterPCX::writeImage(
  const osg::Image& img, 
  const std::string& fileName, const Options* options) const
{
  std::string ext = osgDB::getFileExtension(fileName);
  if (!acceptsExtension(ext)) 
    return osgDB::ReaderWriter::WriteResult::FILE_NOT_HANDLED;

  osgDB::ofstream fout(fileName.c_str(), std::ios::out | std::ios::binary);
  if (!fout) 
    return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;

  return writeImage(img, fout, options);
}

osgDB::ReaderWriter::ReadResult ReaderWriterPCX::readPCXStream(
  std::istream& fin)
{
  int s, t;
  int internalFormat;

  unsigned char *imageData = pcx_load(fin, s, t, internalFormat);
  if (imageData == 0) 
    return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE; 

  unsigned int pixelFormat;
  switch (internalFormat) {
    case 1:
      pixelFormat = GL_LUMINANCE;
      break;
    case 2:
      pixelFormat = GL_LUMINANCE_ALPHA;
      break;
    case 3:
      pixelFormat = GL_RGB;
      break;
    default:
      pixelFormat = GL_RGBA;
      break;
  }

  osg::Image* img = new osg::Image;
  img->setImage(
    s, 
    t, 
    1,
    internalFormat, 
    pixelFormat, 
    GL_UNSIGNED_BYTE, 
    imageData,
    osg::Image::USE_NEW_DELETE);

  return img;
}

// now register with Registry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(pcx, ReaderWriterPCX)
