#ifndef MESH_DATA_H
#define	MESH_DATA_H

#include	"../core/VertexLayouts.h"
#include	"../core/VertexBuffer.h"
#include	"../core/IndexBuffer.h"



namespace vk
{

	class MeshData
	{
	public:

		MeshData();
		MeshData( GraphicsDevice& device );
		~MeshData();
		MeshData( const MeshData& ) = delete;
		MeshData( MeshData&& obj );

		template < typename Vertex >
		void Init( GraphicsDevice& device, const OreOreLib::Memory<uint32>& indices, const OreOreLib::Memory<Vertex>& vertices );
		void Release();

		VkBuffer IndexBuffer() const	{ return m_IndexBuffer.Buffer(); }
		VkBuffer VertexBuffer() const	{ return m_VertexBuffer.Buffer(); }


	private:

		// Vertex buffer relevant member variables
		//OreOreLib::Array<uint32_t>				m_Indices;
		//OreOreLib::Array<VertexLayout::Vertex>	m_Vertices;
		vk::IndexBuffer								m_IndexBuffer;
		vk::VertexBuffer							m_VertexBuffer;

	};




	template < typename Vertex >
	void MeshData::Init( GraphicsDevice& device, const OreOreLib::Memory<uint32>& indices, const OreOreLib::Memory<Vertex>& vertices )
	{

		{
		VkDeviceSize bufferSize = static_cast<VkDeviceSize>( indices.ElementSize() * indices.Length() );
		m_IndexBuffer.Init( device, bufferSize );
		m_IndexBuffer.Update( indices.begin(), bufferSize );
		}

		{
		VkDeviceSize bufferSize = static_cast<VkDeviceSize>( vertices.ElementSize() * vertices.Length()  );
		m_VertexBuffer.Init( device, bufferSize );
		m_VertexBuffer.Update( vertices.begin(), bufferSize );
		}


	}



}// end of namespace vk


#endif // !MESH_DATA_H
