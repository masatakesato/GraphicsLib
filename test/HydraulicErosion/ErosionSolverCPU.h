#ifndef	EROSION_SIMULATOR_H
#define	EROSION_SIMULATOR_H


#include	"oreore/MathLib.h"
#include	"oreore/Texture.h"


namespace OreOreLib
{



	class ErosionSimulator
	{
	public:

		ErosionSimulator( int dimx, int dimy, float length, int num_layers );
		~ErosionSimulator();

		void BindBuffer();
		void UnbindBuffer();

		void InitHeightExamples();
		void InitAdvectionTest();



		void SetWaterDumping( float val );


		void DropRandomWater();
		void Vaporize( float rate );

		void Update();

		void TestAdvection();


		Texture2D* GetTerrainHeightMap() const { return (Texture2D *)&m_TerrainHeight; };
		Texture2D* GetTerrainNormal() const { return (Texture2D *)&m_TerrainNormal; };
		Texture2D* GetWaterLevelMap() const { return (Texture2D *)&m_WaterLevel; };
		Texture2D* GetWaterNormal() const { return (Texture2D *)&m_WaterNormal; };
#ifdef _DEBUG
		Texture2D* GetPressureMap() const { return (Texture2D *)&m_DP; };
		Texture2D* GetAccelationMap() const { return (Texture2D *)&m_Accelation; };
#endif
		Texture2D* GetFlowAmountMap() const { return (Texture2D *)&m_WaterFlow; };
		Texture2D* GetVelocityMap() const { return (Texture2D *)&m_Velocity; };
		Texture2D* GetCapacity() const { return (Texture2D *)&m_SedimentTransportCapacity; };
		Texture2D* GetSedimentMap() const { return (Texture2D *)&m_ActualSediment; };
		Texture2D* GetRegolith() const { return (Texture2D *)&m_ActualRegolithThickness; };

		void SetMackCormakAdvection( bool flag ) { m_UseMacCormackAdvection = flag; };
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


		// 水流データ
		Texture2D	m_TerrainHeight;	// 地面の高さ
		Texture2D	m_TerrainNormal;	// 地面の法線マップ
		Texture2D	m_WaterLevel;		// 水の高さ
		Texture2D	m_WaterNormal;		// 水の法線マップ
		Texture2D	m_WaterFlow;				// セル間の流量

#ifdef _DEBUG
		Texture2D	m_DP;				// 隣接セルとの水圧差 r:+x, g:-x, b:+y, a:-y
		Texture2D	m_Accelation;		// 加速度
#endif

	// 土砂堆積データ
		Texture2D	m_Velocity;			// 流速(m_timeStep時間あたり)
		Texture2D	m_AverageWaterHeight;		// 現時刻と直前時刻の水位平均値
		Texture2D	m_SedimentTransportCapacity;// 運搬可能な土砂の容量
		Texture2D	m_ActualSediment;			// 流送土砂の量

		Texture2D	m_MaxRegolithThickness;
		Texture2D	m_ActualRegolithThickness;
		Texture2D	m_RegolithOutflow;

		Texture2D	m_ActualSediment_tmp;			// 流送土砂の量

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



#endif	// EROSION_SIMULATOR_H //