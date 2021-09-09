#include	"SwapTexture.h"



namespace OreOreLib
{



	//##########################################################################################//
	//								SwapTexture1D class implementation							//
	//##########################################################################################//


	// default constructor
	SwapTexture1D::SwapTexture1D()
	{
		
	}



	// copy constructor(deep copy)
	SwapTexture1D::SwapTexture1D( const SwapTexture1D& obj )
	{
		m_Active		= obj.m_Active;
		m_Inactive		= obj.m_Inactive;
		m_Textures[0]	= obj.m_Textures[0];
		m_Textures[1]	= obj.m_Textures[1];
	}



	// destructor
	SwapTexture1D::~SwapTexture1D()
	{
		Release();
	}



	// operator
	SwapTexture1D& SwapTexture1D::operator = ( const SwapTexture1D& obj )
	{
		m_Active		= obj.m_Active;
		m_Inactive		= obj.m_Inactive;
		m_Textures[0]	= obj.m_Textures[0];
		m_Textures[1]	= obj.m_Textures[1];

		return *this;
	}



	void SwapTexture1D::Allocate( int width, int mip_levels, int array_size, DATA_FORMAT data_format, const void *pData )
	{
		m_Textures[0].Allocate( width, mip_levels, array_size, data_format, pData );
		m_Textures[1].Allocate( width, mip_levels, array_size, data_format, pData );
	}



	void SwapTexture1D::SetFilterMode( FILTER_MODE mode )
	{
		m_Textures[0].SetFilterMode( mode );
		m_Textures[1].SetFilterMode( mode );
	}



	void SwapTexture1D::GenHardwareTexture()
	{
		m_Textures[0].GenHardwareTexture();
		m_Textures[1].GenHardwareTexture();
	}



	void SwapTexture1D::Release()
	{
		m_Textures[0].Release();
		m_Textures[1].Release();
	}




	//##########################################################################################//
	//								SwapTexture2D class implementation							//
	//##########################################################################################//


	// default constructor
	SwapTexture2D::SwapTexture2D()
	{

	}



	// copy constructor(deep copy)
	SwapTexture2D::SwapTexture2D( const SwapTexture2D& obj )
	{
		m_Active		= obj.m_Active;
		m_Inactive		= obj.m_Inactive;
		m_Textures[0]	= obj.m_Textures[0];
		m_Textures[1]	= obj.m_Textures[1];
	}



	// destructor
	SwapTexture2D::~SwapTexture2D()
	{
		Release();
	}



	// operator
	SwapTexture2D& SwapTexture2D::operator = ( const SwapTexture2D& obj )
	{
		m_Active		= obj.m_Active;
		m_Inactive		= obj.m_Inactive;
		m_Textures[0]	= obj.m_Textures[0];
		m_Textures[1]	= obj.m_Textures[1];

		return *this;
	}



	void SwapTexture2D::Allocate( int width, int height, int mip_levels, int array_size, DATA_FORMAT data_format, const void *pData )
	{
		m_Textures[0].Allocate( width, height, mip_levels, array_size, data_format, pData );
		m_Textures[1].Allocate( width, height, mip_levels, array_size, data_format, pData );
	}



	void SwapTexture2D::SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt )
	{
		m_Textures[0].SetAddressMode( wraps, wrapt );
		m_Textures[1].SetAddressMode( wraps, wrapt );
	}



	void SwapTexture2D::SetFilterMode( FILTER_MODE mode )
	{
		m_Textures[0].SetFilterMode( mode );
		m_Textures[1].SetFilterMode( mode );
	}



	void SwapTexture2D::GenHardwareTexture()
	{
		m_Textures[0].GenHardwareTexture();
		m_Textures[1].GenHardwareTexture();
	}



	void SwapTexture2D::Release()
	{
		m_Textures[0].Release();
		m_Textures[1].Release();
	}





	//##########################################################################################//
	//								SwapTexture3D class implementation								//
	//##########################################################################################//


	// default constructor
	SwapTexture3D::SwapTexture3D()
	{
	}



	// copy constructor(deep copy)
	SwapTexture3D::SwapTexture3D( const SwapTexture3D& obj )
	{
		m_Active		= obj.m_Active;
		m_Inactive		= obj.m_Inactive;
		m_Textures[0]	= obj.m_Textures[0];
		m_Textures[1]	= obj.m_Textures[1];
	}



	// destructor
	SwapTexture3D::~SwapTexture3D()
	{
		Release();
	}



	// operator
	SwapTexture3D& SwapTexture3D::operator = ( const SwapTexture3D& obj )
	{
		m_Active	= obj.m_Active;
		m_Inactive	= obj.m_Inactive;
		m_Textures[0]	= obj.m_Textures[0];
		m_Textures[1]	= obj.m_Textures[1];

		return *this;
	}



	void SwapTexture3D::Allocate( int width, int height, int depth, int mip_levels, DATA_FORMAT data_format, const void *pData )
	{
		m_Textures[0].Allocate( width, height, depth, mip_levels, data_format, pData );
		m_Textures[1].Allocate( width, height, depth, mip_levels, data_format, pData );
	}



	void SwapTexture3D::SetAddressMode( TEXTURE_ADDRESS_MODE wraps, TEXTURE_ADDRESS_MODE wrapt, TEXTURE_ADDRESS_MODE wrapr )
	{
		m_Textures[0].SetAddressMode( wraps, wrapt, wrapr );
		m_Textures[1].SetAddressMode( wraps, wrapt, wrapr );
	}



	void SwapTexture3D::SetFilterMode( FILTER_MODE mode )
	{
		m_Textures[0].SetFilterMode( mode );
		m_Textures[1].SetFilterMode( mode );
	}



	void SwapTexture3D::GenHardwareTexture()
	{
		m_Textures[0].GenHardwareTexture();
		m_Textures[1].GenHardwareTexture();
	}


	void SwapTexture3D::Release()
	{
		m_Textures[0].Release();
		m_Textures[1].Release();
	}




}// end of namspace
