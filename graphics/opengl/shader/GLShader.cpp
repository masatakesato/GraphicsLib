#include "GLShader.h"

#include	<windows.h>

#include	<oreore/common/Utility.h>

#include	"../io/FileIO.h"


namespace OreOreLib
{


#define VS_ENTRY		_T("_VERTEX_SHADER_")
#define VS_ENTRY_DEF	_T("#define _VERTEX_SHADER_\n")

#define TCS_ENTRY		_T("_TESSELLATION_CONTROL_SHADER_")
#define TCS_ENTRY_DEF	_T("#define _TESSELLATION_CONTROL_SHADER_\n")

#define TES_ENTRY		_T("_TESSELLATION_EVALUATION_SHADER_")
#define TES_ENTRY_DEF	_T("#define _TESSELLATION_EVALUATION_SHADER_\n")

#define GS_ENTRY		_T("_GEOMETRY_SHADER_")
#define GS_ENTRY_DEF	_T("#define _GEOMETRY_SHADER_\n")

#define FS_ENTRY		_T("_FRAGMENT_SHADER_")
#define FS_ENTRY_DEF	_T("#define _FRAGMENT_SHADER_\n")

	


	GLShader::GLShader()
	{
//		tcout << "GLShader::GLShader()..." << tendl;

		m_Shader_id		= NULL;
		m_Shader_vp		= NULL;
		m_Shader_tcs	= NULL;
		m_Shader_tes	= NULL;
		m_Shader_gp		= NULL;
		m_Shader_fp		= NULL;
	}



	GLShader::GLShader( const GLShader &obj )
	{
//		tcout << "GLShader::GLShader(const GLShader &obj)..." << tendl;
	}



	GLShader::~GLShader()
	{
		Release();
	}



	void GLShader::Init( const TCHAR *filename, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines )
	{
//		tcout << "GLShader::Init( " << filename << " )..."<< tendl;
		
		tstring	shader_defines, shader_source;

		//=============== ファイルからソースコードを読み込む ===============//
		// シェーダファイル読み込み
		if( pDefines )	shader_source = ExtractMacro( pDefines ) + textFileRead(filename);
		else			shader_source = textFileRead(filename);

		IncludeHeader_rec(shader_source);

		if(shader_source.empty())
		{
			tcerr << "Either vertex shader or geometry shader or fragment shader file not found." << tendl;
			return;
		}

		//================= プログラムオブジェクトを作成する ===============//
		m_Shader_id = glCreateProgram();
		
		//=================== シェーダーをコンパイルする ===================//
		if( shader_source.find( VS_ENTRY ) != tstring::npos )
			CompileShaderFromFile( GL_VERTEX_SHADER, g_GLslVersion[version] + VS_ENTRY_DEF + shader_source, m_Shader_id, m_Shader_vp );

		if( shader_source.find( TCS_ENTRY ) != tstring::npos )
			CompileShaderFromFile( GL_TESS_CONTROL_SHADER, g_GLslVersion[version] + TCS_ENTRY_DEF + shader_source, m_Shader_id, m_Shader_tcs );

		if( shader_source.find( TES_ENTRY ) != tstring::npos )
			CompileShaderFromFile( GL_TESS_EVALUATION_SHADER, g_GLslVersion[version] + TES_ENTRY_DEF + shader_source, m_Shader_id, m_Shader_tes );

		if( shader_source.find( GS_ENTRY ) != tstring::npos )
			CompileShaderFromFile( GL_GEOMETRY_SHADER, g_GLslVersion[version] + GS_ENTRY_DEF + shader_source, m_Shader_id, m_Shader_gp );

		if( shader_source.find( FS_ENTRY ) != tstring::npos )
			CompileShaderFromFile( GL_FRAGMENT_SHADER, g_GLslVersion[version] + FS_ENTRY_DEF + shader_source, m_Shader_id, m_Shader_fp );

		//=============== プログラムオブジェクトをリンクする ===============//
		GL_SAFE_CALL( glLinkProgram( m_Shader_id ) );

#ifdef _DEBUG		
		GLenum errCode;
		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << "gl error GLShader::Init(const char *filename)..." << gluErrorString(errCode) << tendl;
			//return;
		}
#endif


		ValidateProgram(m_Shader_id);
	}




	void GLShader::Init( const TCHAR *vsFile, const TCHAR *fsFile, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines )
	{
		//=============== ファイルからソースコードを読み込む ===============//
		tstring	vsText = textFileRead(vsFile);// バーテックスシェーダ読み込み
		IncludeHeader_rec(vsText);

		tstring	fsText = textFileRead(fsFile);
		IncludeHeader_rec(fsText);


		if(vsText.empty())
		{
			tcerr << "Vertex shader file not found." << tendl;
			return;
		}

		if(fsText.empty())
		{
			tcerr << "Fragment shader file not found." << tendl;
			return;
		}

		//================= プログラムオブジェクトを作成する ===============//
		m_Shader_id = glCreateProgram();

		//=================== シェーダーをコンパイルする ===================//
		CompileShaderFromFile( GL_VERTEX_SHADER, g_GLslVersion[version] + vsText, m_Shader_id, m_Shader_vp );
		CompileShaderFromFile( GL_FRAGMENT_SHADER, g_GLslVersion[version] + fsText, m_Shader_id, m_Shader_fp );

		//=============== プログラムオブジェクトをリンクする ===============//
		GL_SAFE_CALL( glLinkProgram( m_Shader_id ) );
		ValidateProgram(m_Shader_id);

	}




	void GLShader::Init( const TCHAR *vsFile, const TCHAR *gsFile, const TCHAR *fsFile, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines )
	{
		//=============== ファイルからソースコードを読み込む ===============//
		tstring	vsText = textFileRead(vsFile);// バーテックスシェーダ読み込み
		IncludeHeader_rec(vsText);

		tstring	gsText = textFileRead(gsFile);
		IncludeHeader_rec(gsText);

		tstring	fsText = textFileRead(fsFile);
		IncludeHeader_rec(fsText);


		if(vsText.empty())
		{
			tcerr << "Vertex shader file not found." << tendl;
			return;
		}

		if(gsText.empty())
		{
			tcerr << "Geometry shader file not found." << tendl;
			return;
		}

		if(fsText.empty())
		{
			tcerr << "Fragment shader file not found." << tendl;
			return;
		}

		//================= プログラムオブジェクトを作成する ===============//
		m_Shader_id = glCreateProgram();

		//=================== シェーダーをコンパイルする ===================//
		CompileShaderFromFile( GL_VERTEX_SHADER, g_GLslVersion[version] + vsText, m_Shader_id, m_Shader_vp );
		CompileShaderFromFile( GL_GEOMETRY_SHADER, g_GLslVersion[version] + gsText, m_Shader_id, m_Shader_gp );
		CompileShaderFromFile( GL_FRAGMENT_SHADER, g_GLslVersion[version] + fsText, m_Shader_id, m_Shader_fp );

		//=============== プログラムオブジェクトをリンクする ===============//
		GL_SAFE_CALL( glLinkProgram( m_Shader_id ) );
		ValidateProgram(m_Shader_id);

	}



	void GLShader::Release()
	{
		if( m_Shader_id )
		{
			if( m_Shader_vp )
			{
				GL_SAFE_CALL( glDetachShader( m_Shader_id, m_Shader_vp ) );
				GL_SAFE_CALL( glDeleteShader( m_Shader_vp ) );
				m_Shader_vp = NULL;
			}

			if( m_Shader_tcs )
			{
				GL_SAFE_CALL( glDetachShader( m_Shader_id, m_Shader_tcs ) );
				GL_SAFE_CALL( glDeleteShader( m_Shader_tcs ) );
				m_Shader_tcs = NULL;
			}

			if( m_Shader_tes )
			{
				GL_SAFE_CALL( glDetachShader( m_Shader_id, m_Shader_tes ) );
				GL_SAFE_CALL( glDeleteShader( m_Shader_tes ) );
				m_Shader_tes = NULL;
			}

			if(m_Shader_gp)
			{
				GL_SAFE_CALL( glDetachShader( m_Shader_id, m_Shader_gp ) );
				GL_SAFE_CALL( glDeleteShader( m_Shader_gp ) );
				m_Shader_gp = NULL;
			}

			if(m_Shader_fp)
			{
				GL_SAFE_CALL( glDetachShader( m_Shader_id, m_Shader_fp ) );
				GL_SAFE_CALL( glDeleteShader( m_Shader_fp ) );
				m_Shader_gp = NULL;
			}

			GL_SAFE_CALL( glDeleteProgram( m_Shader_id ) );
			m_Shader_id	= NULL;
		}

	}



}// end of namespace