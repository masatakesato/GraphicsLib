/*************************************************************************
�@�@DDSLoader.cpp

�@�@Direct Draw Surface Loader

�@�@Date : Nov. 01, 2007
�@�@Version : 1.0
�@�@Author : Pocol
�@�@Memo :
*************************************************************************/

//
//�@include
//
#include <fstream>
#include "DDSLoader.h"
using namespace std;

//�@disable Warning C4996
#pragma warning (disable : 4996 )


//////////////////////////////////////////////////////////////////////////
//�@�@DDSImage
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------
//�@�@DDSImage
//�@�@Desc : �R���X�g���N�^
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
//�@�@~DDSImage
//�@�@Desc : �f�X�g���N�^
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
//�@�@ReadFileDDS
//�@�@Desc : DDS�t�@�C���̓ǂݍ���
//-----------------------------------------------------------------------------------------------------
bool DDSImage::ReadDDS(const char *filename)
{
	FILE *fp;
	char magic[4];
	int mipmapFactor;
	long curr, end;
	DDSurfaceDesc ddsd;

	//�@GLEW�̏�����
	if ( glewInit() != GLEW_OK )
	{
		cout << "Error : GLEW�̏������Ɏ��s���܂���\n";
		return false;
	}

	//�@�t�@�C�����J��
	fp = fopen(filename, "rb");
	if ( !fp )
	{
		cout << "Error : �w�肳�ꂽ�t�@�C�����J���܂���ł���\n";
		cout << "File : " << filename << endl;
		return false;
	}

	//�@�}�W�b�N��ǂݎ��
	fread(&magic, sizeof(char), 4, fp);

	//�@�}�W�b�N���`�F�b�N
	if ( strncmp(magic, "DDS ", 4 ) != 0 )
	{
		cout << "Error : DDS�t�@�C���ł͂���܂���\n";
		fclose(fp);
		return false;
	}

	//�@�w�b�_�[��ǂݎ��
	fread(&ddsd, sizeof(ddsd), 1, fp);

	//�@���E�������i�[
	height = ddsd.height;
	width = ddsd.width;
	numMipmaps = ddsd.mipMapLevels;

	//�@�t�H�[�}�b�g����
	switch ( ddsd.format.fourCC )
	{
	case FOURCC_DXT1:
		//�@DXT1
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		internalFormat = 3;
		mipmapFactor = 2;
		break;

	case FOURCC_DXT3:
		//�@DXT3
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		internalFormat = 4;
		mipmapFactor = 4;
		break;

	case FOURCC_DXT5:
		//�@DXT5
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		internalFormat = 4;
		mipmapFactor = 4;
		break;

	default:
		cout << "Error : ���Ή��̌`���ł��BDXT1, DXT3, DXT5�̂����ꂩ���g�p���Ă�������\n";
		return false;
	}

	//�@�e�N�Z���f�[�^�̃T�C�Y���Z�o
	curr = ftell(fp);
	fseek(fp, 0, SEEK_END);
	end = ftell(fp);
	fseek(fp, curr, SEEK_SET);
	imageSize = end - curr;
	imageData = new GLubyte [imageSize];

	//�@�s�N�Z���f�[�^�̓ǂݍ���
	fread(imageData, sizeof(GLubyte), imageSize, fp);

	//�@�t�@�C�������
	fclose(fp);

	return true;
}

//-----------------------------------------------------------------------------------------------------
//�@�@DecompressDDS
//�@�@Desc : S3TC���k�̉𓀍��
//-----------------------------------------------------------------------------------------------------
void DDSImage::DecompressDDS()
{
	int blockSize;
	int offset = 0;
	GLsizei mWidth = width, mHeight = height, mSize = 0;

	//�@DXT1
	if ( format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
		blockSize = 8;
	//�@DXT3, DXT5
	else
		blockSize = 16;

	//�@��
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
//�@�@Load
//�@�@Desc : DDS�t�@�C����ǂݍ��݁C�e�N�X�`���𐶐�����
//-----------------------------------------------------------------------------------------------------
GLuint DDSImage::Load(const char *filename)
{
	//�@�t�@�C���̓ǂݍ���
	if ( !ReadDDS(filename) )
		return 0;

	//�@�e�N�X�`���𐶐�
	glGenTextures(1, &ID);

	//�@�e�N�X�`�����o�C���h
	glBindTexture(GL_TEXTURE_2D, ID);

	//�@�g��E�k��������@�̎w��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	//�@�e�N�X�`�����̐ݒ�
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//�@�𓀍��
	DecompressDDS();

	if ( imageData )
	{
		delete [] imageData;
		imageData = NULL;
	}

	//
	glBindTexture(GL_TEXTURE_2D, 0);

	//�@���������e�N�X�`���ԍ���Ԃ�
	return ID;
}