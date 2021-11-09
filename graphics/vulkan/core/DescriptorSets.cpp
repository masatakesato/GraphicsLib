#include	"DescriptorSets.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{


	ShaderParamDescs::ShaderParamDescs()
		: m_refDevice( VK_NULL_HANDLE )
		, m_NumSwaps( 0 )
	{

	}



	ShaderParamDescs::ShaderParamDescs( VkDevice device, uint32_t numswaps )
		: m_refDevice( device )
		, m_NumSwaps( numswaps )
	{

	}



	ShaderParamDescs::~ShaderParamDescs()
	{
		Release();
	}




	void ShaderParamDescs::Init( VkDevice device, uint32_t numswaps, const ShaderParamLayout& paramlayout )
	{
		m_NumSwaps	= numswaps;
		m_refDevice	= device;

		InitDescriptorPool( numswaps, paramlayout );
		InitDescriptorSets( numswaps, paramlayout );
	}



	void ShaderParamDescs::Release()
	{
		if( m_refDevice != VK_NULL_HANDLE )
		{
			VK_CHECK_RESULT( vkFreeDescriptorSets( m_refDevice, m_DescPool, static_cast<uint32_t>(m_DescriptorSets.Length()), m_DescriptorSets.begin() ) );
			vkDestroyDescriptorPool( m_refDevice, m_DescPool, nullptr );
		}
		m_DescriptorSets.Release();
		m_refDevice	= VK_NULL_HANDLE;
		m_NumSwaps	= 0;
	}



	void ShaderParamDescs::UpdateUniform( uint32_t set, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformbuffers )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		for( uint32_t img_id=0; img_id< m_NumSwaps; ++img_id )
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer	= uniformbuffers[ img_id ].Buffer();
			bufferInfo.offset	= 0;
			bufferInfo.range	= uniformbuffers[ img_id ].Size();

			VkWriteDescriptorSet writeDescSet = {};

			writeDescSet.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet.dstSet				= m_DescriptorSets( img_id, set );
			writeDescSet.dstBinding			= binding;
			writeDescSet.dstArrayElement	= 0;
			writeDescSet.descriptorCount	= 1;
			writeDescSet.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescSet.pBufferInfo		= &bufferInfo;

			vkUpdateDescriptorSets( m_refDevice, 1, &writeDescSet, 0, nullptr );
		}
	}



	// |------- Swapchain[0]{ DescSet[0], DescSet[1]... } -------|------- Swapchain[1]{ DescSet[0], DescSet[1]... }.... ------|---...

	void ShaderParamDescs::UpdateCombinedImageSampler( uint32_t set, uint32_t binding, VkImageView imageview, VkSampler sampler )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		for( uint32_t img_id=0; img_id< m_NumSwaps; ++img_id )
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView		= imageview;
			imageInfo.sampler		= sampler;


			VkWriteDescriptorSet writeDescSet = {};

			writeDescSet.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet.dstSet				= m_DescriptorSets( img_id, set );
			writeDescSet.dstBinding			= binding;
			writeDescSet.dstArrayElement	= 0;
			writeDescSet.descriptorCount	= 1;
			writeDescSet.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			writeDescSet.pImageInfo			= &imageInfo;

			vkUpdateDescriptorSets( m_refDevice, 1, &writeDescSet, 0, nullptr );
		}

	}



	void ShaderParamDescs::InitDescriptorPool( uint32_t numswaps,  const ShaderParamLayout& paramlayout )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		OreOreLib::Array<VkDescriptorPoolSize> poolSizes( paramlayout.NumTotalBindings() );

		auto poolSize = poolSizes.begin();
		for( int set_id=0; set_id<paramlayout.NumSets(); ++set_id )
		{
			for( int binding_id=0; binding_id<paramlayout.NumBindins( set_id ); ++binding_id )
			{
				poolSize->type	= paramlayout.Binding( set_id, binding_id ).descriptorType;
				poolSize->descriptorCount	= numswaps;
				poolSize++;
			}
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.pNext			= nullptr;
		poolInfo.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets		= numswaps * static_cast<uint32_t>( paramlayout.NumSets() );
		poolInfo.poolSizeCount	= static_cast<uint32_t>( poolSizes.Length() );
		poolInfo.pPoolSizes		= poolSizes.begin();


		VK_CHECK_RESULT( vkCreateDescriptorPool( m_refDevice, &poolInfo, nullptr, &m_DescPool ) );
	}



	void ShaderParamDescs::InitDescriptorSets( uint32_t numswaps, const ShaderParamLayout& paramlayout )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		uint32_t descSetCount = numswaps * static_cast<uint32_t>( paramlayout.NumSets() );

		OreOreLib::Array<VkDescriptorSetLayout> layouts( descSetCount );
		//for( auto& layout : layouts )	layout = paramlayout.Layout(0);

		auto layout	= layouts.begin();
		for( uint32_t img_id=0; img_id<numswaps; ++img_id )
		{
			for( int set_id=0; set_id<paramlayout.NumSets(); ++set_id )
			{
				*layout++ = paramlayout.DescriptorSetLayout( set_id );
			}
		}


		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool		= m_DescPool;
		allocInfo.descriptorSetCount	= descSetCount;
		allocInfo.pSetLayouts			= layouts.begin();

		m_DescriptorSets.Init( numswaps, static_cast<uint32>( paramlayout.NumSets() ) );//descSetCount );
		VK_CHECK_RESULT( vkAllocateDescriptorSets( m_refDevice, &allocInfo, m_DescriptorSets.begin() ) );
	}


}// end of namespae vulkan