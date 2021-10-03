#include "MeshData.h"

#include	<Windows.h>
#include	<fstream>
using namespace std;


#define BUFFER_LENGTH 1024
#define NAME_LENGTH 256

#define DEFAULT_MATERIAL "default_material"
#define INVALID	-1




// ファイルを全部読み込んでstringを返す
static string textFileRead(const char *FilePass)
{
	char		*buf, c;
	ifstream	fin;

	fin.open(FilePass);

	if(!fin.is_open())
	{
		cout << "could not open file: " << FilePass << "..." << endl;
		return string("");
	}

	int	filesize = (int)fin.seekg(0, ios::end).tellg();

	cout << "file size = " << filesize << " [bytes]" << endl;

	fin.clear();
	fin.seekg(0);
	buf = new char[filesize];
	int	curr = 0;

	while(fin.get(c))
	{
		buf[curr++] = c;
	}

	buf[curr] = '\0';

	fin.close();

	return string(buf);
}




// split関数 (http://goodjob.boy.jp/chirashinoura/id/100.html)
// @param string str 分割したい文字列
// @param string delim デリミタ
// @return list<string> 分割された文字列
static vector<string> Split(string str, string delim)
{
    vector<string> result;
    size_t cutAt;
    while( (cutAt = str.find_first_of(delim)) != str.npos )
    {
        if(cutAt > 0)
        {
            result.push_back(str.substr(0, cutAt));
        }
        str = str.substr(cutAt + 1);
    }
    if(str.length() > 0)
    {
        result.push_back(str);
    }
	return result;
}



Vec3f ExtractVector3D(string str, string delim)
{
	int curr = 0;
	Vec3f vec3f = {0.0f, 0.0f, 0.0f};

	//vector<string> result;
    size_t cutAt;
    while( (cutAt = str.find_first_of(delim)) != str.npos )
    {
        if(cutAt > 0)
        {
			if(curr<3)	vec3f.xyz[curr++] = (float)atof(str.substr(0, cutAt).c_str());
          //result.push_back(str.substr(0, cutAt));
        }
        str = str.substr(cutAt + 1);
    }
    if(str.length() > 0)
    {
		if(curr<3)	vec3f.xyz[curr++] = (float)atof(str.substr(0, cutAt).c_str());
       // result.push_back(str);
    }

	return vec3f;

}




MeshData::MeshData()
{
	InitVec(m_BoundingBox[0], 0.0f, 0.0f, 0.0f);
	InitVec(m_BoundingBox[1], 0.0f, 0.0f, 0.0f);

	m_Vertices.clear();
	m_Normals.clear();
	m_Materials.clear();
//	m_MatSubs.clear();
	m_Faces.clear();
	m_Groups.clear();
}
//あああ

bool MeshData::Load( const char *filename )
{

	string FileString = textFileRead(filename);

	if(FileString.empty())
		return false;

	AllocateMemory(FileString);
	ParseObjFile(FileString);

	return true;
}



void MeshData::Information()
{
	cout << "//====================== MeshData Info... ====================//" << endl;
	//cout << "* OBJファイル名：" << objFileName << endl;
	//cout << "* MTLファイル名：" << mtlFileName << endl;
	cout << "頂点数：" << m_Vertices.size() << endl;
	cout << "法線数：" << m_Normals.size() << endl;
	cout << "面　数：" << m_Faces.size() << endl;
	cout << "マテリアル数：" << m_Materials.size() << endl;
	cout << "BoundingBox_Min：" << m_BoundingBox[0].x << ", " << m_BoundingBox[0].y << ", " << m_BoundingBox[0].z << endl;
	cout << "BoundingBox_Max：" << m_BoundingBox[1].x << ", " << m_BoundingBox[1].y << ", " << m_BoundingBox[1].z << endl;
	cout << endl;

	/*
	cout << "//====================== マテリアル ====================//" << endl;
	for(int i=0; i<(int)m_Materials.size(); i++)
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
	for(int i=0; i<m_Groups.size(); i++)
	{
		cout << "GroupName: " << m_Groups[i].name << endl;
		cout << " Faces:     " << m_Groups[i].face_start << "-" << m_Groups[i].face_end << endl;
	}
	*/
}




///////////////////////// private functions ////////////////////////////


bool MeshData::AllocateMemory(const string &aaa)
{
	
	int num_v = 0, num_vt = 0, num_vn = 0, num_g = 0, num_f = 0;

	string	one_line;			// 1行分の文字列
	size_t	idx_beginline = 0;		// 1行の先頭文字の位置
	size_t	idx_endline = 0;		// 1行の終端文字の位置(改行)
	size_t	str_length = aaa.length() - 1;// 文字列長(終端文字'\0'は除く)


	//============== 頂点，テクスチャ座標，法線の数を調べる ===============//

	while(idx_beginline < str_length)
	{
		// 改行記号の位置（終端位置）を取得
		idx_endline = aaa.find("\n", idx_beginline);
		if(idx_endline==string::npos) idx_endline = aaa.find("\r", idx_beginline);
		
		// 1行分の文字列を取得
		one_line = aaa.substr(idx_beginline, idx_endline-idx_beginline);

		
		if( one_line.find("#") != string::npos )
		{
		
		}
		// 頂点数
		else if( one_line.find("v ") != string::npos )
		{
			num_v++;
		}
		// テクスチャ座標を読む
		else if( one_line.find("vt ") != string::npos )
		{
			num_vt++;
		}
		// 法線ベクトルを読む
		else if( one_line.find("vn ") != string::npos )
		{
			num_vn++;
		}
		else if( one_line.find("g ") != string::npos )
		{
			num_g++;
		}
		else if( one_line.find("f ") != string::npos )
		{		
			num_f++;
		}

		// 次の行の先頭位置を設定(改行位置の1文字後)
		idx_beginline = idx_endline + 1;

		//cout << one_line << endl;

	}// end of while

	if(num_v>0)		m_Vertices.reserve(num_v);
	if(num_vt>0)	m_TexCoord.reserve(num_vt);
	if(num_vn>0)	m_Normals.reserve(num_vn);
	if(num_g>0)		m_Groups.reserve(num_g);
	if(num_f>0)		m_Faces.reserve(num_f);

	//cout << "#vertices: " << num_v << endl;
	//cout << "#texcoords: " << num_vt << endl;
	//cout << "#normals: " << num_vn << endl;
	//cout << "#groups: " << num_g << endl;
	//cout << "#faces: " << num_f << endl;

	return true;
}





bool MeshData::ParseObjFile(const string &aaa)
{
	size_t			pivot;
	int			CurrMatSub;
	int			CurrGroup;
	int			CurrFace;
	string		one_line;			// 1行分の文字列
	size_t		idx_beginline = 0;		// 1行の先頭文字の位置
	size_t		idx_endline = 0;		// 1行の終端文字の位置(改行)
	size_t		str_length = aaa.length() - 1;// 文字列長(終端文字'\0'は除く)

	char		tmp_char[NAME_LENGTH];

	ObjNamedGroup	tmp_group = {"default", 0, 0};	// 名前付きグループ

	//====================== 構造体を初期化する ====================//
	CurrMatSub	= INVALID;
	CurrGroup	= INVALID;
	CurrFace	= INVALID;
	

	// デフォルトのマテリアルを作成してm_Materialsに登録しておく
	int tmp = 0;
	m_Materials.resize(1);
	m_Materials[0].SetName("default_material");			// strcpy(m_Materials[0].name, "default_material");
	m_Materials[0].SetAmbient(0.2f, 0.2f, 0.2f, 0.0f);	// InitVec(m_Materials[0].Ka, 0.2, 0.2, 0.2, 0.0);
	m_Materials[0].SetDiffuse(0.2f, 0.2f, 0.2f, 0.0f);	// InitVec(m_Materials[0].Kd, 0.2, 0.2, 0.2, 0.0);
	m_Materials[0].SetSpecular(0.2f, 0.2f, 0.2f, 0.0f);// InitVec(m_Materials[0].Ks, 0.2, 0.2, 0.2, 0.0);
	m_Materials[0].SetSpecularIntensity(1.0f);//m_Materials[0].Ns = 0.0;
	


//cout << "scan file..." << endl;

	//==== ファイルを一行ずつ読みながら先頭文字列に応じた処理を行う ====//
	while (idx_beginline < str_length)
	{
		// 改行記号の位置（終端位置）を取得
		idx_endline = aaa.find("\n", idx_beginline);
		if(idx_endline==string::npos) idx_endline = aaa.find("\r", idx_beginline);
		
		// 1行分の文字列を取得
		one_line = aaa.substr(idx_beginline, idx_endline-idx_beginline);

//char str[1024];
//sprintf(str, "%s\n", one_line.c_str() );
//OutputDebugString(str);


		// #で始まる行は無視する
		if( (pivot=one_line.find("#")) != string::npos )
		{
			//cout << "ignoring row: " << one_line << endl;
		}
		// mtlファイルを読み込む
		else if( (pivot=one_line.find("mtllib ")) != string::npos )
		{
			// ファイル名を抽出する
			tmp_char[0] = '\0';
			sscanf( one_line.substr(pivot).c_str(), "mtllib %s", &tmp_char);

			// MTLのファイルからマテリアル情報を読み込む
			Load_OBJMTL(tmp_char);
		}
		// 頂点座標を読む
		else if( (pivot=one_line.find("v ")) != string::npos )
		{
			AddVertex( one_line.substr(pivot+2) );
		}
		// テクスチャ座標を読む
		else if( (pivot=one_line.find("vt ")) != string::npos )
		{
			AddTexCoord( one_line.substr(pivot+2) );
		}
		// 法線ベクトルを読む
		else if( (pivot=one_line.find("vn ")) != string::npos )
		{
			AddNormal( one_line.substr(pivot+2) );
		}
		// 面情報を読む
		else if( (pivot=one_line.find("f ")) != string::npos )
		{
			// 名前付きグループが一つも存在しないときは，デフォルトのグループを新規作成する
			if(CurrGroup==INVALID)
				CurrGroup = AddNamedGroup("g default_group", CurrFace+1);

			// マテリアルサブセットがないときは，デフォルトマテリアルを作る→直前のマテリアルをコピーする
			if(CurrMatSub==INVALID)
			{/*
				CurrMatSub = m_MatSubs.size();
				m_MatSubs.push_back(m_MatSubs[CurrMatSub-1]);
				m_MatSubs[CurrMatSub].face_start = CurrFace+1;
				m_MatSubs[CurrMatSub].face_end = CurrFace+1;
				*/
//CurrMatSub = AddMaterialSubset( "usemtl default_material", CurrFace+1, m_Groups[CurrGroup].matSubsets );
CurrMatSub = AddMaterialSubset( "usemtl default_material", CurrFace+1, m_MatSubs );
			}
			
			// ObjFace構造体を追加登録する
			CurrFace = AddFace(one_line.substr(pivot+2), CurrMatSub);

			// 名前付きグループのスコープを更新する
			m_Groups[CurrGroup].face_end = CurrFace;

			// マテリアルサブセットのスコープを更新する
//m_Groups[CurrGroup].matSubsets[CurrMatSub].face_end = CurrFace;
m_MatSubs[CurrMatSub].face_end = CurrFace;

		}
		// マテリアル情報を読む
		else if( (pivot=one_line.find("usemtl ")) != string::npos )
		{
//CurrMatSub = AddMaterialSubset(one_line, CurrFace+1, m_Groups[CurrGroup].matSubsets );
CurrMatSub = AddMaterialSubset(one_line, CurrFace+1, m_MatSubs );
		}
		// 名前付きグループを読む
		else if( (pivot=one_line.find("g ")) != string::npos )
		{
			// 名前つきグループを新規登録する
			CurrGroup = AddNamedGroup(one_line, CurrFace+1);
			
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

	for(int i=1; i< m_Vertices.size(); i++)
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


// ファイルをスキャンし，マテリアル1個分の属性を読み取る
bool MeshData::ExtractMaterialComponent(Material &mat, std::ifstream &file, std::string &oneline)
{
	bool flag = true;
	char buf[BUFFER_LENGTH];
	vector<string> elements;
	
	size_t pivot;

	while( !file.eof() ) // ファイルの最後尾か，次の行のnewmtlに到達するまで，，，
	{
		// 1行読み取る
		file.getline(buf, sizeof(buf));
		oneline = string(buf);

		if( (pivot=oneline.find("newmtl ")) != string::npos ) break;// 次のマテリアル定義まで到達したら終了

		//=========== 先頭文字列の種類に応じて処理を実施 =============//
		if( (pivot=oneline.find("#")) != string::npos )				{}// #で始まる行は無視
		else if( (pivot=oneline.find("map_Ka ")) != string::npos )	{}// 環境光テクスチャ
		else if( (pivot=oneline.find("map_Kd ")) != string::npos )	{}// ディフューズテクスチャ
		else if( (pivot=oneline.find("map_bump ")) != string::npos ){}// 法線マップ
		else if( (pivot=oneline.find("bump ")) != string::npos )	{}
		else if( (pivot=oneline.find("map_d ")) != string::npos )	{}
		// Ka (Ambient)
		else if( (pivot=oneline.find("Ka ")) != string::npos )
		{
			Vec3f elements = ExtractVector3D(oneline.substr(pivot+2), " ");
			mat.SetAmbient(elements.x, elements.y, elements.z, 0.0f);
		}
		// Kd (Diffuse)
		else if( (pivot=oneline.find("Kd ")) != string::npos )
		{
			Vec3f elements = ExtractVector3D(oneline.substr(pivot+2), " ");
			mat.SetDiffuse(elements.x, elements.y, elements.z, 0.0f);
		}
		// Ks (Specular)
		else if( (pivot=oneline.find("Ks ")) != string::npos )
		{
			Vec3f elements = ExtractVector3D(oneline.substr(pivot+2), " ");
			mat.SetSpecular(elements.x, elements.y, elements.z, 0.0f);
		}
		// d
		else if( (pivot=oneline.find("d ")) != string::npos )
		{
			elements = Split(oneline.substr(pivot+2), " ");
			float tmp_float = (float)atof(elements[0].c_str());
			mat.SetAlpha(tmp_float);
		}
		//　Ns
		else if( (pivot=oneline.find("Ns ")) != string::npos )
		{
			elements = Split(oneline.substr(pivot+2), " ");
			mat.SetSpecularIntensity( (float)atof(elements[0].c_str()) );
		}
		else{}

	}

	return true;
}




bool MeshData::Load_OBJMTL(const char *filename)
{
	size_t pivot;
	ifstream file;
	int CurrentMaterialID = -1;
	char buf[BUFFER_LENGTH];
	float tmp_float=0.0f;
	vector<string> elements;
	string oneline;


	//============= MTLファイルを読んでマテリアルを追加する =============//
	file.open(filename, ios::in);
	if ( !file.is_open() )
	{
		cout << "Error : 指定されたMTLファイルが開けませんでした\n";
		cout << "File Name : " << filename << endl;

		file.close();
		return false;
	}


	//　1行読み取り
	file.getline(buf, sizeof(buf));
	oneline = string(buf);

	//　ファイルの末端までループ
	while ( !file.eof() )
	{
		if( (pivot=oneline.find("newmtl ")) != string::npos )// マテリアル定義の取得
		{
			// マテリアルを初期化する
			Material newMat;
			char name[256];
			sscanf(oneline.substr(pivot).c_str(), "newmtl %s", name);
			newMat.SetName(name);
			newMat.SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);// InitVec(newMat.Ka, 0.0f, 0.0f, 0.0f, 0.0f);
			newMat.SetDiffuse(0.0f, 0.0f, 0.0f, 0.0f);// InitVec(newMat.Kd, 0.0f, 0.0f, 0.0f, 0.0f);
			newMat.SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);// InitVec(newMat.Ks, 0.0f, 0.0f, 0.0f, 0.0f);
			newMat.SetSpecularIntensity(0.0f);//newMat.Ns = 0.0;

			// マテリアルの中身を埋める→別関数
			ExtractMaterialComponent(newMat, file, oneline);
			
			// m_Materialに追加する
			m_Materials.push_back(newMat);//AddMaterial(buf, CurrentMaterialID);
		}
		else
		{
			file.getline(buf, sizeof(buf));
			oneline = string(buf);
		}

	}

	//　ファイルを閉じる
	file.close();

	return true;
}






void MeshData::AddVertex(string str)
{
	Vec3f	 newVertex;
	vector<string> elements = Split(str, " ");

	if(elements.size() < 3)
	{
		cerr << "Error : Invalid Vertex." << endl;
		return;
	}
	// 頂点座標を設定
	InitVec( newVertex, (float)atof(elements[0].c_str()), (float)atof(elements[1].c_str()), (float)atof(elements[2].c_str()) );
			
	// 頂点座標を追加
	m_Vertices.push_back(newVertex);
}



void MeshData::AddTexCoord(string str)
{
	Vec2f newTexcoord = {0, 0};
	vector<string> elements = Split(str, " ");

	if(elements.empty())
	{
		cerr << "Error : Invalid Texture Coordinate." << endl;
		return;
	}
	
	// テクスチャ座標の格納
	for(size_t i=0; i<min(elements.size(), 3); i++)
	{
		newTexcoord.xy[i] = (float)atof(elements[i].c_str());
	}

	// テクスチャ座標を追加
	m_TexCoord.push_back(newTexcoord);
}


void MeshData::AddNormal(string str)
{
	Vec3f newNormal;
	
	vector<string> elements = Split(str, " ");

	if(elements.size() < 3)
	{
		cerr << "Error : Invalid Normal." << endl;
		return;
	}
	
	// 法線ベクトルを設定
	InitVec( newNormal, (float)atof(elements[0].c_str()), (float)atof(elements[1].c_str()), (float)atof(elements[2].c_str()) );
	
	// 法線ベクトルを追加
	m_Normals.push_back(newNormal);
}




// 1行分の文字列strから面情報を取得し，m_Facesに追加する
int MeshData::AddFace(string str, int matsub_id)
{
	int i;
	ObjFace	newFace;
	Vec3i	VertexAttrib = {-1, -1, -1};

	// スペースで区切られた文字列に分割する
	vector<string> chunks = Split(str, " ");
	
	// 面構造体のメモリを確保化する
	newFace.num_verts		= (int)chunks.size();
	newFace.matsub_index	= matsub_id;
	newFace.VertexAttribIndex.resize(newFace.num_verts, VertexAttrib);
	newFace.use_normal		= false;

	// 文字列chunkごとに頂点情報を格納する
	for(i=0; i<newFace.num_verts; i++)
	{
		vector<string> elements;

		if(chunks[i].find("//") !=string::npos)// Vertex//normal
		{
			elements = Split(chunks[i], "//");// "//"で文字列分割する

			newFace.VertexAttribIndex[i].x = atoi(elements[0].c_str())-1;// Vertex index
			newFace.VertexAttribIndex[i].z = atoi(elements[1].c_str())-1;// Normal index
			newFace.use_normal		= true;
		}
		else if(chunks[i].find("/") !=string::npos)// "/"で区切られている場合→要素2つなら法線と頂点
		{
			elements = Split(chunks[i], "/");// "/"で分割する
			
			if(elements.size()==2)// Vertex/Texture
			{
				newFace.VertexAttribIndex[i].x = atoi(elements[0].c_str())-1;// Vertex index
				newFace.VertexAttribIndex[i].z = atoi(elements[1].c_str())-1;// Texture index
				newFace.use_normal		= false;
			}
			else if(elements.size()==3)// Vertex/Texture/Normal
			{
				newFace.VertexAttribIndex[i].x = atoi(elements[0].c_str())-1;// Vertex index
				newFace.VertexAttribIndex[i].y = atoi(elements[1].c_str())-1;// Texture index
				newFace.VertexAttribIndex[i].z = atoi(elements[2].c_str())-1;// Normal index
				newFace.use_normal			= true;
			}
		}
		else// 区切れ目なし→見つかった数字をそのまんま数字に変換して突っ込んどけ
		{
			newFace.VertexAttribIndex[i].x = atoi(chunks[i].c_str())-1;// Vertex index
			newFace.use_normal		= false;
		}

//if(newFace.VertexAttribIndex[i].x==-1)
//	cout << str.c_str() << endl;
		
	}// end of i loop

	// 面構造体を追加する
	m_Faces.push_back(newFace);

	return (int)m_Faces.size()-1;
}



int MeshData::AddMaterialSubset(string str, int startidx, vector<ObjMaterialSubset> &m_MatSubs)
{
	int i;
	int lastsubset = 0;
	char matname[128];
	ObjMaterialSubset newMatSub;

	// マテリアル名を読み取る
	sscanf(str.c_str(), "usemtl %s", &matname);

	// mtlファイルから読み込んだマテリアル群から名前が一致するものを検索
	for(i=0; i<(int)m_Materials.size(); i++)
	{
		// 名前が一致したら,,,
		if(strcmpi(m_Materials[i].GetName(), matname) == 0)
		{
			// マテリアルサブセットを新規追加する
			newMatSub.material_index	= i;
			newMatSub.face_start		= startidx;
			newMatSub.face_end			= newMatSub.face_start;
			
			m_MatSubs.push_back(newMatSub);

			break;
		}
	}

	return (int)m_MatSubs.size()-1;	
}



int MeshData::AddNamedGroup(string str, int startidx)
{
	char groupname[128];
	ObjNamedGroup newGroup;

	// グループ名を設定
	if(sscanf(str.c_str(), "g %s", &groupname) == 1)
	strcpy(newGroup.name, groupname);
			
	// 頂点インデックスをセット
	newGroup.face_start = (int)m_Faces.size();
	newGroup.face_end = (int)newGroup.face_start;
			
	// 名前付きグループを追加する
	m_Groups.push_back(newGroup);

	return (int)m_Groups.size()-1;
}



// 頂点毎に，テクスチャ座標/法線ベクトルの属性の組み合わせ（複数）を格納する
// [入力]
//  Query: VertexAttribIndex. (x,y,z) = (頂点座標インデックス, テクスチャ座標インデックス, 法線インデックス)
//  Attribs: 頂点の属性。配列データ
// [出力]
//  Attribs配列の対応する要素インデックス
int MeshData::AddVertexAttributes(const Vec3i &Query, vector< vector<Vec3i> > &Attribs)
{
	int i;
	Vec3i newAttrib;
	vector<Vec3i> &currAttribs = Attribs[Query.x];
	
	//============== AttribArrayに既にQueryが登録済みかどうか調べる ===============//
	for(i=0; i<currAttribs.size(); i++)
	{
		if(Query.y==currAttribs[i].x && Query.z==currAttribs[i].y)
			return i;// Queryが登録済みの場合は処理を中止する
	}
	
	//=== Queryのテクスチャ座標インデックス/法線インデックスを新たに登録する =====//
	InitVec(newAttrib, Query.y, Query.z, -1);// 通し番号はまだ未確定なので-1
	currAttribs.push_back(newAttrib);

	return (int)currAttribs.size()-1;
}


// 頂点がに通し番号（頂点インデックス）を付ける
// Attribs: 
int MeshData::AssignVertexIDs(vector< vector<Vec3i> > &Attribs)
{
	int numAttribVertices = 0;// ユニークな属性を持った頂点の総数

	for(int i=0; i<Attribs.size(); i++)// 全ての頂点をスキャンする
	{
		for(int j=0; j<Attribs[i].size(); j++)// 各頂点に付随する属性（複数の頂点/法線の組み合わせが考えられる）も全てスキャンする
		{
			Attribs[i][j].z = numAttribVertices;// 個々の属性レベルで通し番号を振っていく
			numAttribVertices++;
		}
	}

	return numAttribVertices;
}







// 頂点配列とインデックス配列を作成する
void MeshData::GenVertexList(int &numVertAttrs, OreOreLib::VertexLayout **vertexlist, int &numIndices, int **Indices)
{
	int i, j;
	vector< vector<Vec3i> >	VertAttribs;// 頂点ごとの属性.第１次元は頂点インデックス、第2次元は頂点に付随する属性のインデックス。（x:テクスチャ座標ID/y:法線ID/z:通し番号）
	vector< vector<Vec2i> >	FaceAttribs;// ObjFaceの頂点毎の、VertAttribs属性配列のインデックス(x:頂点番号.VertAttribsの第1次元.m_Facesと一義的に対応，y:属性インデックス.VertAttribsの第2次元)
	

	//====================== 全ての面の全頂点の属性を調べ、VertAttrib配列に格納する ======================//
	VertAttribs.resize(m_Vertices.size());
	FaceAttribs.resize(m_Faces.size());

	numIndices = 0;

	for(i=0; i<m_Faces.size(); i++)// 各面について、、、
	{
		for(j=0; j<m_Faces[i].VertexAttribIndex.size(); j++)// 面を構成する頂点毎の属性をVertAttribsに登録する
		{
			int idx = AddVertexAttributes(m_Faces[i].VertexAttribIndex[j], VertAttribs);
			Vec2i newAttribIdx = {m_Faces[i].VertexAttribIndex[j].x, idx};// x:頂点インデックス，y:頂点のどの属性かインデックス
			FaceAttribs[i].push_back(newAttribIdx);
		}

		// 頂点インデックスの総数を累積する
		numIndices += (m_Faces[i].num_verts - 2) * 3;// 3角形ポリゴン数 * 頂点インデックス数3個

	}// end of i loop
	
	numVertAttrs = AssignVertexIDs(VertAttribs)+1;// 全ての頂点の全ての属性に通し番号を振る.



	//======================= 頂点レイアウト配列を作成する =======================//
	
	// 頂点レイアウト配列(頂点座標/テクスチャ座標/法線ベクトル)を作成する
	*vertexlist = new OreOreLib::VertexLayout[numVertAttrs];
	int curr = 0;
	const Vec3f	dumy3 = {-1, -1, -1};
	const Vec2f	dumy2 = {-1, -1};

	for(i=0; i<VertAttribs.size(); i++)
	{
		for(j=0; j<VertAttribs[i].size(); j++)
		{
			int vertIdx		= i;					// 頂点配列m_Vertices上の頂点インデックス
			int texIdx		= VertAttribs[i][j].x;	// テクスチャ座標属性へのインデックス
			int normalIdx	= VertAttribs[i][j].y;	// 法線ベクトル属性へのインデックス

			(*vertexlist)[curr].Position	= vertIdx>=0	? m_Vertices[vertIdx]	: dumy3;	// -1だったら弾く
			(*vertexlist)[curr].TexCoord	= texIdx>=0		? m_TexCoord[texIdx]	: dumy2;	// -1だったら弾く
			(*vertexlist)[curr].Normal		= normalIdx>=0	? m_Normals[normalIdx]	: dumy3;	// -1だったら弾く

//cout << vertexlist[curr].Position.x << ", " << vertexlist[curr].Position.y << ", " << vertexlist[curr].Position.z << endl;

			curr++;
		}
	}

	
	//====================== Facesのインデックスの配列を作る ========================//
	*Indices = new int[numIndices];
	curr = 0;

	for(i=0; i<FaceAttribs.size(); i++)
	{
		// 頂点ごとの属性をVertAttribsに追加登録する
		for(j=1; j<FaceAttribs[i].size()-1; j++)
		{
			Vec2i ID_VertAttrib;

			ID_VertAttrib = FaceAttribs[i][0];
			(*Indices)[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(0)
//cout << Indices[curr-1] << ", ";

			ID_VertAttrib = FaceAttribs[i][j];
			(*Indices)[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j)
//cout << Indices[curr-1] << ", ";

			ID_VertAttrib = FaceAttribs[i][j+1];
			(*Indices)[curr++] = VertAttribs[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点ID(j+1)
//cout << Indices[curr-1] << ", ";

		}// end of j loop

//cout << endl;
	}// end of i loop
	

//cout << curr << ":" << numIndices << endl;
}





void MeshData::GetGroupInfo( int idx )
{
	cout << "Num of Groups = " << m_Groups.size() << endl;

	if( idx >= m_Groups.size() ) return;

	//=================== グループの基本情報 =================//
	int numFaces = m_Groups[idx].face_end - m_Groups[idx].face_start + 1;

	cout << " GroupName : " << m_Groups[idx].name << endl;
	cout << " Face : " << m_Groups[idx].face_start << " - " << m_Groups[idx].face_end << endl;
	cout << " numFaces" << numFaces << endl;

	//======== 同じマテリアルのサブグループを見つける ========//
	int currMtl = -1;//m_Faces[ m_Groups[idx].face_start ].matsub_index;
	



	for(int i= m_Groups[idx].face_start; i<=m_Groups[idx].face_end; ++i )
	{
		if( m_Faces[i].matsub_index != currMtl )
		{
			currMtl	= m_Faces[i].matsub_index;

			cout << " MaterialSubset Found : " << currMtl << endl;

			Material *pMtl	= &m_Materials[ currMtl ];
			pMtl->Info();

			cout << " Vertex Indices..." << endl;
			for(int k=0; k<m_Faces[i].VertexAttribIndex.size(); ++k)
				cout << m_Faces[i].VertexAttribIndex[k].x;


		}


	}

}










/*
void MeshData::Draw(float scale)
{

	vector<ObjFace>::iterator	Face_Iter = m_Faces.begin();

	int pre_mat = -1, cur_mat = 0;

	while(Face_Iter != m_Faces.end())
	{
		//　マテリアルがあるとき
		if(!m_Materials.empty())
		{
			//　インデックスを格納
			cur_mat = m_MatSubs[Face_Iter->matsub_index].material_index;
			
//cout << cur_mat << endl;
			//　前と異なる色のとき
			if(pre_mat != cur_mat)
			{
				//　Object Color
				//glColor4fv(material[cur_mat].color);

				//　Ambient Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_Materials[cur_mat].GetAmbient()->rgba);

				//　Diffuse Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_Materials[cur_mat].GetDiffuse()->rgba);

				//　Specular Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_Materials[cur_mat].GetSpecular()->rgba);

				//　Emission
				//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material[cur_mat].emission);

				//　Shininess
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_Materials[cur_mat].GetSpecularIntensity());

				//　更新
				pre_mat = cur_mat;
			}
			
		}
		
		//　面の描画を開始
		vector<Vec3i>::iterator VertAttr_Iter = Face_Iter->VertexAttribIndex.begin();

		glBegin(Face_Iter->type);

		while(VertAttr_Iter != Face_Iter->VertexAttribIndex.end() )
		{
			//　法線ベクトル
			if(Face_Iter->use_normal==true)
			{
				glNormal3fv(m_Normals[VertAttr_Iter->z].xyz);
			}
//cout << VertAttr_Iter->x << endl;

			if(VertAttr_Iter->x < 0 || VertAttr_Iter->x > m_Vertices.size()-1)
				cout << "out of range: " << VertAttr_Iter->x << endl;

			//　頂点
			glVertex3f(	m_Vertices[VertAttr_Iter->x].x * scale,
						m_Vertices[VertAttr_Iter->x].y * scale, 
						m_Vertices[VertAttr_Iter->x].z * scale);
			
			VertAttr_Iter++;
		}
		//　面の描画を終了
		glEnd();

		Face_Iter++;
	}
}
*/




/*
// メッシュオブジェクトを生成する
void MeshData::GenerateMeshObject()
{
	cout << "SceneGraph::AddMesh()..." << endl;

	
	m_MeshObject.resize(m_Groups.size());



	for(int k=0; k<m_Groups.size(); k++)// グループ単位で属性付き頂点配列とインデックス配列を作成する
	{
		vector< vector<Vec3i> >	VertexLayoutPointer;	// 頂点ごとの属性.第１次元は頂点インデックス、第2次元は頂点に付随する属性のインデックス。（x:テクスチャ座標ID/y:法線ID/z:通し番号）
		vector< vector<Vec2i> >	FaceVertexPointer;		// 面を構成する頂点毎の、VertAttribs属性配列のインデックス(x:頂点番号.VertAttribsの第1次元，y:属性インデックス.VertAttribsの第2次元)
														// [i][j]→i:面ID, j:面の頂点ID


		//====================== 全ての面の全頂点の属性を調べ、VertAttrib配列に格納する ======================//
		int numgFaces = m_Groups[k].face_end - m_Groups[k].face_start +1;// グループの頂点数を取得する

		VertexLayoutPointer.resize(m_Vertices.size());	// 1グループ分の頂点
		FaceVertexPointer.resize(numgFaces);			// 面の各頂点の属性
		
		// メッシュオブジェクトの初期化
		m_MeshObject[k].numVertices		= 0;
		m_MeshObject[k].VertexBuffers	= NULL;
		m_MeshObject[k].numSubMeshs		= m_Groups[k].matSubsets.size();					// サブメッシュ配列を確保する
		m_MeshObject[k].subMeshes		= new SubMeshObject[ m_MeshObject[k].numSubMeshs ];


		// マテリアルサブセット単位で初期化
		for(int f=0; f<m_Groups[k].matSubsets.size(); ++f)
		{
			SubMeshObject	*pSubMesh = &m_MeshObject[k].subMeshes[f];	// サブメッシュオブジェクトへのポインタ
			ObjMaterialSubset	*pMatSub = &m_Groups[k].matSubsets[f];		// マテリアルサブグループへのポインタ

			// サブメッシュ初期化
			pSubMesh->numIndices	= 0;
			pSubMesh->IndexBuffer	= NULL;
			pSubMesh->pMaterial		= NULL;

			for(int l=pMatSub->face_start; l<=pMatSub->face_end; ++l)// マテリアルサブセットのObjFace毎に、、、
			{
				int face_idx_ingroup = l - m_Groups[k].face_start;// グループ内の相対インデックスを取得する

				for(int j=0; j<m_Faces[l].VertexAttribIndex.size(); j++)// ObjFaceの頂点毎に、、、
				{
					int idx				= AddVertexAttributes( m_Faces[l].VertexAttribIndex[j], VertexLayoutPointer );// VertAttribs配列に、属性付き頂点の情報を格納する
					Vec2i newAttribIdx	= { m_Faces[l].VertexAttribIndex[j].x, idx };// x:頂点インデックス，y:頂点のどの属性かインデックス
					FaceVertexPointer[ face_idx_ingroup ].push_back(newAttribIdx);
				}// end of j loop

				pSubMesh->numIndices	+= (m_Faces[l].num_verts - 2) * 3;

			}// end of l loop

		}// end of #matSubsets


		m_MeshObject[k].numVertices		= AssignVertexIDs(VertexLayoutPointer)+1;// 各属性つき頂点に通し番号を割り振る. 戻り値（最終インデックス）に+1して属性付き頂点の総数を取得する
		

		
		//=================================== 頂点レイアウト配列を作成する =============================//

		// 頂点レイアウト配列を作成する
		m_MeshObject[k].VertexBuffers	= new VertexLayout[ m_MeshObject[k].numVertices ];
		
		int curr = 0;// 属性情報を登録する頂点レイアウトの配列インデックス。登録後にインクリメントする

		// 頂点レイアウトデータ(頂点座標/テクスチャ座標/法線ベクトル)を書き出す
		for(int i=0; i<VertexLayoutPointer.size(); i++)// 各頂点について、、、
		{
			for(int j=0; j<VertexLayoutPointer[i].size(); j++)// 各頂点の各属性について、、、
			{
				int vertIdx		= i;
				int texIdx		= VertexLayoutPointer[i][j].x;	// テクスチャ座標インデックス
				int normalIdx	= VertexLayoutPointer[i][j].y;	// 法線ベクトルインデックス
				
				m_MeshObject[k].VertexBuffers[curr].Position	= m_Vertices[vertIdx];
				m_MeshObject[k].VertexBuffers[curr].TexCoord	= m_TexCoord[texIdx];
				m_MeshObject[k].VertexBuffers[curr].Normal		= m_Normals[normalIdx];
				
				//cout << vertexlist[curr].Position.x << ", " << vertexlist[curr].Position.y << ", " << vertexlist[curr].Position.z << endl;
				curr++;
				
			}
		}
		

		
		//====================== マテリアルサブセット毎に頂点インデックスの配列を生成する ======================//
		for(int f=0; f<m_MeshObject[k].numSubMeshs; ++f)
		{
			SubMeshObject	*pSubMesh = &m_MeshObject[k].subMeshes[f];
			pSubMesh->pMaterial	= NULL;
			pSubMesh->IndexBuffer = new int[pSubMesh->numIndices];
			
			int	*pIndexBuffer = pSubMesh->IndexBuffer;
			curr = 0;

			for(int l=m_Groups[k].matSubsets[f].face_start; l<=m_Groups[k].matSubsets[f].face_end; ++l)// マテリアルサブセットのObjFace毎に、、、
			{
				int face_idx_ingroup = l-m_Groups[k].face_start;
				vector <Vec2i> &pFaceVertptr = FaceVertexPointer[face_idx_ingroup];
				
				// 3角形に分割した頂点インデックス群をVertAttribsに追加登録する
				for(int j=1; j<pFaceVertptr.size()-1; j++)//　ポリゴン "ObjFace i"の j 番目の頂点 
				{
					Vec2i ID_VertAttrib;

					ID_VertAttrib = pFaceVertptr[0];
					pIndexBuffer[curr++] = VertexLayoutPointer[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点0のID(ポリゴンの0番目頂点)

					ID_VertAttrib = pFaceVertptr[j];
					pIndexBuffer[curr++] = VertexLayoutPointer[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点1のID(ポリゴンのj番目頂点)

					ID_VertAttrib = pFaceVertptr[j+1];
					pIndexBuffer[curr++] = VertexLayoutPointer[ ID_VertAttrib.x ][ ID_VertAttrib.y ].z;// 頂点2のID(ポリゴンのj+1番目頂点)

				}// end of j loop
				
			}// end of l loop
			
		}// end of f loop
		

		
//===================== マテリアルサブセット毎に頂点インデックス配列を作成する ==============//
//		int numMatSubsets = m_MeshObject[k].numSubMeshs;
//		
//		for(int i=0; i<numMatSubsets; ++i)// 各マテリアルサブセットについて、、、
//		{
//			// マテリアルサブセットが持つ面番号を取得
//
//			SubMeshObject *pSubMesh = &m_MeshObject[k].subMeshes[i];
//
//
//pSubMesh->numIndices;// 不明。マテリアルサブセット毎の頂点インデックス数が分からない
//
//			// マテリアルサブセットが支持するfaceインデックス単位で
//
//			pSubMesh->
//			m_MeshObject[k].subMeshes
//			
//		}// end of i loop
		


		// 作成したGeometryNodeをシーングラフに登録する
	//	m_GeometryList.push_back(newGeomNode);

		
		//================= MaterialNodeをシーングラフに登録する =================//
		//MaterialNode newMaterial;
		//MaterialAttribute *matattr = &newMaterial.MatParam;
		//
		//newMaterial.Type			= MATERIAL;
		//matattr->Ambient			= *m_Materials[m_MatSubsets[k].material_index].GetAmbient();
		//matattr->Diffuse			= *m_Materials[m_MatSubsets[k].material_index].GetDiffuse();
		//matattr->Specular			= *m_Materials[m_MatSubsets[k].material_index].GetSpecular();
		//matattr->SpecularIntensity	= m_Materials[m_MatSubsets[k].material_index].GetSpecularIntensity();
		//matattr->SpecularPower		= m_Materials[m_MatSubsets[k].material_index].GetSpecularPower();
		//matattr->Luminance			= *m_Materials[m_MatSubsets[k].material_index].GetLuminance();

		//m_MaterialList.push_back(newMaterial);
		//
		//Object newObj;

		//newObj.m_prtGeometry	= &m_GeometryList.back();
		//newObj.ptr_Material		= &m_MaterialList.back();

		//m_Objects.push_back(newObj);
		

	}// end of m_Groups loop
	
}
*/