#ifndef MESHDATA_H
#define MESHDATA_H


#include	<string>
//#include	<vector>
//#include	<iostream>
using namespace std;

#include	<oreore/mathlib/GraphicsMath.h>
#include	<oreore/container/Array.h>

#include	<graphics/gl4x/resource/BufferLayout.h>
#include	<graphics/gl4x/scene/Material.h>


#pragma warning(disable : 4996)



class MeshData
{
private:


	// 面
	struct ObjFace
	{	
		int		num_verts;		// 頂点数
		int		matsub_index;	// マテリアルサブセットへのインデックス

		OreOreLib::Array<Vec3i> VertexAttribIndex;// (x,y,z) = (頂点座標インデックス, テクスチャ座標インデックス, 法線インデックス)
		bool	use_normal;		// 法線ベクトル利用の有無
	};

	// マテリアルグループ
	struct ObjMaterialSubset
	{
		int	material_index;	// マテリアルへのインデックス
		int	face_start;		// 開始面インデックス
		int	face_end;		// 終了面インデックス
	};

	// 名前付きグループ
	struct ObjNamedGroup
	{
		char	name[256];	// グループ名
		int		face_start;	// 面index先頭
		int		face_end;	// 面index最後
	};
	

	Vec3f	m_BoundingBox[2];

	// メッシュ形状データ
	OreOreLib::Array<Vec3f>				m_Vertices;	// 頂点配列
	OreOreLib::Array<Vec3f>				m_Normals;	// 法線配列
	OreOreLib::Array<Vec2f>				m_TexCoord;	// テクスチャ座標配列
	OreOreLib::Array<Material>			m_Materials;// マテリアル配列
	OreOreLib::Array<ObjMaterialSubset>	m_MatSubs;	// マテリアルサブセット配列
	OreOreLib::Array<ObjFace>			m_Faces;	// 面配列
	OreOreLib::Array<ObjNamedGroup>		m_Groups;	// 名前付きグループ配列


	

 
	// OBJファイルのローダー
	bool AllocateMemory( const string &aaa );
	bool ParseObjFile(const string &aaa);

	void AddVertex( string str );
	void AddTexCoord( string str );
	void AddNormal( string str );
	int AddFace( string str, int matsub_id );
	int AddMaterialSubset( string str, int startidx, OreOreLib::Array<ObjMaterialSubset> &m_MatSubs );
	int AddNamedGroup( string str, int startidx );

	void AddSmoothGroup(){}// TODO: 時間があったら実装


	// MTLファイルのロード
	bool Load_OBJMTL(const char *filename);
	bool ExtractMaterialComponent(Material &mat, std::ifstream &file, std::string &oneline);// ファイルを読んでmatに属性を追記する


	// 頂点リストの作成
	int AddVertexAttributes(const Vec3i &Query, OreOreLib::Array< OreOreLib::Array<Vec3i> > &Attribs );	// TODO: SceneGraphクラスへ移動
	int AssignVertexIDs( OreOreLib::Array< OreOreLib::Array<Vec3i> > &Attribs);							// TODO: SceneGraphクラスへ移動
	
	
public:

	MeshData();
	~MeshData(){};
	bool Load( const char *filename );
	void GenVertexList(int &numVertAttrs, OreOreLib::VertexLayout **vertexlist, int &numIndices, int **Indices);
void GenVertexList( OreOreLib::Memory<OreOreLib::VertexLayout>& vertexlist, OreOreLib::Memory<uint32>& Indices );

	//void GenerateMeshObject();// メッシュオブジェクトを作成する
	void Information();

	// m_Vertices, m_Normals, m_TexCoord, m_Materials, m_MatSubs, m_Faces, m_Groupsを出力（const型なので値変更不可．参照渡しなのでアドレス変更も不可）
	inline const OreOreLib::Array<Vec3f>& GetVertices() const						{ return m_Vertices; };
	inline const OreOreLib::Array<Vec3f>& GetNormals() const						{ return m_Normals; };
	inline const OreOreLib::Array<Vec2f>& GetTexCoords() const						{ return m_TexCoord; };
	inline const OreOreLib::Array<Material>& GetMaterials() const					{ return m_Materials; };
	inline const OreOreLib::Array<ObjMaterialSubset>& GetMaterialSubsets() const	{ return m_MatSubs; };
	inline const OreOreLib::Array<ObjFace>& GetFaceIndices() const					{ return m_Faces; };
	inline const OreOreLib::Array<ObjNamedGroup>& GetNamedGroups() const			{ return m_Groups; };

	void GetGroupInfo( int idx=0 );
	
	// 描画メソッド
	//friend void DrawObjMesh( MeshData& mesh );
};



#endif /* MESHDATA_H */