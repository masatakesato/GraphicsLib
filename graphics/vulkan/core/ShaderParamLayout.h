#ifndef SHADER_PARAM_LAYOUT_H
#define	SHADER_PARAM_LAYOUT_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>



namespace vulkan
{

	class ShaderParamLayout
	{
	public:

		ShaderParamLayout();
		ShaderParamLayout( std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings );

		ShaderParamLayout( const ShaderParamLayout& ) = delete;
		~ShaderParamLayout();

//		void AddLayout( int set, VkDescriptorSetLayoutBinding binding );
//		void RemoveLayout( int set, int location );

		void InitDescriptorSetLayout( VkDevice device, uint32_t numswaps );


		int NumSets() const	{ return m_Bindings.Length(); }
		int NumBindins( int set ) const	{ return m_Bindings[set].Length(); }

		const VkDescriptorSetLayoutBinding* Bindings( int set ) const	{ return m_Bindings[ set ].begin(); }
		const VkDescriptorSetLayoutBinding& Binding( int set, int location ) const	{ return m_Bindings[set][location]; }

		const VkDescriptorSetLayout& Layout( int set ) const	{ return m_DescSetLayouts[ set ]; }




	private:

		OreOreLib::Array< OreOreLib::Array<VkDescriptorSetLayoutBinding> >	m_Bindings;
		OreOreLib::Array< VkDescriptorSetLayout>							m_DescSetLayouts;// Set毎に1個ずつ必要

	};


}// end of namespace vulkan


#endif // !SHADER_PARAM_LAYOUT_H
