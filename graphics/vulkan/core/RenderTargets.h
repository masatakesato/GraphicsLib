#ifndef RENDER_TARGETS_H
#define	RENDER_TARGETS_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/Array.h>

#include	"RenderBuffer.h"



namespace vk
{
	class RenderTargets
	{
	public:

		RenderTargets();
		~RenderTargets();
		RenderTargets( const RenderTargets& ) = delete;
		RenderTargets( RenderTargets&& obj );

		void Init( GraphicsDevice& device, std::initializer_list<RenderTargetDesc> renderTargetDescs );
		//void Init( GraphicsDevice& device, const OreOreLib::Memory<RenderTargetDesc>& renderTargetDescs );
		void Release();

		
		uint32_t NumBuffers() const	{ return static_cast<uint32_t>( m_RenderBuffers.Length() ); }
		const VkExtent2D& MaxDim() const		{ return m_MaxDim; }
		const VkExtent2D& MinDim() const		{ return m_MinDim; }
		const VkExtent2D& Dim( uint32 i ) const		{ return m_RenderBuffers[i].Extent(); }

		void ExposeImageViews( OreOreLib::Memory<VkImageView>& imageViews );


	private:

		OreOreLib::Array<RenderBuffer>	m_RenderBuffers;

		VkExtent2D	m_MaxDim;
		VkExtent2D	m_MinDim;

	};





}// end of namespace vk

#endif // !RENDER_TARGET_DESC_H
