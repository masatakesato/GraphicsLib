#include	"GbufferShader.h"


#include	<iostream>
using namespace std;

#include	<graphics/gl4x/scene/ViewTransformBuffer.h>
//#include	<graphics/gl4x/scene/LightAttributeBuffer.h>



namespace  OreOreLib
{
	// Default Constructor
	GbufferShader::GbufferShader()
	{
#ifdef _DEBUG
		tcout << _T( "GbufferShader()..." ) << tendl;
#endif
		m_ulMatWorld	= -1;
		m_ulDiffuse		= -1;
	}



	// Constructor
	GbufferShader::GbufferShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG
		tcout << _T( "GbufferShader( const TCHAR *filepath, GLSL_VERSION version )..." ) << tendl;
#endif
		m_ulMatWorld	= -1;
		m_ulDiffuse		= -1;
		
		if( filepath )
			InitShader( filepath, version );
	}



	// Destructor
	GbufferShader::~GbufferShader()
	{
#ifdef _DEBUG
		tcout << _T("~GbufferShader()...") << tendl;
#endif
	}



	bool GbufferShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint program_id;

		//===================== RSMパスの初期化 =======================//
		// シェーダオブジェクト生成
		m_Shader.Init( filepath, version );
		program_id		= m_Shader.ID();

		// Bind Attribute Locations
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Locations
		m_ulMatWorld	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		m_ulDiffuse		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Diffuse" ) );

		// Bind FragData Locations
		GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_POSITION, "outPosition" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_NORMAL, "outNormal" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_DEPTH, "outDepth" ) );
		GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_COLOR, "outColor" ) );

		// プログラムのリンク
		m_Shader.Link();

		return true;
	}



	void GbufferShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type	= typeid(*pbufferobj);
		//tcout << "class name: " << id.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05
	
		if( buffer_type == typeid(ViewTransformBuffer) )	// カメラだった場合
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Shader.ID() ); 
		}
		//else if( buffer_type == typeid(LightAttributeBuffer) )// 光源だった場合
		//{
		//	((LightAttributeBuffer *)pbufferobj)->BindToShader( m_Shader.ID() ); 
		//}
		else
		{
		}
	
	}



	void GbufferShader::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
	{
		MaterialBuffer *pMat	= (MaterialBuffer *)pGeom->GetMaterial();
		
		GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m ) );	// オブジェクトのワールド変換行列


		// Texture
		//if( pMesh->m_pTextureBuffer->pData )
		//{
		//	glActiveTexture( GL_TEXTURE0 );
		//	glBindTexture( GL_TEXTURE_2D, *pMesh->m_pTextureBuffer->texID() );
		//	glUniform1i( m_ulTexture, 0 );
		//}
		
		// Material Parameter
		if( pMat->pData )
		{
			GL_SAFE_CALL( glUniform3fv( m_ulDiffuse, pMat->numElms, (float *)pMat->Diffuse() ) );
			//glUniform4fv( m_ulSpecular, 1, mtlparam->Specular.xyzw );
			//glUniform1f( m_ulShininess, mtlparam->Shininess );
		}
		
		pGeom->Draw();
	}





}// end of namespace