#pragma once

#include	<graphics/common/OBJLoader.h>
#include	"../resource/BufferLayout.h"



class MeshLoader : public OBJLoader
{
public:

	MeshLoader() : OBJLoader(){}
	~MeshLoader(){}

	void GenVertexList( OreOreLib::Memory<OreOreLib::VertexLayout>& vertexlist, OreOreLib::Memory<uint32>& indices );
	void GenVertexList( int& numVertAttrs, OreOreLib::VertexLayout*& vertices, int &numIndices, uint32*& indices );

};