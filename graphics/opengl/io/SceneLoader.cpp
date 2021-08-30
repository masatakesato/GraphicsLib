#include	"SceneLoader.h"



//#if	defined(_DEBUG) & defined(_MT) & defined(_DLL)
//#pragma comment( lib, "fbxsdk-2013.2-mdd.lib" )
//#elif	defined(_DEBUG) & defined(_MT)
//#pragma comment( lib, "fbxsdk-2013.2-mtd.lib" )
//#elif	defined(_MT) & defined(_DLL)
//#pragma comment( lib, "fbxsdk-2013.2-md.lib" )
//#elif	defined(_MT)
//#pragma comment( lib, "fbxsdk-2013.2-mt.lib" )
//#endif



#if	defined(_MT) & defined(_DLL)
#pragma comment( lib, "libfbxsdk-md.lib" )
#elif	defined(_MT)
#pragma comment( lib, "libfbxsdk-mt.lib" )
#endif




namespace OreOreLib
{



	//######################################################################//
	//								FBX Loader								//
	//######################################################################//

	FbxLoader::FbxLoader()
	{
		m_pFbxManager	= NULL;//FbxManager::Create();
		m_pFbxImporter	= NULL;//FbxImporter::Create( m_pFbxManager, "Importer" );
		m_pFbxScene		= NULL;
	}


	FbxLoader::~FbxLoader()
	{
		//IOSREF.FreeIOSettings();
		//if( m_pFbxManager ) m_pFbxManager->Destroy();
		//m_pFbxImporter->Destroy();
		//m_pFbxScene->Clear();
		Release();
	}



	void FbxLoader::Init()
	{
		m_pFbxManager	= FbxManager::Create();
		m_pFbxImporter	= FbxImporter::Create( m_pFbxManager, "Importer" );

	}


	void FbxLoader::Release()
	{
		//IOSREF.FreeIOSettings();
		if( m_pFbxManager )
		{
			m_pFbxManager->Destroy();
			m_pFbxManager = NULL;
		}
		//m_pFbxImporter->Destroy();
		//m_pFbxScene->Clear();
	}


	void FbxLoader::Load( const char *filename )
	{
		// シーンクラスを作成
		if( m_pFbxScene ) m_pFbxScene->Clear();
		m_pFbxScene = FbxScene::Create( m_pFbxManager, "Scene" );

		// FBXファイルを読み込む
		m_pFbxImporter->Initialize( filename );

		// 読み込んだFBXファイルからシーンデータを取り出す
		m_pFbxImporter->Import( m_pFbxScene );

		// ルートフレームの名前を表示
		tcout << "Root = " << m_pFbxScene->GetRootNode()->GetName() << endl;

	}





	void FbxLoader::GetTransform( FbxNode *pFbxNode, Vec3f& pos, Vec3f& scale, Quatf& rot )
	{
		//============================== 回転スケール移動を取得する =================================//
		FbxDouble3 translation	= pFbxNode->LclTranslation;
		FbxDouble3 rotation		= pFbxNode->LclRotation;
		FbxDouble3 scaling		= pFbxNode->LclScaling;

#ifdef _DEBUG
		tcout << "   Translation = " << translation[0] << ", " << translation[1] << ", " << translation[2] << tendl;
		tcout << "   Rotation = " << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << tendl;
		tcout << "   Scaling = " << scaling[0] << ", " << scaling[1] << ", " << scaling[2] << tendl;
#endif

		// 平行移動, 回転, スケーリングを設定
		InitVec( pos, (float)translation[0], (float)translation[1], (float)translation[2] );
		InitVec( scale, (float)scaling[0], (float)scaling[1], (float)scaling[2] );


		const int FbxRotOrder[7][3] =
		{
			{ 0, 1, 2 },	//eEulerXYZ,
			{ 0, 2, 1 },	//eEulerXZY,
			{ 1, 2, 0 },	//eEulerYZX,
			{ 1, 0, 2 },	//eEulerYXZ,
			{ 2, 0, 1 },	//eEulerZXY,
			{ 2, 1, 0 },	//eEulerZYX,
			{ 0, 1, 2 },	//eSphericXYZ
		};
		const Vec3f AXIS[] =
		{
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
		};


		EFbxRotationOrder lRotationOrder;
		pFbxNode->GetRotationOrder( FbxNode::eSourcePivot, lRotationOrder );

		const int *rotOrder = FbxRotOrder[ lRotationOrder ];

		// クォータニオン初期化
		Quatf quat1, quat2, quat3, temp;
		InitQuat( quat1, (float)(rotation[rotOrder[0]] * M_PI / 180.0f), AXIS[rotOrder[0]] );
		InitQuat( quat2, (float)(rotation[rotOrder[1]] * M_PI / 180.0f), AXIS[rotOrder[1]] );
		InitQuat( quat3, (float)(rotation[rotOrder[2]] * M_PI / 180.0f), AXIS[rotOrder[2]] );

		Multiply( temp, quat3, quat2 );
		Multiply_( rot, temp, quat1 );


		// オイラー角の回転行列初期化. 廃止予定(2013.11.19)
		//Mat4f mrot1, mrot2, mrot3, temp;
		//MatRotationX( mrot1, (float)(rotation[rotOrder[0]] * M_PI / 180.0f) );
		//MatRotationY( mrot2, (float)(rotation[rotOrder[1]] * M_PI / 180.0f) );
		//MatRotationZ( mrot3, (float)(rotation[rotOrder[2]] * M_PI / 180.0f) );

		//Multiply( temp, mrot3, mrot2 );
		//Multiply(  transattrib->Rotation, temp, mrot1 );

		//QuatIdentity( transattrib->Quaternion );
		//MatIdentity( transattrib->Rotation );

	}




	void FbxLoader::GetPosition( FbxMesh *pFbxMesh, vector<VertexAttribs>& vertexAttribs, vector<FaceAttribIdx>& faceAttribIdx, AABB* pBoundingBox )
	{
		int numPolygons	= pFbxMesh->GetPolygonCount();			// ポリゴン数
		int numVertices = pFbxMesh->GetControlPointsCount();	// 頂点数
		int numIndices	= pFbxMesh->GetPolygonVertexCount();	// 頂点インデックス数


		//===================== ポリゴン情報の初期化 ======================//
		faceAttribIdx.resize( numPolygons );

		for( int pidx=0; pidx<numPolygons; ++pidx )
		{
			faceAttribIdx[pidx].num_verts = pFbxMesh->GetPolygonSize(pidx);

			for( int vidx=0; vidx<faceAttribIdx[pidx].num_verts; ++vidx )
			{
				int vbidx			= pFbxMesh->GetPolygonVertex( pidx, vidx );
				Vec2i newaddress	= { vbidx, 0 };
				faceAttribIdx[pidx].vbAddress.push_back( newaddress );
			}
		}


		//================= 頂点バッファへのデータコピー ==================//
		vertexAttribs.resize( numVertices );

		FbxVector4* vertex	= pFbxMesh->GetControlPoints();		// 頂点座標配列

		Vec3f bbmin, bbmax;// ローカルスペースのバウンディングボックス
		InitVec( bbmin, (float)vertex[0][0], (float)vertex[0][1], (float)vertex[0][2] );
		bbmax	= bbmin;

		for( int i=0; i<numVertices; ++i )
		{
			InitVec( vertexAttribs[i].pos, (float)vertex[i][0], (float)vertex[i][1], (float)vertex[i][2] );

			Min( bbmin, bbmin, vertexAttribs[i].pos );
			Max( bbmax, bbmax, vertexAttribs[i].pos );
		}
		
		pBoundingBox->SetMinMax( bbmin, bbmax );
	}



	void FbxLoader::GetNormal( FbxMesh *pFbxMesh, vector<VertexAttribs>& vertexAttribs, vector<FaceAttribIdx>& faceAttribIdx )
	{
		tcout << "FbxLoader::GetNormal()..." << endl;

		FbxGeometryElementNormal* elmNormal = pFbxMesh->GetLayer(0)->GetNormals();//GetElementNormal(0);

		if( !elmNormal )	return;

		int	normalNum								= elmNormal->GetDirectArray().GetCount();
		FbxGeometryElement::EMappingMode mapMode	= elmNormal->GetMappingMode();
		FbxGeometryElement::EReferenceMode refMode	= elmNormal->GetReferenceMode();

		if( mapMode==FbxLayerElement::eByPolygonVertex )// ポリゴン頂点に対して法線ベクトルが定義されている場合
		{
			if( refMode==FbxLayerElement::eDirect )
			{
				int numVertices	= pFbxMesh->GetControlPointsCount();	// 頂点数

				// 一つの頂点に対して複数の法線ベクトルが対応する可能性がある
				// ポリゴンの頂点ごとに法線ベクトルを調べて、法線リストを作る
				int numPolygons	= pFbxMesh->GetPolygonCount();

				for( int pidx=0; pidx<numPolygons; ++pidx )
				{
					int numPolyVerts	= pFbxMesh->GetPolygonSize( pidx );  // pidx番目のポリゴンの頂点数
					for( int vidx=0; vidx<numPolyVerts; ++vidx )
					{
						// ポリゴン頂点の法線ベクトルを取得する
						FbxVector4	tmp_normal;
						pFbxMesh->GetPolygonVertexNormal( pidx, vidx, tmp_normal );

						int vbIdx	= pFbxMesh->GetPolygonVertex( pidx, vidx );

						// 頂点バッファの法線リストに法線ベクトルを追加する
						Vec3f new_normal ={ (float)tmp_normal[0], (float)tmp_normal[1], (float)tmp_normal[2] };
						vertexAttribs[vbIdx].normal_list.push_back( new_normal);

						// アトリビュートのインデックス値を登録する
						int remapIdx = (int)vertexAttribs[vbIdx].normal_list.size()-1;
						vertexAttribs[vbIdx].remapIdx.push_back( remapIdx );
						faceAttribIdx[pidx].vbAddress[vidx].y = remapIdx;
					}
				}// end of pidx loop

			}
			else if( refMode==FbxLayerElement::eIndexToDirect )
			{
				tcout << "   normal refMode is eIndexToDirect. Not Supprterd..." << endl;
			}
		}
		else if( mapMode == FbxGeometryElement::eByControlPoint )
		{
			if( refMode==FbxLayerElement::eDirect )// 頂点座標に対してユニークな法線ベクトルが定義されている場合
			{
				for( int k=0; k<normalNum; ++k )
				{
					Vec3f new_normal ={ (float)elmNormal->GetDirectArray().GetAt(k)[0],
						(float)elmNormal->GetDirectArray().GetAt(k)[1],
						(float)elmNormal->GetDirectArray().GetAt(k)[2] };
					vertexAttribs[k].normal_list.push_back( new_normal );
					vertexAttribs[k].remapIdx.push_back( (int)vertexAttribs[k].normal_list.size()-1 );
				}

			}
			else if( refMode==FbxLayerElement::eIndexToDirect )
			{
				tcout << "   normal refMode is eIndexToDirect. Not Supprterd..." << endl;
			}
		}

	}




	void FbxLoader::GetVertexUV( FbxMesh* pFbxMesh, vector<UVSet>& uvSet, vector<VertexAttribs>& vertexAttribs )
	{
		int UVLayerCount = pFbxMesh->GetElementUVCount();

		uvSet.resize( UVLayerCount );

		for( int i=0; i<UVLayerCount; ++i )
		{
			FbxGeometryElementUV* elmUV	= pFbxMesh->GetElementUV(i);
			int UVNum					= elmUV->GetDirectArray().GetCount();	// UV座標の数
			int indexNum				= elmUV->GetIndexArray().GetCount();	// UV座標へのインデックスの数
			int size					= UVNum>indexNum ? UVNum : indexNum;// これは何？

			FbxGeometryElement::EMappingMode mapMode	= elmUV->GetMappingMode();
			FbxGeometryElement::EReferenceMode refMode	= elmUV->GetReferenceMode();

			elmUV	= pFbxMesh->GetLayer(i)->GetUVs();
			//mesh->GetTextureUVIndex( pidx, vidx, FbxLayerElement::eTextureDiffuse );// ポリゴン頂点毎の
#ifdef _DEBUG
			tcout << "UVLayer(" << i << ").Name = " << elmUV->GetName() << endl;
#endif
			if( mapMode==FbxGeometryElement::eByPolygonVertex )// ポリゴン頂点毎にマッピング
			{
				//######################## ポリゴン頂点毎のUV座標を取得できる #########################//
				if( refMode==FbxLayerElement::eDirect )// 直接取得
				{
					for( int k=0; k<size; ++k )
					{
						const Vec2f temp_uv = { (float)elmUV->GetDirectArray().GetAt(k)[0], 1.0f - (float)elmUV->GetDirectArray().GetAt(k)[1] };
						uvSet[i].uvBuffer.push_back( temp_uv );
					}
				}
				else if( refMode==FbxLayerElement::eIndexToDirect )// インデックス配列経由で取得
				{
					for( int k=0; k<size; ++k )
					{
						const int index = elmUV->GetIndexArray().GetAt(k);
						const Vec2f temp_uv = { (float)elmUV->GetDirectArray().GetAt(index)[0], 1.0f - (float)elmUV->GetDirectArray().GetAt(index)[1] };
						uvSet[i].uvBuffer.push_back( temp_uv );
					}// end of k loop

				}
				//#####################################################################################//
			}
			else if( mapMode == FbxGeometryElement::eByControlPoint )
			{
				// 未サポート
				if( refMode==FbxLayerElement::eDirect )
				{
				}
				else if( refMode==FbxLayerElement::eIndexToDirect )// インデックス配列経由で取得
				{
				}
			}

		}// end of UVLayerCount loop


		int polycount=0;
		for( int pidx=0; pidx<pFbxMesh->GetPolygonCount(); ++pidx )
		{
			int numVertices	= pFbxMesh->GetPolygonSize(pidx);
			for( int vidx=0; vidx<numVertices; ++vidx )
			{
				// ポリゴン番号/ポリゴン頂点番号から、頂点バッファのインデックスを計算する
				int vbIdx	= pFbxMesh->GetPolygonVertex( pidx, vidx );

				// ポリゴン番号/ポリゴン頂点番号から、UVのインデックスを計算する
				int uvidx	= polycount + vidx;

				// 各頂点にUVをぶら下げる.ただしレイヤー毎に別々に
				for( int uvlayeridx=0; uvlayeridx<uvSet.size(); ++uvlayeridx )
				{
					// ポリゴン頂点毎のUVを取得する
					Vec2f curr_uv	= uvSet[uvlayeridx].uvBuffer[ uvidx ];
					// uvを頂点データ属性に追加する
					vertexAttribs[ vbIdx ].uv_list.push_back( curr_uv );
					//pAttrIdx->uvIdx.push_back( (int)pAttrIdx->uvIdx.size() );// GetNormalの時にポリゴン頂点単位で格納したのでやらなくていい

					//tcout << "(" << pidx << ", " << vidx << ")..." << 

					break;// 現状UVSetは1個だけ対応
				}

			}// end of vidx loop

			polycount += numVertices;
		}// end of pidx loop


	}// end of method



	void FbxLoader::GetMaterial( FbxMesh *pFbxMesh, vector<MaterialSubset>& matSubset )
	{
		FbxNode* pFbxNode	= pFbxMesh->GetNode();
		int	numMaterials	= pFbxNode->GetMaterialCount();

#ifdef _DEBUG
		tcout << "NumMaterials in Node : " << numMaterials << endl;
#endif

		matSubset.resize( numMaterials );
		for( int midx=0; midx<numMaterials; ++midx )
		{
			// マテリアル情報を取得してバッファに格納する
#ifdef _DEBUG
			tcout <<  "  material name: " << pFbxNode->GetMaterial(midx)->GetName() << endl;
#endif
			matSubset[midx].matID = midx;
			InitVec( matSubset[midx].geomSize, 0, 0 );


			//========================= マテリアルの種類を判別して値を取り出す ==========================//
			FbxSurfaceMaterial* pFbxMtl	= pFbxNode->GetMaterial(midx);
			FbxClassId mtlClass	= pFbxMtl->GetClassId();

			MaterialAttribute *pmatAttrib = &matSubset[midx].matAttrib;


			if( mtlClass.Is( FbxSurfaceLambert::ClassId ) )
			{
#ifdef _DEBUG
				tcout << "Extracting Lambert Material" << endl;
#endif
				FbxSurfaceLambert* pLambert	= (FbxSurfaceLambert *)pFbxMtl;
				InitVec( pmatAttrib->Ambient, (float)pLambert->Ambient.Get()[0], (float)pLambert->Ambient.Get()[1], (float)pLambert->Ambient.Get()[2] );
				InitVec( pmatAttrib->Diffuse, (float)pLambert->Diffuse.Get()[0], (float)pLambert->Diffuse.Get()[1], (float)pLambert->Diffuse.Get()[2] );
				InitVec( pmatAttrib->Specular, 0.0f, 0.0f, 0.0f );
				pmatAttrib->Shininess		= 0.0f;
				pmatAttrib->RefractiveIndex	= 1.0f;
				pmatAttrib->Opacity			= (float)pLambert->TransparencyFactor;
				InitVec( pmatAttrib->TransparentColor, (float)pLambert->TransparentColor.Get()[0], (float)pLambert->TransparentColor.Get()[1], (float)pLambert->TransparentColor.Get()[2] );
			}
			else if( mtlClass.Is( FbxSurfacePhong::ClassId ) )
			{
#ifdef _DEBUG
				tcout << "Extracting Phong Material" << endl;
#endif
				FbxSurfacePhong* pPhong	= (FbxSurfacePhong *)pFbxMtl;

				InitVec( pmatAttrib->Ambient, (float)pPhong->Ambient.Get()[0], (float)pPhong->Ambient.Get()[1], (float)pPhong->Ambient.Get()[2] );
				InitVec( pmatAttrib->Diffuse, (float)pPhong->Diffuse.Get()[0], (float)pPhong->Diffuse.Get()[1], (float)pPhong->Diffuse.Get()[2] );
				InitVec( pmatAttrib->Specular, (float)pPhong->Specular.Get()[0], (float)pPhong->Specular.Get()[1], (float)pPhong->Specular.Get()[2] );
				pmatAttrib->Shininess		= (float)pPhong->Shininess;
				pmatAttrib->RefractiveIndex	= 1.6f;// TODO: 後付けで簡単に設定できるようにしとく
				pmatAttrib->Opacity			= (float)pPhong->TransparencyFactor;
				InitVec( pmatAttrib->TransparentColor, (float)pPhong->TransparentColor.Get()[0], (float)pPhong->TransparentColor.Get()[1], (float)pPhong->TransparentColor.Get()[2] );
			}


			//=========================== テクスチャ情報を取得する ===================================//
			// ディフューズプロパティを検索
			FbxProperty property = pFbxMtl->FindProperty( FbxSurfaceMaterial::sDiffuse );

			// プロパティが持っているレイヤードテクスチャの枚数をチェック
			int layerNum = property.GetSrcObjectCount<FbxLayeredTexture>();


			// レイヤードテクスチャが無ければ通常テクスチャ
			if( layerNum==0 )
			{
				// 通常テクスチャの枚数をチェック
				int numGeneralTexture = property.GetSrcObjectCount<FbxFileTexture>();

				// 各テクスチャについてテクスチャ情報をゲット
				for( int i=0; i<numGeneralTexture; ++i )
				{
					// i番目のテクスチャオブジェクト取得
					FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>(i);

					// テクスチャファイルパスを取得（フルパス）
					const char* fileName = texture->GetFileName();//GetFileName();

					//size_t size = strlen( fileName );
					//memcpy( outTexName, fileName, size );
					//outTexName[ size ] = '\0';
					//				FBXファイルに埋め込まれたテクスチャデータの取得方法が分からない

					break; // とりあえず今は1枚だけサポート
				}
			}

		}// end of midx


		//============================= マテリアル毎にポリゴンのリストを作成する =============================//
		int	numLayers	= pFbxMesh->GetLayerCount();

		for( int pidx=0; pidx<pFbxMesh->GetPolygonCount(); ++pidx )
		{
			for( int l=0; l<numLayers; ++l )
			{
				FbxLayerElementMaterial* mat = pFbxMesh->GetLayer(l)->GetMaterials();

				if( mat )
				{
					int midx = mat->GetIndexArray().GetAt(pidx);// ポリゴンの使用マテリアルIDを取得する
					//FbxSurfaceMaterial *material = pFbxNode->GetMaterial( midx );

					//tcout << "Polygin Index : " << pidx << "  material indx: " << /*pFbxNode->GetMaterialIndex(material->GetName())*/midx << endl;

					matSubset[midx].geomSize.x++;	// ポリゴン数をインクリメント
					matSubset[midx].geomSize.y	+= pFbxMesh->GetPolygonSize(pidx);// 頂点数を加算
					matSubset[midx].polygon_lists.push_back( pidx );

					break;
				}// end of if mat

			}// end of layer loop
		}
	}



	void FbxLoader::ConstructVertexBuffer(	vector<VertexAttribs>& vertexAttribs,
		vector<FaceAttribIdx>& faceAttribIdx,
		vector<UVSet>& uvSet, vector<MaterialSubset>& matSubset,
		VertexBuffer *vertexBuffer, IndexBuffer *indexBuffer, MaterialBuffer *matBuffer,
		FaceGroupBuffer *pFaceGroupBuffer )
	{

		//====================== 頂点アトリビュートの重複を消して、ユニークなIDを付与する =================//
		int attrUniqueID = 0; 
		for( int vbidx=0; vbidx<vertexAttribs.size(); ++vbidx )
		{
			VertexAttribs *pVertexAttrib = &vertexAttribs[vbidx];
			pVertexAttrib->numUniqueAttribs = (int)pVertexAttrib->normal_list.size();

			//============== 法線/UVの組み合わせの重複を検出して無効化する ==================//
			pVertexAttrib->numUniqueAttribs = 0;
			vector<Vec3f>	tempNormals;
			vector<Vec2f>	tempUvs;
			const Vec3f dummy_normal = { 0.0f, 0.0f, 0.0f };
			Vec2f		dummy_uv = { 0.0f, 0.0f };

			int unique_id = 0;
			for( int i=0; i<pVertexAttrib->normal_list.size(); ++i )
			{
				Vec3f *query_normal	= &pVertexAttrib->normal_list[i];
				Vec2f *query_uv			= pVertexAttrib->uv_list.empty() ? &dummy_uv : &pVertexAttrib->uv_list[i];

				bool bExists = false;
				int	j;
				for( j=0; j<i; ++j)
				{
					const Vec3f *curr_normal = &pVertexAttrib->normal_list[j];
					const Vec2f *curr_uv = pVertexAttrib->uv_list.empty() ? &dummy_uv : &pVertexAttrib->uv_list[j];

					if(IsSame( *query_normal, *curr_normal ) && IsSame( *query_uv, *curr_uv ))
					{
						bExists = true;
						break;
					}
				}


				if( bExists )// j番目(既に登録済み)と同じアトリビュート
				{
					pVertexAttrib->remapIdx[i] = pVertexAttrib->remapIdx[j];// アドレスは既存のものjを使う
					// アトリビュートの登録はしない
				}
				else
				{
					pVertexAttrib->remapIdx[i] = unique_id++;// アトリビュートにユニークなIDを割り当てる(割り当てた後でインクリメントする)
					tempNormals.push_back( *query_normal );// アトリビュートを追加する
					tempUvs.push_back( *query_uv );// アトリビュートを追加する
				}

			}// end of i loop

			pVertexAttrib->numUniqueAttribs = unique_id;

			// 重複を消して新しく組みなおしたアトリビュートデータを代入する
			pVertexAttrib->normal_list.resize( tempNormals.size() );
			pVertexAttrib->uv_list.resize( tempUvs.size() );

			for( int i=0; i<tempNormals.size(); ++i )
			{
				pVertexAttrib->normal_list[i]	= tempNormals[i];
				pVertexAttrib->uv_list[i]		= tempUvs[i];
			}// end of i loop


			//====== 重複を除去した法線/UV組み合わせデータ群にユニークなIDを付与する ======//
			pVertexAttrib->ID.resize( pVertexAttrib->numUniqueAttribs );
			for( int i=0; i<pVertexAttrib->numUniqueAttribs; ++i )
			{
				pVertexAttrib->ID[i]	= attrUniqueID++;
			}
		}

		//================================ 頂点バッファを作成する ============================//
		AllocateVertexBuffer( vertexBuffer, attrUniqueID );

		int curr=0;
		for( int i=0; i<vertexAttribs.size(); ++i )
		{
			VertexAttribs *pAttribs = &vertexAttribs[i];
			for( int j=0; j<pAttribs->numUniqueAttribs; ++j )
			{
				*vertexBuffer->Position(curr)	= pAttribs->pos;
				*vertexBuffer->Normal(curr)		= pAttribs->normal_list[j];
				*vertexBuffer->TexCoord(curr)	= pAttribs->uv_list[j];
				InitVec( *vertexBuffer->Color(curr), 0.5f, 0.5f, 0.5f );

				curr++;
			}// end of j loop
		}// end of i loop

#ifdef _DEBUG
		//	for( int i=0; i<vertexBuffer->numElms; ++i )
		//	{
		//tcout << i << ":" << endl;
		//tcout << "Pos: " << vertexBuffer->Position(i)->x << ", " << vertexBuffer->Position(i)->y << ", " << vertexBuffer->Position(i)->z << endl;
		//tcout << "Normal: " << vertexBuffer->Normal(i)->x << ", " << vertexBuffer->Normal(i)->y << ", " << vertexBuffer->Normal(i)->z << endl;
		//tcout << "UV: " << vertexBuffer->TexCoord(i)->x << ", " << vertexBuffer->TexCoord(i)->y << endl;
		//	}
#endif
		//======================= インデックスバッファを作成する ========================//

		// FbxMeshから、法線/UV毎に再構築した頂点インデックスの数を集計する
		int	numIndices = 0;

		for( int midx=0; midx<matSubset.size(); ++midx )
		{
			MaterialSubset *pMatSubset = &matSubset[midx];
			int numPolygons = (int)pMatSubset->polygon_lists.size();

			for( int pidx=0; pidx<numPolygons; ++pidx )
			{
				numIndices += faceAttribIdx[ pMatSubset->polygon_lists[pidx] ].num_verts;
			}// end of pidx loop
		}// end of midx loop


#ifdef _DEBUG
		tcout << _T("numIndices:") << numIndices << endl;
#endif
		// インデックスバッファを確保する
		AllocateIndexBuffer( indexBuffer, numIndices, PRIM_TRIANGLE );
		int indexcount=0;

		// フェースグループバッファを確保する
		AllocateFaceGroupBuffer( pFaceGroupBuffer, matSubset.size() );

		// インデックスバッファにフェース頂点群のインデックス値を入れる.
		for( int midx=0; midx<matSubset.size(); ++midx )
		{
			MaterialSubset *pMatSubset = &matSubset[midx];
			int numPolygons = (int)pMatSubset->polygon_lists.size();

Vec2ui *pFaceGroup = pFaceGroupBuffer->FaceGroup( midx );
InitVec( *pFaceGroup, (uint32)indexcount, (uint32)0 );

			for( int pidx=0; pidx<numPolygons; ++pidx )
			{
				FaceAttribIdx *pFaceAttribIdx = &faceAttribIdx[ pMatSubset->polygon_lists[pidx] ];

				for( int vidx=0; vidx<pFaceAttribIdx->num_verts; ++vidx )
				{
					Vec2i vbidx = pFaceAttribIdx->vbAddress[vidx];
					int trueidx = vertexAttribs[vbidx.x].remapIdx[vbidx.y];// 直接アクセスしないでアドレスリマップが必要
					*indexBuffer->Index( indexcount++ ) = vertexAttribs[vbidx.x].ID[trueidx];
				}// end of vidx loop

			}// end of pidx loop

pFaceGroup->y = indexcount - pFaceGroup->x;

		}// end of midx loop

		
		//======================= マテリアルバッファを作成する ========================//
		AllocateMaterialBuffer( matBuffer, matSubset.size(), MATERIAL_TYPE_OPAQUE );

		for( int midx=0; midx<matSubset.size(); ++midx )
		{
			MaterialAttribute *pmatAttrib		= &matSubset[midx].matAttrib;

			*matBuffer->Ambient(midx)			= pmatAttrib->Ambient;
			*matBuffer->Diffuse(midx)			= pmatAttrib->Diffuse;
			*matBuffer->Specular(midx)			= pmatAttrib->Specular;
			*matBuffer->Shininess(midx)			= pmatAttrib->Shininess;
			*matBuffer->RefractiveIndex(midx)	= pmatAttrib->RefractiveIndex;
			*matBuffer->Opacity( midx )			= pmatAttrib->Opacity;

		}// end of midx loop

	}



	void FbxLoader::GetMesh( FbxNode* pfbxNode, SceneGraph *pSceneGraph, SceneNode *pSceneNode )
	{
		SceneNode *pCurrRoot	= pSceneNode;
		FbxMesh* pFbxMesh		= pfbxNode->GetMesh();

		if( pFbxMesh )
		{
			bool bInstance	= false;
			MeshObject *pSrc = NULL;
			for( int i=0; i<m_MeshNodeList.size(); ++i )
			{
				if( (uint32)pFbxMesh==m_MeshNodeList[i].fbxAddress )
				{
#ifdef _DEBUG
tcout << _T("Mesh Already exists...") << endl;
#endif // _DEBUG
					bInstance = true;//pMesh = (FbxMesh*)m_MeshNodeList[i].fbxAddress;
					pSrc	= (MeshObject *)m_MeshNodeList[i].objAddress;
					break;
				}
			}


			vector<VertexAttribs>	vertexAttribBuffer;
			vector<FaceAttribIdx>	faceAttribIdx;
			vector<UVSet>			uvSet;
			vector<MaterialSubset>	matSubset;

			Vec3f pos, scale;
			Quatf rot;

			InitZero( pos );
			InitVec( scale, 1.0f, 1.0f, 1.0f );
			QuatIdentity( rot );


			// SceneGraphに子ノードを追加する

			if( bInstance )
			{
				MeshAttribute meshAttrib ={ PRIM_TRIANGLE, SHADER_SIMPLEMESH, INSTANCE_ON };
				GetTransform( pfbxNode, pos, scale, rot );
				GetMaterial( pFbxMesh, matSubset );

				pCurrRoot	= pSceneGraph->AddMeshInstanceNode( pCurrRoot, pSrc, &meshAttrib, NULL, NULL, pos, scale, rot );
			}
			else
			{
				pCurrRoot	= pSceneGraph->AddMeshNode( pCurrRoot, NULL, NULL, NULL, NULL, NULL, NULL, pos, scale, rot );

				Vec3f pos, scale;
				Quatf rot;
				VertexBuffer *pVertexBuffer			= ((MeshObject *)pCurrRoot->GetAttachedObject())->m_pVertexBuffer;
				IndexBuffer *pIndexBuffer			= ((MeshObject *)pCurrRoot->GetAttachedObject())->m_pIndexBuffer;
				FaceGroupBuffer *pFaceGroupBuffer	= ((MeshObject *)pCurrRoot->GetAttachedObject() )->m_pFaceGroupBuffer;
				MaterialBuffer *pMtls				= (MaterialBuffer *)((MeshObject *)pCurrRoot->GetAttachedObject())->m_pMaterial;//new MaterialBuffer();

				AABB *pLocaSpaceBoundingBox	= &((MeshObject *)pCurrRoot->GetAttachedObject())->m_ObjectSpaceBoundingBox;


				// 頂点バッファ構築に必要なデータ生成する
				GetTransform( pfbxNode, pos, scale, rot );
				GetPosition( pFbxMesh, vertexAttribBuffer, faceAttribIdx, pLocaSpaceBoundingBox );	// 頂点座標の取得. ついでにバウンディングボックスも計算
				GetNormal( pFbxMesh, vertexAttribBuffer, faceAttribIdx );					// ポリゴン法線の取得
				GetVertexUV( pFbxMesh, uvSet, vertexAttribBuffer );						// ポリゴン頂点UVの取得
				GetMaterial( pFbxMesh, matSubset );										// ポリゴンマテリアルの取得

				// 頂点バッファを構築する
				ConstructVertexBuffer( vertexAttribBuffer, faceAttribIdx, uvSet, matSubset, pVertexBuffer, pIndexBuffer, pMtls, pFaceGroupBuffer );// 頂点バッファ,インデックスバッファ作成

				// 親ノードからのトランスフォームを継承する
				pCurrRoot->GetAttachedObject()->SetTransformAttribute( pos, scale, rot );
				//pCurrRoot->InheritTransform();
			}



			MeshNodeInfo newInfo = { (uint32)pFbxMesh, (uint32)pCurrRoot->GetAttachedObject() };
			m_MeshNodeList.push_back( newInfo );

#ifdef _DEBUG
			tcout << _T("fbxaddress...") << newInfo.fbxAddress << endl;
#endif // _DEBUG

		}


		//====================== 子ノードを探索する ====================//
		for( int i = 0; i < pfbxNode->GetChildCount(); i++ )
		{
			GetMesh( pfbxNode->GetChild(i), pSceneGraph, pCurrRoot );
		}// end of i loop
	}



	void FbxLoader::ScanGeometry( SceneGraph *pSceneGraph )
	{
		m_MeshNodeList.clear();

		GetMesh( m_pFbxScene->GetRootNode(), pSceneGraph, pSceneGraph->GetCurrentMeshNode() );

	}



}// end of namespace
