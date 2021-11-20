#pragma once

#include	"OBJLoader.h"

#include	<graphics/gl4x/resource/BufferLayout.h>



class MeshLoader : public OBJLoader
{
public:

	MeshLoader() : OBJLoader(){}
	~MeshLoader(){}

	void GenVertexList( OreOreLib::Memory<OreOreLib::VertexLayout>& vertexlist, OreOreLib::Memory<uint32>& Indices );

	//void GenVertexList( int &numVertAttrs, OreOreLib::VertexLayout **vertexlist, int &numIndices, int **Indices );
	//void GenVertexList( OreOreLib::Memory<OreOreLib::VertexLayout>& vertexlist, OreOreLib::Memory<uint32>& Indices );

};