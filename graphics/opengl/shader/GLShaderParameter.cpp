#include	"GLShaderParameter.h"


namespace OreOreLib
{

	GLShaderParameter::GLShaderParameter()
	{
		m_Name				= NULL;
		m_CurrentProgramId	= -1;
		m_NumBindedShaders	= 0;
		m_UniformLocations	= NULL;
	}



	GLShaderParameter::GLShaderParameter( const tstring& paramName, int numBindLocations )
	{
		Init( paramName, numBindLocations );
	}



	GLShaderParameter::~GLShaderParameter()
	{
		Release();
	}



	void GLShaderParameter::Init( const tstring& paramName, int numBindLocations )
	{
		Release();

		//m_Name				= paramName;

#ifdef UNICODE
		size_t outputSize = paramName.length() +1;
		m_Name = new char[outputSize];
		size_t wLen = 0;
		errno_t err = 0;
		err = wcstombs_s( &wLen, m_Name, outputSize, paramName.c_str(), paramName.length() );	// Convert from w_char_t to char
#else
		int size = paramName.length()+1;
		m_Name = new char[size];
		strcpy_s( m_Name, size, paramName.c_str() );
#endif // UNICODE

		m_NumBindedShaders	= numBindLocations;

		m_UniformLocations	= new GLint[ m_NumBindedShaders ];
		for( int i=0; i<m_NumBindedShaders; ++i )
			m_UniformLocations[i] = -1;
	}



	void GLShaderParameter::Release()
	{
		SafeDeleteArray( m_Name );
		//m_Name				="";
		m_CurrentProgramId	= -1;
		m_NumBindedShaders	= 0;
		SafeDeleteArray( m_UniformLocations );
	}



	bool GLShaderParameter::BindToShader( int bindSlot, GLuint program_id )
	{
		if( bindSlot >= m_NumBindedShaders )
			return false;

		m_UniformLocations[bindSlot]	= GL_SAFE_CALL( glGetUniformLocation( program_id, m_Name ) );//glGetUniformLocation( program_id, m_Name.c_str() );
	
		return m_UniformLocations[bindSlot]>=0;
	}


}// end of namespace