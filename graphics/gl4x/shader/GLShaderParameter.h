#ifndef GL_SHADER_PARAMETER_H

#include	"../common/GLHelperFunctions.h"



namespace OreOreLib
{

	class GLShaderParameter
	{
	public:

		GLShaderParameter();
		GLShaderParameter( const tstring& paramName, int numBindLocations );
		~GLShaderParameter();

		void Init( const tstring& paramName, int numBindLocations );
		void Release();

		bool BindToShader( int bindSlot, GLuint program_id );

		inline void SetCurrentShader( int shader_idx )		{ m_CurrentProgramId = m_UniformLocations[ Clamp( shader_idx, 0, m_NumBindedShaders-1 ) ]; }
		

		inline void SetUniform1f( GLfloat v0 )							{ glUniform1f( m_CurrentProgramId, v0 ); }
		inline void SetUniform1fv( GLsizei count, GLfloat *value )		{ glUniform1fv( m_CurrentProgramId, count, value ); }
		inline void SetUniform1i( GLint v0 )							{ glUniform1i( m_CurrentProgramId, v0 ); }
		inline void SetUniform1iv( GLsizei count, GLint *value )		{ glUniform1iv( m_CurrentProgramId, count, value ); }

		inline void SetUniform2f( GLfloat v0, GLfloat v1 )				{  glUniform2f( m_CurrentProgramId, v0, v1 ); }
		inline void SetUniform2f( const Vec2f& v )						{ glUniform2f( m_CurrentProgramId, v.x, v.y ); }
		inline void SetUniform2fv( GLsizei count, GLfloat *value )		{ glUniform2fv( m_CurrentProgramId, count, value ); }
		inline void SetUniform2i( GLint v0, GLint v1 )					{ glUniform2i( m_CurrentProgramId, v0, v1 ); }
		inline void SetUniform2i( const Vec2i& v )						{ glUniform2i( m_CurrentProgramId, v.x, v.y ); }
		inline void SetUniform2iv( GLsizei count, GLint *value )		{ glUniform2iv( m_CurrentProgramId, count, value ); }

		inline void SetUniform3f( GLfloat v0, GLfloat v1, GLfloat v2 )	{ glUniform3f( m_CurrentProgramId, v0, v1, v2 ); }
		inline void SetUniform3f( const Vec3f& v )						{ glUniform3f( m_CurrentProgramId, v.x, v.y, v.z ); }
		inline void SetUniform3fv( GLsizei count, GLfloat *value )		{ glUniform3fv( m_CurrentProgramId, count, value ); }
		inline void SetUniform3i( GLint v0, GLint v1, GLint v2 )		{ glUniform3i( m_CurrentProgramId, v0, v1, v2 ); }
		inline void SetUniform3i( const Vec3i& v )						{ glUniform3i( m_CurrentProgramId, v.x, v.y, v.z ); }
		inline void SetUniform3iv( GLsizei count, GLint *value )		{ glUniform3iv( m_CurrentProgramId, count, value ); }

		inline void SetUniform4f( GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )	{ glUniform4f( m_CurrentProgramId, v0, v1, v2, v3 ); }
		inline void SetUniform4f( const Vec4f& v )									{ glUniform4f( m_CurrentProgramId, v.x, v.y, v.z, v.w ); }
		inline void SetUniform4fv( GLsizei count, GLfloat *value )					{ glUniform4fv( m_CurrentProgramId, count, value ); }
		inline void SetUniform4i( GLint v0, GLint v1, GLint v2, GLint v3 )			{ glUniform4i( m_CurrentProgramId, v0, v1, v2, v3 ); }
		inline void SetUniform4i( const Vec4i& v )									{ glUniform4i( m_CurrentProgramId, v.x, v.y, v.z, v.w ); }
		inline void SetUniform4iv( GLsizei count, GLint *value )					{ glUniform4iv( m_CurrentProgramId, count, value ); }

		inline void SetUniformMatrix2fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )	{ glUniformMatrix2fv( m_CurrentProgramId, count, transpose, value ); }
		inline void SetUniformMatrix3fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )	{ glUniformMatrix3fv( m_CurrentProgramId, count, transpose, value ); }
		inline void SetUniformMatrix4fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )	{ glUniformMatrix4fv( m_CurrentProgramId, count, transpose, value ); }


	private:

		// Parameter Name
		//tstring	m_Name;
		char	*m_Name;

		// Current Shader Index
		int		m_CurrentProgramId;

		// Bind Locations
		int		m_NumBindedShaders;
		GLint	*m_UniformLocations;

	};


	typedef GLShaderParameter GLUniform;


}// end of namespace


#endif // !GL_SHADER_PARAM_H
