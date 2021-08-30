#ifndef	QUAD_SHADER_TEX_ARRAY_H
#define	QUAD_SHADER_TEX_ARRAY_H


#include	"IShader.h"

namespace OreOreLib
{


	class QuadShaderTexArray : public IShader
{
public:
	
	QuadShaderTexArray();
	QuadShaderTexArray( const TCHAR *filepath, GLSL_VERSION version );
	~QuadShaderTexArray( );

	void InitShader( const TCHAR *filepath, GLSL_VERSION version );
	void SetMagnitude( float val )	{ m_Magnitude	= val; }
	void Render( Texture2D *tex, int numlayers );


	// Virtual Functions Override
	virtual int NumPasses() const							{ return 1;  }
	virtual GLuint GetPassID( int shader_idx = 0 ) const	{ return m_pShader->ID(); }

	inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
	{
		if( m_pCurrShader == m_pShader )	return;
		m_pCurrShader = m_pShader;
		m_pCurrShader->Bind();
	}

	virtual void BindBufferObject(const IBufferObject* pbufferobj){}
	virtual void UnbindBufferObject(){}
	virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


private:

	GLShader	*m_pShader;
	GLint		m_ulTexture;
	GLint		m_ulLayer;
	GLint		m_ulMagnitude;

	float		m_Magnitude;


};





class QuadShaderTexArrayFactory : IShaderFactory
{
protected:

	IShader* Create( GLSL_VERSION version ){ return new QuadShaderTexArray( _T( "QuadShaderTexArray.glsl" ), version ); }

public:

	virtual ~QuadShaderTexArrayFactory(){}
	IShader* CreateInstance( GLSL_VERSION version );

};








}// end of namespace


#endif	// QUAD_SHADER_TEX_ARRAY_H //