#ifndef	SIMPLE_MESH_RENDERER_H
#define	SIMPLE_MESH_RENDERER_H



#include	"IShader.h"


namespace OreOreLib
{

	struct ulMaterialLayer
	{
		GLint	numMats;
		GLint	Ambient;
		GLint	Diffuse;
		GLint	Specular;

		GLint	Shiness;
		GLint	RefractiveIndex;
		GLint	Opacity;
	};




	class SimpleMeshRenderer : public IShader
	{	
	public:

		SimpleMeshRenderer();
		SimpleMeshRenderer( const TCHAR *filepath, GLSL_VERSION version );
		~SimpleMeshRenderer();

		bool InitShader( const TCHAR *filepath, GLSL_VERSION version );

		inline void Bind()	{ m_Shader.Bind();   };
		inline void Unbind(){ m_Shader.Unbind(); };

		
		
		// Override Virtual Functions
		inline virtual int NumPasses() const						{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const	{ return m_Shader.ID(); }

		inline virtual void BindShader( int shader_idx=0 )	// シェーダーをバインドする
		{
			if( m_pCurrShader == &m_Shader )	return;
			m_pCurrShader = &m_Shader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}

		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat );


		//inline virtual void BindVertexBuffer( VertexBuffer *vb )
		//{
		//	//if( m_pCurrVB == vb )
		//	//	return;
		//	m_pCurrVB = vb;

		//	// 頂点座標
		//	glVertexAttribPointer( ATTRIB_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, (GLsizei)m_pCurrVB->elmSize, m_pCurrVB->Position() ); 
		//	glEnableVertexAttribArray( ATTRIB_LOCATION_POSITION );

		//	// 法線ベクトル
		//	glVertexAttribPointer( ATTRIB_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, (GLsizei)m_pCurrVB->elmSize, m_pCurrVB->Normal() ); 
		//	glEnableVertexAttribArray( ATTRIB_LOCATION_NORMAL );

		//	// テクスチャ座標
		//	glVertexAttribPointer( ATTRIB_LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, (GLsizei)m_pCurrVB->elmSize, m_pCurrVB->TexCoord() ); 
		//	glEnableVertexAttribArray( ATTRIB_LOCATION_TEXCOORD );

		//	// 色
		//	glVertexAttribPointer( ATTRIB_LOCATION_COLOR, 3, GL_FLOAT, GL_FALSE, (GLsizei)m_pCurrVB->elmSize, m_pCurrVB->Color() ); 
		//	glEnableVertexAttribArray( ATTRIB_LOCATION_COLOR );

		//}


		//void BindLightSource( /*const Frustum& lightFrustum*/Mat4f &matLight );
		//void BindShadowMap( Texture2D shadowmaps[], Mat4f matlights[], float splitdepths[], int numcascades );
		//void SetLigtMatrix( Mat4f& mat )	{  glUniformMatrix4fv( m_ulMatLight, 1, GL_FALSE, mat.m );  }


	private:

		//=================== シェーダ ==================//
		GLShader		m_Shader;

		//================= シェーダ変数 ================//
		// uniform location
		GLint			m_ulMatWorld;		// wordl matrix
		ulMaterialLayer	m_ulMaterials;
		GLint			m_ulMaterialID;		// material id
		GLint			m_ulShadowMapArray;

		GLint			m_ulMatLight;


	};





	class SimpleMeshRendererFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new SimpleMeshRenderer( _T( "SimpleMeshRenderer.glsl" ), version ); }

	public:

		virtual ~SimpleMeshRendererFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};


}// end of namespace


#endif	// SIMPLE_MESH_RENDERER_H //