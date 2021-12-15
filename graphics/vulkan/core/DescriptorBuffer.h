#ifndef DESCRIPTOR_BUFFER_H
#define	DESCRIPTOR_BUFFER_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/NDArray.h>

#include	"UniformBuffer.h"
#include	"ShaderParamLayout.h"



namespace vk
{
/*
	swap[0]: VkDescriptorSet[0], VkDescriptorSet[1], VkDescriptorSet[2]...
	swap[1]: VkDescriptorSet[0], VkDescriptorSet[1], VkDescriptorSet[2]...
	swap[2]: VkDescriptorSet[0], VkDescriptorSet[1], VkDescriptorSet[2]...
*/
	// サブパス1個分のディスクリプタセット群を保持するクラス.
	class DescriptorBuffer
	{
	public:

		DescriptorBuffer();
		DescriptorBuffer( VkDevice device, uint32_t numswaps, const ShaderParamLayout& paramlayout );
		~DescriptorBuffer();
		DescriptorBuffer( const DescriptorBuffer& ) = delete;

		void Init( VkDevice device, uint32_t numswaps, const ShaderParamLayout& paramlayout );
		void Release();

		void BindUniformBuffer( uint32_t set, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformbuffers );
		void BindCombinedImageSampler( uint32_t set, uint32_t binding, VkImageView imageview, VkSampler sampler );

		uint32_t NumSwaps() const	{ return static_cast<uint32_t>( m_DescriptorSets.Dim(0) ); }
		uint32_t NumSets() const	{ return static_cast<uint32_t>( m_DescriptorSets.Dim(1) ); }

		const VkDescriptorPool& DecriptorPool() const							{ return m_DescPool; }
		const VkDescriptorSet& DescriptorSets( int swap_id ) const				{ return m_DescriptorSets( swap_id, 0 ); }
		const VkDescriptorSet& DescriptorSet( int swap_id, int set_id ) const	{ return m_DescriptorSets( swap_id, set_id ); }



	private:

		VkDevice	m_refDevice;

		VkDescriptorPool						m_DescPool;
		OreOreLib::NDArray<VkDescriptorSet, 2>	m_DescriptorSets;// スワップチェイン数 x セット数分だけ確保が必要. 1次元目: スワップチェイン数, 2次元目: セット数

		
		void InitDescriptorPool( uint32_t numswaps, const ShaderParamLayout& paramlayout );
		void InitDescriptorSets( uint32_t numswaps, const ShaderParamLayout& paramlayout );

	};



}// end of namespace



#endif // !DESCRIPTOR_BUFFER_H
