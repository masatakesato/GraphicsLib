#ifndef	GL_HELPER_FUNCTIONS_H
#define	GL_HELPER_FUNCTIONS_H


#include	<vector>

#include <GL/glew.h>
#pragma comment(lib, "glew32.lib")
#ifdef _DEBUG
#include <GL/freeglut.h>
#endif

#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/common/Utility.h>
#include	<oreore/common/TString.h>

#include	<oreore/io/FileIO.h>



namespace OreOreLib
{

	//##########################################################################################//
	//										Constant Values										//
	//##########################################################################################//


	// GLSLシェーダーバージョン
	const enum GLSL_VERSION
	{
		GLSL_UNKNOWN	= -1,
		GLSL_110,
		GLSL_120,
		GLSL_130,
		GLSL_140,
		GLSL_150,
		GLSL_330,
		GLSL_400,
		GLSL_410,
		GLSL_420,
		GLSL_430,
		GLSL_440,
		GLSL_450,
	};


	// GLSLシェーダーバージョン定義
	const tstring g_GLslVersion[] =
	{
		_T( "#version 110\n" ),
		_T( "#version 120\n" ),
		_T( "#version 130\n" ),
		_T( "#version 140\n" ),
		_T( "#version 150\n" ),
		_T( "#version 330\n" ),
		_T( "#version 400\n" ),
		_T( "#version 410\n" ),
		_T( "#version 420\n" ),
		_T( "#version 430\n" ),
		_T( "#version 440\n" ),
		_T( "#version 450\n" ),
	};




	// テクスチャユニット
	const GLenum g_DefaultTextureUnits[] =
	{
		GL_TEXTURE0,	GL_TEXTURE1,	GL_TEXTURE2,	GL_TEXTURE3, 
		GL_TEXTURE4,	GL_TEXTURE5,	GL_TEXTURE6,	GL_TEXTURE7,
		GL_TEXTURE8,	GL_TEXTURE9,	GL_TEXTURE10,	GL_TEXTURE11,
		GL_TEXTURE12,	GL_TEXTURE13,	GL_TEXTURE14,	GL_TEXTURE15,
	};

	// フレームバッファオブジェクトのアタッチメントポイント
	const GLenum g_DefaultColorAttachments[] = 
	{
		GL_COLOR_ATTACHMENT0,	GL_COLOR_ATTACHMENT1,	GL_COLOR_ATTACHMENT2,	GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,	GL_COLOR_ATTACHMENT5,	GL_COLOR_ATTACHMENT6,	GL_COLOR_ATTACHMENT7,
		GL_COLOR_ATTACHMENT8,	GL_COLOR_ATTACHMENT9,	GL_COLOR_ATTACHMENT10,	GL_COLOR_ATTACHMENT11,
		GL_COLOR_ATTACHMENT12,	GL_COLOR_ATTACHMENT13,	GL_COLOR_ATTACHMENT14,	GL_COLOR_ATTACHMENT15,
	};


	const int g_DefaultTexLevels[] =
	{ 
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
	};


	const int g_DefaultTexLayers[] =
	{ 
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	};



	//##########################################################################################//
	//										GLSL												//
	//##########################################################################################//


#ifdef _DEBUG

#define GL_CHECK_ERROR()	GLCheckError()

#else

#define GL_CHECK_ERROR()

#endif


	static void GLCheckError()
	{
		GLenum errCode;

		if((errCode=glGetError()) != GL_NO_ERROR)
		{
			tcout << _T("GL Error...") << gluErrorString(errCode) << tendl;
			//return;
		}
	}



#ifdef _DEBUG
#define GL_SAFE_CALL(stmt) \
            stmt; \
			{ \
				GLenum errCode = glGetError(); \
				if( errCode != GL_NO_ERROR ) \
				{ \
					tcout << _T( "OpenGL Error " ) << errCode << _T( " at " ) << __FILENAME__ << _T( ": " ) << __LINE__ << tendl; \
					tcout << _T( "    " ) << #stmt << ": " << gluErrorString(errCode) << tendl; \
					/*exit(1);*/ \
				} \
			}
#else
#define GL_SAFE_CALL(stmt) stmt
#endif


	const int BUFFER_SIZE=512;

	static void ValidateShader( unsigned int shader, const char* file = NULL )
	{
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;

		glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

		if (length > 0)
			tcerr << "GLShader " << shader << " (" << (file?file:"") << ") compile error: " << buffer << tendl;
	}



	static void ValidateProgram( unsigned int program )
	{
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;

		glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
		if (length > 0)
			tcerr << "Program " << program << " link error: " << buffer << tendl;

		glValidateProgram(program);
		int status;
		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
		if (status == GL_FALSE)
			tcerr << "Error validating shader " << program << tendl;
	}



	static std::vector<tstring> GetHeaderFile( const tstring &aaa )
	{
		std::vector<tstring>	headers;
		tstring	one_line;

		// 1行ずつ取得
		size_t	idx_beginline = 0;
		size_t	idx_endline = 0;
		size_t	str_length = aaa.length()/* - 1*/;// 文字列長(終端文字'\0'は除く)

		while(idx_beginline < str_length)
		{
			// 改行記号の位置（終端位置）を取得
			idx_endline = aaa.find( _T("\n"), idx_beginline);
			if( idx_endline==tstring::npos )	idx_endline= aaa.find( _T( "\0" ), idx_beginline );

			one_line = aaa.substr(idx_beginline, idx_endline-idx_beginline);

			// ヘッダーファール名を取得
			bool	check = one_line.find( _T("#pragma include") )!= -1;
			bool	comment_line = one_line.find( _T( "//") ) == 0;

			if(check && !comment_line)
			{
				size_t	file_begin = one_line.find( _T( "\""), 0 );
				size_t	file_end = one_line.rfind( _T( "\""), one_line.length( ) - 1 );

				if( (file_begin!=-1) && (file_end!=-1) && (file_begin<file_end) )
				{
					headers.push_back( one_line.substr(file_begin+1, file_end-file_begin-1) );
				}
			}

			idx_beginline = idx_endline + 1;

		}// end of while

		return headers;
	}



	static void IncludeHeader_rec( tstring &SrcCode, const tstring& root=_T("") )
	{

		// SrcCodeから，"#pragma include ***.h"を探す
		std::vector<tstring> headers = GetHeaderFile(SrcCode);

		if(!headers.empty())// インクルードファイルが見つかったら，，，
		{
			for(int i=0; i<headers.size(); i++)
			{
// ヘッダーファイルを読み込んでソースコードを取得
tstring HeaderCode = tstring( textFileRead( (root + headers[i]).c_str()) ) + _T('\n');
				// ヘッダーのソースコード中にincludeがないか調べる
				IncludeHeader_rec(HeaderCode);
				// #pragmaの直後にソースコードを挿入
				size_t	pivot = SrcCode.find(headers[i]);// ヘッダファイル最後尾を取得

				SrcCode = SrcCode.substr(0, pivot+headers[i].length()+1) + _T('\n') + HeaderCode + SrcCode.substr(pivot+headers[i].length()+2, SrcCode.length()-1);
			}// end of i loop
		}
		else
		{
			return;
		}
	}



	static void CompileShaderFromFile( GLenum shaderType, const tstring& shader_source, GLuint& program, GLuint& shader )
	{
//#ifdef UNICODE
//		size_t wLen		= 0;
//		errno_t err		= 0;
//		char	*wStrC	= new char[ shader_source.length( ) ];
//		setlocale( LC_ALL, "japanese" );	// Set Locate
//		err = wcstombs_s( &wLen, wStrC, 20, shader_source.c_str( ), _TRUNCATE );	// Convert from w_char_t to char
//		const GLchar *shader_code = wStrC;		// ソースコード文字列を取得
//#else
//		const GLchar *shader_code = shader_source.c_str();		// ソースコード文字列を取得
//#endif // UNICODE



#ifdef UNICODE

		
		size_t outputSize = shader_source.length() + 1;
		char	*wStrC = new char[outputSize];
		size_t wLen = 0;
		errno_t err = 0;
		err = wcstombs_s( &wLen, wStrC, outputSize, shader_source.c_str(), shader_source.length() );	// Convert from w_char_t to char
		//err = wcstombs_s( &wLen, wStrC, outputSize, shader_source.c_str(), _TRUNCATE );// Convert from w_char_t to char

		const GLchar *shader_code = wStrC;		// ソースコード文字列を取得
#else
		const GLchar *shader_code = shader_source.c_str();		// ソースコード文字列を取得
#endif // UNICODE




		shader					= glCreateShader( shaderType );	// シェーダID生成
		GL_SAFE_CALL( glShaderSource( shader, 1, &shader_code, 0 ) );	// ソースコード文字列を登録
		GL_SAFE_CALL( glCompileShader( shader ) );
		ValidateShader( shader );
		if( program )	GL_SAFE_CALL( glAttachShader( program, shader ) );

#ifdef UNICODE
		SafeDeleteArray( wStrC );
#endif

	}





}// end of namespace


#endif	// GL_HELPER_FUNCTIONS_H //