#ifndef GRAPHICS_PIPELINE_H
#define	GRAPHICS_PIPELINE_H

#include	"ShaderPass.h"

#include	"ShaderParamLayout.h"



namespace vk
{
	//struct PiipelineConfig
	//{



	//	//VkPipelineVertexInputStateCreateInfo	pVertexInputState;
	//	//VkPipelineInputAssemblyStateCreateInfo	pInputAssemblyState;
	//	VkPipelineTessellationStateCreateInfo	pTessellationState;
	//	VkPipelineViewportStateCreateInfo		pViewportState;
	//	VkPipelineRasterizationStateCreateInfo	pRasterizationState;
	//	VkPipelineMultisampleStateCreateInfo	pMultisampleState;
	//	VkPipelineDepthStencilStateCreateInfo	pDepthStencilState;
	//	VkPipelineColorBlendStateCreateInfo		pColorBlendState;
	//	VkPipelineDynamicStateCreateInfo		pDynamicState;


	//};



	class GraphicsPipeline
	{
	public:


		void Init();


	private:

		VkPipeline			m_Pipeline;
		VkPipelineLayout	m_Layout;
	};





	class EffectPipeline
	{

	private:

		OreOreLib::Array<VkPipeline>	m_Pipelines;



	};


}// end of namespace vk


#endif // !GRAPHICS_PIPELINE_H
