#include	"MeshLoader.h"



void MeshLoader::GenVertexList( OreOreLib::Memory<OreOreLib::VertexLayout>& vertices, OreOreLib::Memory<uint32>& indices )
{
	vertices.Init( numVertAttrs );
	indices.Init( numIndices );

	LoadPositions( vertices.begin(), offsetof(OreOreLib::VertexLayout, Position), sizeof(OreOreLib::VertexLayout) );
	LoadNormals( vertices.begin(), offsetof(OreOreLib::VertexLayout, Normal), sizeof(OreOreLib::VertexLayout) );
	LoadTexCoords( vertices.begin(), offsetof(OreOreLib::VertexLayout, TexCoord), sizeof(OreOreLib::VertexLayout) );
	LoadIndices( indices.begin(), 0, sizeof(uint32) );
}



void MeshLoader::GenVertexList( int& numVertAttrs, OreOreLib::VertexLayout*& vertices, int &numIndices, uint32*& indices )
{
	numVertAttrs	= this->numVertAttrs;
	numIndices		= this->numIndices;
	vertices		= new OreOreLib::VertexLayout[ numVertAttrs ];
	indices			= new uint32[ numIndices ];

	LoadPositions( vertices, offsetof(OreOreLib::VertexLayout, Position), sizeof(OreOreLib::VertexLayout) );
	LoadNormals( vertices, offsetof(OreOreLib::VertexLayout, Normal), sizeof(OreOreLib::VertexLayout) );
	LoadTexCoords( vertices, offsetof(OreOreLib::VertexLayout, TexCoord), sizeof(OreOreLib::VertexLayout) );
	LoadIndices( indices, 0, sizeof(uint32) );
}




// 頂点配列とインデックス配列を作成する
//void MeshLoader::GenVertexList( int &numVertAttrs, OreOreLib::VertexLayout **vertexlist, int &numIndices, int **Indices )
//{
//	//======================= 頂点レイアウト配列を作成する =======================//
//	
//	// 頂点レイアウト配列(頂点座標/テクスチャ座標/法線ベクトル)を作成する
//	*vertexlist = new OreOreLib::VertexLayout[numVertAttrs];
//	int curr = 0;
//	const Vec3f	dumy3 = {-1, -1, -1};
//	const Vec2f	dumy2 = {-1, -1};
//
//	for( int i=0; i<VertAttribs.Length(); ++i )
//	{
//		for( int j=0; j<VertAttribs[i].Length(); ++j )
//		{
//			int vertIdx		= i;					// 頂点配列m_Vertices上の頂点インデックス
//			int texIdx		= VertAttribs[i][j].x;	// テクスチャ座標属性へのインデックス
//			int normalIdx	= VertAttribs[i][j].y;	// 法線ベクトル属性へのインデックス
//
//			(*vertexlist)[curr].Position	= vertIdx>=0	? m_Vertices[vertIdx]	: dumy3;	// -1だったら弾く
//			(*vertexlist)[curr].TexCoord	= texIdx>=0		? m_TexCoord[texIdx]	: dumy2;	// -1だったら弾く
//			(*vertexlist)[curr].Normal		= normalIdx>=0	? m_Normals[normalIdx]	: dumy3;	// -1だったら弾く
//			curr++;
//		}
//	}
//	
//	//====================== Facesのインデックスの配列を作る ========================//
//	*Indices = new int[numIndices];
//	curr = 0;
//
//	for( int i=0; i<FaceAttribs.Length(); ++i )
//	{
//		// 頂点ごとの属性をVertAttribsに追加登録する
//		for( int j=1; j<FaceAttribs[i].Length()-1; ++j )
//		{
//			Vec2i ID_VertAttrib;
//
//			ID_VertAttrib = FaceAttribs[i][0];
//			(*Indices)[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(0)
//
//			ID_VertAttrib = FaceAttribs[i][j];
//			(*Indices)[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j)
//
//			ID_VertAttrib = FaceAttribs[i][j+1];
//			(*Indices)[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j+1)
//		}// end of j loop
//	}// end of i loop
//	
//}



//void MeshLoader::GenVertexList( OreOreLib::Memory<OreOreLib::VertexLayout>& vertexlist, OreOreLib::Memory<uint32>& Indices )
//{
//
//	//======================= 頂点レイアウト配列を作成する =======================//
//	
//	// 頂点レイアウト配列(頂点座標/テクスチャ座標/法線ベクトル)を作成する
//	vertexlist.Init( numVertAttrs );//*vertexlist = new OreOreLib::VertexLayout[numVertAttrs];
//	int curr = 0;
//	const Vec3f	dumy3 = {-1, -1, -1};
//	const Vec2f	dumy2 = {-1, -1};
//
//	for( int i=0; i<VertAttribs.Length(); ++i )
//	{
//		for( int j=0; j<VertAttribs[i].Length(); ++j )
//		{
//			int vertIdx		= i;					// 頂点配列m_Vertices上の頂点インデックス
//			int texIdx		= VertAttribs[i][j].x;	// テクスチャ座標属性へのインデックス
//			int normalIdx	= VertAttribs[i][j].y;	// 法線ベクトル属性へのインデックス
//
//			vertexlist[curr].Position	= vertIdx>=0	? m_Vertices[vertIdx]	: dumy3;	// -1だったら弾く
//			vertexlist[curr].TexCoord	= texIdx>=0		? m_TexCoord[texIdx]	: dumy2;	// -1だったら弾く
//			vertexlist[curr].Normal		= normalIdx>=0	? m_Normals[normalIdx]	: dumy3;	// -1だったら弾く
//			curr++;
//		}
//	}
//	
//	//====================== Facesのインデックスの配列を作る ========================//
//	Indices.Init( numIndices );//*Indices = new int[numIndices];
//	curr = 0;
//
//	for( int i=0; i<FaceAttribs.Length(); ++i )
//	{
//		// 頂点ごとの属性をVertAttribsに追加登録する
//		for( int j=1; j<FaceAttribs[i].Length()-1; ++j )
//		{
//			Vec2i ID_VertAttrib;
//
//			ID_VertAttrib = FaceAttribs[i][0];
//			Indices[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(0)
//
//			ID_VertAttrib = FaceAttribs[i][j];
//			Indices[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j)
//
//			ID_VertAttrib = FaceAttribs[i][j+1];
//			Indices[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j+1)
//		}// end of j loop
//	}// end of i loop
//}