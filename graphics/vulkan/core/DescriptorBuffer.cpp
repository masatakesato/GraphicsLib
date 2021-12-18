#include	"DescriptorBuffer.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{


	DescriptorBuffer::DescriptorBuffer()
		: m_refDevice( VK_NULL_HANDLE )
	{

	}



	DescriptorBuffer::DescriptorBuffer( VkDevice device, uint32 numSwaps, const DescriptorLayout& descLayout )
	{
		Init(  device, numSwaps, descLayout );
	}



	DescriptorBuffer::~DescriptorBuffer()
	{
		Release();
	}



	void DescriptorBuffer::Init( VkDevice device, uint32 numSwaps, const DescriptorLayout& descLayout )
	{
		m_refDevice	= device;

		InitDescriptorPool( numSwaps, descLayout );
		InitDescriptorSets( numSwaps, descLayout );
	}



	void DescriptorBuffer::Release()
	{
		if( m_refDevice != VK_NULL_HANDLE )
		{
			VK_CHECK_RESULT( vkFreeDescriptorSets( m_refDevice, m_DescPool, m_DescriptorSets.Length<uint32_t>(), m_DescriptorSets.begin() ) );
		
			if( m_DescPool != VK_NULL_HANDLE )
			{
				vkDestroyDescriptorPool( m_refDevice, m_DescPool, nullptr );
				m_DescPool = VK_NULL_HANDLE;
			}
		}
		m_DescriptorSets.Release();
		m_refDevice	= VK_NULL_HANDLE;
	}



	void DescriptorBuffer::BindUniformBuffer( uint32 set, uint32 binding, const OreOreLib::Array<UniformBuffer>& uniformbuffers )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		for( uint32 img_id=0; img_id<m_DescriptorSets.Dim<uint32>(0); ++img_id )
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

	void DescriptorBuffer::BindCombinedImageSampler( uint32 set, uint32 binding, VkImageView imageview, VkSampler sampler )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		for( uint32 img_id=0; img_id<m_DescriptorSets.Dim<uint32>(0); ++img_id )
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



	void DescriptorBuffer::InitDescriptorPool( uint32 numSwaps, const DescriptorLayout& descLayout )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		OreOreLib::Array<VkDescriptorPoolSize> poolSizes( descLayout.NumTotalBindings() );

		auto poolSize = poolSizes.begin();
		for( uint32 set_id=0; set_id<descLayout.NumSets(); ++set_id )
		{
			for( uint32 binding_id=0; binding_id<descLayout.NumBindins( set_id ); ++binding_id )
			{
				poolSize->type	= descLayout.Binding( set_id, binding_id ).descriptorType;
				poolSize->descriptorCount	= numSwaps;
				poolSize++;
			}
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.pNext			= nullptr;
		poolInfo.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets		= numSwaps * descLayout.NumSets<uint32_t>();
		poolInfo.poolSizeCount	= poolSizes.Length<uint32_t>();
		poolInfo.pPoolSizes		= poolSizes.begin();


		VK_CHECK_RESULT( vkCreateDescriptorPool( m_refDevice, &poolInfo, nullptr, &m_DescPool ) );
	}



	void DescriptorBuffer::InitDescriptorSets( uint32 numSwaps, const DescriptorLayout& descLayout )
	{
		ASSERT( m_refDevice != VK_NULL_HANDLE );

		uint32_t descSetCount = numSwaps * descLayout.NumSets<uint32_t>();

		OreOreLib::Array<VkDescriptorSetLayout> layouts( descSetCount );

		auto layout	= layouts.begin();
		for( uint32 img_id=0; img_id<numSwaps; ++img_id )
		{
			for( uint32 set_id=0; set_id<descLayout.NumSets(); ++set_id )
			{
				*layout++ = descLayout.DescriptorSetLayout( set_id );
			}
		}


		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool		= m_DescPool;
		allocInfo.descriptorSetCount	= descSetCount;
		allocInfo.pSetLayouts			= layouts.begin();

		m_DescriptorSets.Init( { numSwaps, descLayout.NumSets() } );
		VK_CHECK_RESULT( vkAllocateDescriptorSets( m_refDevice, &allocInfo, m_DescriptorSets.begin() ) );
	}


}// end of namespae vulkan