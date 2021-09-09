#ifndef	ATMOSPHERIC_SHADER_H
#define	ATMOSPHERIC_SHADER_H


#include <iostream>
#include <math.h>

#include	<GL/glew.h>
#include	<GL/glut.h>
#pragma comment(lib, "glew32.lib")

#include	<CG/cg.h>
#include	<CG/cgGL.h>
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cggl.lib")


#include	<oreore/mathlib/GraphicsMath.h>

#include	<graphics/gl4x/resource/Texture.h>
#include	<graphics/gl4x/scene/Camera.h>
#include	"G_Buffer.h"

using namespace OreOreLib;
using namespace std;


// 大気散乱シェーダ("Precomputed Atmospheric Scattering"を実装)
class AtmosphericShader
{
private:

	//========= 惑星パラメータ ============//
	float Rg;// 惑星の半径
	float Rt;// 惑星中心～大気層までの距離
	
	//===== ルックアップテーブル解像度 ====//
	int	TRANSMITTANCE_W;
	int	TRANSMITTANCE_H;

	int	SKY_W;
	int	SKY_H;

	int	RES_R;
	int	RES_MU;
	int	RES_MU_S;
	int	RES_NU;
	
	//======= 大気状態のパラメータ =======//
	// Rayleigh
	float		m_HR;
	Vec3f	m_betaR;
	// Mie
	float		m_HM;
	Vec3f	m_betaMSca;
	float		m_mieG;



	//============== シェーダ ==============//
	int			m_numEffects;
	CGcontext	CgContext;
	CGeffect	*CgEffect;
	CGtechnique	*CgTechnique;
	enum EffectID
	{
		PRECOMPUTE_TRANSMITTANCE	= 0,
		PRECOMPUTE_IRRADIANCE1		= 1,
		PRECOMPUTE_INSCATTER1		= 2,
		COPY_IRRADIANCE				= 3,
		COPY_INSCATTER1				= 4,
		INSCATTER_S					= 5,
		IRRADIANCE_N				= 6,
		INSCATTER_N					= 7,
		COPY_INSCATTER_N			= 8,
		ATMOSPHERIC_SHADER			= 9
	};


	//======= シェーダグローバル変数 =========//
	CGparameter	m_epLightIntensity;
	CGparameter m_epRg;	// 惑星の半径
	CGparameter m_epRt;	// 大気圏の半径

	CGparameter	m_epHR;
	CGparameter	m_epBetaR;

	CGparameter	m_epHM;
	CGparameter	m_epBetaMSca;
	CGparameter	m_epMieG;


	CGparameter	m_epTRANSMITTANCE_W;// Transmittanceルックアップテクスチャの幅
	CGparameter	m_epTRANSMITTANCE_H;// 高さ

	CGparameter	m_epSKY_W;//
	CGparameter	m_epSKY_H;//

	CGparameter	m_epRES_R;
	CGparameter	m_epRES_MU;
	CGparameter	m_epRES_MU_S;
	CGparameter	m_epRES_NU;


	//=========================== ルックアップテクスチャ ============================//
	unsigned int	transmittanceTexture;//unit 1, T table
	unsigned int	irradianceTexture;//unit 2, E table
	unsigned int	inscatterTexture;//unit 3, S table
	unsigned int	deltaETexture;//unit 4, deltaE table
	unsigned int	deltaSRTexture;//unit 5, deltaS table (Rayleigh part)
	unsigned int	deltaSMTexture;//unit 6, deltaS table (Mie part)
	unsigned int	deltaJTexture;//unit 7, deltaJ table
	
	unsigned int	RenderBuffer_SkyColor;	// 空の色
	unsigned int	RenderBuffer_Surface;	// 惑星の球面形状
	unsigned int	StarFieldTexture;// 星

	//============= フレームバッファオブジェクト==============//
	unsigned int	m_fbo;

	//======================== プライベートメンバ関数 ==========================//
	void PrecomputeLookupTable();

	void PrecomputeTransmittance();
	void PrecomputeIrradiance1();
	void setLayer(float &r_float, float *dhdH, const int layer);
	void PrecomputeInscatter1();
	void CopyIrradiance(int k);
	void CopyInscatter1();
	void InscatterS(int order);
	void IrradianceN(int order);
	void InscatterN(int order);
	void CopyInscatterN();

	void LoadCgEffect();
	bool IsValidEffect(CGeffect effect);

	AtmosphericShader();// デフォルトコンストラクタ



public:

	AtmosphericShader(int width, int height);// コンストラクタ
	~AtmosphericShader();// デストラクタ

	void AttachGbuffer(const G_Buffer *gbuffer);
	void Draw(	Camera *cam, Vec3f L, Vec3f linghtIntensity,  
				unsigned int FinalImage, bool scatterflag);

	void SetScreenSize(int width, int height);// 描画領域の大きさを変更する
};



#endif // ATMOSPHERIC_SHADER_H //