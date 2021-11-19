#ifndef OBJ_LOADER_H
#define	OBJ_LOADER_H

#pragma warning(disable : 4996)

#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/container/Array.h>

#include	<graphics/gl4x/resource/BufferLayout.h>
#include	<graphics/gl4x/scene/Material.h>



class OBJLoader
{
	// face
	struct ObjFace
	{	
		int		num_verts;		// 頂点数
		int		matsub_index;	// マテリアルサブセットへのインデックス
		OreOreLib::Array<Vec3i> VertexAttribIndex;// (x,y,z) = (頂点座標インデックス, テクスチャ座標インデックス, 法線インデックス)
		bool	use_normal;		// 法線ベクトル利用の有無
	};

	// material group struct
	struct ObjMaterialSubset
	{
		int	material_index;	// マテリアルへのインデックス
		int	face_start;		// 開始面インデックス
		int	face_end;		// 終了面インデックス
	};

	// named group struct
	struct ObjNamedGroup
	{
		TCHAR	name[256];	// グループ名
		int		face_start;	// 面index先頭
		int		face_end;	// 面index最後
	};


public:

	OBJLoader();
	~OBJLoader(){}
	bool Load( /*const char *filename*/const tstring& filename );
	void GenVertexList(int &numVertAttrs, OreOreLib::VertexLayout **vertexlist, int &numIndices, int **Indices);
	void GenVertexList( OreOreLib::Memory<OreOreLib::VertexLayout>& vertexlist, OreOreLib::Memory<uint32>& Indices );

	//void GenerateMeshObject();// メッシュオブジェクトを作成する
	void Information();
	void GetGroupInfo( int idx=0 );

	// m_Vertices, m_Normals, m_TexCoord, m_Materials, m_MatSubs, m_Faces, m_Groupsを出力（const型なので値変更不可．参照渡しなのでアドレス変更も不可）
	inline const OreOreLib::Array<Vec3f>& GetVertices() const						{ return m_Vertices; };
	inline const OreOreLib::Array<Vec3f>& GetNormals() const						{ return m_Normals; };
	inline const OreOreLib::Array<Vec2f>& GetTexCoords() const						{ return m_TexCoord; };
	inline const OreOreLib::Array<Material>& GetMaterials() const					{ return m_Materials; };
	inline const OreOreLib::Array<ObjMaterialSubset>& GetMaterialSubsets() const	{ return m_MatSubs; };
	inline const OreOreLib::Array<ObjFace>& GetFaceIndices() const					{ return m_Faces; };
	inline const OreOreLib::Array<ObjNamedGroup>& GetNamedGroups() const			{ return m_Groups; };


	// 描画メソッド
	//friend void DrawObjMesh( OBJLoader& mesh );


private:

	Vec3f	m_BoundingBox[2];

	// obj mesh data
	OreOreLib::Array<Vec3f>				m_Vertices;	// vertices
	OreOreLib::Array<Vec3f>				m_Normals;	// vertex normals
	OreOreLib::Array<Vec2f>				m_TexCoord;	// vertex texcoords
	OreOreLib::Array<Material>			m_Materials;// materials
	OreOreLib::Array<ObjMaterialSubset>	m_MatSubs;	// material subset groups
	OreOreLib::Array<ObjFace>			m_Faces;	// face list
	OreOreLib::Array<ObjNamedGroup>		m_Groups;	// named face groups

 
	// Loading obj file
	bool AllocateMemory( const tstring& str );
	bool ParseObjFile( const tstring& str );

	// Loading mlt file
	bool Load_OBJMTL( const tstring& filename );
	bool ExtractMaterialComponent( Material& mat, tifstream& file, tstring& one_line );
	Vec3f ExtractVector3D( const tstring& str, const tstring& delim );

	// Adding geometry/material data from file string
	void AddVertex( const tstring& str );
	void AddTexCoord( const tstring& str );
	void AddNormal( const tstring& str );
	int AddFace( const tstring& str, int matsub_id );
	int AddMaterialSubset( const tstring& str, int startidx, OreOreLib::Array<ObjMaterialSubset> &m_MatSubs );
	int AddNamedGroup( const tstring& str, int startidx );
	void AddSmoothGroup(){}// TODO: 時間があったら実装

	// VetexAttribute data construction
	int AddVertexAttributes( const Vec3i &Query, OreOreLib::Array< OreOreLib::Array<Vec3i> > &Attribs );
	int AssignVertexIDs( OreOreLib::Array< OreOreLib::Array<Vec3i> > &Attribs);
	
	
};


#endif // !OBJ_LOADER_H