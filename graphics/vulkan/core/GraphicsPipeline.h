﻿#ifndef GRAPHICS_PIPELINE_H
#define	GRAPHICS_PIPELINE_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"ShaderPass.h"
#include	"ShaderParamLayout.h"
#include	"VertexLayouts.h"
#include	"PipelineState.h"



namespace vk
{

	class GraphicsPipeline
	{
	public:

		GraphicsPipeline();
		GraphicsPipeline( GraphicsDevice& device );
		GraphicsPipeline( const GraphicsPipeline& ) = delete;
		~GraphicsPipeline();

		void Build( const ShaderPass& shaderpass, const PipelineState& pipelinestate );
		void Release();


	private:

		GraphicsDeviceRef	m_refDevice;

		VkPipeline			m_Pipeline;
		VkPipelineLayout	m_PipelineLayout;

	};





	class EffectPipeline
	{

	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<VkPipeline>	m_Pipelines;



	};


}// end of namespace vk


#endif // !GRAPHICS_PIPELINE_H
