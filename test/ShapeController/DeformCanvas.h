#ifndef DEFORM_CANVAS_H
#define	DEFORM_CANVAS_H

#include	<oreore/container/Array.h>

#include	<graphics/gl4x/scene/Camera.h>

#include	"ControlPointExt.h"



namespace GraphicsLib
{

	namespace CanvasFrameGeoemtry
	{
		// vertices
		const static Vec4f verts[4] = 
		{
			{ 0, -1, -1, 1 },// bl
			{ 0, +1, -1, 1 },// br
			{ 0, +1, +1, 1 },// tr
			{ 0, -1, +1, 1 },// tl
		};

		// color
		const static Vec3f colors[4] = 
		{
			{ 0, 0, 0 },// bl
			{ 1, 0, 0 },// br
			{ 1, 1, 0 },// tr
			{ 0, 1, 0 },// tl
		};

		const float LineWidth = 5.0f;

	};



	template < typename T >
	class DeformCanvas
	{
		using RCP2D = RadialControlPoint2D<T>;


	public:

		DeformCanvas()
		{

		}


		DeformCanvas( const Camera& cam )
			: m_Camera( cam )
		{

		}


		~DeformCanvas()
		{

		}



		int64 AddControlPoint()
		{

		}


		int64 AddControlPoint( const Vec2<T>& pos, T radius )
		{
			m_ControlPoints.AddToTail( RCP2D( (T)pos.x, (T)pos.y, 0, 0, (T)20.0f ) );
		}


		bool DeleteControlPoint( int64 objectid )
		{
			if( objectid < 0 || objectid >=m_ControlPoints.Length<int64>() )	return false;

			m_ControlPoints.Remove( objectid );

			return true;
		}


		Camera& GetCamera()
		{
			return m_Camera;
		}


		const Camera& GetCamera() const
		{
			return m_Camera;
		}


		// Subscript operator for read only.( called if Memory is const )
		template < typename SizeType=uint32 >
		inline const RCP2D& operator[]( SizeType n ) const&
		{
			return m_ControlPoints[n];
		}


		// Subscript operator for read-write.( called if Memory is non-const )
		template < typename SizeType=uint32 >
		inline RCP2D& operator[]( SizeType n ) &
		{
			return m_ControlPoints[n];
		}


		Mat4<T>& WorldMatrix()				{ return m_MatWorld; }
		const Mat4<T>& WorldMatrix() const	{ return m_MatWorld; }




	private:

		Camera	m_Camera;

		OreOreLib::Array<RCP2D>	m_ControlPoints;

		// OpenGLの列優先行列を格納するバッファ. Cameraクラスのは行優先でやつしか入ってないから
		Mat4<T>	m_MatView, m_MatProj, m_MatViewProj, m_MatScreenToWorld;
		Mat4<T>	m_MatWorld;


	};


}



#endif // !DEFORM_CANVAS_H
