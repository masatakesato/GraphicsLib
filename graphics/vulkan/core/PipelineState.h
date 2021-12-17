#ifndef PIPELINE_STATE_H
#define	PIPELINE_STATE_H

#include	<vulkan/vulkan.h>

#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/container/Array.h>



namespace vk
{

	class PipelineState
	{
	public:

		PipelineState();
		~PipelineState();
		PipelineState( const PipelineState& ) = delete;

		void Init();
		void Release();

		// Viewport
		void SetDynamicViewport( bool flag );
		void AddViewportRect( const VkViewport& viewport, const VkRect2D& scissor );

		// Rasterization
		void SetDepthClamp( VkBool32 flag );
		void SetRasterizerDiscard( VkBool32 flag );
		void SetPolygonMode( VkPolygonMode mode );
		void SetLineWidth( float width );
		void SetCullMode( VkCullModeFlags mode );
		void SetFrontFace( VkFrontFace mode );
		void SetDepthBias( VkBool32 flag, float constantfactor, float biasclamp, float slopefactor );

		// Blend
		void SetBlend( VkBool32 flag, int attachment=0 );
		void SetColorBlendFactor( VkBlendFactor src, VkBlendFactor dst, int attachment=0 );
		void SetColorBlendOperation( VkBlendOp op, int attachment=0 );
		void SetAlphaBlendFactor( VkBlendFactor src, VkBlendFactor dst, int attachment=0 );
		void SetAlphaBlendOperation( VkBlendOp op, int attachment=0 );
		void SetWriteMask( VkColorComponentFlagBits mask, int attachment=0 );
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

		// Multisample
		void SetSampleShading( VkBool32 flag );
		void SetRasterizationSamples( VkSampleCountFlagBits flag );
		void SetMinSampleShading( float val );
		void SetSampleMask( VkSampleMask* mask );
		void SetAlphaToCoverage( VkBool32 flag );
		void SetAlphaToOne( VkBool32 flag );

		// Vertex input
		void SetVertexInputState( const OreOreLib::Memory<VkVertexInputBindingDescription>& bindingDescs, const OreOreLib::Memory<VkVertexInputAttributeDescription>& attribDescs );
		void SetPrimitiveType( VkPrimitiveTopology topology );

		// Dynamic states
		void SetDynamicStates( std::initializer_list<VkDynamicState> ilist );


//	private:

		// Viewport state
		bool													m_bDynamicViewport;
		VkPipelineViewportStateCreateInfo						m_ViewportState;
		OreOreLib::Array<VkViewport>							m_Viewports;
		OreOreLib::Array<VkRect2D>								m_Scissors;

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo					m_RasterizationState;

		// Blend state
		VkPipelineColorBlendStateCreateInfo						m_ColorBlendState;
		OreOreLib::Array<VkPipelineColorBlendAttachmentState>	m_ColorBlendAttachmentStates;

		// Depth stencil state
		VkPipelineDepthStencilStateCreateInfo					m_DepthStencilState;

		// Multisample satate
		VkPipelineMultisampleStateCreateInfo					m_MultisampleState;

		// Vertex input state
		VkPipelineVertexInputStateCreateInfo					m_VertexInputState;
		VkPipelineInputAssemblyStateCreateInfo					m_InputAssemblyState;

		// 


		// Dynamic state
		VkPipelineDynamicStateCreateInfo						m_DynamicState;
		OreOreLib::Array<VkDynamicState>						m_States;


		friend class GraphicsPipeline;
	};


}// end of namespace vk


#endif // !PIPELINE_STATE_H
