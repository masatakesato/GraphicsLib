#include	"BoundingVolume.h"



namespace OreOreLib
{

	const Vec3f X_AXIS = { 1.0f, 0.0f, 0.0f };
	const Vec3f Y_AXIS = { 0.0f, 1.0f, 0.0f };
	const Vec3f Z_AXIS = { 0.0f, 0.0f, 1.0f };



	//######################################################################################//
	//								Axis Aligned Bounding Box								//
	//######################################################################################//
	
	AxisAlignedBoundingBox::AxisAlignedBoundingBox()
	{
		Clear();
	}
	


	AxisAlignedBoundingBox::AxisAlignedBoundingBox( const Vec3f& center, const Vec3f& size )
	{
		m_Center	= center;
		m_Size		= size;

		AddScaled( m_Min, m_Center, -0.5f, m_Size );
		AddScaled( m_Max, m_Center, +0.5f, m_Size );
	}



	//AxisAlignedBoundingBox::AxisAlignedBoundingBox( const Vec3f& bbmin, const Vec3f& bbmax )
	//{
	//
	//
	//}



	void AxisAlignedBoundingBox::Clear()
	{
		InitZero( m_Center );
		InitZero( m_Size );
		InitZero( m_Min );
		InitZero( m_Max );
	}



	void AxisAlignedBoundingBox::SetCenter( const Vec3f& center )
	{
		m_Center	= center;
		
		AddScaled( m_Min, m_Center, -0.5f, m_Size );
		AddScaled( m_Max, m_Center, +0.5f, m_Size );
	}

	
	
	void AxisAlignedBoundingBox::SetCenter( float x, float y, float z )
	{
		InitVec( m_Center, x, y, z );
		
		AddScaled( m_Min, m_Center, -0.5f, m_Size );
		AddScaled( m_Max, m_Center, +0.5f, m_Size );
	}


	
	void AxisAlignedBoundingBox::SetSize( const Vec3f& size )
	{
		m_Size	= size;

		AddScaled( m_Min, m_Center, -0.5f, m_Size );
		AddScaled( m_Max, m_Center, +0.5f, m_Size );
	}


	
	void AxisAlignedBoundingBox::SetSize( float width, float height, float depth )
	{
		InitVec( m_Size, width, height, depth );

		AddScaled( m_Min, m_Center, -0.5f, m_Size );
		AddScaled( m_Max, m_Center, +0.5f, m_Size );
	}

	

	void AxisAlignedBoundingBox::SetMinMax( const Vec3f& bbmin, const Vec3f& bbmax )
	{
		m_Min	= bbmin;
		m_Max	= bbmax;
	
		Subtract( m_Size, m_Max, m_Min );
		AddScaled( m_Center, m_Min, 0.5f, m_Size );
	}


	
	void AxisAlignedBoundingBox::SetMinMax( float minx, float miny, float minz, float maxx, float maxy, float maxz )
	{
		InitVec( m_Min, minx, miny, minz );
		InitVec( m_Max, maxx, maxy, maxz );
	
		Subtract( m_Size, m_Max, m_Min );
		AddScaled( m_Center, m_Min, 0.5f, m_Size );	
	}






	//######################################################################################//
	//									Oriented Bounding Box								//
	//######################################################################################//

	OrientedBoundingBox::OrientedBoundingBox()
	{
		Clear();
	}
	


	OrientedBoundingBox::OrientedBoundingBox( const Vec3f& center, const Vec3f& size )
	{
		m_Center	= center;
		m_Size		= size;

		m_Axes[0]	= X_AXIS;
		m_Axes[1]	= Y_AXIS;
		m_Axes[2]	= Z_AXIS;
	}



	void OrientedBoundingBox::Clear()
	{
		InitZero( m_Center );
		InitZero( m_Size );

		m_Axes[0]	= X_AXIS;
		m_Axes[1]	= Y_AXIS;
		m_Axes[2]	= Z_AXIS;
	}



	//void OrientedBoundingBox::SetCenter( const Vec3f& center )
	//{
	//	m_Center	= center;
	//	
	//	AddScaled( m_Min, m_Center, -0.5f, m_Size );
	//	AddScaled( m_Max, m_Center, +0.5f, m_Size );
	//}

	//
	//
	//void OrientedBoundingBox::SetCenter( float x, float y, float z )
	//{
	//	InitVec( m_Center, x, y, z );
	//	
	//	AddScaled( m_Min, m_Center, -0.5f, m_Size );
	//	AddScaled( m_Max, m_Center, +0.5f, m_Size );
	//}


	//
	//void OrientedBoundingBox::SetSize( const Vec3f& size )
	//{
	//	m_Size	= size;

	//	AddScaled( m_Min, m_Center, -0.5f, m_Size );
	//	AddScaled( m_Max, m_Center, +0.5f, m_Size );
	//}


	//
	//void OrientedBoundingBox::SetSize( float width, float height, float depth )
	//{
	//	InitVec( m_Size, width, height, depth );

	//	AddScaled( m_Min, m_Center, -0.5f, m_Size );
	//	AddScaled( m_Max, m_Center, +0.5f, m_Size );
	//}

	//

	//void OrientedBoundingBox::SetMinMax( const Vec3f& bbmin, const Vec3f& bbmax )
	//{
	//	m_Min	= bbmin;
	//	m_Max	= bbmax;
	//
	//	Subtract( m_Size, m_Max, m_Min );
	//	AddScaled( m_Center, m_Min, 0.5f, m_Size );
	//}


	//
	//void OrientedBoundingBox::SetMinMax( float minx, float miny, float minz, float maxx, float maxy, float maxz )
	//{
	//	InitVec( m_Min, minx, miny, minz );
	//	InitVec( m_Max, maxx, maxy, maxz );
	//
	//	Subtract( m_Size, m_Max, m_Min );
	//	AddScaled( m_Center, m_Min, 0.5f, m_Size );	
	//}













}// end of namespace