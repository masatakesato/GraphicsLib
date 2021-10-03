#include	<iostream>
using namespace std;


#include	"MathLib.h"
#include	"SceneGraph.h"
#include	"SceneLoader.h"

using namespace OreOreLib;








/////////////////////////////////////////////////////////
// GetAverageNormal
//   pIndex     : KFbxMeshから得たインデックスバッファ
//   indexCount : インデックス数
//   pNormal    : 法線Element
//   ppOut      : 法線バッファ出力

//bool GetAverageNormal( unsigned short *pIndex, unsigned int indexCount, KFbxLayerElementNormal* pNormal, Vec3f **pOut )
//{
//	unsigned int vertexNum = pNormal->GetDirectArray().GetCount();
//	unsigned int indexNum  = pNormal->GetIndexArray().GetCount();   // 0より大きければインデックスが存在
//	
//	
//	Vec3f *pVec = pVec = new Vec3f[ vertexNum ];// 法線ベクトルを格納する配列
//
//	memset( pVec, 0, sizeof(Vec3f) * vertexNum );
//	KFbxLayerElement::EMappingMode mode  = pNormal->GetMappingMode();
//	KFbxLayerElement::EReferenceMode ref = pNormal->GetReferenceMode();
//	switch( ref )
//	{
//	case KFbxLayerElement::eDirect:
//		{
//			if( mode == KFbxLayerElement::eByPolygonVertex )
//			{
//				// 法線の数はインデックスの数と一致しているが
//				// pNormalがインデックス情報を持っていないので
//				// KFbxMeshから得たインデックスバッファを使用
//				if ( !pIndex )
//					return false;
//				for ( unsigned int i=0; i<indexCount; i++ )
//				{
//					unsigned index = pIndex[i];
//					pVec[index].x += (float)pNormal->GetDirectArray().GetAt(i)[0];
//					pVec[index].y += (float)pNormal->GetDirectArray().GetAt(i)[1];
//					pVec[index].z += (float)pNormal->GetDirectArray().GetAt(i)[2];
//				}
//			}
//			else if( mode == KFbxLayerElement::eByControlPoint )
//			{
//				// 法線の数は頂点数と一致している
//				for( unsigned int i=0; i<vertexNum; i++ )
//				{
//					pVec[i].x = (float)pNormal->GetDirectArray().GetAt(i)[0];
//					pVec[i].y = (float)pNormal->GetDirectArray().GetAt(i)[1];
//					pVec[i].z = (float)pNormal->GetDirectArray().GetAt(i)[2];
//				}
//			}
//			break;
//		}
//
//	case KFbxLayerElement::eIndexToDirect:
//		{
//			// インデックスバッファがpNormalの内部に存在しているのでそれを利用
//			if( mode == KFbxLayerElement::eByPolygonVertex )
//			{
//				// 法線の数はインデックスの数と一致しているので共通頂点の法線を合成
//				for( unsigned int i=0; i<indexNum; i++ )
//				{
//					unsigned int index = pNormal->GetIndexArray().GetAt(i);
//					pVec[index].x += (float)pNormal->GetDirectArray().GetAt(i)[0];
//					pVec[index].y += (float)pNormal->GetDirectArray().GetAt(i)[1];
//					pVec[index].z += (float)pNormal->GetDirectArray().GetAt(i)[2];
//				}
//			}
//			break;
//		}
//
//	}
//
//	// 法線を標準化
//	for( unsigned int i=0; i<vertexNum; i++ )
//	{
//		Normalize( pVec[i] );
//		//D3DXVec3Normalize( &pVec[i], &pVec[i] );
//	}
//
//	*pOut = pVec;
//	return true;
//}
//
//
//
//
//
//
//
//
//
//
//
//
//void GetMesh(FbxNode* node)
//{
//    // メッシュ
//    FbxMesh* pMesh = node->GetMesh();
//
//    if( NULL != pMesh )
//	{
//		// ノード取得
//        tout << "Mesh = " << pMesh->GetName() << ", Node = " << node->GetName() << endl;
//
//		int PolygonNum       = pMesh->GetPolygonCount();
//		int PolygonVertexNum = pMesh->GetPolygonVertexCount();
//		int *IndexAry        = pMesh->GetPolygonVertices();
//		
//		//================== ポリゴン頂点のインデックスを取得する =================//
//		int counter=0;
//		for( int p=0; p<PolygonNum; p++ )
//		{
//			tout << p << ": ";
//			int IndexNumInPolygon = pMesh->GetPolygonSize( p );  // p番目のポリゴンの頂点数
//			for( int n = 0; n < IndexNumInPolygon; n++ )
//			{
//				// ポリゴンpを構成するn番目の頂点のインデックス番号
//				int IndexNumber = pMesh->GetPolygonVertex( p, n );
//				tout << IndexNumber << ", " << IndexAry[counter++] << ",   ";
//
//				
//			}
//			tout << endl;
//		}
//
//		//====================== ポリゴンの頂点座標を取得する ====================//
//		int controlNum = pMesh->GetControlPointsCount();	// 頂点数
//		KFbxVector4* src = pMesh->GetControlPoints();		// 頂点座標配列
//
//		// コピー
//		Vec4f* controlAry = new Vec4f[ controlNum ];
//		for( int i=0; i<controlNum; ++i )
//		{
//			controlAry[ i ].x = (float)src[ i ][ 0 ];
//			controlAry[ i ].y = (float)src[ i ][ 1 ];
//			controlAry[ i ].z = (float)src[ i ][ 2 ];
//			controlAry[ i ].w = (float)src[ i ][ 3 ];
//		}
//
//
//		//===================== レイヤーオブジェクトを取得する ==================//
//		int layerNum = pMesh->GetLayerCount();
//		for( int i=0; i<layerNum; ++i )
//		{
//		   KFbxLayer* layer = pMesh->GetLayer(i);
//		   KFbxLayerElementNormal* normalElem = layer->GetNormals();
//
//		   if( normalElem==0 ) continue;
//
//		  // GetAverageNormal(
//		}
//
//	}// end of if pMesh!=NULL
//
//
//    // 子ノード
//    for(int i = 0; i < node->GetChildCount(); i++)
//        GetMesh(node->GetChild(i));
//}




//int main()
//{
//    // FBXを管理するクラス コレを作らないと始まらない
//    FbxManager* manager = FbxManager::Create();
//
//    // FBXファイルのインポータを作成（第二引数は名前 適当なものでいい）
//    FbxImporter* importer = FbxImporter::Create(manager, "Importer");
//
//    // シーンクラスを作成
//    FbxScene* scene = FbxScene::Create(manager, "Scene");
//    
//    // FBXファイルを読み込む
//    const TCHAR* filename = "Sphere2.fbx";
//    importer->Initialize(filename);
//
//    // 読み込んだFBXファイルからシーンデータを取り出す
//    importer->Import(scene);
//
//    // ルートフレームの名前を表示
//    tout << "Root = " << scene->GetRootNode()->GetName() << endl;
//
//    // メッシュ探索
//    GetMesh(scene->GetRootNode());
//
//
//
//	//FbxNode
//
//    
//    // FBX開放処理
//    manager->Destroy();
//
//    return 0;
//}


int main()
{
	FbxLoader	loader;
	loader.Init();

	loader.Load( _T("TwoCube_TwoMaterials.fbx") );
	
	SceneGraph sg;

	//loader.GetGeometry( &sg );
	//loader.ScanGeometry();
	loader.ScanGeometry( &sg );
	/*
    // FBXを管理するクラス コレを作らないと始まらない
    FbxManager* manager = FbxManager::Create();

    // FBXファイルのインポータを作成（第二引数は名前 適当なものでいい）
    FbxImporter* importer = FbxImporter::Create(manager, "Importer");

    // シーンクラスを作成
    FbxScene* scene = FbxScene::Create(manager, "Scene");
    
    // FBXファイルを読み込む
    const TCHAR* filename = "Sphere2.fbx";
    importer->Initialize(filename);

    // 読み込んだFBXファイルからシーンデータを取り出す
    importer->Import(scene);

    // ルートフレームの名前を表示
    tout << "Root = " << scene->GetRootNode()->GetName() << endl;

    // メッシュ探索
    GetMesh(scene->GetRootNode());



	//FbxNode

    
    // FBX開放処理
    manager->Destroy();
	*/



    return 0;
}