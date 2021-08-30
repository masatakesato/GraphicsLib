#ifndef	MATRIX_BUFFER_H
#define	MATRIX_BUFFER_H


#include	"MathLib.h"
#include	"CacheDataManager.h"
#include	"GLUniformBufferObject.h"
#include	"IBufferObject.h"



namespace OreOreLib
{


	class MatrixBuffer : public IBufferObject
	{
	public:

		MatrixBuffer();
		MatrixBuffer( int num_maxelms );
		~MatrixBuffer();
		MatrixBuffer( const MatrixBuffer& obj );

		void Init( int num_maxelms );
		void Release();
		

		int NumMaxSlots() const					{ return m_CacheManager.numMaxSlots(); }
		int	NumActiveSlots() const				{ return m_CacheManager.numReservedSlots(); }

		bool IsRererved( int slot_id ) const	{ return m_CacheManager.IsRererved(slot_id); }	// 指定スロットが使用中かどうか調べる
		bool IsDirty( int slot_id ) const		{ return m_CacheManager.IsDirty(slot_id); }		// 最近指定スロットへのアクセスがあったかどうか調べる
		bool IsClean( int slot_id ) const		{ return m_CacheManager.IsClean(slot_id); }		// 指定スロットがアクセスなしの放置状態になっているかどうか調べる
		bool IsFull() const						{ return m_CacheManager.IsFull(); }				// キャッシュが満杯かどうかチェックする

		bool SetClean( int slot_id )			{ return m_CacheManager.SetClean(slot_id); }
		bool SetDirty( int slot_id )			{ return m_CacheManager.SetDirty(slot_id); }
		int ReserveSlot()						{ return m_CacheManager.ReserveSlot(); }		// 空きスロットを予約する．
		bool FreeSlot( int slot_id )			{ return m_CacheManager.FreeSlot(slot_id); }	// 指定したスロットを解放する
		void Clear()							{ m_CacheManager.Clear(); }						// 全スロットを解放する

		Mat4f* GetMatrix( int slot_id ) const	{ return m_Matrices + slot_id; }

		void BindToShader( GLuint program_id );
		void UpdateSubresource()				{ m_uboMatrices.UpdateSubresource( m_Matrices, sizeof(Mat4f) * m_CacheManager.numMaxSlots() ); }


	private:

		// Cache Data Manager
		CacheDataManager		m_CacheManager;

		// ShadowMatrices
		Mat4f					*m_Matrices;	// 光源毎の座標変換行列

		// Hardware Resources
		GLUniformBufferObject	m_uboMatrices;// UniformBufferObject
		static GLuint			m_BindPoint;
	};






}// end of namespace


#endif	// MATRIX_BUFFER_H //