#ifndef	FLUID_VOXEL_DATA_2D_H
#define	FLUID_VOXEL_DATA_2D_H

#include	<graphics/gl4x/Texture.h>
#include	<graphics/gl4x/SwapTexture.h>



namespace OreOreLib
{

	class FluidVoxelData2D
	{
	public:

		FluidVoxelData2D();
		FluidVoxelData2D( int width, int height );
		~FluidVoxelData2D();

		void Init( int width, int height );
		void Release();

		int Width() const	{ return m_TexVelocity.Width(); }
		int Height() const	{ return m_TexVelocity.Height(); }
		

//	private:

		enum TEX_FLUID
		{
			TEX_FLUID_VELOCITY,
			TEX_FLUID_VELOCITY_BACK,

			TEX_FLUID_DENSITY,
			TEX_FLUID_DENDITY_BACK,

			TEX_FLUID_PRESSURE,
			TEX_FLUID_PRESSURE_BACK,
			
			TEX_FLUID_TEMPERATURE,
			TEX_FLUID_TEMPERATURE_BACK,
		
			TEX_FLUID_DIVERGENCE,
			TEX_FLUID_OBSTACLES,

			NUM_TEX_FLUID,
		};

		SwapTexture2D	m_TexVelocity,
						m_TexDensity,
						m_TexPressure,
						m_TexTemperature;
		
		Texture2D		m_TexDivergence,
						//m_TexDivergence2,
						m_TexObstacles;


		friend class GPUVoxelFluid2D;

	};




}



#endif	// FLUID_VOXEL_DATA_2D_H //