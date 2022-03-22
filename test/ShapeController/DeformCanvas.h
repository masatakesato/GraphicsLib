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
		const static Vec3f debugColors[4] = 
		{
			{ 0, 0, 0 },// bl
			{ 1, 0, 0 },// br
			{ 1, 1, 0 },// tr
			{ 0, 1, 0 },// tl
		};

		const float LineWidth = 5.0f;

		const float HorizontalSize = 1.0f;

	};



	template < typename T >
	class DeformCanvas
	{
		using RCP2D = RadialControlPoint2D<T>;


	public:

		DeformCanvas()
		{

		}


		DeformCanvas( const Camera& cam, T width, T height )
			: m_Camera( cam )
			, m_Width( width )
			, m_Height( height )
		{
			MatTranslation( matTranslate, *m_Camera.GetPosition() );
			MatRotation( matRotate, *m_Camera.GetForward(), *m_Camera.GetVertical()/*{0.0f, 0.0f, 1.0f}*/ );
			MatScale( matScale, 1.0f, CanvasFrameGeoemtry::HorizontalSize, CanvasFrameGeoemtry::HorizontalSize / m_Camera.GetAspect() );
			
			// matComp = matTranslate * matRotate * matScale;
			Mat4<T> tmp;
			Multiply( tmp, matRotate, matScale );
			Multiply( matComp, matTranslate, tmp );
		}


		~DeformCanvas()
		{

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



		void AddController( const Vec2<T>& pos, T radius, int64& objectid )
		{
			m_ControlPoints.AddToTail( RCP2D( pos.x, pos.y, pos.x, pos.y, radius ) );
			objectid = m_ControlPoints.Length<int64>() - 1;
		}


		bool DeleteController( int64& objectid )
		{
			if( objectid < 0 || objectid >= m_ControlPoints.Length<int64>() )
				return false;

			m_ControlPoints.Remove( (int32)objectid );
			objectid = -1;

			return true;
		}



		bool TranslateControlPointOrigin( int64 objectid, float dx, float dy )
		{
			if( objectid < 0 || objectid >= m_ControlPoints.Length<int64>() )
				return false;

			m_ControlPoints[ (int32)objectid ].TranslateOrigin( dx, dy );

			return true;
		}


		bool TranslateControlPointTarget( int64 objectid, float dx, float dy )
		{
			if( objectid < 0 || objectid >= m_ControlPoints.Length<int64>() )
				return false;

			m_ControlPoints[ (int32)objectid ].TranslateTarget( dx, dy );

			return true;
		}


		bool TranslateControlPointRadius( int64 objectid, float radius )
		{
			if( objectid < 0 || objectid >= m_ControlPoints.Length<int64>() )
				return false;

			m_ControlPoints[ (int32)objectid ].TranslateRadius( radius );

			return true;
		}


		bool GetIntersectedControlPoint( T x, T y, T range, int64& objectid, uint8& type )
		{
			Vec2<T> pos( x, y );

			for( int i=0; i<m_ControlPoints.Length<int>(); ++i )
			{
				// Check origin intersection
				if( Distance( pos, m_ControlPoints[i].Origin() ) <= range )
				{
					objectid = i;
					type = RadialControlPoint2Df::ORIGIN;
					return true;
				}

				// Check target intersection
				else if( Distance( pos, m_ControlPoints[i].Target() ) <= range )
				{
					objectid = i;
					type = RadialControlPoint2Df::TARGET;
					return true;
				}
			}

			objectid = -1;
			return false;
		}


		Mat4<T>& WorldMatrix()
		{
			return matComp;
		}


		const Mat4<T>& WorldMatrix() const
		{
			return matComp;
		}


		const OreOreLib::Array<RCP2D>& ControlPoints()
		{
			return m_ControlPoints;
		}



	private:

		Camera	m_Camera;
		T		m_Width, m_Height;

		OreOreLib::Array<RCP2D>	m_ControlPoints;


		//================= スクリーン空間からワールド空間へ射影する行列 ===================//
		// OpenGLの列優先行列を格納するバッファ. Cameraクラスのは行優先でやつしか入ってないから
		Mat4<T>	m_MatView, m_MatProj, m_MatViewProj, m_MatScreenToWorld;


		//============= ワールド空間上のオブジェクトをカメラ姿勢に変換する行列 ============//
		Mat4f matScale, matRotate, matTranslate, matComp;


	};


}



#endif // !DEFORM_CANVAS_H
