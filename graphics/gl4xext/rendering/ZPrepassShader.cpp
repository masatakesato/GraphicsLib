#include	"ZPrepassShader.h"

#include	<oreore/common/TString.h>
#include	<graphics/gl4x/scene/ViewTransformBuffer.h>



namespace  OreOreLib
{
	// Default Constructor
	ZPrepassShader::ZPrepassShader()
	{
#ifdef _DEBUG		
		tcout << "ZPrepassShader()..." << tendl;
#endif
		m_ulMatWorld = -1;
	}



	// Constructor
	ZPrepassShader::ZPrepassShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef _DEBUG		
		tcout << "ZPrepassShader( const TCHAR *filepath, GLSL_VERSION version )..." << tendl;
#endif

		m_ulMatWorld = -1;

		if( filepath )
			InitShader( filepath, version );
	}


	// Destructor
	ZPrepassShader::~ZPrepassShader()
	{
#ifdef _DEBUG	
		tcout << "~ZPrepassShader()..." << tendl;
#endif
	}


	bool ZPrepassShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint program_id;

		//===================== Init Scene Rendering Pass =======================//
		// シェーダオブジェクト生成
		m_Shader.Init( filepath, version );
		program_id		= m_Shader.ID();

		// 入力データの設定
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );

		// シェーダ変数の設定
		m_ulMatWorld					= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );

		// 出力データの設定
		//glBindFragDataLocation( program_id,	RsmPosition,	"fPosition");	// ワールド座標
		//glBindFragDataLocation( program_id,	RsmNormal,		"fNormal");		// 法線
		//glBindFragDataLocation( program_id,	RsmFlux,		"fFlux");		// 放射束
		//glBindFragDataLocation( program_id,	RsmDepth,		"fDepth");		// 奥行き

		// プログラムのリンク
		m_Shader.Link();

		return true;
	}



	void ZPrepassShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type = typeid( *pbufferobj );
		//tcout << "class name: " << id.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05


		if ( buffer_type == typeid( ViewTransformBuffer ) )	// ビュー行列だった場合
		{
			( (ViewTransformBuffer *)pbufferobj )->BindToShader( m_Shader.ID( ) );
		}
		else
		{
		}

	}


	void ZPrepassShader::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
	{
		GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m ) );
		pGeom->Draw();
	}


}// end of namespace