#ifndef DESCRIPTOR_BUFFER_H
#define	DESCRIPTOR_BUFFER_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/NDArray.h>

#include	"UniformBuffer.h"
#include	"DescriptorLayout.h"



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
		DescriptorBuffer( VkDevice device, uint32 numSwaps, const DescriptorLayout& descLayout );
		~DescriptorBuffer();
		DescriptorBuffer( const DescriptorBuffer& ) = delete;
		//DescriptorBuffer( DescriptorBuffer&& obj );

		void Init( VkDevice device, uint32 numSwaps, const DescriptorLayout& descLayout );
		void Release();

		void BindUniformBuffer( uint32 set, uint32 binding, const UniformBuffers& uniformbuffers );
		void BindCombinedImageSampler( uint32 set, uint32 binding, VkImageView imageview, VkSampler sampler );

		template < typename T=uint32 > T NumSets() const	{ return m_DescriptorSets.Dim<T>(1); }

		const VkDescriptorPool& DecriptorPool() const								{ return m_DescPool; }
		const VkDescriptorSet& DescriptorSets( uint32 swap_id ) const				{ return m_DescriptorSets( swap_id, 0 ); }
		const VkDescriptorSet& DescriptorSet( uint32 swap_id, uint32 set_id ) const	{ return m_DescriptorSets( swap_id, set_id ); }



	private:

		VkDevice	m_refDevice;

		VkDescriptorPool						m_DescPool;
		OreOreLib::NDArray<VkDescriptorSet, 2>	m_DescriptorSets;// スワップチェイン数 x セット数分だけ確保が必要. 1次元目: スワップチェイン数, 2次元目: セット数

		
		void InitDescriptorPool( uint32 numSwaps, const DescriptorLayout& descLayout );
		void InitDescriptorSets( uint32 numSwaps, const DescriptorLayout& descLayout );

	};



}// end of namespace



#endif // !DESCRIPTOR_BUFFER_H
