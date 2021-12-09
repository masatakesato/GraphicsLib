#include	"RenderTargets.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	RenderTargets::RenderTargets()
	{

	}



	RenderTargets::RenderTargets( RenderTargets&& obj )
		: m_RenderBuffers( (OreOreLib::Array<RenderBuffer>&&)obj.m_RenderBuffers )
	{

	}



	RenderTargets::~RenderTargets()
	{
		Release();
	}



	void RenderTargets::Init( GraphicsDevice& device, std::initializer_list<RenderTargetDesc> renderTargetDescs )
	{
		//=============== Allocate buffer array ================//

		uint32 bufferCount = 0;
		for( const auto& rtdesc : renderTargetDescs )
		{
			// count up attachments
			if( rtdesc.RenderBuffer.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.RenderBuffer.Resolve )
				bufferCount+=2;
			else
				bufferCount++;
		}

		m_RenderBuffers.Resize( bufferCount );


		//=============== initialize each buffers ==============//

		auto buffer = m_RenderBuffers.begin();

		for( const auto& rtdesc : renderTargetDescs )
		{
			bool ismultisample = rtdesc.RenderBuffer.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.RenderBuffer.Resolve;

			buffer->Init(	device,
							rtdesc.RenderBuffer.Dim.width, rtdesc.RenderBuffer.Dim.height,
							rtdesc.RenderBuffer.Format,
							rtdesc.RenderBuffer.MultiSampleFlag, rtdesc.RenderBuffer.UsageFlags );

			// Put resolve buffer next to multisample buffer
			if( ismultisample )
			{
				buffer++;

				buffer->Init(	device,
								rtdesc.RenderBuffer.Dim.width, rtdesc.RenderBuffer.Dim.height,
								rtdesc.RenderBuffer.Format,
								VK_SAMPLE_COUNT_1_BIT, rtdesc.RenderBuffer.UsageFlags );
			}

			buffer++;
		}

	}



	//void RenderTargets::Init( GraphicsDevice& device, const OreOreLib::Memory<RenderTargetDesc>& renderTargetDescs )
	//{
	//	//=============== Allocate buffer array ================//

	//	uint32 bufferCount = 0;
	//	for( const auto& rtdesc : renderTargetDescs )
	//	{
	//		// count up attachments
	//		if( rtdesc.RenderBuffer.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.RenderBuffer.Resolve )
	//			bufferCount+=2;
	//		else
	//			bufferCount++;
	//	}

	//	m_RenderBuffers.Resize( bufferCount );


	//	//=============== initialize each buffers ==============//

	//	auto buffer = m_RenderBuffers.begin();

	//	for( const auto& rtdesc : renderTargetDescs )
	//	{
	//		bool ismultisample = rtdesc.RenderBuffer.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.RenderBuffer.Resolve;

	//		buffer->Init(	device,
	//						rtdesc.RenderBuffer.Dim.width, rtdesc.RenderBuffer.Dim.height,
	//						rtdesc.RenderBuffer.Format,
	//						rtdesc.RenderBuffer.MultiSampleFlag, rtdesc.RenderBuffer.UsageFlags );

	//		// Put resolve buffer next to multisample buffer
	//		if( ismultisample )
	//		{
	//			buffer++;

	//			buffer->Init(	device,
	//							rtdesc.RenderBuffer.Dim.width, rtdesc.RenderBuffer.Dim.height,
	//							rtdesc.RenderBuffer.Format,
	//							VK_SAMPLE_COUNT_1_BIT, rtdesc.RenderBuffer.UsageFlags );
	//		}

	//		buffer++;
	//	}

	//}



	void RenderTargets::Release()
	{
		for( auto& buffer : m_RenderBuffers )
			buffer.Release();
		m_RenderBuffers.Release();
	}



	void RenderTargets::ExposeImageViews( OreOreLib::Memory<VkImageView>& imageViews )
	{
		const auto buffer = m_RenderBuffers.begin();
		for( auto& view : imageViews )
			view = buffer->View();
	}


}// end of namespace vk

