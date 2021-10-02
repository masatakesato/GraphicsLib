#ifndef	SCENE_LOADER_H
#define	SCENE_LOADER_H

#ifndef	FBXSDK_NEW_API
#define FBXSDK_NEW_API
#endif
#include	<fbxsdk.h>

#include	<oreore/common/TString.h>
#include	<oreore/mathlib/GraphicsMath.h>

#include	"../scene/SceneGraph.h"



namespace OreOreLib
{


	//######################################################################//
	//						SceneLoader interface							//
	//######################################################################//

	// SceneLoader
	class ISceneLoader
	{
	public:

		ISceneLoader(){};
		virtual ~ISceneLoader(){};

		virtual void Load( const char *filename )=0;


	protected:

	};


	// mesh loader factory
	class ISceneLoaderFactory
	{
	public:

		virtual ~ISceneLoaderFactory(){};
		ISceneLoader* CreateInstance(){ return Create(); };

	protected:

		virtual ISceneLoader* Create()=0;

	};





	//######################################################################//
	//								FBX Loader								//
	//######################################################################//



	class FbxLoader : public ISceneLoader
	{
	public:

		FbxLoader();
		~FbxLoader();

		void Init();
		void Release();

		virtual void Load( const char *filename );

		/*void ScanGeometry();*/
		void ScanGeometry( OreOreLib::SceneGraph *pSceneGraph );


	protected:



	private:

		// 頂点のアトリビュート
		struct VertexAttribs
		{
			Vec3f	pos;	// 位置

			int	numUniqueAttribs;// 
			vector<int>		ID;				// ユニークID
			vector<Vec3f>	normal_list;	// 法線ベクトルリスト
			vector<Vec2f>	uv_list;		// UVリスト. TODO: マルチチャンネルUV対応 2013.11.09
			vector<int>		remapIdx;		// アトリビュートへのアドレス（配列インデックス.重複対策）
		};

		// ポリゴンの各頂点のアトリビュートへのインデックス
		struct FaceAttribIdx
		{
			int				num_verts;	// ポリゴン頂点数
			vector<Vec2i>	vbAddress;	// ポリゴン各頂点の頂点バッファへのインデックス. x:VertexID, y:attribID配列のインデックス
		};


		// UVセット
		struct UVSet
		{
			tstring			uvSetName;	// UVSetの名前
			list<tstring>	texName;	// テクスチャ名
			vector<Vec2f>	uvBuffer;	// UV座標値
		};

		// マテリアルサブセット
		struct MaterialSubset
		{
			int					matID;			// マテリアルID

			Vec2i				geomSize;		// 総ポリゴン数, 総頂点数
			vector<int>			polygon_lists;	// ポリゴンインデックス

			OreOreLib::MaterialAttribute	matAttrib;		// マテリアルデータ
		};


		struct MeshNodeInfo
		{
			uint32 fbxAddress;
			uint32 objAddress;
		};

		vector<MeshNodeInfo>	m_MeshNodeList;


		FbxManager*		m_pFbxManager;
		FbxImporter*	m_pFbxImporter;
		FbxScene*		m_pFbxScene;


		void GetTransform( FbxNode *pFbxNode, Vec3f& pos, Vec3f& scale, Quatf& rot );
		void GetPosition( FbxMesh *pFbxMesh, vector<VertexAttribs>& vertexAttribs, vector<FaceAttribIdx>& faceAttribIdx, AABB* pBoundingBox );
		void GetNormal( FbxMesh *pFbxMesh, vector<VertexAttribs>& vertexAttribs, vector<FaceAttribIdx>& faceAttribIdx );
		void GetVertexUV( FbxMesh *pFbxMesh, vector<UVSet>& uvSet, vector<VertexAttribs>& vertexAttribs );
		void GetMaterial( FbxMesh *pFbxMesh, vector<MaterialSubset>& matSubset );
		void ConstructVertexBuffer( vector<VertexAttribs>& vertexAttribs,
			vector<FaceAttribIdx>& faceAttribIdx,
			vector<UVSet>& uvSet, vector<MaterialSubset>& matSubset,
			VertexBuffer *vertexBuffer, IndexBuffer *indexBuffer, MaterialBuffer *matBuffer, FaceGroupBuffer *pFaceGroupBuffer );

		void GetMesh( FbxNode* pNode, SceneGraph *pSceneGraph, SceneNode *pSceneNode );

	};









	class FbxLoaderFactory : ISceneLoaderFactory
	{
	public:

		virtual ~FbxLoaderFactory(){};
		ISceneLoader* CreateInstance();

	protected:

		ISceneLoader* Create(){ return new FbxLoader(); };

	};





	//######################################################################//
	//								OBJ Loader 								//
	//######################################################################//

	class ObjLoader : ISceneLoader
	{

	};




}// end of namespace





#endif	// MESH_LOADER_H //