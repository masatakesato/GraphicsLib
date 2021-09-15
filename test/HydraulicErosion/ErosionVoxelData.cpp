#include	"ErosionVoxelData.h"




namespace OreOreLib
{


	ErosionVoxelData::ErosionVoxelData()
	{
		
	}
	


	ErosionVoxelData::ErosionVoxelData( int width, int height )
	{
		Init( width, height );	
	}



	ErosionVoxelData::~ErosionVoxelData()
	{
		Release();
	}



	void ErosionVoxelData::Init( int width, int height )
	{
		m_TerrainHeight.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_TerrainHeight.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_TerrainHeight.GenHardwareBuffer();

		m_TerrainNormal.Init( width, height, 0, 0, FORMAT_R32G32B32_FLOAT );
		m_TerrainNormal.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_TerrainNormal.GenHardwareBuffer();

		m_WaterLevel.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_WaterLevel.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_WaterLevel.GenHardwareBuffer();

		m_WaterNormal.Init( width, height, 0, 0, FORMAT_R32G32B32_FLOAT );
		m_WaterNormal.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_WaterNormal.GenHardwareBuffer();

		m_WaterFlow.Init( width, height, 0, 0, FORMAT_R32G32B32A32_FLOAT );
		m_WaterFlow.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_WaterFlow.GenHardwareBuffer();

#ifdef _DEBUG
		m_DP.Init( width, height, 0, 0, FORMAT_R32G32B32A32_FLOAT );
		m_DP.GenHardwareBuffer();

		m_Accelation.Init( width, height, 0, 0, FORMAT_R32G32B32A32_FLOAT );
		m_Accelation.GenHardwareBuffer();
#endif

		m_Velocity.Init( width, height, 0, 0, FORMAT_R32G32_FLOAT );
		m_Velocity.GenHardwareBuffer();

		m_AverageWaterHeight.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_AverageWaterHeight.GenHardwareBuffer();

		m_SedimentTransportCapacity.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_SedimentTransportCapacity.GenHardwareBuffer();

		m_ActualSediment.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_ActualSediment.GenHardwareBuffer();

		m_ActualSediment.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_ActualSediment.GenHardwareBuffer();

		m_MaxRegolithThickness.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_MaxRegolithThickness.GenHardwareBuffer();

		m_ActualRegolithThickness.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_ActualRegolithThickness.GenHardwareBuffer();

		m_RegolithOutflow.Init( width, height, 0, 0, FORMAT_R32G32B32A32_FLOAT );
		m_RegolithOutflow.GenHardwareBuffer();

		m_ActualSediment_tmp.Init( width, height, 0, 0, FORMAT_R32_FLOAT );
		m_ActualSediment_tmp.GenHardwareBuffer();
		
	}
	


	void ErosionVoxelData::Release()
	{
		m_TerrainHeight.Release();
		m_TerrainNormal.Release();
		m_WaterLevel.Release();
		m_WaterNormal.Release();
		m_WaterFlow.Release();

#ifdef _DEBUG
		m_DP.Release();
		m_Accelation.Release();
#endif

		m_Velocity.Release();
		m_AverageWaterHeight.Release();
		m_SedimentTransportCapacity.Release();
		m_ActualSediment.Release();

		m_MaxRegolithThickness.Release();
		m_ActualRegolithThickness.Release();
		m_RegolithOutflow.Release();

		m_ActualSediment_tmp.Release();
	}


}// end of namespace