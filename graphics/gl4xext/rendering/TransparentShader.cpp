#include	"TransparentShader.h"


#include	"ViewTransformBuffer.h"
#include	"LightAttributeBuffer.h"
#include	"ShadowBuffer.h"
#include	"MeshObject.h"


namespace OreOreLib
{


	// default constructor
	TransparentShader::TransparentShader()
	{
#ifdef _DEBUG
		tcout << "TransparentShader()..." << tendl;
#endif // _DEBUG

		m_ulMatWorld		= -1;
		m_ulTexture			= -1;
		m_ulDiffuse			= -1;
		m_ulSpecular		= -1;
		m_ulShininess		= -1;
		m_ulTransparency	= -1;
		m_ulShadowMapArray	= -1;
		m_ulBackgroundColor	= -1;
	}


	// constructor
	TransparentShader::TransparentShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << "TransparentShader( const TCHAR *filepath, GLSL_VERSION version )..." << tendl;
#endif // _DEBUG
		
		m_ulMatWorld		= -1;
		m_ulTexture			= -1;
		m_ulDiffuse			= -1;
		m_ulSpecular		= -1;
		m_ulShininess		= -1;
		m_ulTransparency	= -1;
		m_ulShadowMapArray	= -1;
		m_ulBackgroundColor	= -1;
		
		if( filepath )
			InitShader( filepath, version );
	}



	// destructor
	TransparentShader::~TransparentShader()
	{
#ifdef _DEBUG
		tcout << "~TransparentShader()..." << tendl;
#endif // _DEBUG
		Release();
	}



	void TransparentShader::Release()
	{
		for( int i=0; i<NUM_PASS; ++i )
			m_Pass[i].Release();

		m_ulMatWorld		= -1;
		m_ulTexture			= -1;
		m_ulDiffuse			= -1;
		m_ulSpecular		= -1;
		m_ulShininess		= -1;
		m_ulShadowMapArray	= -1;
		m_ulBackgroundColor	= -1;
	}



	// init shader
	void TransparentShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint	program_id;

		//=================== Init Accumulation Pass ====================//
		// Create Shader
		m_Pass[PASS_ACCUM].Init( _T("BWOITAccumulate.glsl"), version );
		program_id	= m_Pass[ PASS_ACCUM ].ID();

		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );

		// Init Uniform Locations
		m_ulMatWorld			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		
		m_ulTexture				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Texture" ) );
		m_ulDiffuse				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Diffuse" ) );
		m_ulSpecular			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Specular" ) );
		m_ulShininess			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Shininess" ) );
		m_ulTransparency		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Transparency" ) );

		m_ulShadowMapArray		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ShadowMapArray" ) );

		// Init FragData Locations
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 0, "outAccum" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, 1, "outRevealage" ) );

		// Link Shader
		m_Pass[ PASS_ACCUM ].Link();


		//===================== Init Composite Pass ======================//
		// Create Shader
		m_Pass[PASS_COMPOSITE].Init( _T( "BWOITComposite.glsl" ), version );
		program_id	= m_Pass[ PASS_COMPOSITE ].ID();
	
		// Init Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );
		
		// Init Attribute Locations
		m_ulBackgroundColor	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_BackgroundColor" ) );

		// Link Shader
		m_Pass[ PASS_COMPOSITE ].Link();

		// Set Texture Uniform Locations
		m_Pass[ PASS_COMPOSITE ].Bind();
		{
			// Accum Texture
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexAccum" ), TEX_ACCUM ) );
			// Alpha Texture
			GL_SAFE_CALL( glUniform1i( glGetUniformLocation( program_id, "g_TexRevealage" ), TEX_REVEALAGE ) );
		}
		m_Pass[ PASS_COMPOSITE ].Unbind();


	}



	void TransparentShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info& id = typeid(*pbufferobj);

		if( id == typeid(ViewTransformBuffer) )
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Pass[PASS_ACCUM].ID() );
		}
		else if( id == typeid(LightAttributeBuffer) )
		{
			((LightAttributeBuffer *)pbufferobj)->BindToShader( m_Pass[PASS_ACCUM].ID() );

		}
		else if( id == typeid(ShadowBuffer) )
		{
			ShadowBuffer *pshadowbuf	= (ShadowBuffer *)pbufferobj;

			// Bind Uniform Buffer Objects
			pshadowbuf->BindToShader( m_Pass[ PASS_ACCUM ].ID() );

			// Bind ShadowmapArray Texture(シェーダーバインドするたびにテクスチャバインドしなおし?)
			m_Pass[ PASS_ACCUM ].Bind();

			glActiveTexture( GL_TEXTURE6 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID ) );
			glUniform1i( m_ulShadowMapArray, 6 );

			m_Pass[ PASS_ACCUM ].Unbind();
		}


	}



	// メッシュデータをレンダリングする. 事前にBindShaderしておくこと
	void TransparentShader::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
	{
		const MaterialBuffer *pMat	= (MaterialBuffer *)pGeom->GetMaterial();
		const TextureBuffer *pTex	= pGeom->GetTexure();
	
			//=============================== UniformLocationの設定 ===============================//
		// World Matrix
		GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m ) );

		// Texture
		if( pTex->pData )
		{
			glActiveTexture( GL_TEXTURE0 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_2D, *pTex->texID() ) );
			glUniform1i( m_ulTexture, 0 );
		}


		// Material Parameter
		if( pMat->pData )
		{
			//const MaterialParams *mtlparam = ((MaterialBuffer *)pMesh->m_pMaterialBuffer)->Layer();
			GL_SAFE_CALL( glUniform3fv( m_ulDiffuse, pMat->numElms, (float *)pMat->Diffuse() ) );
//			glUniform3fv( m_ulSpecular, pMat->numElms, (float *)pMat->Specular() );
//			glUniform1fv( m_ulShininess, pMat->numElms, (float *)pMat->Shininess() );

			//glUniform3fv( m_ulMaterials.Ambient, pMat->numElms, (float *)pMat->Ambient() );
			//glUniform1fv( m_ulMaterials.RefractiveIndex, pMat->numElms, (float *)pMat->RefractiveIndex() );
			GL_SAFE_CALL( glUniform1f( m_ulTransparency, 1.0f - *pMat->Opacity() ) );
		}


		//=============================== オブジェクトの描画 ==================================//
		pGeom->Draw();

	}
	


	void TransparentShader::Render( const Texture2D* pTexAccum, const Texture2D* pTexRevealage, const Vec3f* bgColor )
	{
		if( !m_refScreenSpaceQuad )	return;

		m_Pass[ PASS_COMPOSITE ].Bind();
		{
			// Bind Textures
			GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[TEX_ACCUM] ) );
			glBindTexture( GL_TEXTURE_2D, pTexAccum->texID );

			GL_SAFE_CALL( glActiveTexture( g_DefaultTextureUnits[TEX_REVEALAGE] ) );
			glBindTexture( GL_TEXTURE_2D, pTexRevealage->texID );

			// Bind Uniform Locations
			GL_SAFE_CALL( glUniform3fv( m_ulBackgroundColor, 1, bgColor->xyz ) );
			
			// Draw ScreenSpace Quad
			m_refScreenSpaceQuad->Draw();

			// Unbind Textures
			glActiveTexture( g_DefaultTextureUnits[TEX_ACCUM] );
			glBindTexture( GL_TEXTURE_2D, 0 );

			glActiveTexture( g_DefaultTextureUnits[TEX_REVEALAGE] );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
		m_Pass[ PASS_COMPOSITE ].Unbind();
	
	}


}// end of namespace