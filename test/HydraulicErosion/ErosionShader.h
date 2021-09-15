#ifndef	EROSION_SHADER_H
#define	EROSION_SHADER_H


#include	"../oreore/IShader.h"
#include	"../oreore/GLShader.h"
using namespace OreOreLib;




// テクスチャへの参照
struct refErosionBuffer
{
	Texture2D	*refHeight;
	Texture2D	*refTerrainNormal;
	Texture2D	*refWaterLevel;
	Texture2D	*refWaterNormal;
	Texture2D	*refPressure;
	Texture2D	*refAccel;
	Texture2D	*refFlow;
	Texture2D	*refVelocity;
	Texture2D	*refCapacity;
	Texture2D	*refSediment;
	Texture2D	*refRegolith;
};



class ErosionShader : public IShader
{
private:

	// shader
	GLShader	*m_pShader;	// デフォルトのシェーダー
	
	// uniform location
	GLint		m_ulTerrainHeight;		// 地面の高さ
	GLint		m_ulTerrainNormal;		// 地面の法線
	GLint		m_ulWaterLevel;			// 水の高さ
	GLint		m_ulWaterNormal;		// 水の法線
	GLint		m_ulFlow;				// セル間の流量
	GLint		m_ulDP;					// 隣接セルとの水圧差 r:+x, g:-x, b:+y, a:-y
	GLint		m_ulAccelation;			// 加速度
	
	GLint		m_ulVelocity;				// 流速(m_timeStep時間あたり)
	GLint		m_ulSedimentTransportCapacity;// 運搬可能な土砂の容量
	GLint		m_ulActualSediment;			// 流送土砂の量
	GLint		m_ulRegolithThickness;
	

	const refErosionBuffer *refTex;


public:

	ErosionShader();
	ErosionShader( const TCHAR* filepath, GLSL_VERSION version );
	~ErosionShader();

	void InitShader( const TCHAR* filepath, GLSL_VERSION version );
	
	inline void BindTexture( const refErosionBuffer *buf )	{ refTex = buf;	}

	void Render();


	// Override Virtual Functions
	inline virtual int NumPasses() const { return 1; }
	inline virtual GLuint GetPassID( int shader_idx=0 ) const { return m_pShader->ID(); }

	inline virtual void BindShader( int shader_idx=0 )
	{
		if( m_pCurrShader == m_pShader )	return;
		m_pCurrShader = m_pShader;
		m_pCurrShader->Bind();
	}

	virtual void BindBufferObject( const IBufferObject* pbufferobj ) {}
	virtual void UnbindBufferObject() {}

	virtual void Render( const GeometryBuffer *pGeom, const ViewTransformMatrix *cam_mat ) {}
	
};





// SimpleMeshRendererオブジェクトを生成するファクトリクラス
class ErosionShaderFactory : IShaderFactory
{
private:

	IShader* CreateInstance( GLSL_VERSION version ){ return new ErosionShader( _T("ErosionShader.glsl"), version ); }

public:

	ErosionShaderFactory(){};
	//~ErosionShaderFactory(){};
};





#endif	// EROSION_SHADER_H //