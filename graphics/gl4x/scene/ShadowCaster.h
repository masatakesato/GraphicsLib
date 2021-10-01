#ifndef	SHADOW_MAP_H
#define	SHADOW_MAP_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"../resource/BufferLayout.h"
#include	"../resource/GLFrameBufferObject.h"

#include	"Frustum.h"



namespace OreOreLib
{


	class IShadowCaster
	{

	};




	class ShadowCaster : public IShadowCaster
	{
	public:

		ShadowCaster();
		ShadowCaster( int width, int height, float distlimit, float logfactor );
		~ShadowCaster();
		//ShadowCaster( const ShadowCaster& obj );
		
		void Init( int width, int height,float distlimit, float logfactor );
		void Release();

		void SetDistanceLimit( float dist )				{ m_MaxDistance = dist; }
		void SetSplitLogFactor( float factor )			{ m_SplitLogFactor = factor; }
		void SetShadowMapSize( int width, int height )	{ InitVec( m_HalfShadowMapSize, 0.5f * float(width), 0.5f * float(height) ); }

		bool CalcMatrix_Directional( int num_cascades, Mat4f matLightViewProjCrops[], float splitDepths[], const Frustum& cameraFrustum, const Frustum& lightFrustum );
		bool CalcMatrix_Omni( int num_cascades, Mat4f matLightViewProjCrops[], float splitDepths[], const Frustum& cameraFrustum, const Frustum& lightFrustum );


	private:

		struct Quad
		{
			Vec3f verts[4];
		};

		// Global Params for Cascade Calculation
		float		m_MaxDistance;
		float		m_SplitLogFactor;
		Vec2f		m_HalfShadowMapSize;

		void UpdateSplitDepths( int num_cascades, float splitdepths[], float znear, float zfar );
		void CalculateCropMatrix( Mat4f* matCrop, const Vec3f& cropBBmin, const Vec3f& cropBBmax );
		void CalculateCropMatrix_flicker( Mat4f* matCrop, const Vec3f& cropBBmin, const Vec3f& cropBBmax );

	};




	// シャドウキャスター毎のフレームバッファアタッチメント. 
	// RenderSceneクラスがメンバ変数として保持する. m_LightAttributBufferとm_ShadowBufferは、このオブジェクトへの参照を保持する
	class ShadowAttachments
	{
	public:

		ShadowAttachments();
		~ShadowAttachments();

		void Init( int numshadowcasters );
		void Release();
		
		inline void Bind( int attachment_id )				{ (m_Attachments + attachment_id)->BindAttachmentLayer(); (m_Attachments + attachment_id)->DrawBuffers(); }
		inline void Bind( int attachment_id, int layer )	{ (m_Attachments + attachment_id)->BindAttachmentLayer(layer); (m_Attachments + attachment_id)->DrawBuffer(layer); }
		//inline void Unbind();

		void BindShadowMap( uint32 texid );
		void UnbindShadowMap();
		
		bool SetupAttachment( int idx, int numattachments, const uint32 texids[], const uint32 attachments[],  const int levels[]=NULL, const int layers[]=NULL );
		
		bool SetNumAttachPoints( int attachment_id, int numattachments );
		bool SetTexID( int attachment_id, uint32 texid )		{ return m_Attachments[attachment_id].SetTexID( texid ); }
		bool SetAttachments( int attachment_id, int numattachments, const uint32 attachments[] );
		bool SetLevels( int attachment_id, int numattachments, const int levels[] );
		bool SetLayers( int attachment_id, int numattachments, const int layers[] );


		int Size() const												{ return m_numShadowCasters; }
		FrameBufferAttachment* GetAttachment( int attachment_id ) const	{ return m_Attachments + attachment_id; }
		int NumShadows( int attachment_id ) const						{ return ( m_Attachments + attachment_id )->NumAttachPoints(); }
		int *GetShadowIndices( int attachment_id ) const				{ return ( m_Attachments + attachment_id )->GetLayers(); }

	private:
	
		// Attachments
		int						m_numShadowCasters;
		FrameBufferAttachment	*m_Attachments;

	};


}// end of namespace


#endif	// SHADOW_MAP_H //