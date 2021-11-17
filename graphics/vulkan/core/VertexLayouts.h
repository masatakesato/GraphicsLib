#ifndef VERTEX_LAYOUT_H
#define	VERTEX_LAYOUT_H

#include	<vulkan/vulkan.h>

#include	<oreore/memory/Memory.h>
#include	<oreore/mathlib/GraphicsMath.h>




namespace vk
{

	// bindingDescs/attribDescs
	// 勝手に外部からデータ改編できなくしたい

	class IVertexLayout
	{
	public:

		virtual const OreOreLib::Memory<VkVertexInputBindingDescription>& BindingDescriptions() const = 0;
		virtual const OreOreLib::Memory<VkVertexInputAttributeDescription>& AttributeDescriptions() const = 0;
	};



	class VertexLayout : IVertexLayout
	{
	public:

		struct Vertex
		{
			Vec3f	Position;
			Vec3f	Normal;
			Vec2f	TexCoord;
			Vec3f	Color;
		};


		const OreOreLib::Memory<VkVertexInputBindingDescription>& BindingDescriptions() const
		{
			return m_BindingDescs;
		}


		const OreOreLib::Memory<VkVertexInputAttributeDescription>& AttributeDescriptions() const
		{
			return m_AttribDescs;
		}



	private:

		const OreOreLib::Memory<VkVertexInputBindingDescription> m_BindingDescs =
		{
			{ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
		};


		const OreOreLib::Memory<VkVertexInputAttributeDescription> m_AttribDescs =
		{
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position) },
			{ 0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal) },
			{ 0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, TexCoord) },
			{ 0, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Color) }
		};

	};






}// end of namespace vk


#endif // !VERTEX_LAYOUT_H
