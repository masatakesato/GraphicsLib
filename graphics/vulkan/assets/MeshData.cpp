#include	"MeshData.h"


namespace vk
{

	MeshData::MeshData()
	{

	}



	MeshData::MeshData( GraphicsDevice& device )
	{

	}



	MeshData::~MeshData()
	{
		Release();
	}



	MeshData::MeshData( MeshData&& obj )
	{

	}



	void MeshData::Release()
	{
		m_IndexBuffer.Release();
		m_VertexBuffer.Release();

	}



}// end of namespace vk