#ifndef	TEXTURE_H
#define	TEXTURE_H

#include	<GL/glew.h>
#include	<GL/gl.h>
#pragma comment(lib, "glew32.lib")
#pragma comment (lib, "opengl32.lib")


#include	<oreore/common/Utility.h>
#include	<oreore/common/TString.h>
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


	//##############################################################################//
	//																				//
	//								Texture Descriptors								//
	//																				//
	//##############################################################################//


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




	//##############################################################################//
	//																				//
	//						Texture and Texture1D/2D/3D classes						//
	//																				//
	//##############################################################################//


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




	//##############################################################################//
	//																				//
	//						Helper functions for OpenGL Texture						//
	//																				//
	//##############################################################################//

	// Allocate 1D texture
	static GLuint AllocateTexture1D(GLint mag_filter, GLint min_filter,
									GLsizei width,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{	
		GLenum errCode;
		GLuint tex_id = NULL;

		glGenTextures(1, &tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("Error at AllocateTexture1D().glGenTextures(): ") << gluErrorString(errCode) << tendl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		glBindTexture(GL_TEXTURE_1D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexImage1D(GL_TEXTURE_1D, 0, iformat, width, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_1D, 0);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("Error at ReallocateTexture1D().glTexImage1D(): ") << gluErrorString(errCode) << tendl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		return tex_id;
	}


	
	// Allocate 2D texuture
	static GLuint AllocateTexture2D(GLint mag_filter, GLint min_filter,
									GLint wrap_s, GLint wrap_t,
									GLsizei width, GLsizei height,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{	
		GLenum errCode;
		GLuint tex_id = NULL;

		glGenTextures(1, &tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("Error at AllocateTexture2D().glGenTextures(): ") << gluErrorString(errCode) << tendl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		glBindTexture(GL_TEXTURE_2D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
			glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("Error at ReallocateTexture2D().glTexImage2D(): ") << gluErrorString(errCode) << tendl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		return tex_id;
	}



	// Rallocate 2D texture
	static void ReallocateTexture2D(GLuint &tex_id, 
									GLint mag_filter, GLint min_filter,
									GLint wrap_s, GLint wrap_t,
									GLsizei width, GLsizei height,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{
		GLenum errCode;

		if(tex_id)	glDeleteTextures(1,	&tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << gluErrorString(errCode) << tendl;
			tex_id = NULL;
			return;
		}
		
		glBindTexture(GL_TEXTURE_2D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
			glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		
		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("Error at ReallocateTexture2D().glTexImage2D(): ") << gluErrorString(errCode) << tendl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return;
		}
	}



	// Allocate 3D texture
	static GLuint AllocateTexture3D(GLint mag_filter, GLint min_filter,
									GLint wrap_s, GLint wrap_t, GLint wrap_r,
									GLsizei width, GLsizei height, GLsizei depth,
									GLenum iformat, GLenum format, GLenum type, const GLvoid* data = NULL)
	{
		GLenum errCode;
		GLuint tex_id;
	
		glGenTextures(1, &tex_id);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("Error at AllocateTexture3D().glGenTextures(): ") << gluErrorString(errCode) << tendl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		glBindTexture(GL_TEXTURE_3D, tex_id);
		{
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mag_filter);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap_s);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap_t);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap_r);
			glTexImage3D(GL_TEXTURE_3D, 0, iformat, width, height, depth, 0, format, type, data);
		}
		glBindTexture(GL_TEXTURE_3D, 0);

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("Error at AllocateTexture3D().glTexImage3D(): ") << gluErrorString(errCode) << tendl;
			glDeleteTextures(1, &tex_id);
			tex_id = NULL;
			return tex_id;
		}

		return tex_id;
	}



	// Delete texture
	static void ReleaseTexture( GLuint &texid )
	{
		if( texid != 0)
		glDeleteTextures( 1, &texid );
	}



	// 2DTexture drawing function for OpenGL 2.0
	static void DrawTexture2D(int width, int height, unsigned int texture)
	{
		glViewport(0, 0, width, height);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);// テクスチャユニット0を指定
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(-1, -1);
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(+1, -1);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(+1, +1);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(-1, +1);
		glEnd();

		glDisable(GL_TEXTURE_2D);

	}



	// 3DTexture drawing function for OpenGL 2.0
	static void DrawTexture3D(int width, int height, int depth, unsigned int texture, int layer=0)
	{
		float z = float(layer) / float(depth);
	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);// テクスチャユニット0を指定
		glEnable(GL_TEXTURE_3D);

		glBindTexture(GL_TEXTURE_3D, texture);
		glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord3f(0.0f, 0.0f, z); /*glColor3f(0,0,0);*/	glVertex2f(-1, -1);
		glTexCoord3f(1.0f, 0.0f, z); /*glColor3f(1,0,0);*/	glVertex2f(+1, -1);
		glTexCoord3f(1.0f, 1.0f, z); /*glColor3f(1,1,0);*/	glVertex2f(+1, +1);
		glTexCoord3f(0.0f, 1.0f, z); /*glColor3f(0,1,0);*/	glVertex2f(-1, +1);
		glEnd();

		glDisable(GL_TEXTURE_3D);
	}



}// end of namespace




#endif	// TEXTURE_H //