#ifndef CONTROL_LINE_H
#define	CONTROL_LINE_H

#include	<oreore/mathlib/GraphicsMath.h>



namespace GraphicsLib
{

	template < typename T >
	class ControlLine2D
	{
	public:

		ControlLine2D()
			: m_bOriginChanged( false )
			, m_bTargetChanged( false )
		{

		}


		ControlLine2D( const T& ox, const T& oy, const T& tx, const T& ty )
			: m_Origin( ox, oy )
			, m_Target( tx, ty )
			, m_bOriginChanged( false )
			, m_bTargetChanged( false )
		{

		}


		~ControlLine2D()
		{
		
		}


		void Clear()
		{
			InitZero( m_Origin );
			InitZero( m_Target );
			m_bOriginChanged = false;
			m_bTargetChanged = false;
		}


		void SetOrigin( const T& x, const T& y )
		{
			m_bOriginChanged = (m_Origin.x != x) || (m_Origin.y != y);
			InitVec( m_Origin, x, y );
		}


		void SetTarget( const T& x, const T& y )
		{
			m_bTargetChanged = (m_Origin.x != x) || (m_Origin.y != y);
			InitVec( m_Target, x, y );
		}


		void TranslateOrigin( const T& x, const T& y )
		{
			m_bOriginChanged = (x != 0) || (y != 0);
			Add( m_Origin, x, y );
		}

		
		void TranslateTarget( const T& x, const T& y )
		{
			m_bTargetChanged = (x != 0) || (y != 0);
			InitVec( m_Target, x, y );
		}


		void ResetOriginChanged()
		{
			m_bOriginChanged = false;
		}


		void ResetTargetChanged()
		{
			m_bOriginChanged = false;
		}


		//Vec2<T>& Origin()
		//{
		//	return m_Origin;
		//}


		const Vec2<T>& Origin() const
		{
			return m_Origin;
		}


		//Vec2<T>& Target()
		//{
		//	return m_Target;
		//}

		const Vec2<T>& Target() const
		{
			return m_Target;
		}


		bool OriginChanged() const
		{
			return m_bOriginChanged;
		}


		bool TargetChanged() const
		{
			return m_bTargetChanged;
		}



	private:

		Vec2<T>	m_Origin, m_Target;
		bool	m_bOriginChanged, m_bTargetChanged;

	};



}


#endif // !CONTROL_LINE_H
