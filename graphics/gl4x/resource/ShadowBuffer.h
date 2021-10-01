#ifndef	SHADOW_BUFFER_H
#define	SHADOW_BUFFER_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"CacheDataManager.h"
#include	"GLFrameBufferObject.h"
#include	"GLUniformBufferObject.h"
#include	"Texture.h"
#include	"IBufferObject.h"



namespace OreOreLib
{

	class ShadowBuffer : public IBufferObject
	{
	public:

		ShadowBuffer();
		~ShadowBuffer();
		
		void Init( int texwidth, int texheight, int num_maxshadows );
		void Release();


		int Width() const						{ return m_ShadowMapArray.Width(); }
		int Height() const						{ return m_ShadowMapArray.Height(); }
		Mat4f* GetMatrix( int slot_id ) const	{ return (m_MatShadows + slot_id); }
		Texture2D* GetTexture() const			{ return (Texture2D *)&m_ShadowMapArray; }
		GLFrameBufferObject* GetFBO() const		{ return (GLFrameBufferObject *)&m_FboShadowMap; }

		void SetMatrix( int slot_id, const Mat4f& mat )								{ m_MatShadows[slot_id] = mat; }
		void SetSplitDepth( int slot_id, float depth )								{ m_SplitDepths[slot_id] = depth; }
		void SetMatrixAndSplitDepth( int slot_id, const Mat4f& mat, float depth )	{ m_MatShadows[slot_id] = mat; m_SplitDepths[slot_id] = depth; }

		int NumMaxSlots() const					{ return m_CacheManager.numMaxSlots(); }
		int	NumActiveSlots() const				{ return m_CacheManager.numReservedSlots(); }

		bool IsRererved( int slot_id ) const	{ return m_CacheManager.IsRererved(slot_id); }	// 指定スロットが使用中かどうか調べる
		bool IsDirty( int slot_id ) const		{ return m_CacheManager.IsDirty(slot_id); }		// 最近指定スロットへのアクセスがあったかどうか調べる
		bool IsClean( int slot_id ) const		{ return m_CacheManager.IsClean(slot_id); }		// 指定スロットがアクセスなしの放置状態になっているかどうか調べる
		bool IsFull() const						{ return m_CacheManager.IsFull(); }				// キャッシュが満杯かどうかチェックする
	
		bool SetClean( int slot_id )			{ return m_CacheManager.SetClean(slot_id); }
		bool SetDirty( int slot_id )			{ return m_CacheManager.SetDirty(slot_id); }
		int ReserveSlot( int numslots=1 )		{ return m_CacheManager.ReserveSlot(numslots); }// 個数を指定して空きスロット(群)を予約する．
		bool FreeSlot( int slot_id )			{ return m_CacheManager.FreeSlot(slot_id); }	// 指定したスロットを解放する
		void Clear()							{ m_CacheManager.Clear(); }						// 全スロットを解放する
		
		bool GetSerialSlots( int slot_id, int num_slots, int *slots )	{ return m_CacheManager.GetSerialSlots( slot_id, num_slots, slots ); }


		inline void Bind()		{ m_FboShadowMap.Bind(); }
		inline void Unbind()	{ m_FboShadowMap.Unbind(); }
		void BindToShader( GLuint program_id );
		void UpdateSubresource();


	private:
		
		// Cache Data Manager
		CacheDataManager		m_CacheManager;

		// Data
		Mat4f					*m_MatShadows;	// ShadowMatrices
		float					*m_SplitDepths;	// Split Depths
		

		// Hardware Resources
		Texture2D				m_ShadowMapArray;	// シャドウマップのGPUリソース. 解像度の異なるTexture2DArray (2048x2048, )
		
		// UniformBufferObject
		GLUniformBufferObject	m_uboMatShadows;
		GLUniformBufferObject	m_uboSplitDepths;
		GLUniformBufferObject	m_uboShadowIDLinkedList;
		
		static GLuint			m_BindPoints[3];


		//	FrameBufferAttachment	m_Attachment;
		GLFrameBufferObject		m_FboShadowMap;


		// Reference
		//ShadowAttachments		*m_refShadowAttachments;


		ShadowBuffer( const ShadowBuffer& obj ){}

	};



}// end of namespace


#endif	// SHADOW_BUFFER_H //