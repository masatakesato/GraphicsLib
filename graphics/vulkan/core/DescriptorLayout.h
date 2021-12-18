#ifndef DESCRIPTOR_LAYOUT_H
#define	DESCRIPTOR_LAYOUT_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>



namespace vk
{
	// サブパス1個分のディスクリプタセットレイアウト群を保持するクラス.

	class DescriptorLayout
	{
	public:

		DescriptorLayout();
		~DescriptorLayout();
		DescriptorLayout( const DescriptorLayout& );// = delete;

		void Init( VkDevice device, std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings );
		void Release();

		template < typename T=uint32 > T NumSets() const				{ return m_Bindings.Length<T>(); }
		template < typename T=uint32 > T NumBindins( uint32 set ) const	{ return m_Bindings[ set ].Length<T>(); }
		uint32 NumTotalBindings() const		{ return m_NumBindings; }

		const VkDescriptorSetLayoutBinding* Bindings( uint32 set ) const						{ return m_Bindings[ set ].begin(); }
		const VkDescriptorSetLayoutBinding& Binding( uint32 set, uint32 location ) const	{ return m_Bindings[set][location]; }

		const OreOreLib::Array<VkDescriptorSetLayout>& DescriptorSetLayouts() const	{ return m_DescSetLayouts; }
		const VkDescriptorSetLayout& DescriptorSetLayout( uint32 set ) const		{ return m_DescSetLayouts[ set ]; }


	private:

		VkDevice	m_refDevice;

		uint32		m_NumBindings;
		OreOreLib::Array< OreOreLib::Array<VkDescriptorSetLayoutBinding> >	m_Bindings;
		OreOreLib::Array< VkDescriptorSetLayout >							m_DescSetLayouts;// Set毎に1個ずつ必要


		void InitDescriptorSetLayout();

	};


}// end of namespace vk


#endif // !DESCRIPTOR_LAYOUT_H
