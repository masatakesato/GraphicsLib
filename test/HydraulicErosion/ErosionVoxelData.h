#ifndef	EROSION_VOXEL_DATA_H
#define	EROSION_VOXEL_DATA_H


#include	<oreore/Texture.h>
#include	<oreore/TextureFlip.h>



namespace OreOreLib
{

	class ErosionVoxelData
	{
	public:

		ErosionVoxelData();
		ErosionVoxelData( int width, int height );
		~ErosionVoxelData();

		void Init( int width, int height );
		void Release();

		int Width() const	{ return m_WaterFlow.Width(); }
		int Height() const	{ return m_WaterFlow.Height(); }
		

	private:

		enum TEX_EROSION
		{
			TEX_EROSION_HEIGHT,
			TEX_EROSION_TERRAIN_NORMAL,
			TEX_EROSION_WATER_LEVEL,
			TEX_EROSION_WATER_NORMAL,
			TEX_EROSION_WATER_FLOW,

#ifdef	_DEBUG
			TEX_EROSION_DP,
			TEX_EROSION_ACCELATION,
#endif
			TEX_EROSION_VELOCITY,
			TEX_EROSION_AVG_WATER_HEIGHT,
			TEX_EROSION_SEDIMENT_TRANSPORT_CAPACITY,
			TEX_EROSION_ACTUAL_SEDIMENT,

			TEX_EROSION_MAX_REGOLITH_THICKNESS,
			TEX_EROSION_ACTUAL_REGOLITH_THICKNESS,
			TEX_EROSION_REGOLITH_OUTFLOW,
					
			TEX_EROSION_ACTUAL_SEDIMENT_TMP,

			NUM_TEX_EROSION,
		};


		Texture2D	m_TerrainHeight,			// 地面の高さ
					m_TerrainNormal,			// 地面の法線マップ
					m_WaterLevel,				// 水の高さ
					m_WaterNormal,				// 水の法線マップ
					m_WaterFlow,				// セル間の流量

#ifdef _DEBUG
					m_DP,						// 隣接セルとの水圧差 r:+x, g:-x, b:+y, a:-y
					m_Accelation,				// 加速度
#endif

					// 土砂堆積データ
					m_Velocity,					// 流速(m_timeStep時間あたり)
					m_AverageWaterHeight,		// 現時刻と直前時刻の水位平均値
					m_SedimentTransportCapacity,// 運搬可能な土砂の容量
					m_ActualSediment,			// 流送土砂の量

					m_MaxRegolithThickness,
					m_ActualRegolithThickness,
					m_RegolithOutflow,
					
					m_ActualSediment_tmp;		// 流送土砂の量


		friend class GPUErosionSimulator;

	};




}



#endif	// EROSION_VOXEL_DATA_H //