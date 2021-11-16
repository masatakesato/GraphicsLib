#ifndef GRAPHICS_PIPELINE_H
#define	GRAPHICS_PIPELINE_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"ShaderPass.h"
#include	"ShaderParamLayout.h"
#include	"VertexLayouts.h"



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

		struct BlendState
		{
			VkPipelineColorBlendAttachmentState		colorBlendAttachment = {};
			VkPipelineColorBlendStateCreateInfo		colorBlending = {};
		};


		struct VertexInputState
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
			};

			VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				nullptr,
				0,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_FALSE
			};
		};


		struct ViewportState
		{
			bool bDynamic = false;
			VkPipelineViewportStateCreateInfo	createInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
			OreOreLib::Array<VkViewport> viewports;
			OreOreLib::Array<VkRect2D> scissors;
		};


		struct DynamicStates
		{
			VkPipelineDynamicStateCreateInfo	createInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
			OreOreLib::Array<VkDynamicState>	states;
		};




	public:


		GraphicsPipeline();
		GraphicsPipeline( GraphicsDevice& device );
		GraphicsPipeline( const GraphicsPipeline& ) = delete;
		~GraphicsPipeline();



		// Rasterization settings
		void SetDepthClamp( VkBool32 flag );
		void SetRasterizerDiscard( VkBool32 flag );
		void SetPolygonMode( VkPolygonMode mode );
		void SetLineWidth( float width );
		void SetCullMode( VkCullModeFlags mode );
		void SetFrontFace( VkFrontFace mode );
		void SetDepthBias( VkBool32 flag, float constantfactor, float biasclamp, float slopefactor );

		// Multisampe settings
		void SetSampleShading( VkBool32 flag );
		void SetRasterizationSamples( VkSampleCountFlagBits flag );
		void SetMinSampleShading( float val );
		void SetSampleMask( VkSampleMask* mask );
		void SetAlphaToCoverage( VkBool32 flag );
		void SetAlphaToOne( VkBool32 flag );

		// Color blending
		void SetBlend( VkBool32 flag );
		void SetColorBlendFactor( VkBlendFactor src, VkBlendFactor dst );
		void SetColorBlendOperation( VkBlendOp op );
		void SetAlphaBlendFactor( VkBlendFactor src, VkBlendFactor dst );
		void SetAlphaBlendOperation( VkBlendOp op );
		void SetWriteMask( VkColorComponentFlagBits mask );
		void SetAttachmentCount( uint32_t count );
		void SetBlendConstants( const Vec4f& color );

		// Depth stencil
		void SetDepthTest( VkBool32 flag );
		void SetDepthWrite( VkBool32 flag );
		void SetDepthCompareOperator( VkCompareOp op );
		void SetDepthBoundsTest( VkBool32 flag );
		void SetDepthBounds( float minbounds, float maxbounds );
		void SetStencilTest( VkBool32 flag );
		void SetStencilOperators( VkStencilOpState front, VkStencilOpState back );

		// Dynamic states
		void SetDynamicStates( std::initializer_list<VkDynamicState> ilist );

		// Vertex input states
		void SetVertexInputState( const IVertexLayout& vertexlayout );
		void SetPrimitiveType( VkPrimitiveTopology topology );

		// Viewport settings
		void SetDynamicViewport( bool flag );
		void AddViewportRect( const VkViewport& viewport, const VkRect2D& scissor );


		void Build( const ShaderPass& shaderpass, const ShaderParamLayout& paramlayout );
		void Release();



	private:

		GraphicsDeviceRef	m_refDevice;


		VkPipeline			m_Pipeline;
		VkPipelineLayout	m_PipelineLayout;


		VkPipelineRasterizationStateCreateInfo	m_RasterizeInfo;

		BlendState								m_BlendState;
		ViewportState							m_ViewportState;
		VertexInputState						m_VertexInputState;
		VkPipelineDepthStencilStateCreateInfo	depthStencil;
		VkPipelineMultisampleStateCreateInfo	multisampling;
		DynamicStates							m_DynamicStates;

	};





	class EffectPipeline
	{

	private:

		GraphicsDeviceRef	m_refDevice;

		OreOreLib::Array<VkPipeline>	m_Pipelines;



	};


}// end of namespace vk


#endif // !GRAPHICS_PIPELINE_H
