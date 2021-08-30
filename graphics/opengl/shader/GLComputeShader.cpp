#include "GLComputeShader.h"

//#include	<windows.h>



#include	"./common/Utility.h"
#include	"FileIO.h"


namespace OreOreLib
{


	GLComputeShader::GLComputeShader()
	{
#ifdef _DEBUG
		tcout << "GLComputeShader()..." << tendl;
#endif // _DEBUG

		m_Shader_id		= NULL;
		m_Shader_cs		= NULL;

		m_WorkGroupSizeX	= 0;
		m_WorkGroupSizeY	= 0;
		m_WorkGroupSizeZ	= 0;
	}



	GLComputeShader::GLComputeShader( const GLComputeShader &obj )
	{
#ifdef _DEBUG
		tcout << "GLComputeShader( const GLComputeShader &obj )..." << tendl;
#endif // _DEBUG

		m_Shader_id		= NULL;
		m_Shader_cs		= NULL;

		m_WorkGroupSizeX	= 0;
		m_WorkGroupSizeY	= 0;
		m_WorkGroupSizeZ	= 0;
	}



	GLComputeShader::~GLComputeShader()
	{
#ifdef _DEBUG
		tcout << "~GLComputeShader()..." << tendl;
#endif // _DEBUG

		Release();
	}



	void GLComputeShader::Init( const TCHAR *filename, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines )
	{
		tcout << "GLComputeShader::Init( " << filename << " )..."<< tendl;

		tstring	shader_source;
		tstring	str;

		//=============== ファイルからソースコードを読み込む ===============//
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
		CompileShaderFromFile( GL_COMPUTE_SHADER, g_GLslVersion[version] + shader_source, m_Shader_id, m_Shader_cs );


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


		ValidateProgram( m_Shader_id );
	}



	void GLComputeShader::Release()
	{
		if( m_Shader_id )
		{
			if( m_Shader_cs )
			{
				GL_SAFE_CALL( glDetachShader( m_Shader_id, m_Shader_cs ) );
				GL_SAFE_CALL( glDeleteShader( m_Shader_cs ) );
				m_Shader_cs = NULL;
			}

			GL_SAFE_CALL( glDeleteProgram( m_Shader_id ) );
			m_Shader_id = NULL;
		}

		m_WorkGroupSizeX	= 0;
		m_WorkGroupSizeY	= 0;
		m_WorkGroupSizeZ	= 0;
	}


	void GLComputeShader::SetWorkGroupSize( GLuint size_x, GLuint size_y, GLuint size_z )
	{
		m_WorkGroupSizeX	= size_x;
		m_WorkGroupSizeY	= size_y;
		m_WorkGroupSizeZ	= size_z;
	}



}// end of namespace