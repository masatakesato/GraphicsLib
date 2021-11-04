#include	"DescriptorSets.h"

#include	"../utils/HelperFunctions.h"



namespace vulkan
{


	ShaderParamDescs::ShaderParamDescs()
		: m_refDevice( VK_NULL_HANDLE )
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
				*layout++ = paramlayout.Layout( set_id );
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











	//void DescriptorSets::CreateDescriptorSets(	VkDevice device, uint32_t numswaps,
	//											const OreOreLib::Array<VkDescriptorSetLayoutBinding>& bindings,
	//											const OreOreLib::Array<UniformBuffer>& uniformbuffers )
	//{
	//	OreOreLib::Array<VkDescriptorSetLayout> layouts( numswaps/*swapChainImages.size()*/ );
	//	for( auto& layout : layouts )	layout = descriptorSetLayout;

	//	VkDescriptorSetAllocateInfo allocInfo = {};
	//	allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	//	allocInfo.descriptorPool		= descriptorPool;
	//	allocInfo.descriptorSetCount	= numswaps;//static_cast<uint32_t>( swapChainImages.size() );
	//	allocInfo.pSetLayouts			= layouts.begin();

	//	m_DescriptorSets.Resize( numswaps );//swapChainImages.size() );
	//	VK_CHECK_RESULT( vkAllocateDescriptorSets( device, &allocInfo, m_DescriptorSets.begin() ) );





	//	for( size_t i=0; i<numswaps; ++i )
	//	{
	//		VkDescriptorBufferInfo bufferInfo = {};
	//		bufferInfo.buffer	= uniformbuffers[i].Buffer();
	//		bufferInfo.offset	= 0;
	//		bufferInfo.range	= uniformbuffers[i].Size();//sizeof( UniformBufferObject );

	//		VkDescriptorImageInfo imageInfo = {};
	//		imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//		imageInfo.imageView		= textureImageView;
	//		imageInfo.sampler		= textureSampler;

	//		OreOreLib::Array<VkWriteDescriptorSet> descriptorWrites( bindings.Length() );
	//		for( int j=0; j<descriptorWrites.Length(); ++j )
	//		{
	//			descriptorWrites[j].sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//			descriptorWrites[j].dstSet				= descriptorSets[i];
	//			descriptorWrites[j].dstBinding			= i;
	//			descriptorWrites[j].dstArrayElement		= 0;
	//			descriptorWrites[j].descriptorCount		= 1;
	//			descriptorWrites[j].descriptorType		= bindings[j].descriptorType;

	//			descriptorWrites[j].pBufferInfo			= &bufferInfo;

	//		}


	//	//	descriptorWrites[0].sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//	//	descriptorWrites[0].dstSet				= descriptorSets[i];
	//	//	descriptorWrites[0].dstBinding			= 0;
	//	//	descriptorWrites[0].dstArrayElement		= 0;
	//	//	descriptorWrites[0].descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//	//	descriptorWrites[0].descriptorCount		= 1;
	//	//	descriptorWrites[0].pBufferInfo			= &bufferInfo;

	//	//	descriptorWrites[1].sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//	//	descriptorWrites[1].dstSet				= descriptorSets[i];
	//	//	descriptorWrites[1].dstBinding			= 1;
	//	//	descriptorWrites[1].dstArrayElement		= 0;
	//	//	descriptorWrites[1].descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//	//	descriptorWrites[1].descriptorCount		= 1;
	//	//	descriptorWrites[1].pImageInfo			= &imageInfo;

	//		vkUpdateDescriptorSets( device, static_cast<uint32_t>( descriptorWrites.Length() ), descriptorWrites.begin(), 0, nullptr );
	//	}
	//}




}// end of namespae vulkan