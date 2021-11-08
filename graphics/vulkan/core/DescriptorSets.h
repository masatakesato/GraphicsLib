﻿#ifndef DESCRIPTOR_SETS_H
#define	DESCRIPTOR_SETS_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/NDArray.h>

#include	"UniformBuffer.h"
#include	"ShaderParamLayout.h"



namespace vulkan
{
	// TODO: poolSizes配列に、全てのset/bindingを登録する
// TODO: VkDescriptorPoolはlayout set毎に必要なのか? -> メモリプール確保なので、VkDescriptorPool1個で必要な容量分まとめて確保すればいい

	class ShaderParamDescs
	{
	public:

		ShaderParamDescs();
		ShaderParamDescs( VkDevice device, uint32_t numswaps );
		ShaderParamDescs( const ShaderParamDescs& ) = delete;
		~ShaderParamDescs();

		void Init( VkDevice device, uint32_t numswaps, const ShaderParamLayout& paramlayout );
		void Release();

		void UpdateUniform( uint32_t set, uint32_t binding, const OreOreLib::Array<UniformBuffer>& uniformbuffers );
		void UpdateCombinedImageSampler( uint32_t set, uint32_t binding, VkImageView imageview, VkSampler sampler );


		const VkDescriptorPool& DecriptorPool() const	{ return m_DescPool; }
		const OreOreLib::NDArray<VkDescriptorSet, 2>& DescriptorSets() const	{ return m_DescriptorSets; }

		const VkDescriptorSet& DescriptorSet( int swap_id, int set_id ) const	{ return m_DescriptorSets( swap_id, set_id ); }



	private:

		VkDevice	m_refDevice;
		uint32_t	m_NumSwaps;

		VkDescriptorPool						m_DescPool;
		OreOreLib::NDArray<VkDescriptorSet, 2>	m_DescriptorSets;// スワップチェイン数 x セット数分だけ確保が必要

		
		void InitDescriptorPool( uint32_t numswaps, const ShaderParamLayout& paramlayout );
		void InitDescriptorSets( uint32_t numswaps, const ShaderParamLayout& paramlayout );

	};



}// end of namespace



#endif // !DESCRIPTOR_SETS_H