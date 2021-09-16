#include	"SilhouetteShader.h"


#include	"../oreore/MeshObject.h"



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
SilhouetteShader::SilhouetteShader( const char *filepath )
{
	const type_info& id = typeid(*this);
	cout << "AbstractClass-typeid: " << id.name() << endl;

	InitShader( filepath );
}

// destructor
SilhouetteShader::~SilhouetteShader()
{
	SafeDelete( m_pShader );


}

// init shader
void SilhouetteShader::InitShader( const char *filepath )
{
	// create shader
	m_pShader	= new GLShader();
	m_pShader->Init( filepath );
	
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

	
	m_ulLineWidth			= glGetUniformLocation(m_pShader->ID(), "HalfWidth");
	m_ulOverhangLength		= glGetUniformLocation(m_pShader->ID(), "OverhangLength");
	m_ulNormalMatrix		= glGetUniformLocation(m_pShader->ID(), "NormalMatrix");
	m_ulProjection			= glGetUniformLocation(m_pShader->ID(), "Projection");
	m_ulSize				= glGetUniformLocation(m_pShader->ID(), "Size");
	m_ulDiffuseMaterial		= glGetUniformLocation(m_pShader->ID(), "DiffuseMaterial");
	m_ulSpecularMaterial	= glGetUniformLocation(m_pShader->ID(), "SpecularMaterial");
	m_ulShininess			= glGetUniformLocation(m_pShader->ID(), "Shininess");
	m_ulAmbientMaterial		= glGetUniformLocation(m_pShader->ID(), "AmbientMaterial");
	m_ulLightPosition		= glGetUniformLocation(m_pShader->ID(), "LightPosition");
	m_ulNormalMap			= glGetUniformLocation(m_pShader->ID(), "NormalMap");


	// link shader
	m_pShader->Link();

	// その他
	// glBindFragDataLocation( program_id, channel, "FragColor0" );
	// サンプル
}


// メッシュデータをレンダリングする(シェーダーはバインド済みと想定)
void SilhouetteShader::Render( MovableObject *pObj, Frustum *pfrustum )
{
	glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

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
	glUniform1f(m_ulLineWidth, 0.005f);
	glUniform1f(m_ulOverhangLength, 0.015f);

	Mat4f nm;
	MatTranspose( nm, *pfrustum->GetViewMatrix() );
	const float packed[9] =
	{
		nm.m00, nm.m01, nm.m02,
		nm.m10, nm.m11, nm.m12,
		nm.m20, nm.m21, nm.m22
	};

	glUniformMatrix3fv( m_ulNormalMatrix, 1, 0, &packed[0] );
	glUniformMatrix4fv(m_ulProjection, 1, GL_FALSE, pfrustum->GetProjectionMatrix()->m );
	glUniform2f( m_ulSize, /*PEZ_VIEWPORT_WIDTH*/1280, /*PEZ_VIEWPORT_HEIGHT*/720 );
	glUniform3f(m_ulDiffuseMaterial, 0, 0.75, 0.75);
	glUniform3f(m_ulSpecularMaterial, 0.5f, 0.5f, 0.5f);
	glUniform1f(m_ulShininess, 50);
	glUniform3f(m_ulAmbientMaterial, 0.04f, 0.04f, 0.04f);
	glUniform3f(m_ulLightPosition, 0.25, 0.25, 1);
	glUniform1i(m_ulNormalMap, 1);

	//=============================== オブジェクトの描画 ==================================//
	pVao->Draw();
		
	m_pShader->Unbind();


	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

}




void SilhouetteShader::Render( MovableObject *pObj, MatViewTransform *cp, const Texture2D& texDepth )
{
	glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

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
	glUniform1f(m_ulLineWidth, 0.005f);
	glUniform1f(m_ulOverhangLength, 0.015f);

	Mat4f nm;
	MatTranspose( nm, cp->matView );
	const float packed[9] =
	{
		nm.m00, nm.m01, nm.m02,
		nm.m10, nm.m11, nm.m12,
		nm.m20, nm.m21, nm.m22
	};
	glUniformMatrix3fv( m_ulNormalMatrix, 1, 0, &packed[0] );
	glUniformMatrix4fv(m_ulProjection, 1, GL_FALSE, cp->matProjection.m );
	glUniform2f( m_ulSize, /*PEZ_VIEWPORT_WIDTH*/1280, /*PEZ_VIEWPORT_HEIGHT*/720 );
	glUniform3f(m_ulDiffuseMaterial, 0, 0.75, 0.75);
	glUniform3f(m_ulSpecularMaterial, 0.5f, 0.5f, 0.5f);
	glUniform1f(m_ulShininess, 50);
	glUniform3f(m_ulAmbientMaterial, 0.04f, 0.04f, 0.04f);
	glUniform3f(m_ulLightPosition, 0.25, 0.25, 1);
	glUniform1i(m_ulNormalMap, 1);
	
	//=============================== オブジェクトの描画 ==================================//
	pVao->Draw();
	
	
	m_pShader->Unbind();

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}




}// end of namespace



/*
  m_ulLineWidth = glGetUniformLocation(program, "HalfWidth");
    if (lineWidth > -1)
        glUniform1f(lineWidth, 0.005f);

    m_ulOverhangLength = glGetUniformLocation(program, "OverhangLength");
    if (overhangLength > -1)
        glUniform1f(overhangLength, 0.15f);

    m_ulNormalMatrix = glGetUniformLocation(program, "NormalMatrix");
    if (normalMatrix > -1) {
        Matrix3 nm = M3Transpose(M4GetUpper3x3(ModelviewMatrix));
        float packed[9] = {
            nm.col0.x, nm.col1.x, nm.col2.x,
            nm.col0.y, nm.col1.y, nm.col2.y,
            nm.col0.z, nm.col1.z, nm.col2.z };
        glUniformMatrix3fv(normalMatrix, 1, 0, &packed[0]);
    }

    m_ulProjection = glGetUniformLocation(program, "Projection");
    if (projection > -1)
        glUniformMatrix4fv(projection, 1, GL_FALSE, &ProjectionMatrix.col0.x);

    m_ulSize = glGetUniformLocation(program, "Size");
    if (size > -1)
        glUniform2f(size, PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT);

    m_ulDiffuseMaterial = glGetUniformLocation(program, "DiffuseMaterial");
    if (diffuseMaterial > -1)
        glUniform3f(diffuseMaterial, 0, 0.75, 0.75);

    m_ulSpecularMaterial = glGetUniformLocation(program, "SpecularMaterial");
    if (specularMaterial > -1)
        glUniform3f(specularMaterial, 0.5f, 0.5f, 0.5f);

    m_ulShininess = glGetUniformLocation(program, "Shininess");
    if (shininess > -1)
        glUniform1f(shininess, 50);

    m_ulAmbientMaterial = glGetUniformLocation(program, "AmbientMaterial");
    if (ambientMaterial > -1)
        glUniform3f(ambientMaterial, 0.04f, 0.04f, 0.04f);

    m_ulLightPosition = glGetUniformLocation(program, "LightPosition");
    if (lightPosition > -1)
        glUniform3f(lightPosition, 0.25, 0.25, 1);

    m_ulNormalMap = glGetUniformLocation(program, "NormalMap");
    if (normalMap > -1)
        glUniform1i(normalMap, 1);

*/