#ifndef	RENDER_SCENE_H
#define	RENDER_SCENE_H


#include	"./common/Utility.h"
#include	"SceneGraph.h"

#include	"GLBindPointManager.h"
#include	"GLBufferObject.h"
#include	"GLVertexArrayObject.h"

#include	"IShader.h"
//#include	"GLFrameBufferObject.h"
#include	"GLRenderTarget.h"
#include	"LightAttributeBuffer.h"
#include	"ShadowCaster.h"
#include	"ShadowBuffer.h"
#include	"SSDOBuffer.h"
#include	"ViewTransformBuffer.h"
#include	"GeometryBuffer.h"
#include	"LPVIrradianceVolume.h"
#include	"VCTVoxelData.h"

#include	"VoxelConeTracingPass.h"


#include	"../labworks/TerrainEngine.h"



namespace OreOreLib
{





	// このクラスの役割り
	// レンダーキューを作る
	// カリング
	// ステートソーティング
	// depth-prepass
	// state sorting

	// 必要な機能
	// ソーティング
	// 空間分割



	class RenderScene
	{
	public:

		RenderScene();	// Default Constructor
		~RenderScene();	// Destructor

		void InitViewTransformBuffer();
		void InitOffScreenBuffers( int screen_width, int screen_height );
		void InitShadowBuffer( int width, int height, int numcascades );
		void InitLightAttributeBuffer( int num_max_lights );
		void InitIrradianceVolume( int width, int height, int depth, int num_cascades, float dim );
		void InitVCTVoxelData( int width, int height, int depth, int numcascades, float dim );
		void InitRSMBuffer( int width, int height );
		void Release();

		void BindSceneGraph( SceneGraph *sg );
		void UnbindSceneGraph();

void SetIrradianceVolumeUpdateFrag()	{ m_bUpdateIrradianceVolume = true; }
bool GetIrradianceVolumeUpdateFrag()	{ return m_bUpdateIrradianceVolume; }

		void Update();
		void UpdateShadowMatrices( Frustum& cameraFrustum );
		void UpdateIrradianceVolume( bool updatepos );
		void UpdateVCTVoxel();
		void UpdateShadowBuffer();
		void UpdateReflectiveShadowMaps( LightObject* pLightObj );

		// render methods for test purpose
		void RenderForward();
		void RenderDeferred( bool enableIV );
		void RenderConstant( const Vec4f* color=NULL );
		void RenderTransparent();
		void RenderGbuffer();
		void RenderZPrepass();
		void RenderAntialias( bool enable );
		void RenderMSAA();
		//void RenderLightPrePass();
		void RenderSSS();
		void RenderSSDO();
		void RenderLocalReflection();
		void RenderLightShaft();
		void RenderDOF();
		void RenderHDR();
		void RenderLightPropagationVolumes();
		void RenderVoxelConeTracing( bool useIrrad );
		void RenderVolumeRayCasting();

		void ViewShadowMaps( int width, int height );
		void ViewReflectiveShadowMaps( int width, int height );
		void RenderQuadTile();


	private:


		Vec2i					m_ScreenSize;

		// Reference to SceneGraph Object
		SceneGraph				*m_pSceneGraph;

		// Shader List
		std::vector<IShader *>	m_Shaders;

		// Render Queue for Shadowmap Rendering
		int						m_numShadowObjects;	// num of Renderabnle Objects
		MovableObject			*m_RenderShadowQueue[MAX_RENDER_QUEUE];

		// Render Queue for Opaque Rendering
		int						m_numRenderObjects[NUM_SHADER_TYPE];
		MovableObject			*m_RenderObjects[NUM_SHADER_TYPE][MAX_RENDER_QUEUE];

		// Render Queue for Transparent Rendering
		int						m_numTransparentObjects[NUM_SHADER_TYPE];
		MovableObject			*m_TransparentObjects[NUM_SHADER_TYPE][MAX_RENDER_QUEUE];


		// TODO: レンダーパスクラスの実験.VoxelConeTracingで使用する描画オブジェクトのリストを管理する (2015.05.04)
		VoxelConeTracingPass	m_VCTPass;

		// Lightsource Lists
		int						m_numLights[NUM_LIGHT_TYPE];
		SceneNode				*m_LightSources[NUM_LIGHT_TYPE][MAX_LIGHTS];


		LightAttributeBuffer	m_LightAttributeBuffer;
		ShadowAttachments		m_ShadowAttachments;

		// Light Geometry
		//MeshObject			m_LightGeometry[ NUM_LGEOM_TYPE ];

		// ShadowBuffer
		ShadowBuffer			m_ShadowBuffer;

		// ShadowCaster
		ShadowCaster			m_ShadowCaster;

		// Camera Matrix
		ViewTransformBuffer		m_ViewTransformBuffer;

		// Gbuffer
		GLRenderTarget			m_RTGbuffer;	// GBuffer RenderTarget

//		GLFrameBufferObject		m_FboGbuffer;
//		FrameBufferAttachment	m_AttachmentGbuffer;
		Texture2D				m_texPosition;
		Texture2D				m_texNormal;
		Texture2D				m_texDepth;
		Texture2D				m_texDiffuse;

		Texture2D				m_RawShadedResult;


		// Irradiance Buffer
		Texture2D				m_IrradianceBuffer;

		// FBO for Weighted Blended OIT
		GLRenderTarget			m_RTOIT;
		//GLFrameBufferObject		m_FboOIT;
		//FrameBufferAttachment	m_AttachmentOIT;

		Texture2D				m_OITAccumBuffer;
		Texture2D				m_OITRevealageBuffer;

		// Reflective Shadow Maps
		GLRenderTarget			m_RTReflectiveShadowMaps;
		//GLFrameBufferObject		m_FboRSM;
		//FrameBufferAttachment	m_AttachmentRSM;
		Texture2D				m_RsmPositionBuffer;// Position(world space)
		Texture2D				m_RsmNormalBuffer;	// Normal
		Texture2D				m_RsmFluxBuffer;	// Flux
		Texture2D				m_RsmDepthBuffer;	// Depth

		// Irradiance Volumes for Light Propagation Volumes
		LPVIrradianceVolume		m_LPVIrradianceVolume;

		// Voxel Data for Voxel Cone Tracing
		VCTVoxelData			m_VCTVoxelData;

		// SSDO Buffer
		SSDOBuffer				m_SSDOBuffer;

		// VertexArrayObjects for mesh rendereing
		int						m_numVAOs;
		GeometryBuffer			m_GeometryObjects[MAX_RENDER_QUEUE];

		// Screen Space Quad
		GLVertexArrayObject		m_ScreenSpaceQuad;

		// Unit Cube
		GLVertexArrayObject		m_UnitCube;


		//######################## Update Frags ###########################//
		bool				m_bUpdateIrradianceVolume;



		SeamlessTileMesh	m_TileMesh;// 実験



		inline void AssignVertexArrayObject( SceneNode *pNode )
		{
			if( pNode == NULL || m_numVAOs >= MAX_RENDER_QUEUE )return;

			if( !pNode->IsRoot() && m_numShadowObjects < MAX_RENDER_QUEUE )
			{
				GeometryBuffer *pGeom	= &m_GeometryObjects[m_numVAOs++];
				MeshObject *pMeshObj	= ((MeshObject *)pNode->m_pObject);
				pGeom->Init( pMeshObj->GetPrimitiveType(), pMeshObj->m_pVertexBuffer, pMeshObj->m_pIndexBuffer, pMeshObj->GetTransformMatrix(), pMeshObj->m_pMaterial, pMeshObj->m_pTexture, pMeshObj->m_pFaceGroupBuffer, GL_STATIC_DRAW, GL_STATIC_DRAW );
				pNode->m_pObject->AttachBufferObject( pGeom );
			}

			AssignVertexArrayObject( (SceneNode *)pNode->children );
			AssignVertexArrayObject( (SceneNode *)pNode->next );
		}



		inline void TraverseMeshTree( SceneNode *pNode )
		{
			if( pNode == NULL )	return;

			if( !pNode->IsRoot() )
			{
				// マテリアルの種類ごとに、レンダーキューにSceneNodeポインタを登録する. TODO: マテリアル種類からシェーダー種類への変換処理が必要.
				const uint8 shd_type = ((MeshObject *)pNode->m_pObject)->m_MeshAttrib.ShaderType;
				int &shd_obj_idx	= m_numRenderObjects[shd_type];

				if( shd_obj_idx < MAX_RENDER_QUEUE )
				{
					m_RenderObjects[shd_type][shd_obj_idx++]	= pNode->GetAttachedObject();
				}

				// TODO: 影生成オブジェクトも登録しておく.一旦は全部対象にする
				if( m_numShadowObjects < MAX_RENDER_QUEUE )
					m_RenderShadowQueue[m_numShadowObjects++] = pNode->GetAttachedObject();

				// TODO: 実験. ボクセルコーントレーシング用オブジェクトを集める
				m_VCTPass.AddRenderObject( pNode->GetAttachedObject() );

			}

			TraverseMeshTree( (SceneNode *)pNode->children );
			TraverseMeshTree( (SceneNode *)pNode->next );
		}



		//	inline void TraverseMeshTree( SceneNode *pNode )
		//	{
		//		if(pNode == NULL)	return;
		//	
		//		if( !pNode->IsRoot() && m_numShadowObjects<MAX_RENDER_QUEUE )
		//		{
		//			// マテリアルの種類ごとに、レンダーキューにSceneNodeポインタを登録する. TODO: マテリアル種類からシェーダー種類への変換処理が必要.
		//			const uint8 shd_type = ((MeshObject *)pNode->m_pObject)->m_MeshAttrib.ShaderType;
		//			int &shd_obj_idx	= m_numRenderObjects[ shd_type ];
		//			m_RenderObjects[ shd_type ][ shd_obj_idx ]	= pNode->GetAttachedObject();
		//			shd_obj_idx++;
		//
		//// TODO: 影生成オブジェクトも登録しておく.一旦は全部対象にする
		//			m_RenderShadowQueue[m_numShadowObjects++] = pNode->GetAttachedObject();
		//		}
		//
		//		TraverseMeshTree( (SceneNode *)pNode->children );
		//		TraverseMeshTree( (SceneNode *)pNode->next );
		//	}


		//TODO: フレーム毎にバッファ再割り当て？？？重い。「キャッシュ」の仕組みをうまく使う方法を考える
		inline void TraverseLightTree( SceneNode *pNode )
		{
			if( pNode == NULL )	return;

			if( !pNode->IsRoot() )
			{
				LightObject *pLightObj = (LightObject *)pNode->m_pObject;
				LIGHT_TYPE ltype = pLightObj->m_LightAttrib.LightType;


				if( pLightObj->EnableLighting() && m_numLights[ltype] < MAX_LIGHTS && !m_LightAttributeBuffer.IsFull() )	//if( pLightObj->EnableLighting() )
				{
					Frustum *pLightFrustum	= pLightObj->GetFrustum();

					//####################### LightNodeとLightAttributeBufferを対応付ける ##########################//
					if( pLightObj->GetBufferID() == -1 )	m_LightAttributeBuffer.ReserveSlot( pLightObj );
					int slot_id	= pLightObj->GetBufferID();


					//######################### ShadowBufferを確保する. とりあえず6枚 ##############################//
					int	shadowbuffer_id	= -1;
					int	numShadows		= 0;
					int	shadowIDs[MAX_SHADOW_CASCADES];

					if( pLightObj->CastShadow() )
					{
						// ShadowBufferを確保する
						numShadows		= 4;
						shadowbuffer_id	= m_ShadowBuffer.ReserveSlot( numShadows );

						// ShadowAttachmentを初期化する
						m_ShadowBuffer.GetSerialSlots( shadowbuffer_id, numShadows, shadowIDs );
						m_ShadowAttachments.SetupAttachment( slot_id, numShadows, NULL, g_DefaultColorAttachments, g_DefaultTexLevels, shadowIDs );
						m_ShadowAttachments.SetTexID( slot_id, m_ShadowBuffer.GetTexture()->texID );
					}

					//############################# LightAttributeへデータを転送する ###############################//
					m_LightAttributeBuffer.SetLightAttribute( slot_id, *pLightFrustum->GetAttribute(), pLightObj->m_LightAttrib, (uint8)numShadows, (short)shadowbuffer_id );

					// 光源の種類毎にノード参照を保持する
					m_LightSources[(int)ltype][m_numLights[ltype]++]	= pNode;
				}

			}

			TraverseLightTree( (SceneNode *)pNode->children );
			TraverseLightTree( (SceneNode *)pNode->next );
		}


	};

	// しばらくアクセスのない光源データは自動的に削除する -> m_BufferIDを更新する仕組みが必要になる



}// end of namespace


#endif	// RENDER_SCENE_H //



// 2階層のレイヤーがある？？
// 属性とカテゴリ分け
// 属性をベースにした描画オブジェクトの