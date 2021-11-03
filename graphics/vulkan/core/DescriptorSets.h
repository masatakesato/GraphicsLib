#ifndef DESCRIPTOR_SETS_H
#define	DESCRIPTOR_SETS_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>

#include	"UniformBuffer.h"
#include	"ShaderParamLayout.h"



namespace vulkan
{

	struct BindLayout
	{
		VkDescriptorType	DescType;
		uint32_t			Set;
		uint32_t			Binding;
	};



	class DescriptorSets
	{
	public:

		DescriptorSets();
		DescriptorSets( VkDevice device, uint32_t numswaps );
		DescriptorSets( const DescriptorSets& ) = delete;
		~DescriptorSets();

		//void InitLayout( VkDevice device, uint32_t numswaps, const OreOreLib::Array<VkDescriptorSetLayoutBinding>& bindings );
		void Init( VkDevice device, uint32_t numswaps, const ShaderParamLayout& paramlayout );

		void Clear();// 
		void Release();


		void UpdateUniform( VkDevice device, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformbuffers );
		void UpdateCombinedImageSampler( VkDevice device, uint32_t binding, VkImageView imageview, VkSampler sampler );


	private:

		VkDevice	m_refDevice;
		uint32_t	m_NumSwaps;

		VkDescriptorPool					m_DescPool;
		OreOreLib::Array<VkDescriptorSet>	m_DescriptorSets;// 2次元配列? [set][num_swapchain_images]  set毎に必要?

		
		void InitDescriptorPool( VkDevice device, uint32_t numswaps, const ShaderParamLayout& paramlayout );
		void InitDescriptorSets( VkDevice device, OreOreLib::Array<VkDescriptorSet>& descSets, uint32_t numswaps, const ShaderParamLayout& paramlayout );

	};



}// end of namespace



#endif // !DESCRIPTOR_SETS_H
