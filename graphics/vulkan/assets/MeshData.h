#ifndef MESH_DATA_H
#define	MESH_DATA_H

#include	"../core/VertexLayouts.h"
#include	"../core/VertexBuffer.h"
#include	"../core/IndexBuffer.h"



namespace vk
{

	
	//template < typename T >
	class MeshData
	{
	public:

		MeshData();
		MeshData( GraphicsDevice& device );
		~MeshData();
		MeshData( const MeshData& ) = delete;
		MeshData( MeshData&& obj );

		void Init( GraphicsDevice& device );
		void Release();


	private:

		// Vertex buffer relevant member variables
		//OreOreLib::Array<uint32_t>				m_Indices;
		//OreOreLib::Array<VertexLayout::Vertex>	m_Vertices;
		IndexBuffer								m_IndexBuffer;
		VertexBuffer							m_VertexBuffer;

	};



}// end of namespace vk


#endif // !MESH_DATA_H
