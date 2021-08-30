#ifndef	BOUNDING_VOLUME_H
#define	BOUNDING_VOLUME_H

#include	<oreore/mathlib/GraphicsMath.h>



namespace OreOreLib
{


	class IBoundingVolume
	{
	public:
	
		//virtual Vec3f* GetMininimum() = 0;
		//virtual Vec3f* GetMaximum() = 0;
		//virtual Vec3f* GetCenter() = 0;
		//virtual void Transform( const Mat4f &mat_trans ) = 0;
	
	
	protected:
	
	
	private:
	
	
	};



	//######################################################################################//
	//								Axis Aligned Bounding Box								//
	//######################################################################################//

	class AxisAlignedBoundingBox : public IBoundingVolume
	{
	public:

		AxisAlignedBoundingBox();
		AxisAlignedBoundingBox( const Vec3f& center, const Vec3f& size );
		//AxisAlignedBoundingBox( const Vec3f& bbmin, const Vec3f& bbmax );
		~AxisAlignedBoundingBox(){}
		
		void Clear();

		void SetCenter( const Vec3f& center );
		void SetCenter( float x, float y, float z );
		void SetSize( const Vec3f& size );
		void SetSize( float width, float height, float depth );
		void SetMinMax( const Vec3f& bbmin, const Vec3f& bbmax );
		void SetMinMax( float minx, float miny, float minz, float maxx, float maxy, float maxz );
		
		const Vec3f* GetCenter() const	{ return &m_Center; }
		const Vec3f* GetSize() const	{ return &m_Size; }
		const float GetWidth() const	{ return m_Size.x; }
		const float GetHeight() const	{ return m_Size.y; }
		const float GetDepth() const	{ return m_Size.z; }
		const Vec3f* GetMin() const		{ return &m_Min; }
		const Vec3f* GetMax() const		{ return &m_Max; }

		
	private:

		Vec3f	m_Center;
		Vec3f	m_Size;
		Vec3f	m_Min;
		Vec3f	m_Max;

	};


	typedef AxisAlignedBoundingBox AABB;




	//######################################################################################//
	//									Oriented Bounding Box								//
	//######################################################################################//


	class OrientedBoundingBox : public IBoundingVolume
	{
	public:

		OrientedBoundingBox();
		OrientedBoundingBox( const Vec3f& center, const Vec3f& size );
		~OrientedBoundingBox(){}
		
		void Clear();

		void SetCenter( const Vec3f& center );
		void SetCenter( float x, float y, float z );
		void SetSize( const Vec3f& size );
		void SetSize( float width, float height, float depth );
		void SetMinMax( const Vec3f& bbmin, const Vec3f& bbmax );
		void SetMinMax( float minx, float miny, float minz, float maxx, float maxy, float maxz );
		
		const Vec3f* GetCenter() const	{ return &m_Center; }
		const Vec3f* GetSize() const	{ return &m_Size; }
		const float GetWidth() const	{ return m_Size.x; }
		const float GetHeight() const	{ return m_Size.y; }
		const float GetDepth() const	{ return m_Size.z; }
		//const Vec3f* GetMin() const		{ return &m_Min; }
		//const Vec3f* GetMax() const		{ return &m_Max; }

		
	private:

		Vec3f	m_Center;
		Vec3f	m_Axes[3];
		Vec3f	m_Size;
		//Vec3f	m_Min;
		//Vec3f	m_Max;

	};


	typedef AxisAlignedBoundingBox AABB;






	//######################################################################################//
	//									Bounding Sphere										//
	//######################################################################################//

	class BoundingSphere : public IBoundingVolume
	{
	public:

		BoundingSphere();
		~BoundingSphere();


	private:


	};


}// end of namesapce



#endif	// BOUNDING_VOLUME_H //