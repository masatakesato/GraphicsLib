#ifndef	GPU_EROSION_SIMULATOR_H
#define	GPU_EROSION_SIMULATOR_H


#include	<oreore/GLComputeShader.h>
#include	<oreore/Framebufferobject.h>

#include	"ErosionVoxelData.h"




namespace OreOreLib
{

	class GPUErosionSimulator
	{
	public:

		GPUErosionSimulator();
		GPUErosionSimulator( int dimx, int dimy, float length, int num_layers );
		~GPUErosionSimulator();

		void Init();
		void Release();

		void BindVoxelData( ErosionVoxelData* pdata );
		void UnbindVoxelData();


		void InitTerrain( const Texture2D* pTexTerrain );
		void InitAdvectionTest();



		void SetTimeStep( float timestep )	{ m_timeStep = timestep; }

		//void SetImpulseDensity( float density )			{ m_ImpulseDensity = density; }
		//void SetSmokeBuoyancy( float buoyancy )			{ m_SmokeBuoyancy = buoyancy; }
		//void SetSmokeWeight( float weight )				{ m_SmokeWeight = weight; }
		//void SetTemperatureDissipation( float diss )	{ m_TemperatureDissipation = diss; }
		//void SetVelocityDissipation( float diss )		{ m_VelocityDissipation = diss; }
		//void SetDensityDissipation( float diss )		{ m_DensityDissipation = diss; }
		//void SetVorticityMagnitude( float val )			{ m_VorticityMagnitude = val; }
		//void SetViscosity( float val )					{ m_Viscosity	= val; }
		//void EnableMackCormackAdvection( bool flag )	{ m_bEnableMacCormackAdvection = flag; }
		//void EnableBuoyancy( bool flag )				{ m_bEnableBuoyancy = flag; }

		void SetWaterDumping( float val )	{ m_WaterDumping = val; }


		void DropRandomWater();
		void Vaporize( float rate );

		void Update();

		//void TestAdvection();



		void SetMackCormakAdvection( bool flag ) { m_UseMacCormackAdvection = flag; }
		float GetToalWaterAmount();


	private:

		Vec2i	m_Dim;		// シミュレーション空間の解像度
		float	m_Length;	// セル間の距離
		int		m_numLayers;// 地層の数

		float	m_timeStep;	// タイムステップ
		float	m_Rho;		// 流体の密度
		float	m_G;		// 重力加速度
		float	m_Vaporize;	// 蒸発量

		float	m_C;		// セル底面積

		bool	m_UseMacCormackAdvection;

		float	m_WaterDumping;





		enum EROSION_PASS
		{
			EROSION_PASS_INIT_TERRAIN,

#ifdef _DEBUG
			EROSION_PASS_CALC_WATER_PRESSURE,
			EROSION_PASS_CALC_ACCELERATION,
#endif

			EROSION_PASS_CALC_WaterFlow,
			EROSION_PASS_CALC_WaterLevel,
			EROSION_PASS_CALC_WaterVelocity,
			EROSION_PASS_ForceBasedErosion,
			EROSION_PASS_DissolutionBasedErosion,

			EROSION_PASS_CALC_NormalMap,

			EROSION_PASS_CALC_SedimentTransportCapacity,
			EROSION_PASS_CALC_MaxRegolithThickness,

			EROSION_PASS_CALC_DepositionDissolution,
			EROSION_PASS_CALC_RegolithDepositionDissolution,
			EROSION_PASS_CALC_RegolithOutflow,
			EROSION_PASS_CALC_RegolithThickness,

			NUM_EROSION_PASS,
		};


		GLComputeShader		m_Pass[ NUM_EROSION_PASS ];


		// FBO
		FramebufferObject		m_Fbo;
		FramebufferAttachment	m_Attachment;




#ifdef _DEBUG
		void CalcWaterPressure();// デバッグ用の機能
		void CalcAcceleration();// デバッグ用の機能
#endif

		void CalcWaterFlow();
		void CalcWaterLevel();
		void CalcWaterVelocity();
		void ForceBasedErosion();
		void DissolutionBasedErosion();// 岩石表面の柔らかい層のシミュレーション


		void CalcNormalMap();
		float TiltAngle( const Vec2i &pos, const Texture2D &normalmap );
		void CalcSedimentTransportCapacity();
		void CalcMaxRegolithThickness();	// Dissolution based erosion

		void CalcDepositionDissolution();
		void CalcRegolithDepositionDissolution();
		void CalcRegolithOutflow();
		void CalcRegolithThickness();



		// Semi-Lagrangian Advection
		void Advect_SemiLagrangian( const Texture2D *in, const Texture2D *velocity, Texture2D *out );
		// MacCormack Advection
		void Advect_MacCormack( const Texture2D *in, const Texture2D *velocity, Texture2D *out );

	};



}



#endif	// GPU_EROSION_SIMULATOR_H //