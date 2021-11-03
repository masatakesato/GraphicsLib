#include	"ShaderParamLayout.h"

#include	"../utils/HelperFunctions.h"



namespace vulkan
{


	ShaderParamLayout::ShaderParamLayout()
	{

	}



	ShaderParamLayout::ShaderParamLayout( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings )
	{
		m_Bindings.Init( bindings.size() );
		auto layoutset = m_Bindings.begin();

		for( auto& binding : bindings )
		{
			layoutset->Init( binding );
			layoutset++;
		}
	}



	ShaderParamLayout::ShaderParamLayout( const ShaderParamLayout& obj )
		: m_Bindings( obj.m_Bindings )
	{

	}



	ShaderParamLayout::~ShaderParamLayout()
	{
//		for( auto& descSetLayout : m_DescSetLayouts )
//			vkDestroyDescriptorSetLayout( device, descSetLayout, nullptr );
	}



	void ShaderParamLayout::InitDescriptorSetLayout( VkDevice device, uint32_t numswaps )
	{
		for( auto& descSetLayout : m_DescSetLayouts )
			vkDestroyDescriptorSetLayout( device, descSetLayout, nullptr );


		m_DescSetLayouts.Init( m_Bindings.Length() );

		for( int set_id=0; set_id<m_DescSetLayouts.Length(); ++set_id )
		{
			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.pNext		= nullptr;
			layoutInfo.flags		= 0;
			layoutInfo.bindingCount	= static_cast<uint32_t>( m_Bindings[ set_id ].Length() );
			layoutInfo.pBindings	= m_Bindings[ set_id ].begin();

			VK_CHECK_RESULT( vkCreateDescriptorSetLayout( device, &layoutInfo, nullptr, &m_DescSetLayouts[ set_id ] ) );
		}
	}



}// end of namespace vulkan
