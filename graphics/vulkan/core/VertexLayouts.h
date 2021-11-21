#ifndef VERTEX_LAYOUT_H
#define	VERTEX_LAYOUT_H

#include	<vulkan/vulkan.h>

#include	<oreore/container/StaticArray.h>
#include	<oreore/mathlib/GraphicsMath.h>




namespace vk
{

	namespace VertexLayout
	{

		struct Vertex
		{
			Vec3f	Position;
			Vec3f	Normal;
			Vec2f	TexCoord;
			Vec3f	Color;
		};

// https://gist.github.com/SaschaWillems/428d15ed4b5d71ead462bc63adffa93a
//TODO: VkVertexInputBindingDescription::bindingは一体何？ -> 頂点アトリビュートをグループ化するインデックス(例えば、頂点座標をbinding=0, スキンメッシュ情報をbinding=1とか)
//TODO: VkPipelineVertexInputStateCreateInfo::pVertexBindingDescriptionsは複数個保持できるようになってるけど、なんで？ -> binding毎に頂点データ分けて管理できるようにするため
		static const OreOreLib::StaticArray<VkVertexInputBindingDescription, 1> BindingDescs =
		{
			{ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
		};


		static const OreOreLib::StaticArray<VkVertexInputAttributeDescription, 4> AttribDescs =
		{
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal) },
			{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, TexCoord) },
			{ 3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Color) }
		};

	}// end of namespace VertexLayout


}// end of namespace vk


#endif // !VERTEX_LAYOUT_H
