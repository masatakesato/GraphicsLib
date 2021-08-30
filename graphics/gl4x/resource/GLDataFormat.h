#ifndef GL_FORMAT_H
#define	GL_FORMAT_H


#include <GL/glew.h>
#pragma comment(lib, "glew32.lib")
#ifdef _DEBUG
#include <GL/glut.h>
#endif


#include	"BufferLayout.h"


namespace OreOreLib
{

	struct GLBufferDesc
	{
		GLuint	target;
		GLsizei	buffer_size;
		GLenum	usage;
		//GLsizei	element_size;
	};



	struct GLAttribDesc
	{
		GLuint	attribLocation;
		uint32	type;
		uint8	channels;
		GLsizei	element_size;
		bool	bNormalized;
		int		stride;
		int		offset;
	};



	// Input Element Description
	struct InputElementDesc
	{
		GLuint		AttribLocation;	// IRender::POSIATION, etc...
		DATA_FORMAT	Format;			// DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT, etc...
		bool		bNormalized;	// normalize flag
		int			Stride;			// stride
		int			Offset;
	};



	static void GetGLFormat( DATA_FORMAT dataFormat, int &internalFormat, uint32 &format, uint32 &type, uint8 &channels )
	{

		//============== identify opengl texture format ===========//
		switch( dataFormat )
		{
			//R32G32B32A32
		case FORMAT_R32G32B32A32_FLOAT:
			internalFormat	= GL_RGBA32F;
			format			= GL_RGBA;
			type			= GL_FLOAT;
			channels		= 4;
			return;
		case FORMAT_R32G32B32A32_UINT:
			internalFormat	= GL_RGBA32UI;
			format			= GL_RGBA_INTEGER;
			type			= GL_UNSIGNED_INT;
			channels		= 4;
			return;
		case FORMAT_R32G32B32A32_SINT:
			internalFormat	= GL_RGBA32I;
			format			= GL_RGBA_INTEGER;
			type			= GL_INT;
			channels		= 4;
			return;


			//R16G16B16A16
		case FORMAT_R16G16B16A16_FLOAT:
			internalFormat	= GL_RGBA16F;
			format			= GL_RGBA;
			type			= GL_HALF_FLOAT;
			channels		= 4;
			return;
		case FORMAT_R16G16B16A16_UNORM:
			internalFormat	= GL_RGBA16;
			format			= GL_RGBA;
			type			= GL_UNSIGNED_SHORT;
			channels		= 4;
			return;
		case FORMAT_R16G16B16A16_UINT:
			internalFormat	= GL_RGBA16UI;
			format			= GL_RGBA_INTEGER;
			type			= GL_UNSIGNED_SHORT;
			channels		= 4;
			return;
		case FORMAT_R16G16B16A16_SNORM:
			internalFormat	= GL_RGBA16_SNORM;
			format			= GL_RGBA;
			type			= GL_BYTE;
			channels		= 4;
			return;
		case FORMAT_R16G16B16A16_SINT:
			internalFormat	= GL_RGBA16I;
			format			= GL_RGBA_INTEGER;
			type			= GL_SHORT;
			channels		= 4;
			return;


			//R8G8B8A8
		case FORMAT_R8G8B8A8_UNORM:
			internalFormat	= GL_RGBA8;
			format			= GL_RGBA;
			type			= GL_UNSIGNED_BYTE;
			channels		= 4;
			return;
		case FORMAT_R8G8B8A8_UINT:
			internalFormat	= GL_RGBA8UI;
			format			= GL_RGBA_INTEGER;
			type			= GL_UNSIGNED_BYTE;
			channels		= 4;
			return;
		case FORMAT_R8G8B8A8_SNORM:
			internalFormat	= GL_RGBA8_SNORM;
			format			= GL_RGBA;
			type			= GL_BYTE;
			channels		= 4;
			return;
		case FORMAT_R8G8B8A8_SINT:
			internalFormat	= GL_RGBA8I;
			format			= GL_RGBA_INTEGER;
			type			= GL_BYTE;
			channels		= 4;
			return;


			//B8G8R8A8
		case FORMAT_B8G8R8A8_UNORM:
			internalFormat	= GL_RGBA8;
			format			= GL_BGRA;
			type			= GL_UNSIGNED_BYTE;
			channels		= 4;
			return;


			//R32G32B32
		case FORMAT_R32G32B32_FLOAT:
			internalFormat	= GL_RGB32F;
			format			= GL_RGB;
			type			= GL_FLOAT;
			channels		= 3;
			return;
		case FORMAT_R32G32B32_UINT:
			internalFormat	= GL_RGB32UI;
			format			= GL_RGB_INTEGER;
			type			= GL_UNSIGNED_INT;
			channels		= 3;
			return;
		case FORMAT_R32G32B32_SINT:
			internalFormat	= GL_RGB32I;
			format			= GL_RGB_INTEGER;
			type			= GL_INT;
			channels		= 3;
			return;


			//R16G16B16
		case FORMAT_R16G16B16_FLOAT:
			internalFormat	= GL_RGB16F;
			format			= GL_RGB;
			type			= GL_HALF_FLOAT;
			channels		= 3;
			return;
		case FORMAT_R16G16B16_UNORM:
			internalFormat	= GL_RGB16;
			format			= GL_RGB;
			type			= GL_UNSIGNED_SHORT;
			channels		= 3;
			return;
		case FORMAT_R16G16B16_UINT:
			internalFormat	= GL_RGB16UI;
			format			= GL_RGB_INTEGER;
			type			= GL_UNSIGNED_SHORT;
			channels		= 3;
			return;
		case FORMAT_R16G16B16_SNORM:
			internalFormat	= GL_RGB16_SNORM;
			format			= GL_RGB;
			type			= GL_SHORT;
			channels		= 3;
			return;
		case FORMAT_R16G16B16_SINT:
			internalFormat	= GL_RGB16I;
			format			= GL_RGB_INTEGER;
			type			= GL_SHORT;
			channels		= 3;
			return;


			//R8G8B8
		case FORMAT_R8G8B8_UNORM:
			internalFormat	= GL_RGB8;
			format			= GL_RGB;
			type			= GL_UNSIGNED_BYTE;
			channels		= 3;
			return;
		case FORMAT_R8G8B8_UINT:
			internalFormat	= GL_RGB8UI;
			format			= GL_RGB_INTEGER;
			type			= GL_UNSIGNED_BYTE;
			channels		= 3;
			return;
		case FORMAT_R8G8B8_SNORM:
			internalFormat	= GL_RGB8_SNORM;
			format			= GL_RGB;
			type			= GL_BYTE;
			channels		= 3;
			return;
		case FORMAT_R8G8B8_SINT:
			internalFormat	= GL_RGB8I;
			format			= GL_RGB_INTEGER;
			type			= GL_BYTE;
			channels		= 3;
			return;


			// B8G8R8
		case FORMAT_B8G8R8_UNORM:
			internalFormat	= GL_RGB8;
			format			= GL_BGR;
			type			= GL_UNSIGNED_BYTE;
			channels		= 3;
			return;



			//R32G32
		case FORMAT_R32G32_FLOAT:
			internalFormat	= GL_RG32F;
			format			= GL_RG;
			type			= GL_FLOAT;
			channels		= 2;
			return;
			//case FORMAT_R32G32_UNORM:
			//	return;
		case FORMAT_R32G32_UINT:
			internalFormat	= GL_RG32UI;
			format			= GL_RG_INTEGER;
			type			= GL_UNSIGNED_INT;
			channels		= 2;
			return;
			//case FORMAT_R32G32_SNORM:
			//	return;
		case FORMAT_R32G32_SINT:
			internalFormat	= GL_RG32I;
			format			= GL_RG_INTEGER;
			type			= GL_INT;
			channels		= 2;
			return;


			//R16G16
		case FORMAT_R16G16_FLOAT:
			internalFormat	= GL_RG16F;
			format			= GL_RG;
			type			= GL_HALF_FLOAT;
			channels		= 2;
			return;
		case FORMAT_R16G16_UNORM:
			internalFormat	= GL_RG16;
			format			= GL_RG;
			type			= GL_UNSIGNED_SHORT;
			channels		= 2;
			return;
		case FORMAT_R16G16_UINT:
			internalFormat	= GL_RG16UI;
			format			= GL_RG_INTEGER;
			type			= GL_UNSIGNED_SHORT;
			channels		= 2;
			return;
		case FORMAT_R16G16_SNORM:
			internalFormat	= GL_RG16_SNORM;
			format			= GL_RG;
			type			= GL_SHORT;
			channels		= 2;
			return;
		case FORMAT_R16G16_SINT:
			internalFormat	= GL_RG16I;
			format			= GL_RG_INTEGER;
			type			= GL_SHORT;
			channels		= 2;
			return;


			//R8G8
			//case FORMAT_R8G8_FLOAT:
			//	return;
		case FORMAT_R8G8_UNORM:
			internalFormat	= GL_RG8;
			format			= GL_RG;
			type			= GL_UNSIGNED_BYTE;
			channels		= 2;
			return;
		case FORMAT_R8G8_UINT:
			internalFormat	= GL_RG8UI;
			format			= GL_RG_INTEGER;
			type			= GL_UNSIGNED_BYTE;
			channels		= 2;
			return;
		case FORMAT_R8G8_SNORM:
			internalFormat	= GL_RG8_SNORM;
			format			= GL_RG;
			type			= GL_BYTE;
			channels		= 2;
			return;
		case FORMAT_R8G8_SINT:
			internalFormat	= GL_RG8I;
			format			= GL_RG_INTEGER;
			type			= GL_BYTE;
			channels		= 2;
			return;


			//R32
		case FORMAT_D32_FLOAT:
			internalFormat	= GL_DEPTH_COMPONENT32F;
			format			= GL_DEPTH_COMPONENT;
			type			= GL_FLOAT;
			channels		= 1;
			return;
		case FORMAT_R32_FLOAT:
			internalFormat	= GL_R32F;
			format			= GL_RED;
			type			= GL_FLOAT;
			channels		= 1;
			return;
			//case FORMAT_R32_UNORM:
			//	return;
		case FORMAT_R32_UINT:
			internalFormat	= GL_R32UI;
			format			= GL_RED_INTEGER;
			type			= GL_UNSIGNED_INT;
			channels		= 1;
			return;
			//case FORMAT_R32_SNORM:
			//	return;
		case FORMAT_R32_SINT:
			internalFormat	= GL_R32I;
			format			= GL_RED_INTEGER;
			type			= GL_INT;
			channels		= 1;
			return;


			//R16
		case FORMAT_R16_FLOAT:
			internalFormat	= GL_R16F;
			format			= GL_RED;
			type			= GL_HALF_FLOAT;
			channels		= 1;
			return;
		case FORMAT_D16_UNORM:
			internalFormat	= GL_DEPTH_COMPONENT16;
			format			= GL_DEPTH_COMPONENT;
			type			= GL_UNSIGNED_SHORT;
			channels		= 1;
			return;
		case FORMAT_R16_UNORM:
			internalFormat	= GL_R16;
			format			= GL_RED;
			type			= GL_UNSIGNED_SHORT;
			channels		= 1;
			return;
		case FORMAT_R16_UINT:
			internalFormat	= GL_R16UI;
			format			= GL_RED_INTEGER;
			type			= GL_UNSIGNED_SHORT;
			channels		= 1;
			return;
		case FORMAT_R16_SNORM:
			internalFormat	= GL_R16_SNORM;
			format			= GL_RED;
			type			= GL_SHORT;
			channels		= 1;
			return;
		case FORMAT_R16_SINT:
			internalFormat	= GL_R16I;
			format			= GL_RED_INTEGER;
			type			= GL_SHORT;
			channels		= 1;
			return;


			//R8
			//case FORMAT_R8_FLOAT:
			//	return;
		case FORMAT_R8_UNORM:
			internalFormat	= GL_R8;
			format			= GL_RED;
			type			= GL_UNSIGNED_BYTE;
			channels		= 1;
			return;
		case FORMAT_R8_UINT:
			internalFormat	= GL_R8UI;
			format			= GL_RED_INTEGER;
			type			= GL_UNSIGNED_BYTE;
			channels		= 1;
			return;
		case FORMAT_R8_SNORM:
			internalFormat	= GL_R8_SNORM;
			format			= GL_RED;
			type			= GL_BYTE;
			channels		= 1;
			return;
		case FORMAT_R8_SINT:
			internalFormat	= GL_R8I;
			format			= GL_RED_INTEGER;
			type			= GL_BYTE;
			channels		= 1;
			return;

		case FORMAT_D24_UNORM_S8_UINT:
			internalFormat	= GL_DEPTH24_STENCIL8;
			format			= GL_DEPTH_STENCIL;
			type			= GL_UNSIGNED_INT_24_8;
			channels		= 2;
			return;

			//default format
		default:
			internalFormat	= GL_RGBA16F;
			format			= GL_RGBA;
			type			= GL_HALF_FLOAT;
			channels		= 4;
			return;
		}

	}




	static GLint GetGLAddressMode( TEXTURE_ADDRESS_MODE wrap )
	{
		switch( wrap )
		{
		case TEX_ADDRESS_WRAP:		return GL_REPEAT;
		case TEX_ADDRESS_MIRROR:	return GL_MIRRORED_REPEAT;
		case TEX_ADDRESS_CLAMP:		return GL_CLAMP;
		case TEX_ADDRESS_BORDER:	return GL_CLAMP_TO_BORDER;
			//case TEX_ADDRESS_MIRROR_ONCE:
		default:					return GL_CLAMP;
		}
	}



	static void GetGLFilterMode( int mode, GLint &mag_filter, GLint &min_filter )
	{
		switch( mode )
		{
		case FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_NEAREST:
			mag_filter = GL_NEAREST;
			min_filter = GL_NEAREST_MIPMAP_NEAREST;
			return;
		case FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_NEAREST:
			mag_filter = GL_NEAREST;
			min_filter = GL_LINEAR_MIPMAP_NEAREST;
			return;
		case FILTER_MAG_NEAREST_MIN_NEAREST_MIPMAP_LINEAR:
			mag_filter = GL_NEAREST;
			min_filter = GL_NEAREST_MIPMAP_LINEAR;
			return;
		case FILTER_MAG_NEAREST_MIN_LINEAR_MIPMAP_LINEAR:
			mag_filter = GL_NEAREST;
			min_filter = GL_LINEAR_MIPMAP_LINEAR;
			return;
		case FILTER_MAG_MIN_NEAREST:
			mag_filter = GL_NEAREST;
			min_filter = GL_NEAREST;
			return;
		case FILTER_MAG_NEAREST_MIN_LINEAR:
			mag_filter = GL_NEAREST;
			min_filter = GL_LINEAR;
			return;
		case FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_NEAREST:
			mag_filter = GL_LINEAR;
			min_filter = GL_NEAREST_MIPMAP_NEAREST;
			return;
		case FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_NEAREST:
			mag_filter = GL_LINEAR;
			min_filter = GL_LINEAR_MIPMAP_NEAREST;
			return;
		case FILTER_MAG_LINEAR_MIN_NEAREST_MIPMAP_LINEAR:
			mag_filter = GL_LINEAR;
			min_filter = GL_NEAREST_MIPMAP_LINEAR;
			return;
		case FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_LINEAR:
			mag_filter = GL_LINEAR;
			min_filter = GL_LINEAR_MIPMAP_LINEAR;
			return;
		case FILTER_MAG_LINEAR_MIN_NEAREST:
			mag_filter = GL_LINEAR;
			min_filter = GL_NEAREST;
			return;
		case FILTER_MAG_MIN_LINEAR:
			mag_filter = GL_LINEAR;
			min_filter = GL_LINEAR;
			return;
		default:
			mag_filter = GL_NEAREST;
			min_filter = GL_NEAREST;
			return;
		}


	}


}// end of namespace



#endif // !GL_FORMAT_H
