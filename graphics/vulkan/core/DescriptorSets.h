#ifndef DESCRIPTOR_SETS_H
#define	DESCRIPTOR_SETS_H

#include	<vulkan/vulkan.h>


#include	<oreore/container/Array.h>


namespace vulkan
{

	class DescriptorSets
	{
	public:

		DescriptorSets();
		DescriptorSets( VkDevice device);
		DescriptorSets( const DescriptorSets& ) = delete;
		~DescriptorSets();

		void Init( VkDevice device );
		void Release();


	private:

		VkDevice		m_refDevice;

	};



}// end of namespace



#endif // !DESCRIPTOR_SETS_H
