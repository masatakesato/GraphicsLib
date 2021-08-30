#include	"SimpleMeshRenderer.h"


#include	"./common/TString.h"
#include	"Frustum.h"
#include	"ViewTransformBuffer.h"
#include	"LightAttributeBuffer.h"
#include	"ShadowBuffer.h"



namespace  OreOreLib
{

	// Default Constructor
	SimpleMeshRenderer::SimpleMeshRenderer()
	{
		m_ulMatWorld					= -1;
		m_ulMaterials.numMats			= -1;
		m_ulMaterials.Ambient			= -1;
		m_ulMaterials.Diffuse			= -1;
		m_ulMaterials.Specular			= -1;
		m_ulMaterials.Shiness			= -1;
		m_ulMaterials.RefractiveIndex	= -1;
		m_ulMaterials.Opacity			= -1;
		m_ulMaterialID					= -1;
		m_ulShadowMapArray				= -1;
		m_ulMatLight					= -1;
	}



	// Constructor
	SimpleMeshRenderer::SimpleMeshRenderer( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG		
		tcout << _T("SimpleMeshRenderer( const char *filepath, GLSL_VERSION version )...") << tendl;
#endif

		m_ulMatWorld					= -1;
		m_ulMaterials.numMats			= -1;
		m_ulMaterials.Ambient			= -1;
		m_ulMaterials.Diffuse			= -1;
		m_ulMaterials.Specular			= -1;
		m_ulMaterials.Shiness			= -1;
		m_ulMaterials.RefractiveIndex	= -1;
		m_ulMaterials.Opacity			= -1;
		m_ulMaterialID					= -1;
		m_ulShadowMapArray				= -1;
		m_ulMatLight					= -1;
		
		if( filepath )
			InitShader( filepath, version );
	}


	// Destructor
	SimpleMeshRenderer::~SimpleMeshRenderer()
	{
#ifdef	_DEBUG
		tcout << _T("~SimpleMeshRenderer()...") << tendl;
#endif
	}


	bool SimpleMeshRenderer::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint program_id;

		//===================== Init Scene Rendering Pass =======================//
		// シェーダオブジェクト生成
		m_Shader.Init( filepath, version );
		program_id		= m_Shader.ID();

		// 入力データの設定
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_COLOR, "COLOR" ) );

		// シェーダ変数の設定
		m_ulMatWorld					= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		//m_ulMatViewProj					= glGetUniformLocation( program_id, "g_MatViewProj");

		m_ulMaterials.Ambient			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Materials.Ambient" ) );
		m_ulMaterials.Diffuse			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Materials.Diffuse" ) );
		m_ulMaterials.Specular			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Materials.Specular" ) );
		m_ulMaterials.Shiness			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Materials.Shiness" ) );
		m_ulMaterials.RefractiveIndex	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Materials.RefractiveIndex" ) );
		m_ulMaterials.Opacity			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Materials.Opacity" ) );

		m_ulMaterialID					= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MaterialID" ) );

		m_ulShadowMapArray				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ShadowMapArray" ) );
		m_ulMatLight					= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatLight" ) );


		// 出力データの設定
		//glBindFragDataLocation( program_id,	RsmPosition,	"fPosition");	// ワールド座標
		//glBindFragDataLocation( program_id,	RsmNormal,		"fNormal");		// 法線
		//glBindFragDataLocation( program_id,	RsmFlux,		"fFlux");		// 放射束
		//glBindFragDataLocation( program_id,	RsmDepth,		"fDepth");		// 奥行き

		// プログラムのリンク
		m_Shader.Link();
		
		return true;
	}



	void SimpleMeshRenderer::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type	= typeid(*pbufferobj);
		//tcout << "class name: " << id.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05


		if( buffer_type == typeid(ViewTransformBuffer) )	// ビュー行列だった場合
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Shader.ID() ); 
		}
		else if( buffer_type == typeid(LightAttributeBuffer) )// 光源だった場合
		{
			((LightAttributeBuffer *)pbufferobj)->BindToShader( m_Shader.ID() ); 
		}
		else if( buffer_type == typeid(ShadowBuffer) )	// シャドウバッファの場合
		{
			ShadowBuffer *pshadowbuf = (ShadowBuffer *)pbufferobj;

			// Bind UniformBuffer to Shader
			pshadowbuf->BindToShader( m_Shader.ID() ); 

			// Bind Shadowmap texture
			m_Shader.Bind();
			{
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D_ARRAY, pshadowbuf->GetTexture()->texID );
				glUniform1i( m_ulShadowMapArray, 0 );
			}
			m_Shader.Unbind();
		}
		else
		{
		}

	}



	//void SimpleMeshRenderer::BindLightSource( /*const Frustum& lightFrustum*/Mat4f &matLight )
	//{
	//	// 光源位置をセット
	//	//glUniform3fv( m_ulLightPos, 1, *lightFrustum.GetPosition().xyz );

	//	// 光源系への座標変換行列をセット
	//	glUniformMatrix4fv( m_ulMatLight, 1, GL_FALSE, matLight.m );
	//}



	void SimpleMeshRenderer::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
	{
		MaterialBuffer *pMat	= (MaterialBuffer *)pGeom->GetMaterial();

		//============================= Uniformの設定 ================================//
		glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m );// オブジェクトのワールド変換行列
		
		glUniform3fv( m_ulMaterials.Ambient, pMat->numElms, (float *)pMat->Ambient() );
		glUniform3fv( m_ulMaterials.Diffuse, pMat->numElms, (float *)pMat->Diffuse() );
		glUniform3fv( m_ulMaterials.Specular, pMat->numElms, (float *)pMat->Specular() );
		glUniform1fv( m_ulMaterials.Shiness, pMat->numElms, (float *)pMat->Shininess() );
		glUniform1fv( m_ulMaterials.RefractiveIndex, pMat->numElms, (float *)pMat->RefractiveIndex() );
		glUniform1fv( m_ulMaterials.Opacity, pMat->numElms, (float *)pMat->Opacity() );

		//========================== オブジェクトの描画 ==============================//
		pGeom->Bind();

		for( int i=0; i<pGeom->NumFaceGroups(); ++i )
		{
			glUniform1i( m_ulMaterialID, i );
			pGeom->DrawGroup(i);
		}

		pGeom->Unbind();
	}





}