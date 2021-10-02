#include	"ConstantShader.h"


#include	<oreore/common/TString.h>
#include	<graphics/gl4x/scene/Frustum.h>
#include	<graphics/gl4x/scene/ViewTransformBuffer.h>



namespace  OreOreLib
{
	// Default Constructor
	ConstantShader::ConstantShader()
	{
#ifdef _DEBUG		
		tcout << _T( "ConstantShader::ConstantShader()..." ) << tendl;
#endif
		m_ulMatWorld		= -1;
		m_ulColor			= -1;
		m_ulUseVertexColor	= -1;
	}


	// コンストラクタ
	ConstantShader::ConstantShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG		
		tcout << _T( "ConstantShader::ConstantShader( const char *filepath )..." ) << tendl;
#endif
		m_ulMatWorld		= -1;
		m_ulColor			= -1;
		m_ulUseVertexColor	= -1;

		if( filepath )
			InitShader( filepath, version );
	}


	// デストラクタ
	ConstantShader::~ConstantShader()
	{
#ifdef	_DEBUG
		tcout << _T( "~ConstantShader()..." ) << tendl;
#endif
	}


	bool ConstantShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint program_id;

		//===================== Init Scene Rendering Pass =======================//
		// シェーダオブジェクト生成
		m_Shader.Init( filepath, version );
		program_id		= m_Shader.ID();

		// 入力データの設定
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_NORMAL, "NORMAL" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_COLOR, "COLOR" ) );

		// シェーダ変数の設定
		m_ulMatWorld		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		m_ulColor			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Color" ) );
		m_ulUseVertexColor	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_UseVertexColor" ) );

		// 出力データの設定

		// プログラムのリンク
		m_Shader.Link();

		return true;
	}



	void ConstantShader::Release()
	{
		m_Shader.Release();
	}



	void ConstantShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type	= typeid(*pbufferobj);
		//tcout << "class name: " << id.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05


		if( buffer_type == typeid(ViewTransformBuffer) )	// ビュー行列だった場合
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_Shader.ID() );
		}
		else
		{
		}

	}



	void ConstantShader::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
	{
		// Set Uniforms
		glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m );
		glUniform1i( m_ulUseVertexColor, 1 );

		// Draw Geometry
		pGeom->Draw();
	}


	void ConstantShader::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat, const Vec4f *color )
	{
		// Set Uniforms
		glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m );
		glUniform4fv( m_ulColor, 1, color->xyzw );
		glUniform1i( m_ulUseVertexColor, 0 );
		
		// Draw Geometry
		pGeom->Draw();
	}



}// end of namespace