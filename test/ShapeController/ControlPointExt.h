#ifndef CONTROL_POINT_EXT_H
#define	CONTROL_POINT_EXT_H

#include	<tuple>

#include	<graphics/authoring/controller/ControlPoint.h>



namespace GraphicsLib
{

	template < typename T >
	class RadialControlPoint2D : public ControlPoint2D<T>
	{
	public:

		RadialControlPoint2D()
			: ControlPoint2D<T>()
			, m_Radius( 1 )
			, m_bRadiusChanged( false )
		{

		}


		RadialControlPoint2D( const T& ox, const T& oy, const T& tx, const T& ty, const T& r )
			: ControlPoint2D<T>( ox, oy, tx, ty  )
			, m_Radius( r )
			, m_bRadiusChanged( false )
		{

		}


		~RadialControlPoint2D()
		{
		
		}


		void Clear()
		{
			ControlPoint2D<T>::Clear();
			m_bRadiusChanged = false;
			m_Radius = 1;
		}


		void SetRadius( const T& r )
		{
			assert( r >= 0 );//ASSERT( );
			m_bRadiusChanged = (m_Radius != r);
			m_Radius = r;
		}


		void ResetRadiusChanged()
		{
			m_bRadiusChanged = false;
		}


		const T& Radius() const
		{
			return m_Radius;
		}


		bool RadiusChanged() const
		{
			return m_bRadiusChanged;
		}

		
	private:

		T		m_Radius;
		bool	m_bRadiusChanged;

	};





	// 変数型と名称をテンプレート引数で決め打ちして構造体を作りたい. -> 下記コード参考に実装は可能.
	// https://stackoverflow.com/questions/63415651/variadic-template-parameter-inference-with-nested-maps

	//template < typename... >
	//struct Typelist {};


	//template < class CLASS, size_t ... IDS >	class ControlPointExt;


	//template < class ... CLASSES, size_t ... IDS >
	//class ControlPointExt< Typelist<CLASSES...>, IDS... >
	//{
	//public:

	//	ControlPointExt(){}




	//	
	//private:

	//	const size_t Index[ sizeof ...(IDS)  ] = { IDS... };
	//	std::tuple< CLASSES... >	data;

	//};



	//ControlPointExt< Typelist<int, float, short>, 1, 2, 0 > g_CPExt;

}



#endif // !CONTROL_POINT_EXT_H
