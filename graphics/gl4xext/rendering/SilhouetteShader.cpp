#include	"SilhouetteShader.h"

#include	<graphics/gl4x/scene/MeshObject.h>



namespace OreOreLib
{


// default constructor
	SilhouetteShader::SilhouetteShader()
	{
		m_pShader				= NULL;
		m_ulMatWorld			= -1;
		m_ulMatViewProj			= -1;
		m_ulTexture				= -1;
		m_ulDiffuse				= -1;
	}


	// constructor
	SilhouetteShader::SilhouetteShader( const TCHAR* filepath, GLSL_VERSION version )
	{
		m_pShader				= NULL;
		m_ulMatWorld			= -1;
		m_ulMatViewProj			= -1;
		m_ulTexture				= -1;
		m_ulDiffuse				= -1;

		if( filepath )
			InitShader( filepath, version );
	}

	// destructor
	SilhouetteShader::~SilhouetteShader()
	{
		SafeDelete( m_pShader );


	}

	// init shader
	void SilhouetteShader::InitShader( const TCHAR* filepath, GLSL_VERSION version )
	{
		// create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );

		// init attribute location
		glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_POSITION, "POSITION" );
		glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" );
		glBindAttribLocation( m_pShader->ID(), ATTRIB_LOCATION_NORMAL, "NORMAL" );

		// init uniform location
		m_ulMatWorld			= glGetUniformLocation( m_pShader->ID(), "g_MatWorld" );
		m_ulMatViewProj			= glGetUniformLocation( m_pShader->ID(), "g_MatViewProj" );

		m_ulTexture				= glGetUniformLocation( m_pShader->ID(), "g_Texture" );
		m_ulDepthMap			= glGetUniformLocation( m_pShader->ID(), "DepthSampler" );
		m_ulDiffuse				= glGetUniformLocation( m_pShader->ID(), "g_Diffuse" );


		m_ulLineWidth			= glGetUniformLocation( m_pShader->ID(), "HalfWidth" );
		m_ulOverhangLength		= glGetUniformLocation( m_pShader->ID(), "OverhangLength" );
		m_ulNormalMatrix		= glGetUniformLocation( m_pShader->ID(), "NormalMatrix" );
		m_ulProjection			= glGetUniformLocation( m_pShader->ID(), "Projection" );
		m_ulSize				= glGetUniformLocation( m_pShader->ID(), "Size" );
		m_ulDiffuseMaterial		= glGetUniformLocation( m_pShader->ID(), "DiffuseMaterial" );
		m_ulSpecularMaterial	= glGetUniformLocation( m_pShader->ID(), "SpecularMaterial" );
		m_ulShininess			= glGetUniformLocation( m_pShader->ID(), "Shininess" );
		m_ulAmbientMaterial		= glGetUniformLocation( m_pShader->ID(), "AmbientMaterial" );
		m_ulLightPosition		= glGetUniformLocation( m_pShader->ID(), "LightPosition" );
		m_ulNormalMap			= glGetUniformLocation( m_pShader->ID(), "NormalMap" );


		// link shader
		m_pShader->Link();

		// その他
		// glBindFragDataLocation( program_id, channel, "FragColor0" );
		// サンプル
	}


	// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
	void SilhouetteShader::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
	{
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_BLEND );

		MeshObject *pMesh = (MeshObject *)pObj;
		GLVertexArrayObject *pVao = pMesh->GetBufferObject<GLVertexArrayObject>();

		m_pShader->Bind();

		//=============================== UniformLocationの設定 ===============================//
		// World Matrix
		glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pMesh->GetTransform()->matWorldTransform.m );
		// View Projection Matrix
		glUniformMatrix4fv( m_ulMatViewProj, 1, GL_FALSE, pfrustum->GetViewProjectionMatrix()->m );

		// Texture
		if( pMesh->m_pTextureBuffer->pData )
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, *pMesh->m_pTextureBuffer->texID() );
			glUniform1i( m_ulTexture, 0 );
		}

		// Material Parameter
		if( pMesh->m_pMaterialBuffer->pData )
		{
			const MaterialParams *mtlparam = (MaterialParams *)pMesh->m_pMaterialBuffer->pData;
			//const MaterialParams *mtlparam = ((MaterialBuffer *)pMesh->m_pMaterialBuffer)->Layer();
			glUniform4fv( m_ulDiffuse, 1, mtlparam->Diffuse.xyzw );
		}
		m_ulDiffuse				= glGetUniformLocation( m_pShader->ID(), "g_Diffuse" );


		// Silhouette parameters
		glUniform1f( m_ulLineWidth, 0.005f );
		glUniform1f( m_ulOverhangLength, 0.015f );

		Mat4f nm;
		MatTranspose( nm, *pfrustum->GetViewMatrix() );
		const float packed[9] =
		{
			nm.m00, nm.m01, nm.m02,
			nm.m10, nm.m11, nm.m12,
			nm.m20, nm.m21, nm.m22
		};

		glUniformMatrix3fv( m_ulNormalMatrix, 1, 0, &packed[0] );
		glUniformMatrix4fv( m_ulProjection, 1, GL_FALSE, pfrustum->GetProjectionMatrix()->m );
		glUniform2f( m_ulSize, /*PEZ_VIEWPORT_WIDTH*/1280, /*PEZ_VIEWPORT_HEIGHT*/720 );
		glUniform3f( m_ulDiffuseMaterial, 0, 0.75, 0.75 );
		glUniform3f( m_ulSpecularMaterial, 0.5f, 0.5f, 0.5f );
		glUniform1f( m_ulShininess, 50 );
		glUniform3f( m_ulAmbientMaterial, 0.04f, 0.04f, 0.04f );
		glUniform3f( m_ulLightPosition, 0.25, 0.25, 1 );
		glUniform1i( m_ulNormalMap, 1 );

		//=============================== オブジェクトの描画 ==================================//
		pVao->Draw();

		m_pShader->Unbind();


		glEnable( GL_DEPTH_TEST );
		glEnable( GL_BLEND );

	}




	void SilhouetteShader::Render( MovableObject *pObj, MatViewTransform *cp, const Texture2D& texDepth )
	{
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_BLEND );

		MeshObject *pMesh = (MeshObject *)pObj;
		GLVertexArrayObject *pVao = pMesh->GetBufferObject<GLVertexArrayObject>();

		m_pShader->Bind();



		//=============================== UniformLocationの設定 ===============================//
		// World Matrix
		glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pMesh->GetTransform()->matWorldTransform.m );
		// View Projection Matrix
		glUniformMatrix4fv( m_ulMatViewProj, 1, GL_FALSE, cp->matViewProjection.m );

		// Texture
		if( pMesh->m_pTextureBuffer->pData )
		{
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, *pMesh->m_pTextureBuffer->texID() );
			glUniform1i( m_ulTexture, 0 );
		}

		// Material Parameter
		if( pMesh->m_pMaterialBuffer->pData )
		{
			const MaterialParams *mtlparam = (MaterialParams *)pMesh->m_pMaterialBuffer->pData;
			//const MaterialParams *mtlparam = ((MaterialBuffer *)pMesh->m_pMaterialBuffer)->Layer();
			glUniform4fv( m_ulDiffuse, 1, mtlparam->Diffuse.xyzw );
		}
		m_ulDiffuse				= glGetUniformLocation( m_pShader->ID(), "g_Diffuse" );


		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, texDepth.texID );
		glUniform1i( m_ulDepthMap, 1 );

		// Silhouette parameters
		glUniform1f( m_ulLineWidth, 0.005f );
		glUniform1f( m_ulOverhangLength, 0.015f );

		Mat4f nm;
		MatTranspose( nm, cp->matView );
		const float packed[9] =
		{
			nm.m00, nm.m01, nm.m02,
			nm.m10, nm.m11, nm.m12,
			nm.m20, nm.m21, nm.m22
		};
		glUniformMatrix3fv( m_ulNormalMatrix, 1, 0, &packed[0] );
		glUniformMatrix4fv( m_ulProjection, 1, GL_FALSE, cp->matProjection.m );
		glUniform2f( m_ulSize, /*PEZ_VIEWPORT_WIDTH*/1280, /*PEZ_VIEWPORT_HEIGHT*/720 );
		glUniform3f( m_ulDiffuseMaterial, 0, 0.75, 0.75 );
		glUniform3f( m_ulSpecularMaterial, 0.5f, 0.5f, 0.5f );
		glUniform1f( m_ulShininess, 50 );
		glUniform3f( m_ulAmbientMaterial, 0.04f, 0.04f, 0.04f );
		glUniform3f( m_ulLightPosition, 0.25, 0.25, 1 );
		glUniform1i( m_ulNormalMap, 1 );

		//=============================== オブジェクトの描画 ==================================//
		pVao->Draw();


		m_pShader->Unbind();

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_BLEND );
	}




}// end of namespace
