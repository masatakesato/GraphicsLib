#include	"ShaderParamLayout.h"

#include	"../utils/HelperFunctions.h"



namespace vulkan
{


	ShaderParamLayout::ShaderParamLayout()
		: m_refDevice( VK_NULL_HANDLE )
		, m_NumBindings( 0 )
	{

	}



	ShaderParamLayout::ShaderParamLayout( const ShaderParamLayout& obj )
		: m_Bindings( obj.m_Bindings )
		, m_NumBindings( obj.m_NumBindings )
	{

	}



	ShaderParamLayout::~ShaderParamLayout()
	{
		if( m_refDevice != VK_NULL_HANDLE )
		{
			for( auto& descSetLayout : m_DescSetLayouts )
				vkDestroyDescriptorSetLayout( m_refDevice, descSetLayout, nullptr );
		}

		m_refDevice	= VK_NULL_HANDLE;
	}



	void ShaderParamLayout::Init( VkDevice device, std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
	{
		m_refDevice	= device;
		m_NumBindings = 0;

		m_Bindings.Init( bindings.size() );
		auto layoutset = m_Bindings.begin();

		for( auto& binding : bindings )
		{
			m_NumBindings += binding.size();
			layoutset->Init( binding );
			layoutset++;
		}


		InitDescriptorSetLayout();
	}



	void ShaderParamLayout::InitDescriptorSetLayout()
	{
		ASSERT( m_refDevice	!= VK_NULL_HANDLE );

		for( auto& descSetLayout : m_DescSetLayouts )
			vkDestroyDescriptorSetLayout( m_refDevice, descSetLayout, nullptr );

		m_DescSetLayouts.Init( m_Bindings.Length() );

		for( int set_id=0; set_id<m_DescSetLayouts.Length(); ++set_id )
		{
			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.pNext		= nullptr;
			layoutInfo.flags		= 0;
			layoutInfo.bindingCount	= static_cast<uint32_t>( m_Bindings[ set_id ].Length() );
			layoutInfo.pBindings	= m_Bindings[ set_id ].begin();

			VK_CHECK_RESULT( vkCreateDescriptorSetLayout( m_refDevice, &layoutInfo, nullptr, &m_DescSetLayouts[ set_id ] ) );
		}
	}



}// end of namespace vulkan
