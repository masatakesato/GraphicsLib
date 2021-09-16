#ifndef	VOXEL_SHADER_H
#define	VOXEL_SHADER_H


#include	"IShader.h"
#include	"GLShader.h"


namespace OreOreLib
{


	// ボクセルデータ可視化シェーダー
	class VoxelShader : public IShader
	{	
	public:
		
		VoxelShader();
		VoxelShader( const TCHAR* filepath, GLSL_VERSION version );
		~VoxelShader();

		void InitShader( const TCHAR* filepath, GLSL_VERSION version );
		void Release();
		void Render( Texture3D *tex3d, int lod, const Mat4f* matTransform );


		// Override Virtual Functions
		inline virtual int NumPasses() const	{ return 1; }
		inline virtual GLuint GetPassID( int shader_idx=0 ) const{ return m_pShader->ID(); }

		inline virtual void BindShader( int passid=0 )
		{
			if( m_pCurrShader == m_pShader )	return;

			m_pCurrShader = m_pShader;
			m_pCurrShader->Bind();
		}

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject(){}


		virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ){}


	private:

		// shader
		GLShader	*m_pShader;	// デフォルトのシェーダー

		// uniform location
		GLint		m_ulTexture3D;
		//GLint		m_ulMatWorld;
		GLint		m_ulMatTransform;
		GLint		m_ulLod;
		GLint		m_ulScale;
		//GLint		m_ulCellSize;

	};



	// VoxelShaderオブジェクトを生成するファクトリクラス
	class VoxelShaderFactory : IShaderFactory
	{
	protected:

		IShader* Create( GLSL_VERSION version ){ return new VoxelShader( _T( "VoxelShader.glsl" ), version ); }

	public:

		virtual ~VoxelShaderFactory(){}
		IShader* CreateInstance( GLSL_VERSION version );

	};



}// end of namespace




#endif	// VOXEL_SHADER_H //