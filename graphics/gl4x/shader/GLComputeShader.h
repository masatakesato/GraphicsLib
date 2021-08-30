#ifndef GL_COMPUTE_SHADER_H
#define GL_COMPUTE_SHADER_H

#include	<oreore/common/TString.h>

#include	"../common/GLHelperFunctions.h"



namespace OreOreLib
{

	class GLComputeShader
	{
	public:

		GLComputeShader();
		GLComputeShader( const GLComputeShader &obj );
		~GLComputeShader();

		void Init( const TCHAR *filename, GLSL_VERSION version, const GL_SHADER_MACRO *pDefines=NULL );
		void Release();

		inline void Link() const		{ glLinkProgram(m_Shader_id); };
		inline void Bind() const		{ glUseProgram(m_Shader_id); };
		inline void Unbind() const		{ glUseProgram(0); };
		inline void Dispatch() const	{ glDispatchCompute( m_WorkGroupSizeX, m_WorkGroupSizeY, m_WorkGroupSizeZ ); }
		inline void Dispatch( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z ) const	{ glDispatchCompute( num_groups_x, num_groups_y, num_groups_z ); }

		void SetWorkGroupSize( GLuint size_x, GLuint size_y, GLuint size_z );

		inline GLuint ID() const				{ return m_Shader_id; }
		inline GLuint GetWorkGroupSizeX() const	{ return m_WorkGroupSizeX; }
		inline GLuint GetWorkGroupSizeY() const	{ return m_WorkGroupSizeY; }
		inline GLuint GetWorkGroupSizeZ() const	{ return m_WorkGroupSizeZ; }

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
		GLuint	m_Shader_cs;	// Compute Shader

		GLuint	m_WorkGroupSizeX, m_WorkGroupSizeY, m_WorkGroupSizeZ;
	};


}// end of namespace


#endif	// GL_COMPUTE_SHADER_H //
