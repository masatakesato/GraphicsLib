#ifndef	TEXTURE_H
#define	TEXTURE_H

#include	<GL/glew.h>
#include	<GL/gl.h>
#pragma comment(lib, "glew32.lib")
#pragma comment (lib, "opengl32.lib")


#include	"./common/Utility.h"
#include	"./common/TString.h"
#include	"BufferLayout.h"

// TODO: サンプラーステート情報とテクスチャ情報を完全分離できるか？



namespace OreOreLib
{


#if defined( FREEIMAGE_SUPPORT )
	extern void InitFreeImage();
	extern void DeinitFreeImage();

	//#elif defined( IL_SUPPORT )
#elif defined( RESIL_SUPPORT )
	extern void InitIL();

#endif



	struct GL_Texture1D_Desc
	{
		DATA_FORMAT	DataFormat;
		int			Width;
		uint8		Channels;
		int			MipLevels;
		int			ArraySize;

		// format
		int			InternalFormat;
		uint32		Format;
		uint32		Type;	// GL_UNSIGNED_INT, GL_FLOAT, etc...

		// default sampler state
		int			MagFilter;
		int			MinFilter;
	};


	struct GL_Texture2D_Desc
	{
		DATA_FORMAT	DataFormat;
		int			Width;
		int			Height;
		uint8		Channels;
		int			MipLevels;
		int			ArraySize;// Texture array length. 0 for non-array texture

		// format
		int			InternalFormat;
		uint32		Format;
		uint32		Type;	// GL_UNSIGNED_INT, GL_FLOAT, etc...

		// default sampler state parameters
		int			MagFilter;
		int			MinFilter;
		int			WrapS;
		int			WrapT;
	};


	struct GL_Texture3D_Desc
	{
		DATA_FORMAT	DataFormat;
		int			Width;
		int			Height;
		int			Depth;
		uint8		Channels;
		int			MipLevels;

		// format
		int			InternalFormat;
		uint32		Format;
		uint32		Type;	// GL_UNSIGNED_INT, GL_FLOAT, etc...

		// default sampler state parameters
		int			MagFilter;
		int			MinFilter;
		int			WrapS;
		int			WrapT;
		int			WrapR;
	};


	// abstract texture class
	struct Texture
	{
		uint32	texID;		// テクスチャID
		size_t	size;		// 要素サイズ
		int		numdim;		// 次元数

		uint8	*m_pData;	// データ

		inline uint32 TexID() const	{ return texID; }
		virtual void GenHardwareTexture() = 0;

		virtual int Width() const	= 0;
		virtual int Height() const	= 0;
		virtual int Depth() const	= 0;
		virtual int numMipLevels() const = 0;
		virtual int numChannels() const = 0;
		virtual DATA_FORMAT DataFormat() const = 0;
		virtual GLuint GLFormat() const			= 0;
		virtual GLint GLInternalFormat() const	= 0;

		virtual void AllocateBuffer( const void *pData=NULL )=0;
		virtual void ReleaseBuffer()=0;

	};


	// Texture1D class
	struct Texture1D : Texture
	{
		GL_Texture1D_Desc	texDesc;

		Texture1D();
		Texture1D( const Texture1D& obj );
		~Texture1D();

		Texture1D& operator = ( const Texture1D& obj );

		void Allocate( int width, int mip_levels, int array_size, DATA_FORMAT data_format, const void *pData=NULL );
		void SetFilterMode( FILTER_MODE mode );

		void Load( const TCHAR *filepath, DATA_FORMAT data_format=FORMAT_R16G16B16A16_FLOAT );
		void Write( const TCHAR *filepath );
		void GenHardwareTexture();
		void Release();

		void ReadBack2CPU();
		void Transfer2GPU();
		void Upload2GPU( const TCHAR *filepath, DATA_FORMAT data_format, int &subWidth, bool bNormalize );

		inline void *Data( int x ) const
		{
			const int x_ = Clamp( x, 0, texDesc.Width-1 );
			return m_pData +  x_ * size;
		}

		inline void* operator () ( int x ) const
		{
			const int x_ = Clamp( x, 0, texDesc.Width-1 );
			return m_pData + x_ * size;
		}


		// Virtual Functions Overrides
		virtual int Width() const				{ return texDesc.Width; }
		virtual int Height() const				{ return 1; }
		virtual int Depth() const				{ return 1; }
		virtual int numMipLevels() const		{ return texDesc.MipLevels; }
		virtual int numChannels() const			{ return texDesc.Channels; }
		virtual DATA_FORMAT DataFormat() const	{ return texDesc.DataFormat; }
		virtual GLuint GLFormat() const			{ return texDesc.Format; }
		virtual GLint GLInternalFormat() const	{ return texDesc.InternalFormat; }

		virtual void AllocateBuffer( const void *pData=NULL );
		virtual void ReleaseBuffer();
	};



	// 2D texture class
	struct Texture2D : Texture
	{
		GL_Texture2D_Desc	texDesc;

		Texture2D();
		Texture2D( const Texture2D& obj );
		~Texture2D();

		Texture2D& operator = ( const Texture2D& obj );


		void Allocate( int width, int height, int mip_levels, int array_size, DATA_FORMAT format, const void *pData=NULL );
		void SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt );
		void SetFilterMode( FILTER_MODE mode );

		void Load( const TCHAR* filepath, DATA_FORMAT data_format = FORMAT_R16G16B16A16_FLOAT );
		void Write( const TCHAR *filepath );
		void GenHardwareTexture();
		void Release();

		void ReadBack2CPU();
		void Transfer2GPU();
		void Upload2GPU( const TCHAR *filepath, DATA_FORMAT data_format, int &subWidth, int &subHeight, bool bNormalize );
		//void Convert2Luminance( bool bNormalize=true );

		inline void *Data( int x, int y ) const
		{
			const int x_ = Clamp( x, 0, texDesc.Width - 1 );
			const int y_ = Clamp( y, 0, texDesc.Height - 1 );
			return m_pData + ( texDesc.Width * y_ + x_ ) * size;
		}

		inline void* operator () ( int x, int y ) const
		{
			const int x_ = Clamp( x, 0, texDesc.Width - 1 );
			const int y_ = Clamp( y, 0, texDesc.Height - 1 );
			return m_pData + ( texDesc.Width * y_ + x_ ) * size;
		}


		// Virtual Functions Overrides
		virtual int Width() const				{ return texDesc.Width; }
		virtual int Height() const				{ return texDesc.Height; }
		virtual int Depth() const				{ return 1; }
		virtual int numMipLevels() const		{ return texDesc.MipLevels; }
		virtual int numChannels() const			{ return texDesc.Channels; }
		virtual DATA_FORMAT DataFormat() const	{ return texDesc.DataFormat; }
		virtual GLuint GLFormat() const			{ return texDesc.Format; }
		virtual GLint GLInternalFormat() const	{ return texDesc.InternalFormat; }

		virtual void AllocateBuffer( const void *pData=NULL );
		virtual void ReleaseBuffer();
	};



	// 3D texture class
	struct Texture3D : Texture
	{
		GL_Texture3D_Desc	texDesc;

		Texture3D();
		Texture3D( const Texture3D& obj );
		~Texture3D();

		Texture3D& operator = ( const Texture3D& obj );


		void Allocate( int width, int height, int depth, int mip_levels, DATA_FORMAT data_format, const void *pData=NULL );
		void SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt, TEXTURE_ADDRESS_MODE wrapr );
		void SetFilterMode( FILTER_MODE mode );

		void Load( const TCHAR *filepath, DATA_FORMAT data_format=FORMAT_R16G16B16A16_FLOAT );
		void Write( const TCHAR *filepath );
		void GenHardwareTexture();
		void Release();

		void Transfer2GPU();
		void ReadBack2CPU();
		void Upload2GPU( const TCHAR *filepath, int &subWidth, int &subHeight, int &subDepth, bool bNormalize ){}
		void Convert2Luminance( bool bNOrmalize=true ){}


		inline void *Data( int x, int y, int z ) const
		{
			const int x_ = Clamp( x, 0, texDesc.Width - 1 );
			const int y_ = Clamp( y, 0, texDesc.Height - 1 );
			const int z_ = Clamp( z, 0, texDesc.Depth - 1 );
			//						↓xyスライス面移動				  ↓y軸移動			  ↓x軸移動
			return m_pData + ( texDesc.Width * texDesc.Height * z_ + texDesc.Width * y_ + x_ ) * size;
		}

		inline void* operator () ( int x, int y, int z ) const
		{
			const int x_ = Clamp( x, 0, texDesc.Width - 1 );
			const int y_ = Clamp( y, 0, texDesc.Height - 1 );
			const int z_ = Clamp( z, 0, texDesc.Depth - 1 );
			//						↓xyスライス面移動				  ↓y軸移動			  ↓x軸移動
			return m_pData + ( texDesc.Width * texDesc.Height * z_ + texDesc.Width * y_ + x_ ) * size;
		}


		// Virtual Functions Overrides
		virtual int Width() const				{ return texDesc.Width; }
		virtual int Height() const				{ return texDesc.Height; }
		virtual int Depth() const				{ return texDesc.Depth; }
		virtual int numMipLevels() const		{ return texDesc.MipLevels; }
		virtual int numChannels() const			{ return texDesc.Channels; }
		virtual DATA_FORMAT DataFormat() const	{ return texDesc.DataFormat; }
		virtual GLuint GLFormat() const			{ return texDesc.Format; }
		virtual GLint GLInternalFormat() const	{ return texDesc.InternalFormat; }

		virtual void AllocateBuffer( const void *pData=NULL );
		virtual void ReleaseBuffer();
	};



}// end of namespace

#endif	// TEXTURE_H //