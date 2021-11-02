#include	"DescriptorSets.h"

#include	"../utils/HelperFunctions.h"



namespace vulkan
{


	DescriptorSets::DescriptorSets()
		: m_refDevice( VK_NULL_HANDLE )
	{

	}



	DescriptorSets::DescriptorSets( VkDevice device, uint32_t numswaps )
		: m_refDevice( device )
		, m_NumSwaps( numswaps )
	{

	}



	void DescriptorSets::InitLayout( VkDevice device, uint32_t numswaps, const OreOreLib::Array<VkDescriptorSetLayoutBinding>& bindings )
	{
		m_refDevice	= device;
		m_NumSwaps	= numswaps;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount	= static_cast<uint32_t>( bindings.Length() );
		layoutInfo.pBindings	= bindings.begin();

		VK_CHECK_RESULT( vkCreateDescriptorSetLayout( device, &layoutInfo, nullptr, &m_DescSetLayout ) );
	}
	


	void DescriptorSets::InitPoolAndDescSets( VkDevice device, uint32_t numswaps, const OreOreLib::Array<VkDescriptorSetLayoutBinding>& bindings )
	{
		InitDescriptorPool( device, m_DescPool, numswaps, bindings );
		InitDescriptorSets( device, m_DescriptorSets, numswaps, m_DescSetLayout, m_DescPool );
	}



	void DescriptorSets::Release()
	{
		if( m_refDevice != VK_NULL_HANDLE )
		{

			vkDestroyDescriptorSetLayout( m_refDevice, m_DescSetLayout, nullptr );

			VK_CHECK_RESULT( vkFreeDescriptorSets( m_refDevice, m_DescPool, static_cast<uint32_t>(m_DescriptorSets.Length()), m_DescriptorSets.begin() ) );
			vkDestroyDescriptorPool( m_refDevice, m_DescPool, nullptr );

		}

		m_refDevice	= VK_NULL_HANDLE;
	}




	void DescriptorSets::UpdateUniform( VkDevice device, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformbuffers )
	{
		for( int i=0; i< m_NumSwaps; ++i )
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer	= uniformbuffers[i].Buffer();
			bufferInfo.offset	= 0;
			bufferInfo.range	= uniformbuffers[i].Size();//sizeof( UniformBufferObject );

			VkWriteDescriptorSet writeDescSet = {};

			writeDescSet.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet.dstSet				= m_DescriptorSets[i];
			writeDescSet.dstBinding			= binding;
			writeDescSet.dstArrayElement	= 0;
			writeDescSet.descriptorCount	= 1;
			writeDescSet.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescSet.pBufferInfo		= &bufferInfo;

			vkUpdateDescriptorSets( device, 1, &writeDescSet, 0, nullptr );
		}
	}



	void DescriptorSets::UpdateCombinedImageSampler( VkDevice device, uint32_t binding, VkImageView imageview, VkSampler sampler )
	{
		for( int i=0; i< m_NumSwaps; ++i )
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView		= imageview;
			imageInfo.sampler		= sampler;


			VkWriteDescriptorSet writeDescSet = {};

			writeDescSet.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet.dstSet				= m_DescriptorSets[i];
			writeDescSet.dstBinding			= binding;
			writeDescSet.dstArrayElement	= 0;
			writeDescSet.descriptorCount	= 1;
			writeDescSet.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			writeDescSet.pImageInfo			= &imageInfo;

			vkUpdateDescriptorSets( device, 1, &writeDescSet, 0, nullptr );
		}

	}



	void DescriptorSets::InitDescriptorPool( VkDevice device, VkDescriptorPool descPool, uint32_t numswaps, const OreOreLib::Array<VkDescriptorSetLayoutBinding>& bindings )
	{
		OreOreLib::Array<VkDescriptorPoolSize> poolSizes( bindings.Length() );

		auto binding = bindings.begin();
		for( auto& poolSize : poolSizes )
		{
			poolSize.type				= binding->descriptorType;
			poolSize.descriptorCount	= numswaps;
			binding++;
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.pNext			= nullptr;
		poolInfo.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets		= numswaps;
		poolInfo.poolSizeCount	= static_cast<uint32_t>( poolSizes.Length() );
		poolInfo.pPoolSizes		= poolSizes.begin();


		VK_CHECK_RESULT( vkCreateDescriptorPool( device, &poolInfo, nullptr, &m_DescPool ) );
	}



	void DescriptorSets::InitDescriptorSets( VkDevice device, OreOreLib::Array<VkDescriptorSet>& descSets, uint32_t numswaps, VkDescriptorSetLayout descSetLayout, VkDescriptorPool descPool )
	{
		OreOreLib::Array<VkDescriptorSetLayout> layouts( numswaps );
		for( auto& layout : layouts )	layout = descSetLayout;

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool		= descPool;
		allocInfo.descriptorSetCount	= numswaps;
		allocInfo.pSetLayouts			= layouts.begin();

		descSets.Resize( numswaps );
		VK_CHECK_RESULT( vkAllocateDescriptorSets( device, &allocInfo, descSets.begin() ) );
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