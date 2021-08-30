#include	"ReflectiveShadowMapsShader.h"



#include	"./common/TString.h"
#include	"ViewFrustum.h"
#include	"LightAttributeBuffer.h"




namespace OreOreLib
{

	// Default constructor
	ReflectiveShadowMapsShader::ReflectiveShadowMapsShader()
	{
#ifdef _DEBUG
		tcout << _T( "ReflectiveShadowMapsShader()..." ) << tendl;
#endif // _DEBUG

		m_ProjectionType	= RENDER_PASS_PERSPECTIVE;
		//m_refFbo			= NULL;
		InitZero( m_RsmSize );
	}



	// Constructor
	ReflectiveShadowMapsShader::ReflectiveShadowMapsShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << _T( "ReflectiveShadowMapsShader( const TCHAR *filepath, GLSL_VERSION version )..." ) << tendl;
#endif // _DEBUG

		m_ProjectionType	= RENDER_PASS_PERSPECTIVE;
		//m_refFbo			= NULL;
		InitZero( m_RsmSize );

		if( filepath )
			InitShader( filepath, version );
	}



	// destructor
	ReflectiveShadowMapsShader::~ReflectiveShadowMapsShader()
	{
#ifdef _DEBUG
		tcout << _T( "~ReflectiveShadowMapsShader()..." ) << tendl;
#endif // _DEBUG

		for( int i=0; i<NUM_RENDER_PASS; ++i )	m_Pass[i].Release();
	}



	// init shader
	void ReflectiveShadowMapsShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{

		for( int pass=0; pass<NUM_RENDER_PASS; ++pass )
		{
			GLuint	program_id;

			// create Shader
			m_Pass[pass].Init( filepath, version );
			program_id	= m_Pass[ pass ].ID();

			// init attribute location
			GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
			GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );
			GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );// マスクで必要
			
			// init uniform location
			m_Param[pass].ulMatWorld	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
			m_Param[pass].ulMatLight	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatLight" ) );
			//m_Param[ pass ].ulViewPos	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ViewPos" ) );
			m_Param[pass].ulRsmSize		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_RsmSize" ) );
			m_Param[pass].ulLightID		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LightID" ) );
			//m_Param[ pass ].ulLightPos	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_LightPos" ) );
			m_Param[pass].ulTexelCoeff	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_TexelCoeff" ) );


			m_Param[pass].ulDiffuse		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Diffuse" ) );

			
			// init fragment output location
			GL_SAFE_CALL( glBindFragDataLocation( program_id,	FRAG_LOCATION_POSITION,	"fPosition" ) );	// ワールド座標
			GL_SAFE_CALL( glBindFragDataLocation( program_id,	FRAG_LOCATION_NORMAL,	"fNormal" ) );		// 法線
			GL_SAFE_CALL( glBindFragDataLocation( program_id,	FRAG_LOCATION_FLUX,		"fFlux" ) );		// 放射束
			GL_SAFE_CALL( glBindFragDataLocation( program_id,	FRAG_LOCATION_DEPTH,	"fDepth" ) );		// 奥行き

			// link shader
			m_Pass[ RENDER_PASS_PERSPECTIVE ].Link();		
		}

	}



	void ReflectiveShadowMapsShader::SetLightID( int light_id )
	{
		GL_SAFE_CALL( glUniform1i( m_Param[m_ProjectionType].ulLightID, light_id ) );
	}


	
	void ReflectiveShadowMapsShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info& id = typeid(*pbufferobj);

		if( id == typeid(LightAttributeBuffer) )
		{
			for( int i=0; i<NUM_RENDER_PASS; ++i )
				((LightAttributeBuffer *)pbufferobj)->BindToShader( m_Pass[i].ID() );
		}
		
	}



	//void ReflectiveShadowMapsShader::BindOutputTexture( Texture2D* texposition, Texture2D* texnormal, Texture2D* texflux, Texture2D* texdepth )
	//{
	//	InitVec( m_RsmSize, texposition->Width(), texposition->Height() );

	//	// フレームバッファオブジェクトを初期化する
	//	m_Fbo.Init( m_RsmSize.x, m_RsmSize.y );

	//	// フレームバッファオブジェクトにテクスチャをアタッチする
	//	const uint32 chennels[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	//	const uint32 texids[] = { texposition->texID, texnormal->texID, texflux->texID, texdepth->texID };
	//	m_Attachment.Init( ARRAYSIZE(texids), texids, chennels );
	//	m_Fbo.BindAttachment( m_Attachment );

	//	for( int i=0; i<NUM_RENDER_PASS; ++i )
	//	{
	//		m_Pass[i].Bind();
	//		glUniform2iv( m_Param[i].ulRsmSize, 1, m_RsmSize.xy );		// RSMsResolution
	//		m_Pass[i].Unbind();
	//	}

	//}



	//void ReflectiveShadowMapsShader::UnbindInputTexture()
	//{
	//	m_Fbo.UnbindAttachment();
	//	m_Fbo.Release();
	//}



	
	void ReflectiveShadowMapsShader::Render( const GeometryBuffer *pGeom, Frustum *pfrustum )
	{
		const MaterialBuffer *pMat	= (MaterialBuffer *)pGeom->GetMaterial();
		float texelCoeff	= (4.0f * *pfrustum->GetAspectRatio() * pow( tan( *pfrustum->GetFov() / 2.0f ), 2.0f )) / float( m_RsmSize.x * m_RsmSize.y );


		//=============================== UniformLocationの設定 ===============================//
		// World Matrix
		GL_SAFE_CALL( glUniformMatrix4fv( m_Param[m_ProjectionType].ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m ) );

		// Light View Projection Matrix
		GL_SAFE_CALL( glUniformMatrix4fv( m_Param[m_ProjectionType].ulMatLight, 1, GL_FALSE, pfrustum->GetViewProjectionMatrix()->m ) );
		
		// RSM Size
		GL_SAFE_CALL( glUniform2iv( m_Param[m_ProjectionType].ulRsmSize, 1, m_RsmSize.xy ) );		// RSMsResolution
		
		// Texel Coeff
		GL_SAFE_CALL( glUniform1f( m_Param[m_ProjectionType].ulTexelCoeff, texelCoeff ) );
		
		// Material Parameter
		if( pMat->pData )
		{
			const MaterialAttribute *mtlparam = (MaterialAttribute *)pMat->pData;
			//const MaterialParams *mtlparam = ((MaterialBuffer *)pMesh->m_pMaterialBuffer)->Layer();
			GL_SAFE_CALL( glUniform3fv( m_Param[m_ProjectionType].ulDiffuse, 1, mtlparam->Diffuse.rgb ) );
		}
		
		
		//=============================== オブジェクトの描画 ==================================//
		pGeom->Draw();
		
	}





}// end of namespace