#ifndef SHADER_PARAM_LAYOUT_H
#define	SHADER_PARAM_LAYOUT_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>



namespace vk
{
	// サブパス1個分のディスクリプタセットレイアウト群を保持するクラス.

	class ShaderParamLayout
	{
	public:

		ShaderParamLayout();
		~ShaderParamLayout();
		ShaderParamLayout( const ShaderParamLayout& );// = delete;

		void Init( VkDevice device, std::initializer_list< std::initializer_list<VkDescriptorSetLayoutBinding> > bindings );
		void Release();

//		void AddLayout( int set, VkDescriptorSetLayoutBinding binding );
//		void RemoveLayout( int set, int location );

		void BindDevice( VkDevice device ) { m_refDevice = device; }

		int NumSets() const	{ return m_Bindings.Length(); }
		int NumBindins( int set ) const	{ return m_Bindings[set].Length(); }
		int NumTotalBindings() const	{ return m_NumBindings; }

		const VkDescriptorSetLayoutBinding* Bindings( int set ) const	{ return m_Bindings[ set ].begin(); }
		const VkDescriptorSetLayoutBinding& Binding( int set, int location ) const	{ return m_Bindings[set][location]; }

		const OreOreLib::Array<VkDescriptorSetLayout>& DescriptorSetLayouts() const	{ return m_DescSetLayouts; }
		const VkDescriptorSetLayout& DescriptorSetLayout( int set ) const	{ return m_DescSetLayouts[ set ]; }



	private:

		VkDevice	m_refDevice;

		uint32_t	m_NumBindings;
		OreOreLib::Array< OreOreLib::Array<VkDescriptorSetLayoutBinding> >	m_Bindings;
		OreOreLib::Array< VkDescriptorSetLayout >							m_DescSetLayouts;// Set毎に1個ずつ必要


		void InitDescriptorSetLayout();

	};


}// end of namespace vk


#endif // !SHADER_PARAM_LAYOUT_H
