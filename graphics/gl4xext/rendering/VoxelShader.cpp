#include	"VoxelShader.h"

#include	<graphics/gl4x/scene/ViewTransformBuffer.h>
#include	<graphics/gl4x/other/GLPrimitives.h>



namespace OreOreLib
{
	// Default Constructor
	VoxelShader::VoxelShader()
	{
#ifdef	_DEBUG
		tcout << _T("VoxelShader()...") << tendl;
#endif
		m_pShader			= NULL;
		m_ulTexture3D		= -1;
		//m_ulMatWorld		= -1;
		m_ulMatTransform	= -1;
		m_ulLod				= -1;
		m_ulScale			= -1;
		//m_ulCellSize		= -1;	
	}



	// Constructor
	VoxelShader::VoxelShader( const TCHAR *filepath, GLSL_VERSION version )
	{
#ifdef	_DEBUG
		tcout << _T("VoxelShader( const TCHAR *filepath, GLSL_VERSION version )...") << tendl;
#endif
		m_pShader			= NULL;
		m_ulTexture3D		= -1;
		//m_ulMatWorld		= -1;
		m_ulMatTransform	= -1;
		m_ulLod				= -1;
		m_ulScale			= -1;
		//m_ulCellSize		= -1;

		if( filepath )
			InitShader( filepath, version );
	}



	// Destructor
	VoxelShader::~VoxelShader()
	{
#ifdef	_DEBUG
		tcout << _T("~VoxelShader()...") << tendl;
#endif
		Release();
	}



	void VoxelShader::Release()
	{
		SafeDelete( m_pShader );
		m_ulTexture3D		= -1;
		//m_ulMatWorld		= -1;
		m_ulMatTransform	= -1;
		m_ulLod				= -1;
		m_ulScale			= -1;
		//m_ulCellSize		= -1;
	}



	// init shader
	void VoxelShader::InitShader( const TCHAR *filepath, GLSL_VERSION version )
	{
		GLuint	program_id;

		// Create shader
		m_pShader	= new GLShader();
		m_pShader->Init( filepath, version );
		program_id	= m_pShader->ID();

		// Init Attribute Location
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_POSITION, "POSITION" ) );
		GL_SAFE_CALL( glBindAttribLocation( program_id, ATTRIB_LOCATION_TEXCOORD, "TEXCOORD" ) );

		// Init Uniform Location	
		m_ulTexture3D		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Texture3D" ) );
		//m_ulMatWorld		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatWorld" ) );
		m_ulMatTransform	= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_MatTransform" ) );
		m_ulLod				= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Lod" ) );
		m_ulScale			= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_Scale" ) );
		//m_ulCellSize		= GL_SAFE_CALL( glGetUniformLocation( program_id, "g_CellSize" ) );

		// Link Shader
		m_pShader->Link();

		// その他
		// glBindFragDataLocation( program_id, channel, "FragColor0" );
		// サンプル
	}



	void VoxelShader::BindBufferObject( const IBufferObject* pbufferobj )
	{
		const type_info& id = typeid(*pbufferobj);

		if( id == typeid(ViewTransformBuffer) )
		{
			((ViewTransformBuffer *)pbufferobj)->BindToShader( m_pShader->ID() );
		}
		
	}



	void VoxelShader::Render( Texture3D *tex3d, int lod, const Mat4f* matTransform )
	{
		if( !m_refUnitCube )
			return;

		const float w	=  float(tex3d->Width()) / powf(2.0f, lod);
		const float h	=  float(tex3d->Height()) / powf(2.0f, lod);
		const float d	=  float(tex3d->Depth()) / powf(2.0f, lod);
		
		//============= ボクセル毎に立方体を描画する ==================//
		m_pShader->Bind();
		{
			// UniformLocationの設定
			glActiveTexture( GL_TEXTURE0 );
			GL_SAFE_CALL( glBindTexture( GL_TEXTURE_3D, tex3d->texID ) );
			GL_SAFE_CALL( glUniform1i( m_ulTexture3D, 0 ) );
			
			GL_SAFE_CALL( glUniformMatrix4fv( m_ulMatTransform, 1, GL_FALSE, matTransform->m ) );
			GL_SAFE_CALL( glUniform1f( m_ulScale, 0.5 ) );

			GL_SAFE_CALL( glUniform1i( m_ulLod, lod ) );

			// 立方体の描画
			m_refUnitCube->DrawInstanced( w*h*d );
		}

		m_pShader->Unbind();
	}



}// end of namespace