#ifndef GL_SHADER_H
#define GL_SHADER_H

#include	<oreore/common/TString.h>

#include	"../common/GLHelperFunctions.h"



namespace OreOreLib
{

	class GLShader
	{
	public:

		GLShader();
		GLShader( const GLShader &obj );
		~GLShader();

		void Init( const TCHAR *filename, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines=NULL );
		void Init( const TCHAR *vsFile, const TCHAR *fsFile, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines=NULL );
		void Init( const TCHAR *vsFile, const TCHAR *gsFile, const TCHAR *fsFile, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines=NULL );
		//void Init( const char *vsFile, const char *tcsFile, const char *tesFile, const char *fsFile, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines=NULL );	// TODO: 必要なら追加実装する
		//void Init( const char *vsFile, const char *tcsFile, const char *tesFile, const char *gsFile, const char *fsFile, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines=NULL );// TODO: 必要なら追加実装する

		void Release();

		inline void Link() const	{ glLinkProgram(m_Shader_id); };
		inline void Bind() const	{ glUseProgram(m_Shader_id); };
		inline void Unbind() const	{ glUseProgram(0); };

		inline GLuint ID() const	{ return m_Shader_id; };


		inline void setUniform(GLint loc, GLfloat v0)										{ glUniform1f(loc, v0);				};
		inline void setUniform(GLint loc, GLfloat v0, GLfloat v1)							{ glUniform2f(loc, v0, v1);			};
		inline void setUniform(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2)				{ glUniform3f(loc, v0, v1, v2);		};
		inline void setUniform(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)	{ glUniform4f(loc, v0, v1, v2, v3); };
		inline void setUniform(GLint loc, GLint v0)											{ glUniform1i(loc, v0);				};
		inline void setUniform(GLint loc, GLint v0, GLint v1)								{ glUniform2i(loc, v0, v1);			};
		inline void setUniform(GLint loc, GLint v0, GLint v1, GLint v2)						{ glUniform3i(loc, v0, v1, v2);		};
		inline void setUniform(GLint loc, GLint v0, GLint v1, GLint v2, GLint v3)			{ glUniform4i(loc, v0, v1, v2, v3);	};


	private:

		GLuint	m_Shader_id;	// Program Object
		GLuint	m_Shader_vp;	// Vertex Shader
		GLuint	m_Shader_tcs;	// Tessellation Control Shader
		GLuint	m_Shader_tes;	// Tessellation Evaluation Shader
		GLuint	m_Shader_gp;	// Geometry Shader
		GLuint	m_Shader_fp;	// Fragment Shader

	};



}// end of namespace


#endif	// GL_SHADER_H //
