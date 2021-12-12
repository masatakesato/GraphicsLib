#ifndef GRAPHICS_PIPELINE_H
#define	GRAPHICS_PIPELINE_H

#include	<oreore/mathlib/GraphicsMath.h>

#include	"ShaderPass.h"
#include	"ShaderParamLayout.h"
#include	"VertexLayouts.h"
#include	"PipelineState.h"



namespace vk
{

	union PipelineBarrier
	{
		struct
		{
			VkPipelineStageFlagBits StageFlag;
			VkAccessFlagBits AccessFlag;
		};

		uint64	Flag;


		PipelineBarrier()
			: StageFlag( VK_PIPELINE_STAGE_NONE_KHR )
			, AccessFlag( VK_ACCESS_NONE_KHR )
		{

		}


		PipelineBarrier( VkPipelineStageFlagBits stageflag, VkAccessFlagBits accessflag )
			: StageFlag( stageflag )
			, AccessFlag( accessflag )
		{
		
		}


		PipelineBarrier( const uint64& flag )
			: Flag(flag)
		{
		
		}

	};



	inline PipelineBarrier operator | ( const PipelineBarrier& t1, const PipelineBarrier& t2 )
	{
		return PipelineBarrier( t1.Flag | t2.Flag );
	}




	namespace PipelineStageBarrier
	{
		// Top of pipe
		const PipelineBarrier TopOfPipe							{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE_KHR };

		// Draw indirect
		const PipelineBarrier DrawIndirect						{ VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, VK_ACCESS_INDIRECT_COMMAND_READ_BIT };

		// Vertex input
		const PipelineBarrier VertxInput						{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier VertxInputIndexRead				{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_INDEX_READ_BIT };
		const PipelineBarrier VertxInputAttribRead				{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT };
		
		// Vertex shader
		const PipelineBarrier VertexShader						{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier VertexShaderUniformRead			{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT };
		const PipelineBarrier VertexShaderRead					{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT };
		const PipelineBarrier VertexShaderWrite					{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT };

		// Tessellation control shader
		const PipelineBarrier TessControllShader				{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier TessControllShaderUniformRead		{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT };
		const PipelineBarrier TessControllShaderRead			{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT };
		const PipelineBarrier TessControllShaderWrite			{ VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT };

		// Tessellation evaluation shader
		const PipelineBarrier TessEvaluationShader				{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier TessEvaluationShaderUniformRead	{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT };
		const PipelineBarrier TessEvaluationShaderRead			{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT };
		const PipelineBarrier TessEvaluationShaderWrite			{ VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT };

		// Geometry shader
		const PipelineBarrier GeometryShader					{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier GeometryShaderUniformRead			{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT };
		const PipelineBarrier GeometryShaderRead				{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT };
		const PipelineBarrier GeometryShaderWrite				{ VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT };

		// Early fragment test
		const PipelineBarrier EarlyFragment						{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier EarlyFragmentRead					{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT };// depth load
		const PipelineBarrier EarlyFragmentWrite				{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT };// depth store( clear operation? )

		// Fragment shader
		const PipelineBarrier FragmentShader					{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier FragmentShaderUniformRead			{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT };
		const PipelineBarrier FragmentShaderInputAttachmentRead	{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT };
		const PipelineBarrier FragmentShaderRead				{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT };
		const PipelineBarrier FragmentShaderWrite				{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT };

		// Late fragment test
		const PipelineBarrier LateFragment						{ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier LateFragmentRead					{ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT };
		const PipelineBarrier LateFragmentWrite					{ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT };// depth store

		// Color attachment
		const PipelineBarrier ColorAttachment					{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier ColorAttachmentRead				{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT };// color read
		const PipelineBarrier ColorAttachmentWrite				{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT };// color write
		const PipelineBarrier ColorAttachmentReadWrite			{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkAccessFlagBits(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT) };

		// Bottom of pipe
		const PipelineBarrier BottomOfPipe						{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_NONE_KHR };

		// Compute shader
		const PipelineBarrier ComputeShader						{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier ComputeShaderUniformRead			{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT };
		const PipelineBarrier ComputeShaderRead					{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT };
		const PipelineBarrier ComputeShaderWrite				{ VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT };

		// Transfer
		const PipelineBarrier Transfer							{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier TransferRead						{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT };
		const PipelineBarrier TransferWrite						{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT };

		// Host
		const PipelineBarrier Host								{ VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_NONE_KHR };
		const PipelineBarrier HostRead							{ VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_HOST_READ_BIT };
		const PipelineBarrier HostWrite							{ VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_HOST_WRITE_BIT };

	};






	class GraphicsPipeline
	{
	public:

		GraphicsPipeline();
		GraphicsPipeline( GraphicsDevice& device );
		~GraphicsPipeline();
		GraphicsPipeline( const GraphicsPipeline& ) = delete;
		GraphicsPipeline( GraphicsPipeline&& obj );

		void Init( const ShaderPass& shaderpass, const PipelineState& pipelinestate, VkRenderPass renderPass, uint32_t subpass );
		void Release();

		const VkPipeline Pipeline() const		{ return m_Pipeline; }
		const VkPipelineLayout Layout() const	{ return m_PipelineLayout; }


	private:

		GraphicsDeviceRef	m_refDevice;

		VkPipeline			m_Pipeline;
		VkPipelineLayout	m_PipelineLayout;

	};


}// end of namespace vk


#endif // !GRAPHICS_PIPELINE_H
