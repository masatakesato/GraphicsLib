﻿#ifndef STAGING_BUFFER_H
#define	STAGING_BUFFER_H

#include	<oreore/container/Array.h>

#include	"GraphicsDevice.h"



namespace vk
{

	class StagingBuffer
	{
	public:

		StagingBuffer();
		StagingBuffer( GraphicsDevice& device, VkDeviceSize bufferSize );
		StagingBuffer( const StagingBuffer& ) = delete;
		~StagingBuffer();

		void Init( GraphicsDevice& device, VkDeviceSize bufferSize );
		void Release();
		void Update( const void* pData, VkDeviceSize size );

		VkBuffer Buffer() const				{ return m_Buffer; }
		VkDeviceMemory DeviceMemory() const	{ return m_DeviceMemory; }


	private:

		GraphicsDeviceRef	m_refDevice;

		VkBuffer		m_Buffer;
		VkDeviceMemory	m_DeviceMemory;

	};



}// end of namespace



#endif // !STAGING_BUFFER_H
