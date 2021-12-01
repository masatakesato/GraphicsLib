#include	"Texture.h"

#include	<comdef.h>

#include	"../common/GLHelperFunctions.h"



#if defined( FREEIMAGE_SUPPORT )
	// #define	FREEIMAGE_LIB	// needed for static link
	#include	<FreeImage.h>

	#if defined(_DLL)
		#pragma comment( lib, "FreeImage.lib" )
	#else
		#pragma comment( lib, "FreeImage.lib" )
	#endif

//#elif defined(IL_SUPPORT)

	//#include	<IL/il.h>
	//#include	<IL/ilu.h>

	//#if defined(UNICODE) || defined(_UNICODE)
		//#pragma comment(lib, "unicode/DevIL.lib")
		//#pragma comment(lib, "unicode/ILU.lib")
		//#pragma comment(lib, "unicode/ILUT.lib")
	//#else
		//#pragma comment(lib, "DevIL.lib")
		//#pragma comment(lib, "ILU.lib")
		//#pragma comment(lib, "ILUT.lib")
	//#endif

#elif defined ( RESIL_SUPPORT )

	#include	<il/il.h>
	#include	<il/ilu.h>
	//#include	<il/ilut.h>

	#if defined(_DLL)
		#pragma comment(lib, "x64-msvcrt/ResIL.lib")
		#pragma comment(lib, "x64-msvcrt/ILU.lib")
		//#pragma comment(lib, "x64-msvcrt/ILUT.lib")
	#else
		#pragma comment(lib, "x64-libcmt/ResIL.lib")
		#pragma comment(lib, "x64-libcmt/ILU.lib")
		//#pragma comment(lib, "x64-libcmt/ILUT.lib")
	#endif

#endif	// Image Library Support //


#include	"GLDataFormat.h"





namespace OreOreLib
{

#if defined( FREEIMAGE_SUPPORT )

	static FREE_IMAGE_TYPE GetFreeImageType( DATA_FORMAT data_format )
	{
		switch( data_format )
		{
		case DATA_FORMAT::FORMAT_UNKNOWN:// unknown type
			return FIT_UNKNOWN;

		case DATA_FORMAT::FORMAT_R8G8B8_UNORM:// standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
			return FIT_BITMAP;

		case DATA_FORMAT::FORMAT_B8G8R8_UNORM:
			return FIT_BITMAP;

		case DATA_FORMAT::FORMAT_R16_UNORM:// array of unsigned short	: unsigned 16-bit
			return FIT_UINT16;

		case DATA_FORMAT::FORMAT_R16_SINT:// array of short			: signed 16-bit
			return FIT_INT16;

		case DATA_FORMAT::FORMAT_R32_UINT:// array of unsigned long	: unsigned 32-bit
			return FIT_UINT32;

		case DATA_FORMAT::FORMAT_R32_SINT:// array of long			: signed 32-bit
			return FIT_INT32;

		case DATA_FORMAT::FORMAT_R32_FLOAT:// array of float			: 32-bit IEEE floating point
			return FIT_FLOAT;

			//case DATA_FORMAT::FORMAT_UNKNOWN:// array of double			: 64-bit IEEE floating point
			//	return FIT_DOUBLE;

			//case DATA_FORMAT::FORMAT_UNKNOWN:// array of FICOMPLEX		: 2 x 64-bit IEEE floating point
			//	return FIT_COMPLEX;

		case DATA_FORMAT::FORMAT_R16G16B16_UNORM:// 48-bit RGB image			: 3 x 16-bit
			return FIT_RGB16;

		case DATA_FORMAT::FORMAT_R16G16B16_FLOAT:// 48-bit RGB float image			: 3 x 16-bit
			return FIT_RGB16;

		case DATA_FORMAT::FORMAT_R16G16B16A16_UNORM:// 64-bit RGBA image		: 4 x 16-bit
			return FIT_RGBA16;

		case DATA_FORMAT::FORMAT_R16G16B16A16_FLOAT:// 64-bit RGBA float image		: 4 x 16-bit
			return FIT_RGBA16;

		case DATA_FORMAT::FORMAT_R32G32B32_FLOAT:// 96-bit RGB float image	: 3 x 32-bit IEEE floating point
			return FIT_RGBF;

		case DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT:// 128-bit RGBA float image	: 4 x 32-bit IEEE floating point
			return FIT_RGBAF;

		default:// unknown type
			return FIT_UNKNOWN;

		};

	}




	static DATA_FORMAT GetDataFormat( FREE_IMAGE_TYPE fit, int bpp )
	{
		switch( fit )
		{
		case FIT_UNKNOWN:// unknown type
			return DATA_FORMAT::FORMAT_UNKNOWN;

		case FIT_BITMAP:// standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
			if( bpp == 8 )		return DATA_FORMAT::FORMAT_R8_UNORM;
			//else if(bpp == 16)		return DATA_FORMAT::FORMAT_G8R8_UNORM;
			else if( bpp==24 )			return DATA_FORMAT::FORMAT_B8G8R8_UNORM;
			else if( bpp == 32 )	return DATA_FORMAT::FORMAT_B8G8R8A8_UNORM;

			return DATA_FORMAT::FORMAT_B8G8R8A8_UNORM;//return DATA_FORMAT::FORMAT_R8G8B8A8_UNORM;

		case FIT_UINT16:// array of unsigned short	: unsigned 16-bit
			return DATA_FORMAT::FORMAT_R16_UNORM;

		case FIT_INT16:// array of short			: signed 16-bit
			return DATA_FORMAT::FORMAT_R16_SINT;

		case FIT_UINT32:// array of unsigned long	: unsigned 32-bit
			return DATA_FORMAT::FORMAT_R32_UINT;

		case FIT_INT32:// array of long			: signed 32-bit
			return DATA_FORMAT::FORMAT_R32_SINT;

		case FIT_FLOAT:// array of float			: 32-bit IEEE floating point
			return DATA_FORMAT::FORMAT_R32_FLOAT;

		case FIT_DOUBLE:// array of double			: 64-bit IEEE floating point
			return DATA_FORMAT::FORMAT_UNKNOWN;

		case FIT_COMPLEX:// array of FICOMPLEX		: 2 x 64-bit IEEE floating point
			return DATA_FORMAT::FORMAT_UNKNOWN;

		case FIT_RGB16:// 48-bit RGB image			: 3 x 16-bit
			return DATA_FORMAT::FORMAT_R16G16B16_UNORM;

		case FIT_RGBA16:// 64-bit RGBA image		: 4 x 16-bit
			return DATA_FORMAT::FORMAT_R16G16B16A16_UNORM;

		case FIT_RGBF:// 96-bit RGB float image	: 3 x 32-bit IEEE floating point
			return DATA_FORMAT::FORMAT_R32G32B32_FLOAT;

		case FIT_RGBAF:// 128-bit RGBA float image	: 4 x 32-bit IEEE floating point
			return DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT;

		default:// unknown type
			return DATA_FORMAT::FORMAT_UNKNOWN;

		};
	}




	//#elif defined( IL_SUPPORT )
#elif defined( RESIL_SUPPORT )


	static void GetILFormat( DATA_FORMAT data_format, ILenum &ilformat, ILenum &iltype )
	{
		switch( data_format )
		{
			//R32G32B32A32
		case FORMAT_R32G32B32A32_FLOAT:
			ilformat		= IL_RGBA;
			iltype			= IL_FLOAT;
			return;
		case FORMAT_R32G32B32A32_UINT:
			ilformat		= IL_RGBA;
			iltype			= IL_UNSIGNED_INT;
			return;
		case FORMAT_R32G32B32A32_SINT:
			ilformat		= IL_RGBA;
			iltype			= IL_INT;
			return;


			//R16G16B16A16
		case FORMAT_R16G16B16A16_FLOAT:
			ilformat		= IL_RGBA;
			iltype			= IL_FLOAT;
			return;
		case FORMAT_R16G16B16A16_UNORM:
			ilformat		= IL_RGBA;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16G16B16A16_UINT:
			ilformat		= IL_RGBA;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16G16B16A16_SNORM:
			ilformat		= IL_RGBA;
			iltype			= IL_BYTE;
			return;
		case FORMAT_R16G16B16A16_SINT:
			ilformat		= IL_RGBA;
			iltype			= IL_SHORT;
			return;


			//R8G8B8A8
		case FORMAT_R8G8B8A8_UNORM:
			ilformat		= IL_RGBA;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8G8B8A8_UINT:
			ilformat		= IL_RGBA;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8G8B8A8_SNORM:
			ilformat		= IL_RGBA;
			iltype			= IL_BYTE;
			return;
		case FORMAT_R8G8B8A8_SINT:
			ilformat		= IL_RGBA;
			iltype			= IL_BYTE;
			return;


			//R32G32B32
		case FORMAT_R32G32B32_FLOAT:
			ilformat		= GL_RGB;
			iltype			= IL_FLOAT;
			return;
		case FORMAT_R32G32B32_UINT:
			ilformat		= IL_RGB;
			iltype			= IL_UNSIGNED_INT;
			return;
		case FORMAT_R32G32B32_SINT:
			ilformat		= IL_RGB;
			iltype			= IL_INT;
			return;


			//R16G16B16
		case FORMAT_R16G16B16_FLOAT:
			ilformat		= GL_RGB;
			iltype			= IL_FLOAT;
			return;
		case FORMAT_R16G16B16_UNORM:
			ilformat		= GL_RGB;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16G16B16_UINT:
			ilformat		= IL_RGB;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16G16B16_SNORM:
			ilformat		= GL_RGB;
			iltype			= IL_SHORT;
			return;
		case FORMAT_R16G16B16_SINT:
			ilformat		= IL_RGB;
			iltype			= IL_SHORT;
			return;


			//R8G8B8
		case FORMAT_R8G8B8_UNORM:
			ilformat		= GL_RGB;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8G8B8_UINT:
			ilformat		= IL_RGB;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8G8B8_SNORM:
			ilformat		= GL_RGB;
			iltype			= IL_BYTE;
			return;
		case FORMAT_R8G8B8_SINT:
			ilformat		= IL_RGB;
			iltype			= IL_BYTE;
			return;


			//R32G32
		case FORMAT_R32G32_FLOAT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_FLOAT;
			return;
			//case FORMAT_R32G32_UNORM:
			//	return;
		case FORMAT_R32G32_UINT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_UNSIGNED_INT;
			return;
			//case FORMAT_R32G32_SNORM:
			//	return;
		case FORMAT_R32G32_SINT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_INT;
			return;


			//R16G16
		case FORMAT_R16G16_FLOAT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_FLOAT;
			return;
		case FORMAT_R16G16_UNORM:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16G16_UINT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16G16_SNORM:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_SHORT;
			return;
		case FORMAT_R16G16_SINT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_SHORT;
			return;


			//R8G8
			//case FORMAT_R8G8_FLOAT:
			//	return;
		case FORMAT_R8G8_UNORM:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8G8_UINT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8G8_SNORM:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_BYTE;
			return;
		case FORMAT_R8G8_SINT:
			ilformat		= IL_LUMINANCE_ALPHA;
			iltype			= IL_BYTE;
			return;


			//R32
		case FORMAT_R32_FLOAT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_FLOAT;
			return;
			//case FORMAT_R32_UNORM:
			//	return;
		case FORMAT_R32_UINT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_UNSIGNED_INT;
			return;
			//case FORMAT_R32_SNORM:
			//	return;
		case FORMAT_R32_SINT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_INT;
			return;


			//R16
		case FORMAT_R16_FLOAT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_FLOAT;
			return;
		case FORMAT_R16_UNORM:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16_UINT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_UNSIGNED_SHORT;
			return;
		case FORMAT_R16_SNORM:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_SHORT;
			return;
		case FORMAT_R16_SINT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_SHORT;
			return;


			//R8
			//case FORMAT_R8_FLOAT:
			//	return;
		case FORMAT_R8_UNORM:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8_UINT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_UNSIGNED_BYTE;
			return;
		case FORMAT_R8_SNORM:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_BYTE;
			return;
		case FORMAT_R8_SINT:
			ilformat		= IL_LUMINANCE;
			iltype			= IL_BYTE;
			return;

		default:
			return;
		}

	}
#endif	// IL_SUPPORT //









	//##########################################################################################//
	//								Texture1D class implementation								//
	//##########################################################################################//


	// default constructor
	Texture1D::Texture1D()
	{
		texID	= 0;
		size	= 0;
		numdim	= 2;
		m_pData	= NULL;

		texDesc.DataFormat	= DATA_FORMAT::FORMAT_UNKNOWN;
		texDesc.Width		= 0;
		texDesc.MipLevels	= 0;
		texDesc.ArraySize	= 0;
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
	}


	// copy constructor(deep copy)
	Texture1D::Texture1D( const Texture1D& obj )
	{
		texDesc	= obj.texDesc;
		//texID	= obj.texID;
		size	= obj.size;

		// init data
		m_pData	= new uint8[size * texDesc.Width];
		memcpy( m_pData, obj.m_pData, size * texDesc.Width );

		if( obj.texID )	GenHardwareTexture();
	}


	// destructor
	Texture1D::~Texture1D()
	{
		Release();
		//SafeDeleteArray( m_pData );
	}


	// operator
	Texture1D& Texture1D::operator = ( const Texture1D& obj )
	{
		texDesc	= obj.texDesc;
		//texID	= obj.texID;
		size	= obj.size;

		// init data
		m_pData	= new uint8[size * texDesc.Width];
		memcpy( m_pData, obj.m_pData, size * texDesc.Width );

		if( obj.texID )	GenHardwareTexture();

		return *this;
	}


	void Texture1D::Allocate( int width, int mip_levels, int array_size, DATA_FORMAT data_format, const void *pData )
	{
		Release();

		texDesc.DataFormat	= data_format;
		texDesc.Width		= width;
		texDesc.MipLevels	= mip_levels;
		texDesc.ArraySize	= array_size;

		// set texture format
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );

		// set default sampler state
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;

		// initialize data
		AllocateBuffer( pData );
	}


	void Texture1D::SetFilterMode( FILTER_MODE mode )
	{
		GetGLFilterMode( mode, texDesc.MagFilter, texDesc.MinFilter );
	}


	void Texture1D::Load( const TCHAR *filepath, DATA_FORMAT data_format )
	{
		const char* file_name = TStringToChar( tstring(filepath) );

#if defined( FREEIMAGE_SUPPORT )

		uint32 type = 0, num = 3;

		// ファイルの中身を調べてフォーマットを取得する
		FREE_IMAGE_FORMAT	fif	= FreeImage_GetFileType( file_name );

		// フォーマット不明の場合は拡張子から推測する
		if( fif == FIF_UNKNOWN )
			FreeImage_GetFIFFromFilename( file_name );

		// フォーマット不明 or 未対応の場合はファイル読み込みを中止する
		if( fif == FIF_UNKNOWN || FreeImage_FIFSupportsReading( fif ) == FALSE )
		{
			SafeDeleteArray( file_name );
			return;
		}

		// 画像ファイルからメモリ上にデータを読み込む
		FIBITMAP *dib	= FreeImage_Load( fif, file_name );

		// 指定フォーマットに変換する(オレオレフォーマットに対応するFreeImageFormatを取得する)
		DATA_FORMAT target_format;//	= data_format;
		FREE_IMAGE_TYPE	fitype		= FreeImage_GetImageType( dib );
		int bpp						= FreeImage_GetBPP( dib );

		target_format				= GetDataFormat( fitype, bpp );//	 FreeImageから取得したフォーマットをもとにDATA_FORMATを決める


		SafeDeleteArray( file_name );

		//=============== initialize OpenGL Texture ===============//
		Release();

		texDesc.DataFormat	= target_format;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
		size	= SizeOfFormatElement( texDesc.DataFormat );

		texDesc.Width	= FreeImage_GetWidth( dib );
		//texDesc.Height	= FreeImage_GetHeight( dib );


		m_pData	= new uint8[size * texDesc.Width /* texDesc.Height*/];
		memcpy( m_pData, FreeImage_GetBits( dib ), size * texDesc.Width /* texDesc.Height*/ );

		FreeImage_Unload( dib );

		//#elif defined(IL_SUPPORT)
#elif defined( RESIL_SUPPORT )

		//ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		//ilEnable( IL_ORIGIN_SET );

		Release();

		//=============== initialize texture description ===============//
		texDesc.DataFormat	= data_format;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
		size	= SizeOfFormatElement( texDesc.DataFormat );


		//========================= load texture =====================//
		ILenum	ilformat, iltype;
		ILuint	ImgId	= 0;

		ilGenImages( 1, &ImgId );
		ilBindImage( ImgId );
		{
			// load texture file
			if( !ilLoadImage( file_name ) )
			{
				tcerr << _T( "Failed loading " ) << file_name << tendl;
				return;
			}

			// copy texture data
			texDesc.Width	= ilGetInteger( IL_IMAGE_WIDTH );
			GetILFormat( texDesc.DataFormat, ilformat, iltype );

			m_pData	= new uint8[size * texDesc.Width];
			ilCopyPixels( 0, 0, 0, texDesc.Width, 1, 1, ilformat, iltype, m_pData );
		}
		ilBindImage( 0 );
		ilDeleteImage( ImgId );
#endif	// IL_SUPPORT //
	}


	void Texture1D::Write( const TCHAR *filepath )
	{
		const char* file_name = TStringToChar( tstring(filepath) );

#ifdef	IL_SUPPORT
		//ilInit();
		//ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		//ilEnable( IL_ORIGIN_SET );

		// DevIL variables
		ILuint ImgId = 0;

		// Save the image
		ilEnable( IL_FILE_OVERWRITE );
		ilGenImages( 1, &ImgId );
		ilBindImage( ImgId );
		{
			ILenum	format, type;
			GetILFormat( texDesc.DataFormat, format, type );

			ilTexImage( texDesc.Width, 1, 1, texDesc.Channels, format, type, m_pData );

			// Save image
			ilSaveImage( (const ILstring)file_name );
		}
		ilBindImage( 0 );
		ilDeleteImage( ImgId );

#endif	// IL_SUPPORT //

		SafeDeleteArray( file_name );

	}


	void Texture1D::GenHardwareTexture()
	{
		glGenTextures( 1, &texID );

		if( texDesc.ArraySize >= 1 )
		{
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D_ARRAY, texID ) );
			{
				glTexParameteri( GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, texDesc.MagFilter );
				glTexParameteri( GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, texDesc.MinFilter );
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );	// なんかこれやったらクラッシュしないで動く！！！
				GL_SAFE_CALL( glTexImage2D( GL_TEXTURE_1D_ARRAY, texDesc.MipLevels, texDesc.InternalFormat, texDesc.Width, texDesc.ArraySize, 0, texDesc.Format, texDesc.Type, m_pData ) );
			}
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D_ARRAY, 0 ) );
		}
		else
		{
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D, texID ) );
			{
				glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, texDesc.MagFilter );
				glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, texDesc.MinFilter );
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

				for( int miplv=0; miplv <= texDesc.MipLevels; ++miplv )
					GL_SAFE_CALL( glTexImage1D( GL_TEXTURE_1D, miplv, texDesc.InternalFormat, texDesc.Width, 0, texDesc.Format, texDesc.Type, m_pData ) );
			}
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D, 0 ) );
		}

	}


	void Texture1D::Release()
	{
		size	= 0;
		numdim	= 1;
		ReleaseBuffer();

		if( texID )
		{
			GL_SAFE_CALL( glDeleteTextures( 1, &texID ) );
			texID	= 0;
		}

		texDesc.DataFormat	= FORMAT_R16G16B16A16_FLOAT;
		texDesc.Width		= 0;
		texDesc.Channels	= 0;
		texDesc.MipLevels	= 0;
		texDesc.ArraySize	= 0;
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
	}


	void Texture1D::Transfer2GPU()
	{
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D, texID ) );
		GL_SAFE_CALL( glTexSubImage1D( GL_TEXTURE_1D, 0, 0, texDesc.Width, texDesc.Format, texDesc.Type, m_pData ) );
		glBindTexture( GL_TEXTURE_1D, 0 );
	}


	void Texture1D::ReadBack2CPU()
	{
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D, texID ) );
		GL_SAFE_CALL( glGetTexImage( GL_TEXTURE_1D, 0, texDesc.Format, texDesc.Type, m_pData ) );
		glBindTexture( GL_TEXTURE_1D, 0 );
	}


	void Texture1D::Upload2GPU( const TCHAR *filepath, DATA_FORMAT data_format, int &subWidth, bool bNormalize )
	{
		const char* file_name = TStringToChar( tstring(filepath) );;

#ifdef	IL_SUPPORT
		if( !texID )	return;

		uint8 *m_pSubData	= NULL;

		ilInit();
		ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		ilEnable( IL_ORIGIN_SET );

		//====================== Load Texture ========================//
		ILuint ImgId = 0;
		ilGenImages( 1, &ImgId );
		ILenum	format, type;

		ilBindImage( ImgId );
		{
			// load texture file
			if( !ilLoadImage( file_name ) )// DevILの文字列がUnicode未対応. 強制的に型変換
			{
				ILenum err = ilGetError();
				tcerr << _T( "Failed loading... " ) << iluErrorString( err ) << tendl;
				SafeDeleteArray( file_name );
				return;//return 0;
			}
			subWidth		= Min( ilGetInteger( IL_IMAGE_WIDTH ), texDesc.Width );

			GetILFormat( texDesc.DataFormat, format, type );

			//tcout << "subWidth: " << subWidth << tendl;

			// copy pixel data
			m_pSubData	= new uint8[size * subWidth];// サブテクスチャのデータ領域確保もしない

			ilCopyPixels( 0, 0, 0, subWidth, 1, 1, format, type, m_pSubData );

			if( bNormalize==false )
			{
				for( int i=0; i<subWidth*texDesc.Channels; ++i )
					*(float *)( m_pSubData+i*4 ) *= 255.0f;
			}
		}
		ilBindImage( 0 );
		ilDeleteImage( ImgId );


		// bind texture
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_1D, texID ) );

		// transfer to gpu memory
		GL_SAFE_CALL( glTexSubImage1D( GL_TEXTURE_1D, 0, 0, subWidth, texDesc.Format, texDesc.Type, m_pSubData ) );

		// unbind texture again
		glBindTexture( GL_TEXTURE_1D, 0 );


		ReadBack2CPU();

		SafeDeleteArray( m_pSubData );
#endif	// IL_SUPPORT //

		SafeDeleteArray( file_name );
	}


	//void Texture1D::Convert2Luminance( bool bNormalize )
	//{
	//
	//
	//}



	void Texture1D::AllocateBuffer( const void *pData )
	{
		ReleaseBuffer();

		// init data
		size	= SizeOfFormatElement( texDesc.DataFormat );
		int length	= texDesc.Width * Max( texDesc.ArraySize, 1 );
		m_pData	= new uint8[size * length];

		if( !pData )	memset( m_pData, 0, size * length );
		else		memcpy( m_pData, pData, size * length );
	}



	void Texture1D::ReleaseBuffer()
	{
		size	= 0;
		SafeDeleteArray( m_pData );
	}










	//##########################################################################################//
	//								Texture2D class implementation								//
	//##########################################################################################//


	// default constructor
	Texture2D::Texture2D()
	{
		texID	= 0;
		size	= 0;
		numdim	= 2;
		m_pData	= NULL;

		texDesc.DataFormat	= DATA_FORMAT::FORMAT_UNKNOWN;
		texDesc.Width		= 0;
		texDesc.Height		= 0;
		texDesc.MipLevels	= 0;
		texDesc.ArraySize	= 0;
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
		texDesc.WrapS		= GL_CLAMP;
		texDesc.WrapT		= GL_CLAMP;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
	}


	// copy constructor(deep copy)
	Texture2D::Texture2D( const Texture2D& obj )
	{
		texDesc	= obj.texDesc;
		//texID	= obj.texID;
		size	= obj.size;

		// init data
		m_pData	= new uint8[size * texDesc.Width * texDesc.Height];
		memcpy( m_pData, obj.m_pData, size * texDesc.Width * texDesc.Height );

		if( obj.texID )	GenHardwareTexture();
	}


	// destructor
	Texture2D::~Texture2D()
	{
		Release();
	}


	// operator
	Texture2D& Texture2D::operator = ( const Texture2D& obj )
	{
		texDesc	= obj.texDesc;
		//texID	= obj.texID;
		size	= obj.size;

		// init data
		m_pData	= new uint8[size * texDesc.Width * texDesc.Height];
		memcpy( m_pData, obj.m_pData, size * texDesc.Width * texDesc.Height );

		if( obj.texID )	GenHardwareTexture();

		return *this;
	}


	void Texture2D::Allocate( int width, int height, int mip_levels, int array_size, DATA_FORMAT data_format, const void *pData )
	{
		Release();

		texDesc.DataFormat	= data_format;
		texDesc.Width		= width;
		texDesc.Height		= height;
		texDesc.MipLevels	= mip_levels;
		texDesc.ArraySize	= array_size;

		// set texture format
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );

		// set default sampler state
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
		texDesc.WrapS		= GL_CLAMP;
		texDesc.WrapT		= GL_CLAMP;

		// initialize data
		AllocateBuffer( pData );
	}


	void Texture2D::SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt )
	{
		texDesc.WrapS	= GetGLAddressMode( wraps );
		texDesc.WrapT	= GetGLAddressMode( wrapt );
	}


	void Texture2D::SetFilterMode( FILTER_MODE mode )
	{
		GetGLFilterMode( mode, texDesc.MagFilter, texDesc.MinFilter );
	}


	void Texture2D::Load( const TCHAR* filepath, DATA_FORMAT data_format )
	{
		auto aa = tstring(filepath);
		const char* file_name = TCharToChar( aa.c_str(), aa.length() );
	
#if defined( FREEIMAGE_SUPPORT )

		uint32 type = 0, num = 3;

		// ファイルの中身を調べてフォーマットを取得する
		FREE_IMAGE_FORMAT	fif	= FreeImage_GetFileType( file_name );

		// フォーマット不明の場合は拡張子から推測する
		if( fif == FIF_UNKNOWN )
			FreeImage_GetFIFFromFilename( file_name );

		// フォーマット不明 or 未対応の場合はファイル読み込みを中止する
		if( fif == FIF_UNKNOWN || FreeImage_FIFSupportsReading( fif ) == FALSE )
			return;

		// 画像ファイルからメモリ上にデータを読み込む
		FIBITMAP *dib	= FreeImage_Load( fif, file_name );

		// 指定フォーマットに変換する(オレオレフォーマットに対応するFreeImageFormatを取得する)
		DATA_FORMAT target_format;//	= data_format;
		FREE_IMAGE_TYPE	fitype	= FreeImage_GetImageType( dib );
		int bpp					= FreeImage_GetBPP( dib );

		target_format			= GetDataFormat( fitype, bpp );//	 FreeImageから取得したフォーマットをもとにDATA_FORMATを決める

		//=============== initialize OpenGL Texture ===============//
		Release();

		texDesc.DataFormat	= target_format;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
		size	= SizeOfFormatElement( texDesc.DataFormat );

		texDesc.Width	= FreeImage_GetWidth( dib );
		texDesc.Height	= FreeImage_GetHeight( dib );

		m_pData	= new uint8[size * texDesc.Width * texDesc.Height];
		memcpy( m_pData, FreeImage_GetBits( dib ), size * texDesc.Width * texDesc.Height );

		FreeImage_Unload( dib );

		//#elif defined(IL_SUPPORT)
#elif defined( RESIL_SUPPORT )


		//	ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		//	ilEnable( IL_ORIGIN_SET );

		Release();

		//=============== initialize texture description ===============//
		texDesc.DataFormat	= data_format;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
		size	= SizeOfFormatElement( texDesc.DataFormat );


		//========================= load texture =====================//
		ILenum	ilformat, iltype;
		ILuint	ImgId	= 0;

		ilGenImages( 1, &ImgId );
		ilBindImage( ImgId );

		ILenum err = ilGetError();

		{
			// load texture file
			if( !ilLoadImage( file_name ) )
			{
				ILenum err = ilGetError();
				tcerr << _T( "Failed loading... " ) << iluErrorString( err ) << tendl;
				return;
			}

			// copy texture data
			texDesc.Width	= ilGetInteger( IL_IMAGE_WIDTH );
			texDesc.Height	= ilGetInteger( IL_IMAGE_HEIGHT );
			GetILFormat( texDesc.DataFormat, ilformat, iltype );

			m_pData	= new uint8[size * texDesc.Width * texDesc.Height];
			ilCopyPixels( 0, 0, 0, texDesc.Width, texDesc.Height, 1, ilformat, iltype, m_pData );
		}
		ilBindImage( 0 );
		ilDeleteImage( ImgId );

#endif	// IL_SUPPORT //

		SafeDeleteArray( file_name );

	}


	void Texture2D::Write( const TCHAR *filepath )
	{
		const char* file_name = TStringToChar( tstring(filepath) );

#if defined( FREEIMAGE_SUPPORT )

		//TODO: 出力フォーマットに合わせてデータ変換が必要

		// ファイル名称からフォーマットを調べる
		FREE_IMAGE_FORMAT fif	= FreeImage_GetFIFFromFilename( file_name );

		// フォーマット不明 or 未対応フォーマットの場合は処理を中止する
		if( fif == FIF_UNKNOWN || FreeImage_FIFSupportsReading( fif ) == FALSE )
		{
			SafeDeleteArray( file_name );
			return;
		}

		FREE_IMAGE_TYPE fit	= GetFreeImageType( texDesc.DataFormat );
		int pixel_bytesize	= SizeOfFormatElement( texDesc.DataFormat );
		FIBITMAP* dib = FreeImage_AllocateT( fit, texDesc.Width, texDesc.Height, pixel_bytesize * 8 );
		
		uint32 bpp = FreeImage_GetBPP( dib );
		memcpy( FreeImage_GetBits( dib ), m_pData, texDesc.Width * texDesc.Height * pixel_bytesize );
		
		FreeImage_Save( fif, dib, file_name );

//#elif defined( IL_SUPPORT )

#elif defined( RESIL_SUPPORT )

		//ilInit();
		//ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		//ilEnable( IL_ORIGIN_SET );

		// DevIL variables
		ILuint ImgId	= 0;

		// Save the image
		ilEnable( IL_FILE_OVERWRITE );
		ilGenImages( 1, &ImgId );
		ilBindImage( ImgId );
		{
			ILenum	format, type;
			GetILFormat( texDesc.DataFormat, format, type );

			ilTexImage( texDesc.Width, texDesc.Height, 1, texDesc.Channels, format, type, m_pData );

			// Save image
			ilSaveImage( (const ILstring)file_name );
		}
		ilBindImage( 0 );
		ilDeleteImage( ImgId );

		//ILenum ilError = ilGetError( );
		//const char *err = ilGetString( ilError );

#endif	// IL_SUPPORT //

		SafeDeleteArray( file_name );
	}


	void Texture2D::GenHardwareTexture()
	{
		glGenTextures( 1, &texID );

		if( texDesc.ArraySize >= 1 )
		{
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, texID ) );
			{
				glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, texDesc.MagFilter );
				glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, texDesc.MinFilter );
				glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, texDesc.WrapS );
				glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, texDesc.WrapT );
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );	// なんかこれやったらクラッシュしないで動く！！！
				glTexImage3D( GL_TEXTURE_2D_ARRAY, texDesc.MipLevels, texDesc.InternalFormat, texDesc.Width, texDesc.Height, texDesc.ArraySize, 0, texDesc.Format, texDesc.Type, m_pData );
			}
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, 0 ) );
		}
		else
		{
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texID ) );
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texDesc.MagFilter );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texDesc.MinFilter );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texDesc.WrapS );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texDesc.WrapT );
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );	// なんかこれやったらクラッシュしないで動く！！！
				for( int miplv=0; miplv <= texDesc.MipLevels; ++miplv )
					glTexImage2D( GL_TEXTURE_2D, miplv, texDesc.InternalFormat, texDesc.Width, texDesc.Height, 0, texDesc.Format, texDesc.Type, m_pData );
			}
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, 0 ) );
		}

	}


	void Texture2D::Release()
	{
		size	= 0;
		numdim	= 2;
		ReleaseBuffer();

		if( texID )
		{
			GL_SAFE_CALL( glDeleteTextures( 1, &texID ) );
			texID	= 0;
		}

		texDesc.DataFormat	= DATA_FORMAT::FORMAT_UNKNOWN;
		texDesc.Width		= 0;
		texDesc.Height		= 0;
		texDesc.Channels	= 0;
		texDesc.MipLevels	= 0;
		texDesc.ArraySize	= 0;
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
		texDesc.WrapS		= GL_CLAMP;
		texDesc.WrapT		= GL_CLAMP;

	}


	void Texture2D::Transfer2GPU()
	{
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texID ) );
		GL_SAFE_CALL( glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, texDesc.Width, texDesc.Height, texDesc.Format, texDesc.Type, m_pData ) );
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, 0 ) );
	}


	void Texture2D::ReadBack2CPU()
	{
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, texID ) );
		GL_SAFE_CALL( glGetTexImage( GL_TEXTURE_2D, 0, texDesc.Format, texDesc.Type, m_pData ) );
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, 0 ) );
	}


	void Texture2D::Upload2GPU( const TCHAR *filepath, DATA_FORMAT data_format, int &subWidth, int &subHeight, bool bNormalize )
	{
		if( !texID )
			return;

		uint8 *m_pSubData = NULL;
		const char* file_name = TStringToChar( tstring(filepath) );

#if defined(FREEIMAGE_SUPPORT)

		uint32 type = 0, num = 3;

		// ファイルの中身を調べてフォーマットを取得する
		FREE_IMAGE_FORMAT	fif = FreeImage_GetFileType(file_name);

		// フォーマット不明の場合は拡張子から推測する
		if (fif == FIF_UNKNOWN)
			FreeImage_GetFIFFromFilename(file_name);

		// フォーマット不明 or 未対応の場合はファイル読み込みを中止する
		if (fif == FIF_UNKNOWN || FreeImage_FIFSupportsReading(fif) == FALSE)
		{
			SafeDeleteArray( file_name );
			return;
		}

		// 画像ファイルからメモリ上にデータを読み込む
		FIBITMAP *dib = FreeImage_Load(fif, file_name);

		// 指定フォーマットに変換する(オレオレフォーマットに対応するFreeImageFormatを取得する)
		DATA_FORMAT target_format;//	= data_format;
		FREE_IMAGE_TYPE	fitype = FreeImage_GetImageType(dib);
		int bpp = FreeImage_GetBPP(dib);

		target_format = GetDataFormat(fitype, bpp);//	 FreeImageから取得したフォーマットをもとにDATA_FORMATを決める

		texDesc.DataFormat = target_format;
		GetGLFormat(texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels);
		size = SizeOfFormatElement(texDesc.DataFormat);

		texDesc.Width = FreeImage_GetWidth(dib);
		texDesc.Height = FreeImage_GetHeight(dib);

		m_pSubData = new uint8[size * texDesc.Width * texDesc.Height];
		memcpy(m_pSubData, FreeImage_GetBits(dib), size * texDesc.Width * texDesc.Height);


		FreeImage_Unload( dib );

#elif defined(IL_SUPPORT)

		ilInit();
		ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		ilEnable( IL_ORIGIN_SET );

		//====================== Load Texture ========================//
		ILuint ImgId = 0;
		ilGenImages( 1, &ImgId );
		ILenum	format, type;

		ilBindImage( ImgId );
		{
			// load texture file
			if( !ilLoadImage( file_name ) )// DevILの文字列がUnicode未対応. 強制的に型変換
			{
				tcerr << _T("Failed loading ") << file_name << tendl;
				SafeDeleteArray( file_name );
				return;//return 0;
			}
			subWidth		= Min( ilGetInteger( IL_IMAGE_WIDTH ), texDesc.Width );
			subHeight		= Min( ilGetInteger( IL_IMAGE_HEIGHT ), texDesc.Height );

			GetILFormat( texDesc.DataFormat, format, type );

			//tcout << _T("subWidth: ") << subWidth << tendl;
			//tcout << _T("subHeight: ") << subHeight << tendl;

			// copy pixel data
			m_pSubData	= new uint8[size * subWidth * subHeight];// サブテクスチャのデータ領域確保もしない

			ilCopyPixels( 0, 0, 0, subWidth, subHeight, 1, format, type, m_pSubData );

			if( bNormalize==false )
			{
				for( int i=0; i<subWidth*subHeight*texDesc.Channels; ++i )
					*(float *)( m_pSubData+i*4 ) *= 255.0f;
			}
		}
		ilBindImage( 0 );
		ilDeleteImage( ImgId );

#endif	// IL_SUPPORT //

		// bind texture
		GL_SAFE_CALL(glBindTexture(GL_TEXTURE_2D, texID));

		// transfer to gpu memory
		GL_SAFE_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, subWidth, subHeight, texDesc.Format, texDesc.Type, m_pSubData));

		// unbind texture again
		glBindTexture(GL_TEXTURE_2D, 0);

		ReadBack2CPU();

		SafeDeleteArray(m_pSubData);
		SafeDeleteArray( file_name );

	}


	//void Texture2D::Convert2Luminance( bool bNormalize )
	//{
	//
	//
	//}



	void Texture2D::AllocateBuffer( const void *pData )
	{
		ReleaseBuffer();

		// init data
		size	= SizeOfFormatElement( texDesc.DataFormat );
		int length	= texDesc.Width * texDesc.Height * Max( texDesc.ArraySize, 1 );
		m_pData	= new uint8[ size * length ];

		if( !pData )	memset( m_pData, 0, size * length );
		else		memcpy( m_pData, pData, size * length );
	}



	void Texture2D::ReleaseBuffer()
	{
		size	= 0;
		SafeDeleteArray( m_pData );
	}






	//##########################################################################################//
	//								Texture3D class implementation								//
	//##########################################################################################//


	// default constructor
	Texture3D::Texture3D()
	{
		texID	= 0;
		size	= 0;
		numdim	= 3;
		m_pData	= NULL;

		texDesc.DataFormat	= DATA_FORMAT::FORMAT_UNKNOWN;
		texDesc.Width		= 0;
		texDesc.Height		= 0;
		texDesc.Depth		= 0;
		texDesc.MipLevels	= 0;
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
		texDesc.WrapS		= GL_CLAMP;
		texDesc.WrapT		= GL_CLAMP;
		texDesc.WrapR		= GL_CLAMP;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
	}


	// copy constructor(deep copy)
	Texture3D::Texture3D( const Texture3D& obj )
	{
		texDesc	= obj.texDesc;
		//texID	= obj.texID;
		size	= obj.size;

		// init data
		m_pData	= new uint8[size * texDesc.Width * texDesc.Height * texDesc.Depth];
		memcpy( m_pData, obj.m_pData, size * texDesc.Width * texDesc.Height * texDesc.Depth );

		if( obj.texID )	GenHardwareTexture();
	}


	// destructor
	Texture3D::~Texture3D()
	{
		Release();
		//SafeDeleteArray( m_pData );
	}


	// operator
	Texture3D& Texture3D::operator = ( const Texture3D& obj )
	{
		texDesc	= obj.texDesc;
		//texID	= obj.texID;
		size	= obj.size;

		// init data
		m_pData	= new uint8[size * texDesc.Width * texDesc.Height * texDesc.Depth];
		memcpy( m_pData, obj.m_pData, size * texDesc.Width * texDesc.Height * texDesc.Depth );

		if( obj.texID )	GenHardwareTexture();

		return *this;
	}


	void Texture3D::Allocate( int width, int height, int depth, int mip_levels, DATA_FORMAT data_format, const void *pData )
	{
		Release();

		texDesc.DataFormat	= data_format;
		texDesc.Width		= width;
		texDesc.Height		= height;
		texDesc.Depth		= depth;
		texDesc.MipLevels	= mip_levels;

		// set texture format
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );

		// set default sampler state
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
		texDesc.WrapS		= GL_CLAMP;
		texDesc.WrapT		= GL_CLAMP;
		texDesc.WrapR		= GL_CLAMP;


		// initialize data
		AllocateBuffer( pData );
	}


	void Texture3D::SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt, TEXTURE_ADDRESS_MODE wrapr )
	{
		texDesc.WrapS	= GetGLAddressMode( wraps );
		texDesc.WrapT	= GetGLAddressMode( wrapt );
		texDesc.WrapR	= GetGLAddressMode( wrapr );
	}


	void Texture3D::SetFilterMode( FILTER_MODE mode )
	{
		GetGLFilterMode( mode, texDesc.MagFilter, texDesc.MinFilter );
	}


	void Texture3D::Load( const TCHAR *filepath, DATA_FORMAT data_format )
	{
#if defined(FREEIMAGE_SUPPORT)



#elif defined(IL_SUPPORT)

		//ilInit();
		//ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		//ilEnable( IL_ORIGIN_SET );

		Release();

		//=============== initialize texture description ===============//
		texDesc.DataFormat	= data_format;
		GetGLFormat( texDesc.DataFormat, texDesc.InternalFormat, texDesc.Format, texDesc.Type, texDesc.Channels );
		size	= SizeOfFormatElement( texDesc.DataFormat );


		//========================= load texture =====================//
		ILenum	ilformat, iltype;
		ILuint	ImgId	= 0;

		ilGenImages( 1, &ImgId );
		ilBindImage( ImgId );
		{
			// load texture file
			if( !ilLoadImage( filepath ) )
			{
				tcerr << _T( "Failed loading " ) << filepath << tendl;
				return;
			}

			// copy texture data
			texDesc.Width	= ilGetInteger( IL_IMAGE_WIDTH );
			texDesc.Height	= ilGetInteger( IL_IMAGE_HEIGHT );
			texDesc.Depth	= ilGetInteger( IL_IMAGE_DEPTH );
			GetILFormat( texDesc.DataFormat, ilformat, iltype );

			m_pData	= new uint8[size * texDesc.Width * texDesc.Height * texDesc.Depth];
			ilCopyPixels( 0, 0, 0, texDesc.Width, texDesc.Height, texDesc.Depth, ilformat, iltype, m_pData );
		}
		ilBindImage( 0 );
		ilDeleteImage( ImgId );

#endif	// IL_SUPPORT //
	}


	void Texture3D::Write( const TCHAR *filepath )
	{


	}


	void Texture3D::GenHardwareTexture()
	{
		GL_SAFE_CALL( glGenTextures( 1, &texID ) );

		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, texID ) );
		{
			glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, texDesc.MagFilter );
			glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, texDesc.MinFilter );
			glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, texDesc.WrapS );
			glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, texDesc.WrapT );
			glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, texDesc.WrapR );

			//glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

			for( int miplv=0; miplv <= texDesc.MipLevels; ++miplv )
			{
				GL_SAFE_CALL( glTexImage3D( GL_TEXTURE_3D, miplv, texDesc.InternalFormat, texDesc.Width / powf( 2, (float)miplv ), texDesc.Height / powf( 2, (float)miplv ), texDesc.Depth / powf( 2, (float)miplv ), 0, texDesc.Format, texDesc.Type, m_pData ) );
			}

		}
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, 0 ) );
	}


	void Texture3D::Release()
	{
		size	= 0;
		numdim	= 3;
		ReleaseBuffer();

		if( texID )
		{
			GL_SAFE_CALL( glDeleteTextures( 1, &texID ) );
			texID	= 0;
		}

		texDesc.DataFormat	= FORMAT_R16G16B16A16_FLOAT;
		texDesc.Width		= 0;
		texDesc.Height		= 0;
		texDesc.Depth		= 0;
		texDesc.Channels	= 0;
		texDesc.MipLevels	= 0;
		texDesc.MagFilter	= GL_NEAREST;
		texDesc.MinFilter	= GL_NEAREST;
		texDesc.WrapS		= GL_CLAMP;
		texDesc.WrapT		= GL_CLAMP;
		texDesc.WrapR		= GL_CLAMP;
	}


	void Texture3D::Transfer2GPU()
	{
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, texID ) );
		GL_SAFE_CALL( glTexSubImage3D( GL_TEXTURE_3D, 0, 0, 0, 0, texDesc.Width, texDesc.Height, texDesc.Depth, texDesc.Format, texDesc.Type, m_pData ) );
		glBindTexture( GL_TEXTURE_3D, 0 );
	}


	void Texture3D::ReadBack2CPU()
	{
		GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, texID ) );
		GL_SAFE_CALL( glGetTexImage( GL_TEXTURE_3D, 0, texDesc.Format, texDesc.Type, m_pData ) );
		glBindTexture( GL_TEXTURE_3D, 0 );
	}


	//void Texture3D::Upload2GPU( const char *filepath, DATA_FORMAT data_format, int &subWidth, int &subHeight, int &subDepth, bool bNormalize )
	//{
	//
	//}


	//void Texture3D::Convert2Luminance( bool bNormalize )
	//{
	//
	//
	//}



	void Texture3D::AllocateBuffer( const void *pData )
	{
		ReleaseBuffer();

		// init data
		size	= SizeOfFormatElement( texDesc.DataFormat );
		int length	= texDesc.Width * texDesc.Height * texDesc.Depth;
		m_pData	= new uint8[size * length];

		if( !pData )	memset( m_pData, 0, size * length );
		else		memcpy( m_pData, pData, size * length );
	}



	void Texture3D::ReleaseBuffer()
	{
		size	= 0;
		SafeDeleteArray( m_pData );
	}









#if defined( FREEIMAGE_SUPPORT )

	void InitFreeImage()
	{
		FreeImage_Initialise();
	}


	void DeinitFreeImage()
	{
		FreeImage_DeInitialise();
	}



	//#elif defined( IL_SUPPORT )
#elif defined( RESIL_SUPPORT )

	void InitIL()
	{
		ilInit();
		iluInit();

		//	ilOriginFunc( IL_ORIGIN_LOWER_LEFT );
		//	ilEnable( IL_ORIGIN_SET );
	}

#endif









}// end of namspace
