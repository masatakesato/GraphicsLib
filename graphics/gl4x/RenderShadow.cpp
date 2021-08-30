#include	"RenderShadow.h"


#include	"./common/TString.h"
#include	"GLHelperFunctions.h"
#include	"ShadowBuffer.h"



namespace  OreOreLib
{

	const tstring shaderFileName[ 2 ]	=
	{
		_T( "RenderShadowPerspective.glsl" ),
		_T( "RenderShadowOrthographic.glsl" )
	};



	// デフォルトコンストラクタ
	RenderShadow::RenderShadow()
	{
		//m_ulMatWorld	= -1;
		//m_ulMatLight	= -1;
		//m_ulShadowIndex	= -1;

		m_ProjectionType	= PROJECION_TYPE_PERSPECTIVE;
	}



	// コンストラクタ
	RenderShadow::RenderShadow( const TCHAR *filepath, GLSL_VERSION version )
	{
		m_ProjectionType	= PROJECION_TYPE_PERSPECTIVE;
		InitShader( filepath, version );
	}


	// デストラクタ
	RenderShadow::~RenderShadow()
	{
#ifdef _DEBUG
		tcout << "~RenderShadow()..." << tendl;
#endif
	}


	bool RenderShadow::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint program_id;

		for( int i=0; i<2; ++i )
		{
			// シェーダオブジェクト生成
			m_Pass[i].Init( shaderFileName[i].c_str(), version );
			program_id		= m_Pass[i].ID();

			// 入力データの設定
			GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );

			// シェーダ変数の設定
			m_ulParam[i].ulMatWorld		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
			m_ulParam[i].ulMatLight		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatLight" ) );
			m_ulParam[i].ulShadowIndex	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_ShadowIndex" ) );

			// 出力データの設定
			GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_DEPTH, "outDepth" ) );	// 奥行き
			GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_POSITION, "outPosition" ) );// ワールド座標
			GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_NORMAL, "outNormal" ) );	// 法線
			GL_SAFE_CALL( glBindFragDataLocation( program_id, FRAG_LOCATION::FRAG_FLUX, "outFlux" ) );	// 放射束

			// プログラムのリンク
			m_Pass[i].Link();
		}

		return true;
	}



	void RenderShadow::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info &buffer_type	= typeid(*pbufferobj);
		//tcout << "class name: " << id.name() << tendl;// .name()使うと、コンパイラが間違えてメモリリーク報告してくる. 2013.08.05

		if( buffer_type == typeid(ShadowBuffer) )// 光源だった場合
		{
			for( int i=0; i<2; ++i )
				((ShadowBuffer *)pbufferobj)->BindToShader( m_Pass[i].ID() );

		}

	}



	void RenderShadow::Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat )
	{
		//============================= Uniformの設定 ================================//
		glUniformMatrix4fv( m_ulParam[m_ProjectionType].ulMatWorld, 1, GL_FALSE, pGeom->GetTransform()->matWorld.m );// オブジェクトのワールド変換行列
		//	glUniformMatrix4fv( m_ulMatLight, 1, GL_FALSE, cam_mat->matViewProjection.m );

		//========================== オブジェクトの描画 ==============================//
		pGeom->Draw();
	}





}