#ifndef DEFORM_CANVAS_H
#define	DEFORM_CANVAS_H

#include	<oreore/container/Array.h>

#include	<graphics/gl4x/scene/Camera.h>

#include	"ControlPointExt.h"



namespace GraphicsLib
{

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
			g_ControlPoints.AddToTail( RCP2D( (T)pos.x, (T)pos.y, 0, 0, (T)20.0f ) );
		}


		bool DeleteControlPoint( int64 objectid )
		{
			if( objectid < 0 || objectid >=g_ControlPoints.Length<int64>() )	return false;

			g_ControlPoints.Remove( objectid );

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
			return g_ControlPoints[n];
		}


		// Subscript operator for read-write.( called if Memory is non-const )
		template < typename SizeType=uint32 >
		inline RCP2D& operator[]( SizeType n ) &
		{
			return g_ControlPoints[n];
		}



	private:

		Camera	m_Camera;

		OreOreLib::Array<RCP2D>	g_ControlPoints;

		// OpenGLの列優先行列を格納するバッファ. Cameraクラスのは行優先でやつしか入ってないから
		Mat4<T>	m_MatView, m_MatProj, m_MatViewProj, m_MatInvViewProj;


	};


}



#endif // !DEFORM_CANVAS_H
