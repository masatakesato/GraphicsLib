#ifndef UNIFORM_BUFFER_H
#define	UNIFORM_BUFFER_H

#include	<oreore/container/Array.h>

#include	"MemoryBuffer.h"



namespace vk
{

	//##############################################################################################################//
	//																												//
	//										UniformBuffer class ( single buffer )									//
	//																												//
	//##############################################################################################################//

	class UniformBuffer : public MemoryBuffer
	{
	public:

		UniformBuffer();
		UniformBuffer( GraphicsDevice& device, VkDeviceSize bufferSize );
		~UniformBuffer();
		UniformBuffer( const UniformBuffer& ) = delete;
		UniformBuffer( UniformBuffer&& obj );

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize );
		void Update( void* pData );
		void Update( void* pData, VkDeviceSize size );

		//VkBuffer Buffer() const				{ return m_Buffer; }
		//VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }
		//VkDeviceSize Size() const			{ return m_Size; }


//	private:

//		GraphicsDeviceRef	m_refDevice;

//		VkDeviceSize	m_Size;
//		VkBuffer		m_Buffer;
//		VkDeviceMemory	m_DeviceMemory;

	};




	//##############################################################################################################//
	//																												//
	//								UniformBuffers class ( multiple backbuffers )									//
	//																												//
	//##############################################################################################################//

	class UniformBuffers
	{
	public:

		UniformBuffers();
		UniformBuffers( GraphicsDevice& device, VkDeviceSize bufferSize, uint32 numSwaps );
		~UniformBuffers();
		UniformBuffers( const UniformBuffers& ) = delete;
		UniformBuffers( UniformBuffers&& obj );

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize, uint32 numSwaps );
		void Release();

		void Update( void* pData, uint32 swapIndex );
		void Update( void* pData, VkDeviceSize size, uint32 swapIndex );

		VkDeviceSize Size( uint32 i=0 ) const			{ return m_UniformBuffers[i].Size(); }
		VkBuffer Buffer( uint32 i=0 ) const				{ return m_UniformBuffers[i].Buffer(); }
		VkDeviceMemory DeviceMemory( uint32 i=0 ) const	{ return m_UniformBuffers[i].DeviceMemory(); }


	private:

		OreOreLib::Memory<UniformBuffer>	m_UniformBuffers;




		struct MemBuffer
		{
			VkBuffer		m_Buffer;
			VkDeviceMemory	m_DeviceMemory;
		};
 
		OreOreLib::Array<MemBuffer> m_MemBuffers;


	};


}// end of namespace



#endif // !UNIFORM_BUFFER_H
