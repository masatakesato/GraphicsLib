#ifndef	SWAP_TEXTURE_H
#define	SWAP_TEXTURE_H

#include	"Texture.h"



namespace OreOreLib
{

	class SwapBuffer
	{
	public:

		SwapBuffer()	{ m_Active = 0; m_Inactive = 1; }
		void Swap()		{ m_Active ^= 1; m_Inactive ^= 1; }
		
		virtual int Width() const	= 0;
		virtual int Height() const	= 0;
		virtual int Depth() const	= 0;
		virtual int numMipLevels() const = 0;
		virtual int numChannels() const = 0;
		virtual DATA_FORMAT DataFormat() const = 0;
		virtual GLuint GLFormat() const			= 0;
		virtual GLint GLInternalFormat() const	= 0;


	protected:

		uint8	m_Active;
		uint8	m_Inactive;
	};



	// SwapTexture1D class
	class SwapTexture1D : public SwapBuffer
	{
	public:

		SwapTexture1D();
		SwapTexture1D( const SwapTexture1D& obj );
		~SwapTexture1D();

		SwapTexture1D& operator = ( const SwapTexture1D& obj );

		void Allocate( int width, int mip_levels, int array_size, DATA_FORMAT data_format, const void *pData=NULL );
		void SetFilterMode( FILTER_MODE mode );

		void GenHardwareTexture();
		void Release();
		
		Texture1D* FrontBuffer() const	{ return (Texture1D *)&m_Textures[m_Active]; }
		Texture1D* BackBuffer() const	{ return (Texture1D *)&m_Textures[m_Inactive]; }

		// Virtual Functions Overrides
		virtual int Width() const				{ return m_Textures[0].Width(); }
		virtual int Height() const				{ return m_Textures[0].Height(); }
		virtual int Depth() const				{ return m_Textures[0].Depth(); }
		virtual int numMipLevels() const		{ return m_Textures[0].numMipLevels(); }
		virtual int numChannels() const			{ return m_Textures[0].numChannels(); }
		virtual DATA_FORMAT DataFormat() const	{ return m_Textures[0].DataFormat(); }
		virtual GLuint GLFormat() const			{ return m_Textures[0].GLFormat(); }
		virtual GLint GLInternalFormat() const	{ return m_Textures[0].GLInternalFormat(); }


	private:

		Texture1D	m_Textures[2];
		//SwapSwitch	m_Switch;

	};



	// SwapTexture2D class
	class SwapTexture2D : public SwapBuffer
	{
	public:

		SwapTexture2D();
		SwapTexture2D( const SwapTexture2D& obj );
		~SwapTexture2D();

		SwapTexture2D& operator = ( const SwapTexture2D& obj );
		
		void Allocate( int width, int height, int mip_levels, int array_size, DATA_FORMAT format, const void *pData=NULL );
		void SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt );
		void SetFilterMode( FILTER_MODE mode );
		
		void GenHardwareTexture();
		void Release();

		Texture2D* FrontBuffer() const	{ return (Texture2D *)&m_Textures[m_Active]; }
		Texture2D* BackBuffer() const	{ return (Texture2D *)&m_Textures[m_Inactive]; }

		// Virtual Functions Overrides
		virtual int Width() const				{ return m_Textures[0].Width(); }
		virtual int Height() const				{ return m_Textures[0].Height(); }
		virtual int Depth() const				{ return m_Textures[0].Depth(); }
		virtual int numMipLevels() const		{ return m_Textures[0].numMipLevels(); }
		virtual int numChannels() const			{ return m_Textures[0].numChannels(); }
		virtual DATA_FORMAT DataFormat() const	{ return m_Textures[0].DataFormat(); }
		virtual GLuint GLFormat() const			{ return m_Textures[0].GLFormat(); }
		virtual GLint GLInternalFormat() const	{ return m_Textures[0].GLInternalFormat(); }


	private:

		Texture2D	m_Textures[2];

	};



	// SwapTexture3D class
	class SwapTexture3D : public SwapBuffer
	{
	public:

		SwapTexture3D();
		SwapTexture3D( const SwapTexture3D& obj );
		~SwapTexture3D();

		SwapTexture3D& operator = ( const SwapTexture3D& obj );
		
		void Allocate( int width, int height, int depth, int mip_levels, DATA_FORMAT data_format, const void *pData=NULL );
		void SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt, TEXTURE_ADDRESS_MODE wrapr );
		void SetFilterMode( FILTER_MODE mode );
		
		void GenHardwareTexture();
		void Release();

		Texture3D* FrontBuffer() const	{ return (Texture3D *)&m_Textures[m_Active]; }
		Texture3D* BackBuffer() const	{ return (Texture3D *)&m_Textures[m_Inactive]; }

		// Virtual Functions Overrides
		virtual int Width() const				{ return m_Textures[0].Width(); }
		virtual int Height() const				{ return m_Textures[0].Height(); }
		virtual int Depth() const				{ return m_Textures[0].Depth(); }
		virtual int numMipLevels() const		{ return m_Textures[0].numMipLevels(); }
		virtual int numChannels() const			{ return m_Textures[0].numChannels(); }
		virtual DATA_FORMAT DataFormat() const	{ return m_Textures[0].DataFormat(); }
		virtual GLuint GLFormat() const			{ return m_Textures[0].GLFormat(); }
		virtual GLint GLInternalFormat() const	{ return m_Textures[0].GLInternalFormat(); }


	private:

		Texture3D	m_Textures[2];

	};



}// end of namespace

#endif	// SWAP_TEXTURE_H //