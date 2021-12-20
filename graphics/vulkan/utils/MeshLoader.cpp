#include	"MeshLoader.h"



namespace vk
{

	void MeshLoader::GenVertexList( OreOreLib::Memory<VertexLayout::Vertex>& vertices, OreOreLib::Memory<uint32>& indices )
	{
		vertices.Init( numVertAttrs );
		indices.Init( numIndices );

		LoadPositions( vertices.begin(), offsetof(VertexLayout::Vertex, Position), sizeof(VertexLayout::Vertex) );
		LoadNormals( vertices.begin(), offsetof(VertexLayout::Vertex, Normal), sizeof(VertexLayout::Vertex) );
		LoadTexCoords( vertices.begin(), offsetof(VertexLayout::Vertex, TexCoord), sizeof(VertexLayout::Vertex) );
		LoadIndices( indices.begin(), 0, sizeof(uint32) );
	}



	void MeshLoader::GenVertexList( int& numVertAttrs, VertexLayout::Vertex*& vertices, int &numIndices, uint32*& indices )
	{
		numVertAttrs	= this->numVertAttrs;
		numIndices		= this->numIndices;
		vertices		= new VertexLayout::Vertex[ numVertAttrs ];
		indices			= new uint32[ numIndices ];

		LoadPositions( vertices, offsetof(VertexLayout::Vertex, Position), sizeof(VertexLayout::Vertex) );
		LoadNormals( vertices, offsetof(VertexLayout::Vertex, Normal), sizeof(VertexLayout::Vertex) );
		LoadTexCoords( vertices, offsetof(VertexLayout::Vertex, TexCoord), sizeof(VertexLayout::Vertex) );
		LoadIndices( indices, 0, sizeof(uint32) );
	}


}// end of namespace vk
