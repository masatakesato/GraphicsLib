#include	"RenderTargets.h"

#include	"../utils/HelperFunctions.h"



namespace vk
{

	RenderTargets::RenderTargets()
		: m_MaxDim{ 0, 0 }
		, m_MinDim{ ~0u, ~0u }
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
			if( rtdesc.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.Resolve )
				bufferCount+=2;
			else
				bufferCount++;

			// check max/min dims
			m_MinDim.width = Min( m_MinDim.width, rtdesc.Dim.width );
			m_MinDim.height = Min( m_MinDim.height, rtdesc.Dim.height );

			m_MaxDim.width = Max( m_MaxDim.width, rtdesc.Dim.width );
			m_MaxDim.height = Max( m_MaxDim.height, rtdesc.Dim.height );
		}

		m_RenderBuffers.Resize( bufferCount );


		//=============== initialize each buffers ==============//

		auto buffer = m_RenderBuffers.begin();

		for( const auto& rtdesc : renderTargetDescs )
		{
			bool ismultisample = rtdesc.MultiSampleFlag != VK_SAMPLE_COUNT_1_BIT && rtdesc.Resolve;

			buffer->Init(	device,
							rtdesc.Dim.width, rtdesc.Dim.height,
							rtdesc.Format,
							rtdesc.MultiSampleFlag, rtdesc.UsageFlags );

			// Put resolve buffer next to multisample buffer
			if( ismultisample )
			{
				buffer++;

				buffer->Init(	device,
								rtdesc.Dim.width, rtdesc.Dim.height,
								rtdesc.Format,
								VK_SAMPLE_COUNT_1_BIT, rtdesc.UsageFlags );
			}

			buffer++;
		}

	}



	void RenderTargets::Release()
	{
		m_RenderBuffers.Release();

		m_MaxDim = { 0, 0 };
		m_MinDim = { ~0u, ~0u };
	}



	void RenderTargets::ExposeImageViews( OreOreLib::Memory<VkImageView>& imageViews )
	{
		const auto buffer = m_RenderBuffers.begin();
		for( auto& view : imageViews )
			view = buffer->View();
	}


}// end of namespace vk

