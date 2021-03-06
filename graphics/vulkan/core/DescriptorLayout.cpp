#include	"DescriptorLayout.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{


	DescriptorLayout::DescriptorLayout()
		: m_refDevice( VK_NULL_HANDLE )
		, m_NumBindings( 0 )
	{

	}



	DescriptorLayout::DescriptorLayout( const DescriptorLayout& obj )
		: m_Bindings( obj.m_Bindings )
		, m_NumBindings( obj.m_NumBindings )
	{

	}



	DescriptorLayout::~DescriptorLayout()
	{
		Release();
	}



	// https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VkDescriptorSetLayout
	// set毎にvkCreateDescriptorSetLayoutが1つ必要になる
	// set毎のbinding情報は、VkDescriptorSetLayoutBinding配列で与える
	void DescriptorLayout::Init( VkDevice device, std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
	{
		m_refDevice	= device;
		m_NumBindings = 0;

		m_Bindings.Init( static_cast<int>( bindings.size() ) );
		auto layoutset = m_Bindings.begin();

		for( auto& binding : bindings )
		{
			m_NumBindings += static_cast<uint32_t>( binding.size() );
			layoutset->Init( binding );
			layoutset++;
		}


		InitDescriptorSetLayout();
	}



	void DescriptorLayout::Release()
	{
		if( m_refDevice != VK_NULL_HANDLE )
		{
			for( auto& descSetLayout : m_DescSetLayouts )
				vkDestroyDescriptorSetLayout( m_refDevice, descSetLayout, nullptr );
		}

		m_Bindings.Release();
		m_DescSetLayouts.Release();
		m_refDevice	= VK_NULL_HANDLE;
	}



	void DescriptorLayout::InitDescriptorSetLayout()
	{
		ASSERT( m_refDevice	!= VK_NULL_HANDLE );

		for( auto& descSetLayout : m_DescSetLayouts )
			vkDestroyDescriptorSetLayout( m_refDevice, descSetLayout, nullptr );

		m_DescSetLayouts.Init( m_Bindings.Length() );

		for( uint32 set_id=0; set_id<m_DescSetLayouts.Length<uint32>(); ++set_id )
		{
			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.pNext		= nullptr;
			layoutInfo.flags		= 0;
			layoutInfo.bindingCount	= m_Bindings[ set_id ].Length<uint32_t>();
			layoutInfo.pBindings	= m_Bindings[ set_id ].begin();

			VK_CHECK_RESULT( vkCreateDescriptorSetLayout( m_refDevice, &layoutInfo, nullptr, &m_DescSetLayouts[ set_id ] ) );
		}
	}



}// end of namespace vk
