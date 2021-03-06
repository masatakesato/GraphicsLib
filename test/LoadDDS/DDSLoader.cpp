/*************************************************************************
　　DDSLoader.cpp

　　Direct Draw Surface Loader

　　Date : Nov. 01, 2007
　　Version : 1.0
　　Author : Pocol
　　Memo :
*************************************************************************/

//
//　include
//
#include <fstream>
#include "DDSLoader.h"
using namespace std;

//　disable Warning C4996
#pragma warning (disable : 4996 )


//////////////////////////////////////////////////////////////////////////
//　　DDSImage
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------
//　　DDSImage
//　　Desc : コンストラクタ
//-----------------------------------------------------------------------------------------------------
DDSImage::DDSImage()
{
	imageSize = 0;
	imageData = NULL;
	format = GL_RGB;
	internalFormat = GL_RGB;
	width = 0;
	height = 0;
	bpp = 0;
	ID = 0;
	numMipmaps = 0;
}

//-----------------------------------------------------------------------------------------------------
//　　~DDSImage
//　　Desc : デストラクタ
//-----------------------------------------------------------------------------------------------------
DDSImage::~DDSImage()
{
	if ( imageData )
	{
		delete [] imageData;
		imageData = NULL;
	}
}

//-----------------------------------------------------------------------------------------------------
//　　ReadFileDDS
//　　Desc : DDSファイルの読み込み
//-----------------------------------------------------------------------------------------------------
bool DDSImage::ReadDDS(const char *filename)
{
	FILE *fp;
	char magic[4];
	int mipmapFactor;
	long curr, end;
	DDSurfaceDesc ddsd;

	//　GLEWの初期化
	if ( glewInit() != GLEW_OK )
	{
		cout << "Error : GLEWの初期化に失敗しました\n";
		return false;
	}

	//　ファイルを開く
	fp = fopen(filename, "rb");
	if ( !fp )
	{
		cout << "Error : 指定されたファイルを開けませんでした\n";
		cout << "File : " << filename << endl;
		return false;
	}

	//　マジックを読み取り
	fread(&magic, sizeof(char), 4, fp);

	//　マジックをチェック
	if ( strncmp(magic, "DDS ", 4 ) != 0 )
	{
		cout << "Error : DDSファイルではありません\n";
		fclose(fp);
		return false;
	}

	//　ヘッダーを読み取り
	fread(&ddsd, sizeof(ddsd), 1, fp);

	//　幅・高さを格納
	height = ddsd.height;
	width = ddsd.width;
	numMipmaps = ddsd.mipMapLevels;

	//　フォーマット判別
	switch ( ddsd.format.fourCC )
	{
	case FOURCC_DXT1:
		//　DXT1
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		internalFormat = 3;
		mipmapFactor = 2;
		break;

	case FOURCC_DXT3:
		//　DXT3
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		internalFormat = 4;
		mipmapFactor = 4;
		break;

	case FOURCC_DXT5:
		//　DXT5
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		internalFormat = 4;
		mipmapFactor = 4;
		break;

	default:
		cout << "Error : 未対応の形式です。DXT1, DXT3, DXT5のいずれかを使用してください\n";
		return false;
	}

	//　テクセルデータのサイズを算出
	curr = ftell(fp);
	fseek(fp, 0, SEEK_END);
	end = ftell(fp);
	fseek(fp, curr, SEEK_SET);
	imageSize = end - curr;
	imageData = new GLubyte [imageSize];

	//　ピクセルデータの読み込み
	fread(imageData, sizeof(GLubyte), imageSize, fp);

	//　ファイルを閉じる
	fclose(fp);

	return true;
}

//-----------------------------------------------------------------------------------------------------
//　　DecompressDDS
//　　Desc : S3TC圧縮の解凍作業
//-----------------------------------------------------------------------------------------------------
void DDSImage::DecompressDDS()
{
	int blockSize;
	int offset = 0;
	GLsizei mWidth = width, mHeight = height, mSize = 0;

	//　DXT1
	if ( format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
		blockSize = 8;
	//　DXT3, DXT5
	else
		blockSize = 16;

	//　解凍
	for ( int i=0; i<(int)numMipmaps; i++ )
	{
		mSize = ( (mWidth+3)/4 ) * ( (mHeight+3)/4 ) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, mWidth, mHeight, 0, mSize, imageData + offset );
		
		if ( mWidth >> 1 )	mWidth = (mWidth >> 1);
		else	mWidth = 1;

		if ( mHeight >> 1 )	mHeight = (mHeight >> 1);
		else	mHeight = 1;

		offset += mSize;
	}
}

//-----------------------------------------------------------------------------------------------------
//　　Load
//　　Desc : DDSファイルを読み込み，テクスチャを生成する
//-----------------------------------------------------------------------------------------------------
GLuint DDSImage::Load(const char *filename)
{
	//　ファイルの読み込み
	if ( !ReadDDS(filename) )
		return 0;

	//　テクスチャを生成
	glGenTextures(1, &ID);

	//　テクスチャをバインド
	glBindTexture(GL_TEXTURE_2D, ID);

	//　拡大・縮小する方法の指定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	//　テクスチャ環境の設定
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//　解凍作業
	DecompressDDS();

	if ( imageData )
	{
		delete [] imageData;
		imageData = NULL;
	}

	//
	glBindTexture(GL_TEXTURE_2D, 0);

	//　生成したテクスチャ番号を返す
	return ID;
}