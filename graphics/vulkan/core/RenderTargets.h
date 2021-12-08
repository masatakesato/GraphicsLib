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

		void Init( GraphicsDevice& device, const OreOreLib::Memory<RenderTargetDesc>& renderTargetDescs );
		void Release();

		void ExposeImageViews( OreOreLib::Memory<VkImageView>& imageViews );


	private:


		OreOreLib::Array<RenderBuffer>	m_RenderBuffers;

	};





}// end of namespace vk

#endif // !RENDER_TARGET_DESC_H
