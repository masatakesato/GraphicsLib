#include "OBJLoader.h"

#include	<oreore/io/FileIO.h>



const size_t BUFFER_LENGTH	= 1024;
const size_t NAME_LENGTH	= 256;
const int32 INVALID = -1;


OBJLoader::OBJLoader()
{
	InitVec(m_BoundingBox[0], 0.0f, 0.0f, 0.0f);
	InitVec(m_BoundingBox[1], 0.0f, 0.0f, 0.0f);

	m_Vertices.Release();
	m_Normals.Release();
	m_Materials.Release();
//	m_MatSubs.Release();
	m_Faces.Release();
	m_Groups.Release();
}



OBJLoader::~OBJLoader()
{

}



bool OBJLoader::Load( const tstring& filename )
{
	tstring FileString = OreOreLib::textFileRead( filename );

	if(FileString.empty())
		return false;

	AllocateMemory(FileString);
	ParseObjFile(FileString);

	ConstructVertexAttributes();

	return true;
}



void OBJLoader::LoadPositions( void* pBuffer, size_t offset, size_t stride )
{
	int curr = 0;
	const Vec3f	dumy3 = {-1, -1, -1};

	for( int i=0; i<VertAttribs.Length(); ++i )
	{
		for( int j=0; j<VertAttribs[i].Length(); ++j )
		{
			*reinterpret_cast<Vec3f*>( reinterpret_cast<uint8*>(pBuffer) + offset + curr * stride ) = i>=0 ? m_Vertices[i] : dumy3;// -1だったら弾く
			curr++;
		}
	}
}



void OBJLoader::LoadNormals( void* pBuffer, size_t offset, size_t stride )
{
	int curr = 0;
	const Vec3f	dumy3 = {-1, -1, -1};

	for( int i=0; i<VertAttribs.Length(); ++i )
	{
		for( int j=0; j<VertAttribs[i].Length(); ++j )
		{
			int normalIdx = VertAttribs[i][j].y;	// 法線ベクトル属性へのインデックス
			*reinterpret_cast<Vec3f*>( reinterpret_cast<uint8*>(pBuffer) + offset + curr * stride ) = normalIdx>=0 ? m_Normals[ normalIdx ] : dumy3;// -1だったら弾く
			curr++;
		}
	}
}



void OBJLoader::LoadTexCoords( void* pBuffer, size_t offset, size_t stride )
{
	int curr = 0;
	const Vec2f	dumy2 = {-1, -1};

	for( int i=0; i<VertAttribs.Length(); ++i )
	{
		for( int j=0; j<VertAttribs[i].Length(); ++j )
		{
			int texIdx = VertAttribs[i][j].x;
			*reinterpret_cast<Vec2f*>( reinterpret_cast<uint8*>(pBuffer) + offset + curr * stride ) = texIdx>=0 ? m_TexCoord[texIdx] : dumy2;// -1だったら弾く
			curr++;
		}
	}

}



void OBJLoader::LoadIndices( void* pBuffer, size_t offset, size_t stride )
{
	int curr = 0;

	for( int i=0; i<FaceAttribs.Length(); ++i )
	{
		// 頂点ごとの属性をVertAttribsに追加登録する
		for( int j=1; j<FaceAttribs[i].Length()-1; ++j )
		{
			Vec2i ID_VertAttrib;

			ID_VertAttrib = FaceAttribs[i][0];
			*reinterpret_cast<uint32*>( reinterpret_cast<uint8*>(pBuffer) + offset + curr * stride ) = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(0)
			curr++;

			ID_VertAttrib = FaceAttribs[i][j];
			*reinterpret_cast<uint32*>( reinterpret_cast<uint8*>(pBuffer) + offset + curr * stride ) = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j)
			curr++;

			ID_VertAttrib = FaceAttribs[i][j+1];
			*reinterpret_cast<uint32*>( reinterpret_cast<uint8*>(pBuffer) + offset + curr * stride ) = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j+1)
			curr++;

		}// end of j loop
	}// end of i loop
}







void OBJLoader::Information()
{
	tcout << _T("//====================== OBJLoader Info... ====================//\n");
	//tcout << "* OBJファイル名：" << objFileName << tendl;
	//tcout << "* MTLファイル名：" << mtlFileName << tendl;
	tcout << _T("#vertices: ") << m_Vertices.Length() << tendl;
	tcout << _T("#normals: ") << m_Normals.Length() << tendl;
	tcout << _T("#faces: ") << m_Faces.Length() << tendl;
	tcout << _T("#materias: ") << m_Materials.Length() << tendl;
	tcout << _T("BoundingBox_Min: ") << m_BoundingBox[0].x << _T(", ") << m_BoundingBox[0].y << _T(", ") << m_BoundingBox[0].z << tendl;
	tcout << _T("BoundingBox_Max: ") << m_BoundingBox[1].x << _T(", ") << m_BoundingBox[1].y << _T(", ") << m_BoundingBox[1].z << tendl;
	tcout << tendl;

	/*
	cout << "//====================== マテリアル ====================//" << endl;
	for(int i=0; i<(int)m_Materials.Length(); i++)
	{
		const Vec4f &Ka = *m_Materials[i].GetAmbient();
		const Vec4f &Kd = *m_Materials[i].GetDiffuse();
		const Vec4f &Ks = *m_Materials[i].GetSpecular();
		const float Ns = m_Materials[i].GetSpecularIntensity();

		cout << "MaterialName: " << m_Materials[i].GetName() << endl;
		cout << " Ka = (" << Ka.x << ", " << Ka.y << ", " << Ka.z << ", " << Ka.w << ")" << endl;
		cout << " Kd = (" << Kd.x << ", " << Kd.y << ", " << Kd.z << ", " << Kd.w << ")" << endl;
		cout << " Ks = (" << Ks.x << ", " << Ks.y << ", " << Ks.z << ", " << Ks.w << ")" << endl;
		cout << " Ns = " << Ns << endl;
 	}
	*/
	/*
	cout << "//================ 名前付きグループ ====================//" << endl;
	for(int i=0; i<m_Groups.Length(); i++)
	{
		cout << "GroupName: " << m_Groups[i].name << endl;
		cout << " Faces:     " << m_Groups[i].face_start << "-" << m_Groups[i].face_end << endl;
	}
	*/
}



void OBJLoader::GetGroupInfo( int idx )
{
	tcout << _T("Num of Groups = ") << m_Groups.Length() << tendl;

	if( idx >= m_Groups.Length() ) return;

	//=================== グループの基本情報 =================//
	int numFaces = m_Groups[idx].face_end - m_Groups[idx].face_start + 1;

	tcout << _T(" GroupName: ") << m_Groups[idx].name << tendl;
	tcout << _T(" Face: ") << m_Groups[idx].face_start << " - " << m_Groups[idx].face_end << tendl;
	tcout << _T(" numFaces: ") << numFaces << tendl;

	//======== 同じマテリアルのサブグループを見つける ========//
	int currMtl = -1;//m_Faces[ m_Groups[idx].face_start ].matsub_index;

	for( int i= m_Groups[idx].face_start; i<=m_Groups[idx].face_end; ++i )
	{
		if( m_Faces[i].matsub_index != currMtl )
		{
			currMtl	= m_Faces[i].matsub_index;

			tcout << _T(" MaterialSubset Found : ") << currMtl << tendl;

			Material *pMtl	= &m_Materials[ currMtl ];
			pMtl->Info();

			tcout << _T(" Vertex Indices...\n");
			for(int k=0; k<m_Faces[i].VertexAttribIndex.Length(); ++k)
				tcout << m_Faces[i].VertexAttribIndex[k].x;
			tcout << tendl;
		}
	}
}





///////////////////////// private functions ////////////////////////////




bool OBJLoader::AllocateMemory( /*const string &aaa*/const tstring& aaa )
{
	
	int num_v = 0, num_vt = 0, num_vn = 0, num_g = 0, num_f = 0;

	tstring	one_line;			// 1行分の文字列
	size_t	idx_beginline = 0;		// 1行の先頭文字の位置
	size_t	idx_endline = 0;		// 1行の終端文字の位置(改行)
	size_t	str_length = aaa.length() - 1;// 文字列長(終端文字'\0'は除く)


	//============== 頂点，テクスチャ座標，法線の数を調べる ===============//

	while(idx_beginline < str_length)
	{
		// 改行記号の位置（終端位置）を取得
		idx_endline = aaa.find( _T("\n"), idx_beginline );
		if( idx_endline == tstring::npos )
			idx_endline = aaa.find( _T("\r"), idx_beginline );
		
		// 1行分の文字列を取得
		one_line = aaa.substr(idx_beginline, idx_endline-idx_beginline);

		
		if( one_line.find( _T("#") ) != tstring::npos )
		{
		
		}
		// 頂点数
		else if( one_line.find( _T("v ") ) != tstring::npos )
		{
			num_v++;
		}
		// テクスチャ座標を読む
		else if( one_line.find( _T("vt ") ) != tstring::npos )
		{
			num_vt++;
		}
		// 法線ベクトルを読む
		else if( one_line.find( _T("vn ") ) != tstring::npos )
		{
			num_vn++;
		}
		else if( one_line.find( _T("g ") ) != tstring::npos )
		{
			num_g++;
		}
		else if( one_line.find( _T("f ") ) != tstring::npos )
		{		
			num_f++;
		}

		// 次の行の先頭位置を設定(改行位置の1文字後)
		idx_beginline = idx_endline + 1;

		//tcout << one_line << tendl;

	}// end of while
	if( num_v > 0 )		m_Vertices.Reserve( num_v );
	if( num_vt > 0 )	m_TexCoord.Reserve( num_vt );
	if( num_vn > 0 )	m_Normals.Reserve( num_vn );
	if( num_g > 0 )		m_Groups.Reserve( num_g );
	if( num_f > 0 )		m_Faces.Reserve( num_f );
	
	//tcout << _T( "#vertices: " ) << num_v << tendl;
	//tcout << _T( "#texcoords: " ) << num_vt << tendl;
	//tcout << _T( "#normals: " ) << num_vn << tendl;
	//tcout << _T( "#groups: " ) << num_g << tendl;
	//tcout << _T( "#faces: " ) << num_f << tendl;

	return true;
}



bool OBJLoader::ParseObjFile( const tstring& aaa )
{
	size_t		pivot;
	int			CurrMatSub;
	int			CurrGroup;
	int			CurrFace;
	tstring		one_line;			// 1行分の文字列
	size_t		idx_beginline = 0;		// 1行の先頭文字の位置
	size_t		idx_endline = 0;		// 1行の終端文字の位置(改行)
	size_t		str_length = aaa.length() - 1;// 文字列長(終端文字'\0'は除く)

	TCHAR		tmp_char[ NAME_LENGTH ];

	ObjNamedGroup	tmp_group = { _T("default"), 0, 0 };	// 名前付きグループ

	//====================== 構造体を初期化する ====================//
	CurrMatSub	= INVALID;
	CurrGroup	= INVALID;
	CurrFace	= INVALID;
	

	// デフォルトのマテリアルを作成してm_Materialsに登録しておく
	int tmp = 0;
	m_Materials.Resize(1);
	m_Materials[0].SetName( _T("default_material") );			// strcpy(m_Materials[0].name, "default_material");
	m_Materials[0].SetAmbient(0.2f, 0.2f, 0.2f, 0.0f);	// InitVec(m_Materials[0].Ka, 0.2, 0.2, 0.2, 0.0);
	m_Materials[0].SetDiffuse(0.2f, 0.2f, 0.2f, 0.0f);	// InitVec(m_Materials[0].Kd, 0.2, 0.2, 0.2, 0.0);
	m_Materials[0].SetSpecular(0.2f, 0.2f, 0.2f, 0.0f);// InitVec(m_Materials[0].Ks, 0.2, 0.2, 0.2, 0.0);
	m_Materials[0].SetSpecularIntensity(1.0f);//m_Materials[0].Ns = 0.0;
	


//cout << "scan file..." << endl;

	//==== ファイルを一行ずつ読みながら先頭文字列に応じた処理を行う ====//
	while (idx_beginline < str_length)
	{
		// 改行記号の位置（終端位置）を取得
		idx_endline = aaa.find( _T("\n"), idx_beginline );
		if( idx_endline == tstring::npos ) idx_endline = aaa.find( _T("\r"), idx_beginline );
		
		// 1行分の文字列を取得
		one_line = aaa.substr( idx_beginline, idx_endline-idx_beginline );

//char str[1024];
//sprintf(str, "%s\n", one_line.c_str() );
//OutputDebugString(str);


		// #で始まる行は無視する
		if( (pivot=one_line.find( _T("#") )) != tstring::npos )
		{
			//cout << "ignoring row: " << one_line << endl;
		}
		// mtlファイルを読み込む
		else if( (pivot=one_line.find( _T("mtllib ") )) != tstring::npos )
		{
			// ファイル名を抽出する
			tmp_char[0] = '\0';
			//sscanf( one_line.substr(pivot).c_str(), "mtllib %s", &tmp_char);
			tsscanf( one_line.substr(pivot).c_str(), _T("mtllib %s"), &tmp_char );

			// MTLのファイルからマテリアル情報を読み込む
			Load_OBJMTL( tmp_char );
		}
		// 頂点座標を読む
		else if( (pivot=one_line.find( _T("v ") )) != tstring::npos )
		{
			AddVertex( one_line.substr(pivot+2) );
		}
		// テクスチャ座標を読む
		else if( (pivot=one_line.find( _T("vt ") )) != tstring::npos )
		{
			AddTexCoord( one_line.substr(pivot+2) );
		}
		// 法線ベクトルを読む
		else if( (pivot=one_line.find( _T("vn ") )) != tstring::npos )
		{
			AddNormal( one_line.substr(pivot+2) );
		}
		// 面情報を読む
		else if( (pivot=one_line.find( _T("f ") )) != tstring::npos )
		{
			// 名前付きグループが一つも存在しないときは，デフォルトのグループを新規作成する
			if(CurrGroup==INVALID)
				CurrGroup = AddNamedGroup( _T("g default_group"), CurrFace+1 );

			// マテリアルサブセットがないときは，デフォルトマテリアルを作る→直前のマテリアルをコピーする
			if(CurrMatSub==INVALID)
			{/*
				CurrMatSub = m_MatSubs.Length();
				m_MatSubs.AddToTail(m_MatSubs[CurrMatSub-1]);
				m_MatSubs[CurrMatSub].face_start = CurrFace+1;
				m_MatSubs[CurrMatSub].face_end = CurrFace+1;
				*/
//CurrMatSub = AddMaterialSubset( "usemtl default_material", CurrFace+1, m_Groups[CurrGroup].matSubsets );
CurrMatSub = AddMaterialSubset( _T("usemtl default_material"), CurrFace+1, m_MatSubs );
			}
			
			// ObjFace構造体を追加登録する
			CurrFace = AddFace( one_line.substr(pivot+2), CurrMatSub );

			// 名前付きグループのスコープを更新する
			m_Groups[CurrGroup].face_end = CurrFace;

			// マテリアルサブセットのスコープを更新する
//m_Groups[CurrGroup].matSubsets[CurrMatSub].face_end = CurrFace;
m_MatSubs[CurrMatSub].face_end = CurrFace;

		}
		// マテリアル情報を読む
		else if( (pivot=one_line.find( _T("usemtl ") )) != tstring::npos )
		{
//CurrMatSub = AddMaterialSubset(one_line, CurrFace+1, m_Groups[CurrGroup].matSubsets );
CurrMatSub = AddMaterialSubset(one_line, CurrFace+1, m_MatSubs );
		}
		// 名前付きグループを読む
		else if( (pivot=one_line.find( _T("g ") )) != tstring::npos )
		{
			// 名前つきグループを新規登録する
			CurrGroup = AddNamedGroup( one_line, CurrFace+1 );
			
			// 現在有効なマテリアルサブセットIDを無効にする（グループを跨いでマテリアルサブセットの設定を継承しないようにする.正しい？）
			CurrMatSub = INVALID;
		}
		else{}

		// 次の行の先頭位置を設定(改行位置の1文字後)
		idx_beginline = idx_endline + 1;
		
	}

//cout << "end of scan..." << endl;

	//======================= BoundingBoxを求める =====================//
	m_BoundingBox[0] = m_BoundingBox[1] = m_Vertices[0];

	for(int i=1; i< m_Vertices.Length(); i++)
	{
		// 最小点
		m_BoundingBox[0].x = m_Vertices[i].x < m_BoundingBox[0].x ? m_Vertices[i].x : m_BoundingBox[0].x;
		m_BoundingBox[0].y = m_Vertices[i].y < m_BoundingBox[0].y ? m_Vertices[i].y : m_BoundingBox[0].y;
		m_BoundingBox[0].z = m_Vertices[i].z < m_BoundingBox[0].z ? m_Vertices[i].z : m_BoundingBox[0].z;
		// 最大点
		m_BoundingBox[1].x = m_Vertices[i].x > m_BoundingBox[1].x ? m_Vertices[i].x : m_BoundingBox[1].x;
		m_BoundingBox[1].y = m_Vertices[i].y > m_BoundingBox[1].y ? m_Vertices[i].y : m_BoundingBox[1].y;
		m_BoundingBox[1].z = m_Vertices[i].z > m_BoundingBox[1].z ? m_Vertices[i].z : m_BoundingBox[1].z;
	}


	return true;
}



bool OBJLoader::Load_OBJMTL( const tstring& filename )
{
	size_t pivot;
	tifstream file;
	int CurrentMaterialID = -1;
	TCHAR buf[ BUFFER_LENGTH ];
	float tmp_float=0.0f;
	tstring one_line;


	//============= MTLファイルを読んでマテリアルを追加する =============//
	file.open( filename/*, std::ios::in*/ );
	if ( !file.is_open() )
	{
		tcout << _T("Error : Cloud not open MTL file... ") << filename << tendl;

		file.close();
		return false;
	}

	//　1行読み取り
	file.getline(buf, sizeof(buf));
	one_line = tstring(buf);

	//　ファイルの末端までループ
	while ( !file.eof() )
	{
		if( (pivot=one_line.find( _T("newmtl ") )) != tstring::npos )// マテリアル定義の取得
		{
			// マテリアルを初期化する
			Material newMat;
			TCHAR name[256];
			tsscanf( one_line.substr(pivot).c_str(), _T("newmtl %s"), name );
			newMat.SetName( name );
			newMat.SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);// InitVec(newMat.Ka, 0.0f, 0.0f, 0.0f, 0.0f);
			newMat.SetDiffuse(0.0f, 0.0f, 0.0f, 0.0f);// InitVec(newMat.Kd, 0.0f, 0.0f, 0.0f, 0.0f);
			newMat.SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);// InitVec(newMat.Ks, 0.0f, 0.0f, 0.0f, 0.0f);
			newMat.SetSpecularIntensity(0.0f);//newMat.Ns = 0.0;

			// マテリアルの中身を埋める→別関数
			ExtractMaterialComponent( newMat, file, one_line );
			
			// m_Materialに追加する
			m_Materials.AddToTail(newMat);//AddMaterial(buf, CurrentMaterialID);
		}
		else
		{
			file.getline(buf, sizeof(buf));
			one_line = tstring(buf);
		}
	}

	//　ファイルを閉じる
	file.close();

	return true;
}



// ファイルをスキャンし，マテリアル1個分の属性を読み取る
bool OBJLoader::ExtractMaterialComponent( Material& mat, tifstream& file, tstring& one_line )
{
	bool flag = true;
	/*char*/TCHAR buf[ BUFFER_LENGTH ];
	OreOreLib::Array<tstring> elements;
	
	size_t pivot;

	while( !file.eof() ) // ファイルの最後尾か，次の行のnewmtlに到達するまで，，，
	{
		// 1行読み取る
		file.getline(buf, sizeof(buf));
		one_line = tstring( buf );

		if( (pivot=one_line.find( _T("newmtl ") )) != tstring::npos ) break;// 次のマテリアル定義まで到達したら終了

		//=========== 先頭文字列の種類に応じて処理を実施 =============//
		// #で始まる行は無視
		if( (pivot=one_line.find( _T("#") )) != tstring::npos )
		{
			continue;
		}
		// Ambient texture
		else if( (pivot=one_line.find( _T("map_Ka ") )) != tstring::npos )
		{

		}
		// Diffuse texture
		else if( (pivot=one_line.find( _T("map_Kd ") )) != tstring::npos )
		{

		}
		// Bump map
		else if( (pivot=one_line.find( _T("map_bump ") )) != tstring::npos || (pivot=one_line.find( _T("bump ") )) != tstring::npos )
		{

		}
		// Alpha texture
		else if( (pivot=one_line.find( _T("map_d ") )) != tstring::npos )
		{

		}
		// Ka (Ambient)
		else if( (pivot=one_line.find( _T("Ka ") )) != tstring::npos )
		{
			Vec3f elements = ExtractVector3D( one_line.substr(pivot+2), _T(" ") );
			mat.SetAmbient(elements.x, elements.y, elements.z, 0.0f);
		}
		// Kd (Diffuse)
		else if( (pivot=one_line.find( _T("Kd ") )) != tstring::npos )
		{
			Vec3f elements = ExtractVector3D( one_line.substr(pivot+2), _T(" ") );
			mat.SetDiffuse(elements.x, elements.y, elements.z, 0.0f);
		}
		// Ks (Specular)
		else if( (pivot=one_line.find( _T("Ks ") )) != tstring::npos )
		{
			Vec3f elements = ExtractVector3D( one_line.substr(pivot+2), _T(" ") );
			mat.SetSpecular(elements.x, elements.y, elements.z, 0.0f);
		}
		// d
		else if( (pivot=one_line.find( _T("d ") )) != tstring::npos )
		{
			elements = OreOreLib::splitString( one_line.substr(pivot+2), _T(" ") );
			float tmp_float = (float)tatof( elements[0].c_str() );
			mat.SetAlpha(tmp_float);
		}
		//　Ns
		else if( (pivot=one_line.find( _T("Ns ") )) != tstring::npos )
		{
			elements = OreOreLib::splitString(one_line.substr(pivot+2), _T(" ") );
			mat.SetSpecularIntensity( (float)tatof( elements[0].c_str() ) );
		}
		else
		{

		}
	}

	return true;
}



Vec3f OBJLoader::ExtractVector3D( const tstring& str, const tstring& delim )
{
	int curr = 0;
	Vec3f vec3f = {0.0f, 0.0f, 0.0f};
	tstring vec_str = str;

    size_t cutAt;
    while( (cutAt = vec_str.find_first_of(delim)) != tstring::npos )
    {
        if(cutAt > 0)
        {
			if(curr<3)	vec3f.xyz[curr++] = (float)tatof( vec_str.substr(0, cutAt).c_str() );
        }
        vec_str = vec_str.substr(cutAt + 1);
    }
    if( vec_str.length() > 0 )
    {
		if(curr<3)	vec3f.xyz[curr++] = (float)tatof( vec_str.substr(0, cutAt).c_str() );
    }

	return vec3f;
}



void OBJLoader::AddVertex( const tstring& str )
{
	Vec3f newVertex;
	auto elements = OreOreLib::splitString( str, _T(" ") );

	if(elements.Length() < 3)
	{
		tcerr << _T("Error : Invalid Vertex.\n");
		return;
	}
	// 頂点座標を設定
	InitVec( newVertex, (float)tatof( elements[0].c_str()), (float)tatof(elements[1].c_str()), (float)tatof(elements[2].c_str()) );
			
	// 頂点座標を追加
	m_Vertices.AddToTail(newVertex);
}



void OBJLoader::AddTexCoord( const tstring& str )
{
	Vec2f newTexcoord = {0, 0};
	auto elements = OreOreLib::splitString( str, _T(" ") );

	if(elements.Empty())
	{
		tcerr << _T("Error : Invalid Texture Coordinate.\n");
		return;
	}
	
	// テクスチャ座標の格納
	for( int i=0; i<Min( elements.Length(), 3 ); ++i )
		newTexcoord.xy[i] = (float)tatof( elements[i].c_str() );

	// テクスチャ座標を追加
	m_TexCoord.AddToTail(newTexcoord);
}



void OBJLoader::AddNormal( const tstring& str )
{
	Vec3f newNormal;
	auto elements = OreOreLib::splitString( str, _T(" ") );

	if(elements.Length() < 3)
	{
		tcerr << _T("Error : Invalid Normal.\n");
		return;
	}
	
	// 法線ベクトルを設定
	InitVec( newNormal, (float)tatof( elements[0].c_str() ), (float)tatof( elements[1].c_str() ), (float)tatof( elements[2].c_str() ) );
	
	// 法線ベクトルを追加
	m_Normals.AddToTail(newNormal);
}



// 1行分の文字列strから面情報を取得し，m_Facesに追加する
int OBJLoader::AddFace( const tstring& str, int matsub_id )
{
	int i;
	ObjFace	newFace;
	Vec3i	VertexAttrib = {-1, -1, -1};

	// スペースで区切られた文字列に分割する
	auto chunks = OreOreLib::splitString( str, _T(" ") );
	
	// 面構造体のメモリを確保化する
	newFace.num_verts		= (int)chunks.Length();
	newFace.matsub_index	= matsub_id;
	newFace.VertexAttribIndex.Resize( newFace.num_verts, VertexAttrib );
	newFace.use_normal		= false;

	// 文字列chunkごとに頂点情報を格納する
	for(i=0; i<newFace.num_verts; i++)
	{
		OreOreLib::Array<tstring> elements;

		if( chunks[i].find( _T("//") ) != tstring::npos )// Vertex//normal
		{
			elements = OreOreLib::splitString( chunks[i], _T("//") );// "//"で文字列分割する

			newFace.VertexAttribIndex[i].x	= tatoi( elements[0].c_str() ) - 1;// Vertex index
			newFace.VertexAttribIndex[i].z	= tatoi( elements[1].c_str() ) - 1;// Normal index
			newFace.use_normal				= true;
		}
		else if(chunks[i].find( _T("/") ) != tstring::npos )// "/"で区切られている場合→要素2つなら法線と頂点
		{
			elements = OreOreLib::splitString( chunks[i], _T("/") );// "/"で分割する
			
			if(elements.Length()==2)// Vertex/Texture
			{
				newFace.VertexAttribIndex[i].x	= tatoi( elements[0].c_str() ) - 1;// Vertex index
				newFace.VertexAttribIndex[i].z	= tatoi( elements[1].c_str() ) - 1;// Texture index
				newFace.use_normal				= false;
			}
			else if(elements.Length()==3)// Vertex/Texture/Normal
			{
				newFace.VertexAttribIndex[i].x	= tatoi( elements[0].c_str() ) - 1;// Vertex index
				newFace.VertexAttribIndex[i].y	= tatoi( elements[1].c_str() ) - 1;// Texture index
				newFace.VertexAttribIndex[i].z	= tatoi( elements[2].c_str() ) - 1;// Normal index
				newFace.use_normal				= true;
			}
		}
		else// 区切れ目なし→見つかった数字をそのまんま数字に変換して突っ込んどけ
		{
			newFace.VertexAttribIndex[i].x	= tatoi( chunks[i].c_str() ) - 1;// Vertex index
			newFace.use_normal				= false;
		}

		//if(newFace.VertexAttribIndex[i].x==-1)
		//	cout << str.c_str() << endl;
		
	}// end of i loop

	// 面構造体を追加する
	m_Faces.AddToTail( newFace );

	return (int)m_Faces.Length()-1;
}



int OBJLoader::AddMaterialSubset( const tstring& str, int startidx, OreOreLib::Array<ObjMaterialSubset>& m_MatSubs )
{
	int i;
	int lastsubset = 0;
	TCHAR matname[ 128 ];
	ObjMaterialSubset newMatSub;

	// マテリアル名を読み取る
	tsscanf( str.c_str(), _T("usemtl %s"), &matname );

	// mtlファイルから読み込んだマテリアル群から名前が一致するものを検索
	for(i=0; i<(int)m_Materials.Length(); i++)
	{
		// 名前が一致したら,,,
		if( tstrcmpi( m_Materials[i].GetName().c_str(), matname ) == 0 )
		{
			// マテリアルサブセットを新規追加する
			newMatSub.material_index	= i;
			newMatSub.face_start		= startidx;
			newMatSub.face_end			= newMatSub.face_start;
			
			m_MatSubs.AddToTail(newMatSub);

			break;
		}
	}

	return (int)m_MatSubs.Length()-1;	
}



int OBJLoader::AddNamedGroup( const tstring& str, int startidx )
{
	TCHAR groupname[128];
	ObjNamedGroup newGroup;
	
	// グループ名を設定
	if( tsscanf( str.c_str(), _T("g %s"), &groupname) == 1 )
		tstrcpy( newGroup.name, groupname );
			
	// 頂点インデックスをセット
	newGroup.face_start = (int)m_Faces.Length();
	newGroup.face_end = (int)newGroup.face_start;
			
	// 名前付きグループを追加する
	m_Groups.AddToTail(newGroup);

	return (int)m_Groups.Length()-1;
}





void OBJLoader::ConstructVertexAttributes()
{
	//====================== 全ての面の全頂点の属性を調べ、VertAttrib配列に格納する ======================//
	VertAttribs.Resize(m_Vertices.Length());
	FaceAttribs.Resize(m_Faces.Length());

	numIndices = 0;

	for( int i=0; i<m_Faces.Length(); ++i )// 各面について、、、
	{
		for( int j=0; j<m_Faces[i].VertexAttribIndex.Length(); ++j )// 面を構成する頂点毎の属性をVertAttribsに登録する
		{
			int idx = AddVertexAttributes( m_Faces[i].VertexAttribIndex[j], VertAttribs );
			Vec2i newAttribIdx = {m_Faces[i].VertexAttribIndex[j].x, idx};// x:頂点インデックス，y:頂点のどの属性かインデックス
			FaceAttribs[i].AddToTail(newAttribIdx);
		}

		// 頂点インデックスの総数を累積する
		numIndices += (m_Faces[i].num_verts - 2) * 3;// 3角形ポリゴン数 * 頂点インデックス数3個

	}// end of i loop
	
	numVertAttrs = AssignVertexIDs(VertAttribs);// 全ての頂点の全ての属性に通し番号を振る.
}




// 頂点毎に，テクスチャ座標/法線ベクトルの属性の組み合わせ（複数）を格納する
// [入力]
//  Query: VertexAttribIndex. (x,y,z) = (頂点座標インデックス, テクスチャ座標インデックス, 法線インデックス)
//  Attribs: 頂点の属性。配列データ
// [出力]
//  Attribs配列の対応する要素インデックス
int OBJLoader::AddVertexAttributes( const Vec3i &Query, OreOreLib::Array< OreOreLib::Array<Vec3i> > &Attribs )
{
	int i;
	Vec3i newAttrib;
	OreOreLib::Array<Vec3i>& currAttribs = Attribs[ Query.x ];
	
	//============== AttribArrayに既にQueryが登録済みかどうか調べる ===============//
	for(i=0; i<currAttribs.Length(); i++)
	{
		if(Query.y==currAttribs[i].x && Query.z==currAttribs[i].y)
			return i;// Queryが登録済みの場合は処理を中止する
	}
	
	//=== Queryのテクスチャ座標インデックス/法線インデックスを新たに登録する =====//
	InitVec(newAttrib, Query.y, Query.z, -1);// 通し番号はまだ未確定なので-1
	currAttribs.AddToTail(newAttrib);

	return (int)currAttribs.Length()-1;
}



// 頂点がに通し番号（頂点インデックス）を付ける
// Attribs: 
int OBJLoader::AssignVertexIDs( OreOreLib::Array< OreOreLib::Array<Vec3i> >& Attribs )
{
	int numAttribVertices = 0;// ユニークな属性を持った頂点の総数

	for(int i=0; i<Attribs.Length(); i++)// 全ての頂点をスキャンする
	{
		for(int j=0; j<Attribs[i].Length(); j++)// 各頂点に付随する属性（複数の頂点/法線の組み合わせが考えられる）も全てスキャンする
		{
			Attribs[i][j].z = numAttribVertices++;// 個々の属性レベルで通し番号を振っていく
		}
	}

	return numAttribVertices;
}








//void OBJLoader::Draw()
//{
//
//	vector<ObjFace>::iterator	Face_Iter = m_Faces.begin();
//
//	int pre_mat = -1, cur_mat = 0;
//
//	while(Face_Iter != m_Faces.end())
//	{
//		//　マテリアルがあるとき
//		if(!m_Materials.empty())
//		{
//			//　インデックスを格納
//			cur_mat = m_MatSubs[Face_Iter->matsub_index].material_index;
//			
////cout << cur_mat << endl;
//			//　前と異なる色のとき
//			if(pre_mat != cur_mat)
//			{
//				//　Object Color
//				//glColor4fv(material[cur_mat].color);
//
//				//　Ambient Color
//				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_Materials[cur_mat].GetAmbient()->rgba);
//
//				//　Diffuse Color
//				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_Materials[cur_mat].GetDiffuse()->rgba);
//
//				//　Specular Color
//				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_Materials[cur_mat].GetSpecular()->rgba);
//
//				//　Emission
//				//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material[cur_mat].emission);
//
//				//　Shininess
//				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_Materials[cur_mat].GetSpecularIntensity());
//
//				//　更新
//				pre_mat = cur_mat;
//			}
//			
//		}
//		
//		//　面の描画を開始
//		vector<Vec3i>::iterator VertAttr_Iter = Face_Iter->VertexAttribIndex.begin();
//
//		glBegin(Face_Iter->type);
//
//		while(VertAttr_Iter != Face_Iter->VertexAttribIndex.end() )
//		{
//			//　法線ベクトル
//			if(Face_Iter->use_normal==true)
//			{
//				glNormal3fv(m_Normals[VertAttr_Iter->z].xyz);
//			}
////cout << VertAttr_Iter->x << endl;
//
//			if(VertAttr_Iter->x < 0 || VertAttr_Iter->x > m_Vertices.Length()-1)
//				cout << "out of range: " << VertAttr_Iter->x << endl;
//
//			//　頂点
//			glVertex3f(	m_Vertices[VertAttr_Iter->x].x,
//						m_Vertices[VertAttr_Iter->x].y, 
//						m_Vertices[VertAttr_Iter->x].z );
//			
//			VertAttr_Iter++;
//		}
//		//　面の描画を終了
//		glEnd();
//
//		Face_Iter++;
//	}
//}
