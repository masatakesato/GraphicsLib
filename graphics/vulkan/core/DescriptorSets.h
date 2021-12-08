#ifndef DESCRIPTOR_SETS_H
#define	DESCRIPTOR_SETS_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/NDArray.h>

#include	"UniformBuffer.h"
#include	"ShaderParamLayout.h"



namespace vk
{
	// サブパス1個分のディスクリプタセット群を保持するクラス.

	class ShaderParamDescs
	{
	public:

		ShaderParamDescs();
		ShaderParamDescs( VkDevice device, uint32_t numswaps );
		~ShaderParamDescs();
		ShaderParamDescs( const ShaderParamDescs& ) = delete;


		void Init( VkDevice device, uint32_t numswaps, const ShaderParamLayout& paramlayout );
		void Release();

		void BindUniformBuffer( uint32_t set, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformbuffers );
		void BindCombinedImageSampler( uint32_t set, uint32_t binding, VkImageView imageview, VkSampler sampler );


		const VkDescriptorPool& DecriptorPool() const	{ return m_DescPool; }
		const OreOreLib::NDArray<VkDescriptorSet, 2>& DescriptorSets() const	{ return m_DescriptorSets; }

		const VkDescriptorSet& DescriptorSet( int swap_id, int set_id ) const	{ return m_DescriptorSets( swap_id, set_id ); }



	private:

		VkDevice	m_refDevice;

		VkDescriptorPool						m_DescPool;
		OreOreLib::NDArray<VkDescriptorSet, 2>	m_DescriptorSets;// スワップチェイン数 x セット数分だけ確保が必要

		
		void InitDescriptorPool( uint32_t numswaps, const ShaderParamLayout& paramlayout );
		void InitDescriptorSets( uint32_t numswaps, const ShaderParamLayout& paramlayout );

	};



}// end of namespace



#endif // !DESCRIPTOR_SETS_H
