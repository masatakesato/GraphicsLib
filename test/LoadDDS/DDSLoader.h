/*************************************************************************
　　DDSLoader.h　

　　Direct Draw Surface Loader

　　Date : Nov 01, 2007
　　Version : 1.0
　　Author : Pocol
　　Memo : DDSファイルを読み込むためにGLEWを使用します
*************************************************************************/

#ifndef _DDS_LOADER_H_INCLUDED_
#define _DDS_LOADER_H_INCLUDED_

//
//　include
//
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
using namespace std;

//
//　link
//
#pragma comment(lib, "glew32.lib")

//
//　define
//
#define FOURCC_DXT1 '1TXD'
#define FOURCC_DXT3 '3TXD'
#define FOURCC_DXT5 '5TXD'

//
//　struct
//

typedef struct _DDPixelFormat
{
	GLuint size;
	GLuint flgas;
	GLuint fourCC;
	GLuint bpp;
	GLuint redMask;
	GLuint greenMask;
	GLuint blueMask;
	GLuint alphaMask;
} DDPixelFormat;

typedef struct _DDSCaps
{
	GLuint caps;
	GLuint caps2;
	GLuint caps3;
	GLuint caps4;
} DDSCaps;

typedef struct _DDColorKey
{
	GLuint lowVal;
	GLuint highVal;
} DDColorKey;

typedef struct _DDSurfaceDesc
{
	GLuint size;
	GLuint flags;
	GLuint height;
	GLuint width;
	GLuint pitch;
	GLuint depth;
	GLuint mipMapLevels;
	GLuint alphaBitDepth;
	GLuint reserved;
	GLuint surface;

	DDColorKey ckDestOverlay;
	DDColorKey ckDestBlt;
	DDColorKey ckSrcOverlay;
	DDColorKey ckSrcBlt;

	DDPixelFormat format;
	DDSCaps caps;

	GLuint textureStage;
} DDSurfaceDesc;


//////////////////////////////////////////////////////////////////////////
//　　DDSImage class
//////////////////////////////////////////////////////////////////////////
class DDSImage
{
protected:
	GLuint imageSize;
	GLubyte *imageData;
	GLenum format;
	GLuint internalFormat;
	GLuint width;
	GLuint height;
	GLuint bpp;

	GLuint numMipmaps;

public:
	GLuint ID;
	DDSImage();
	~DDSImage();
	void DecompressDDS();
	bool ReadDDS(const char *filename);
	GLuint Load(const char *filename);
};

#endif	//　_DDS_LOADER_H_INCLUDED_