#include "SeamlessPatches.h"

#include	<iostream>


#include	<GL/glew.h>
#include	<GL/glut.h>
#pragma comment(lib, "glew32.lib")

using namespace std;



//#######################################################################################################################//
//################################################# public functions ####################################################//
//#######################################################################################################################//

MeshGenerator::MeshGenerator(int dim, int lv, float scale)
{
tcout << _T("MeshGenerator::MeshGenerator()...") << tendl;

	m_Dim = dim;
	m_numLevels = lv;

	//====================== 頂点データを生成する ======================//
	GenerateTileVertices(m_numVertices, &m_Vertices, m_Dim, m_Dim, scale);
	
	//=================== 頂点インデックスを生成する ===================//
	GenerateTileIndices(m_numPatchIndices, &m_PatchIndices, m_numLevels, m_Dim, m_Dim);
	

	//============== パッチインデックスのオフセットを計算する ==========//
	m_PSizeMR[0] = numTriangles_Tile(4)*3;
	m_PSizeMR[1] = numTriangles_Tile(2)*3;
	m_PSizeMR[2] = numTriangles_Tile(1)*3;
	m_POffsetMR[0] = 0;						// 解像度Lv0タイルのインデックス開始番号
	m_POffsetMR[1] = m_PSizeMR[0];	// 解像度Lv1タイルのインデックス開始番号
	m_POffsetMR[2] = m_POffsetMR[1] + m_PSizeMR[1];// 解像度Lv2タイルのインデックス開始番号
	m_PBlockSize = m_POffsetMR[2] + m_PSizeMR[2];


	//============ 継ぎ目インデックスのオフセットを計算する ============//
	GenerateStitchIndices(m_numStitchIndices, &m_StitchIndices, m_numLevels, m_Dim, m_Dim);
	
	m_SSizeMR[0][0] = numTriangles_StitchSYM(4)*3;
	m_SSizeMR[0][1] = numTriangles_StitchCWCCW(4)*3;
	m_SSizeMR[0][2] = 0;
	m_SSizeMR[1][0] = numTriangles_StitchCWCCW(4)*3;
	m_SSizeMR[1][1] = numTriangles_StitchSYM(2)*3;
	m_SSizeMR[1][2] = numTriangles_StitchCWCCW(2)*3;
	m_SSizeMR[2][0] = 0;
	m_SSizeMR[2][1] = numTriangles_StitchCWCCW(2)*3;
	m_SSizeMR[2][2] = numTriangles_StitchSYM(1)*3;
	
	m_SOffsetMR[0][0] = 0;
	m_SOffsetMR[0][1] = m_SSizeMR[0][0]; 
	m_SOffsetMR[0][2] = -1;
	m_SOffsetMR[1][0] = m_SSizeMR[0][0] + m_SSizeMR[0][1];
	m_SOffsetMR[1][1] = m_SSizeMR[0][0] + m_SSizeMR[0][1] + m_SSizeMR[1][0];
	m_SOffsetMR[1][2] = m_SSizeMR[0][0] + m_SSizeMR[0][1] + m_SSizeMR[1][0] + m_SSizeMR[1][1];
	m_SOffsetMR[2][0] = -1;
	m_SOffsetMR[2][1] = m_SSizeMR[0][0] + m_SSizeMR[0][1] + m_SSizeMR[1][0] + m_SSizeMR[1][1] + m_SSizeMR[1][2];
	m_SOffsetMR[2][2] = m_SSizeMR[0][0] + m_SSizeMR[0][1] + m_SSizeMR[1][0] + m_SSizeMR[1][1] + m_SSizeMR[1][2] + m_SSizeMR[2][1];
	m_SBlockSize = m_SOffsetMR[2][2] + m_SSizeMR[2][2];

	
	CreateVertexBufferObject();
}


MeshGenerator::~MeshGenerator()
{
tcout << _T("MeshGenerator::~MeshGenerator()...") << tendl;
	//=============== 頂点の削除 ==============//
	delete [] m_Vertices;
	delete [] m_PatchIndices;
	delete [] m_StitchIndices;

	//==== 頂点バッファオブジェクトの削除 =====//
	glDeleteBuffers(3, m_buffers);
}



void MeshGenerator::Bind()
{
	//=================== 頂点配列バッファの有効化 =====================//
	glEnableClientState(GL_VERTEX_ARRAY); // 頂点配列の有効化
	
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTEX_BUFFER]);
	glVertexPointer(2, GL_FLOAT, 0, NULL);
	
	glEnableClientState(GL_INDEX_ARRAY);  // 頂点インデックス配列の有効化
	
}


void MeshGenerator::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
}



void MeshGenerator::Draw( int tileres[] )// 4辺の解像度を指定して描画
{
	unsigned short	res_offset, startindex;

	//======================== パッチをレンダリングする ============================//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER_TILE]);

	// 左
	startindex = 0 + m_POffsetMR[ tileres[0] ];
	res_offset = m_PSizeMR[ tileres[0] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );

	// 上
	startindex = m_PBlockSize + m_POffsetMR[ tileres[1] ];
	res_offset = m_PSizeMR[ tileres[1] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );

	// 右
	startindex = m_PBlockSize*2 + m_POffsetMR[ tileres[2] ];
	res_offset = m_PSizeMR[ tileres[2] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );

	// 下
	startindex = m_PBlockSize*3 + m_POffsetMR[ tileres[3] ];
	res_offset = m_PSizeMR[ tileres[3] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );


	//======================= パッチ継ぎ目をレンダリングする =======================//
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER_STITCH] );

	glColor3f(1,1,0);
	// 左下
	startindex = m_SOffsetMR[ tileres[3] ][ tileres[0] ];
	res_offset = m_SSizeMR[ tileres[3] ][ tileres[0] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );

	// 左上
	startindex = m_SBlockSize + m_SOffsetMR[ tileres[0] ][ tileres[1] ];
	res_offset = m_SSizeMR[ tileres[0] ][ tileres[1] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );

	// 右上
	startindex = m_SBlockSize*2 + m_SOffsetMR[ tileres[1] ][ tileres[2] ];
	res_offset = m_SSizeMR[ tileres[1] ][ tileres[2] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );

	// 右下
	startindex = m_SBlockSize*3 + m_SOffsetMR[ tileres[2] ][ tileres[3] ];
	res_offset = m_SSizeMR[ tileres[2] ][ tileres[3] ];
	glDrawElements( GL_TRIANGLES, res_offset, GL_UNSIGNED_SHORT, BUFFER_OFFSET(startindex) );
}




//#######################################################################################################################//
//################################################ private functions ####################################################//
//#######################################################################################################################//

void MeshGenerator::CreateVertexBufferObject()
{
tcout << _T("MeshGenerator::CreateVertexBufferObject()...") << tendl;
	glGenBuffers(3, m_buffers);
	
	// 頂点バッファ作成
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*m_numVertices, m_Vertices, GL_STATIC_DRAW);// 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// タイルインデックスバッファ作成
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER_TILE]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*m_numPatchIndices, m_PatchIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// 継ぎ目インデックスバッファ作成
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER_STITCH]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*m_numStitchIndices, m_StitchIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}



// [-0.5,+0.5]の2次元平面上に，タイルを構成する頂点群を生成する
void MeshGenerator::GenerateTileVertices(int &numPoints, Vec2f **Vertices, int dimx, int dimy, float scale)
{
tcout << _T("MeshGenerator::GenerateTileVertices()...") << tendl;
	int i, j;
	float	offset = scale * 0.5f;

	m_Dim = dimx;
	
	numPoints = dimx*dimy;
	(*Vertices) = new Vec2f[numPoints];

	for(j=0; j<m_Dim; j++)
	{
		for(i=0; i<m_Dim; i++)
		{
			(*Vertices)[j*m_Dim + i].u = float(i*scale)/float(m_Dim-1) - offset;
			(*Vertices)[j*m_Dim + i].v = float(j*scale)/float(m_Dim-1) - offset;
		}// end of j loop
	}// end of i loop
	
}



// 複数解像度分のタイル頂点インデックス配列を一括生成する
void MeshGenerator::GenerateTileIndices(int &numElements, unsigned short **TileIndices, int numLevels, int dimx, int dimy)
{
tcout << _T("MeshGenerator::GenerateTileIndices()...") << tendl;

	int i;
	static int j;
	int step;
	
	m_Dim = dimx;

	numElements = 0;

	//================ メモリ領域を確保する ===============//
	for(i=0; i<numLevels; i++)
		numElements += numTriangles_Tile( (int)pow(2.0f,(float)i) );
	
	numElements *= 4 * 3;// タイル枚数*トライアングル頂点数
	(*TileIndices) = new unsigned short[numElements];

	
	//================= 頂点インデックスを生成する ================//
	j=0;

	//各方向ごとに，レベルぶんの頂点インデックスを格納する
	
	// 左側タイルレベル数ぶん登録
	for(i=0; i<numLevels; i++)//for(i=numLevels-1; i>=0; i--)
	{
		step = (int)pow(2.0f,float(numLevels-1-i));
		ConstructTile_LEFT(j,(*TileIndices), step );
	}
	// 上側タイルレベル数ぶん登録
	for(i=0; i<numLevels; i++)//for(i=numLevels-1; i>=0; i--)
	{
		step = (int)pow(2.0f,float(numLevels-1-i));
		ConstructTile_TOP(j,(*TileIndices), step );
	}
	// 右側タイルレベル数ぶん登録
	for(i=0; i<numLevels; i++)//for(i=numLevels-1; i>=0; i--)
	{
		step = (int)pow(2.0f,float(numLevels-1-i));
		ConstructTile_RIGHT(j,(*TileIndices), step );
	}
	// 下側タイルレベル数ぶん登録
	for(i=0; i<numLevels; i++)//for(i=numLevels-1; i>=0; i--)
	{
		step = (int)pow(2.0f,float(numLevels-1-i));
		ConstructTile_BOTTOM(j,(*TileIndices), step );
	}
}



// [-0.5,+0.5]の２次元平面上に，タイルの継ぎ目となる頂点群を生成する
void MeshGenerator::GenerateStitchIndices(int &numElements, unsigned short **StitchIndices, int numLevels, int dimx, int dimy)
{
tcout << _T("MeshGenerator::GenerateStitchIndices()...") << tendl;
	int i,j;
	static int k;
	int step;
	
	m_Dim = dimx;

	numElements = 0;

	//================ メモリ領域を確保する ===============//
	for(i=0; i<numLevels; i++)
	{
		step = (int)pow(2.0f,float(numLevels-i-1) );

		for(j=max(i-1,0); j<=min(numLevels-1,i+1); j++)
		{
			int step2 = max(step, (int)pow(2.0f,float(numLevels-j-1)) );

			if(i==j)		numElements += numTriangles_StitchSYM(step);// 対称継ぎ目のトライアングル数を登録
			else if(step2>1)numElements += numTriangles_StitchCWCCW(step2);// 非対称継ぎ目のトライアングル数を登録
		}
	}
	
	numElements *= 4 * 3;// タイル枚数*トライアングル頂点数
	(*StitchIndices) = new unsigned short[numElements];
	
	//================= 頂点インデックスを生成する ================//
	k=0;
	
	//各方向ごとに，レベルぶんの頂点インデックスを格納する
	
	// 左下継ぎ目レベル数ぶん登録
	for(i=0; i<numLevels; i++)
	{
		step = (int)pow(2.0f,float(numLevels-i-1) );
		
		for(j=max(i-1,0); j<=min(numLevels-1,i+1); j++)
		{
			int step2 = max(step, (int)pow(2.0f,float(numLevels-j-1)) );
			if(i==j)				ConstructSymmetricStitch_BL(k, (*StitchIndices), step);// 対称継ぎ目のインデックスを登録
			else if(i<j && step2>1)	ConstructCWStitch_BL(k, (*StitchIndices), step2);// 非対称継ぎ目（CW）のインデックスを登録
			else if(step2>1)		ConstructCCWStitch_BL(k, (*StitchIndices), step2);// 非対称継ぎ目の(CCW)トインデックスを登録
		}
	}
	
	// 左上継ぎ目レベル数ぶん登録
	for(i=0; i<numLevels; i++)
	{
		step = (int)pow(2.0f,float(numLevels-i-1) );
		
		for(j=max(i-1,0); j<=min(numLevels-1,i+1); j++)
		{
			int step2 = max(step, (int)pow(2.0f,float(numLevels-j-1)) );
			if(i==j)				ConstructSymmetricStitch_TL(k, (*StitchIndices), step);// 対称継ぎ目のインデックスを登録
			else if(i<j && step2>1)	ConstructCWStitch_TL(k, (*StitchIndices), step2);// 非対称継ぎ目（CW）のインデックスを登録
			else if(step2>1)		ConstructCCWStitch_TL(k, (*StitchIndices), step2);// 非対称継ぎ目の(CCW)トインデックスを登録
		}
	}

	// 右上継ぎ目レベル数ぶん登録
	for(i=0; i<numLevels; i++)
	{
		step = (int)pow(2.0f,float(numLevels-i-1) );

		for(j=max(i-1,0); j<=min(numLevels-1,i+1); j++)
		{
			int step2 = max(step, (int)pow(2.0f,float(numLevels-j-1)) );
			if(i==j)				ConstructSymmetricStitch_TR(k, (*StitchIndices), step);// 対称継ぎ目のインデックスを登録
			else if(i<j && step2>1)	ConstructCWStitch_TR(k, (*StitchIndices), step2);// 非対称継ぎ目（CW）のインデックスを登録
			else if(step2>1)		ConstructCCWStitch_TR(k, (*StitchIndices), step2);// 非対称継ぎ目の(CCW)トインデックスを登録
		}
	}

	// 右下継ぎ目レベル数ぶん登録
	for(i=0; i<numLevels; i++)
	{
		step = (int)pow(2.0f,float(numLevels-i-1) );

		for(j=max(i-1,0); j<=min(numLevels-1,i+1); j++)
		{
			int step2 = max(step, (int)pow(2.0f,float(numLevels-j-1)) );
			if(i==j)				ConstructSymmetricStitch_BR(k, (*StitchIndices), step);// 対称継ぎ目のインデックスを登録
			else if(i<j && step2>1)	ConstructCWStitch_BR(k, (*StitchIndices), step2);// 非対称継ぎ目（CW）のインデックスを登録
			else if(step2>1)		ConstructCCWStitch_BR(k, (*StitchIndices), step2);// 非対称継ぎ目の(CCW)トインデックスを登録
		}
	}
	
}




// タイルを構成する三角形メッシュの数を返す
int MeshGenerator::numTriangles_Tile(int step)
{
	int	num_triangles = ((m_Dim/step) - 2) / 2;
	
	num_triangles *= num_triangles*2;

	return num_triangles;
}

// タイル継ぎ目（つなぎ合わせるタイルの解像度は異なる）を構成する三角形メッシュ数を返す
int MeshGenerator::numTriangles_StitchCWCCW(int step_larger)
{
	int	step = step_larger/2;
	int	num_large_triangles = (m_Dim/2) / (2*step) -1;
	int	num_middle_triangles= (m_Dim/2) / step;
	int	num_small_triangles = (m_Dim/2) / step * 2 -1;

	return num_large_triangles + num_middle_triangles + num_small_triangles;
}

// タイル継ぎ目（つなぎ合わせるタイルの解像度レベルは同じ）を構成する三角形メッシュ数を返す
int MeshGenerator::numTriangles_StitchSYM(int step)
{
	int	num_triangles = (m_Dim/2) / step * 4 -2;
	
	return num_triangles;
}



// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//	v2 *
//	   |
//	v0 *---* v1
void MeshGenerator::AddTriangleIndex_BL(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v1 = v0 + step,
		v2 = v0 + m_Dim*step;
	
	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//	v2 *---* v1
//	   |
//	v0 *
void MeshGenerator::AddTriangleIndex_TL(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v2 = v0 + m_Dim*step,
		v1 = v2 + step;
		
	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//	v2 *---* v1
//	       |
//	       * v0
void MeshGenerator::AddTriangleIndex_TR(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v1 = v0 + m_Dim*step,
		v2 = v1 - step;
	
	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//	        * v2
//	        |
//   v0 *---* v1
void MeshGenerator::AddTriangleIndex_BR(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v1 = v0 + step,
		v2 = v1 + m_Dim*step;

	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
// v2 *   
//    |
//    |    * v1
//    |
// v0 *
void MeshGenerator::AddTriangleIndex_LEFT(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v1 = v0 + (m_Dim+1)*step,
		v2 = v0 + 2*m_Dim*step;

	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
// v2 *-------* v1
//    
//        *
//        v0
void MeshGenerator::AddTriangleIndex_TOP(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v1 = v0 + (m_Dim+1)*step,
		v2 = v0 + (m_Dim-1)*step;

	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//        * v1
//        |
// v2 *   |    
//        |
//        * v0
void MeshGenerator::AddTriangleIndex_RIGHT(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v1 = v0 + 2*m_Dim*step,
		v2 = v0 + (m_Dim-1)*step;

	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//       v2
//        *
//
// v0 *-------* v1
void MeshGenerator::AddTriangleIndex_BOTTOM(int &curr_index, unsigned short *TileIndices, int startid, int step)
{
	int	v0 = startid,
		v1 = v0 + 2*step,
		v2 = v0 + (m_Dim+1)*step;

	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//  v3 +--+ v2
//     | /|
//     |/ |
//  v0 *--+ v1
void MeshGenerator::AddQuadIndex1(int &curr_index, unsigned short *TileIndices, int v0, int step)
{
	int	v1 = v0 + step,
		v2 = v1 + m_Dim*step,
		v3 = v2 - step;

	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v2;

	TileIndices[curr_index++] = v2;
	TileIndices[curr_index++] = v3;
	TileIndices[curr_index++] = v0;
}


// 以下の絵の順番に頂点インデックスを格納する（左下がx,y軸負の方向）
//  v3 +--+ v2
//     |- |
//     | -|
//  v0 *--+ v1
void MeshGenerator::AddQuadIndex2(int &curr_index, unsigned short *TileIndices, int v0, int step)
{
	int	v1 = v0 + step,
		v2 = v1 + m_Dim*step,
		v3 = v2 - step;

	TileIndices[curr_index++] = v0;
	TileIndices[curr_index++] = v1;
	TileIndices[curr_index++] = v3;

	TileIndices[curr_index++] = v2;
	TileIndices[curr_index++] = v3;
	TileIndices[curr_index++] = v1;
}



// y_max    *
//          |
//          |
// y_mid    *      * x_max
//          |
//          |
// y_min    *
// 左側タイルのインデックスを作成する
void MeshGenerator::ConstructTile_LEFT(int &numIndices, unsigned short *TileIndices, int step)
{
	int x, y;
	int	y_min,	// y頂点インデックス最下
		y_mid,		// y頂点インデックス中央
		y_max,		// y頂点インデックス最上
		x_max;
	int v0;		// 頂点インデックス基準

	//===================== 変数初期化 =======================//
	y_min	= step;
	y_mid	= m_Dim/2;
	y_max	= (m_Dim-1)-step;
	
	
	//====================== 下半分の三角形インデックスを作成 ========================//
	for(y= y_min; y <=y_mid-step; y+=step)
	{
		x_max = y - step;

		for(x = 0; x <= x_max; x+=step)
		{
			v0 = y*m_Dim + x;
			if(x == x_max)	AddTriangleIndex_TL(numIndices, TileIndices, v0, step);// 左上半分の三角だけ登録
			else			AddQuadIndex1(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

		}// end of x loop
	}// end od y loop
	
	//====================== 上半分の三角形インデックスを作成 ========================//
	for(y= y_mid; y <=y_max-step; y+=step)
	{
		x_max = (m_Dim-step) - y - step;

		for(x = 0; x < x_max; x+=step)
		{
			v0 = y*m_Dim + x;
			if(x == x_max-1)	AddTriangleIndex_BL(numIndices, TileIndices, v0, step);// 左上半分の三角だけ登録
			else			AddQuadIndex2(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

		}// end of x loop
	}// end of y loop
	
}



// x_min *------*-------* x_mid
//
//
//              *  y_min
// 上側タイルのインデックスを作成する
void MeshGenerator::ConstructTile_TOP(int &numIndices, unsigned short *TileIndices, int step)
{
	int x, y;
	int	x_min,	// x頂点インデックス最下
		x_mid,	// x頂点インデックス中央
		x_max,	// x頂点インデックス最上
		y_min;	// y頂点インデックス最下
	int v0;		// 頂点インデックス基準

	//===================== 変数初期化 =======================//
	x_min	= step;
	x_mid	= m_Dim/2;
	x_max	= (m_Dim-1)-step;
	
	
	//====================== 上半分の三角形インデックスを作成 ========================//
	for(x=x_min; x<=x_mid-step; x+=step)
	{
		y_min = m_Dim -1 - x;

		for(y = y_min; y < m_Dim; y+=step)
		{
			v0 = y*m_Dim + x;
			if(y == y_min)		AddTriangleIndex_TR(numIndices, TileIndices, v0+step, step);// 半分の三角だけ登録
			else if(y < m_Dim-1)	AddQuadIndex2(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

		}// end of x loop
	}// end od y loop

	//====================== 右半分の三角形インデックスを作成 ========================//
	for(x=x_mid; x<=x_max-step; x+=step)
	{
		y_min = x + step;

		for(y = y_min; y < m_Dim; y+=step)
		{
			v0 = y*m_Dim + x;
			if(y == y_min)		AddTriangleIndex_TL(numIndices, TileIndices, v0, step);// 半分の三角だけ登録
			else if(y < m_Dim-1)	AddQuadIndex1(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

			
		}// end of x loop
		
	}// end of y loop

}



//              *  y_max
//              |
//              |
// x_min *      * y_mid
//              |
//              |
//              *  y_min
// 右側タイルのインデックスを作成する
void MeshGenerator::ConstructTile_RIGHT(int &numIndices, unsigned short *TileIndices, int step)
{
	int x, y;
	int	y_min,	// y頂点インデックス最下
		y_mid,	// y頂点インデックス中央
		y_max,	// y頂点インデックス最上
		x_min;
	int v0;		// 頂点インデックス基準

	//===================== 変数初期化 =======================//
	y_min	= step;
	y_mid	= m_Dim/2;
	y_max	= (m_Dim-1)-step;
	
	
	//====================== 下半分の三角形インデックスを作成 ========================//
	for(y=y_min; y <=y_mid-step; y+=step)
	{
		x_min = m_Dim -1 - y;

		for(x = x_min; x < m_Dim; x+=step)
		{
			v0 = y*m_Dim + x;
			if(x == x_min)			AddTriangleIndex_TR(numIndices, TileIndices, v0+step, step);// 半分の三角だけ登録
			else if(x < m_Dim-1)	AddQuadIndex2(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

		}// end of x loop
	}// end od y loop

	//====================== 上半分の三角形インデックスを作成 ========================//
	for(y=y_mid; y <=y_max-step; y+=step)
	{
		x_min = y + step;

		for(x = x_min; x < m_Dim-1; x+=step)
		{
			v0 = y*m_Dim + x;
			if(x == x_min)	AddTriangleIndex_BR(numIndices, TileIndices, v0, step);// 半分の三角だけ登録
			else			AddQuadIndex1(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

		}// end of x loop
	}// end of y loop

}




void MeshGenerator::ConstructTile_BOTTOM(int &numIndices, unsigned short *TileIndices, int step)
{
	int x, y;
	int	x_min,	// x頂点インデックス最下
		x_mid,	// x頂点インデックス中央
		x_max,	// x頂点インデックス最上
		y_max;	// y頂点インデックス最下
	int v0;		// 頂点インデックス基準

	//===================== 変数初期化 =======================//
	x_min	= step;
	x_mid	= m_Dim/2;
	x_max	= (m_Dim-1)-step;
	
	
	//====================== 左半分の三角形インデックスを作成 ========================//
	for(x=x_min; x<=x_mid-step; x+=step)
	{
		y_max = x - step;

		for(y=0; y<=y_max; y+=step)
		{
			v0 = y*m_Dim + x;
			if(y==y_max)	AddTriangleIndex_BR(numIndices, TileIndices, v0, step);// 半分の三角だけ登録
			else			AddQuadIndex1(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

		}// end of x loop
	}// end od y loop

	//====================== 右半分の三角形インデックスを作成 ========================//
	for(x=x_mid; x<=x_max-step; x+=step)
	{
		y_max = (m_Dim-step) - (x+step);

		for(y=0; y<=y_max-1; y+=step)
		{
			v0 = y*m_Dim + x;
			if(y==y_max-1)	AddTriangleIndex_BL(numIndices, TileIndices, v0, step);// 半分の三角だけ登録
			else			AddQuadIndex2(numIndices, TileIndices, v0, step);	// 四角形ぜんぶ登録

		}// end of x loop
	}// end of y loop
	
}



void MeshGenerator::ConstructSymmetricStitch_BL(int &numIndices, unsigned short *TileIndices, int step)
{
	int x;
	int v0;		// 頂点インデックス基準

	//====================== 左下 ========================//
	v0 = 0;
	
	for(x=0; x<m_Dim/2; x+=step)
	{
		AddQuadIndex1(numIndices, TileIndices, v0, step);// 四角
		if(x>0)// 三角
		{
			AddTriangleIndex_BR(numIndices, TileIndices, v0 - step, step);
			AddTriangleIndex_TL(numIndices, TileIndices, v0 - m_Dim*step, step);
		}

		v0 += step*(m_Dim+1);
	}// end of x loop
}


void MeshGenerator::ConstructSymmetricStitch_TL(int &numIndices, unsigned short *TileIndices, int step)
{
	int x;
	int v0;		// 頂点インデックス基準

	//====================== 左上 ========================//
	v0 = m_Dim*(m_Dim-1);

	for(x=0; x<m_Dim/2; x+=step)
	{
		AddQuadIndex2(numIndices, TileIndices, v0 - m_Dim*step, step);// 四角
		if(x>0)// 三角
		{
			AddTriangleIndex_BL(numIndices, TileIndices, v0, step);
			AddTriangleIndex_TR(numIndices, TileIndices, v0 - m_Dim*step, step);
		}
		
		v0 += step * (1-m_Dim);
	}// end of x loop
}


void MeshGenerator::ConstructSymmetricStitch_TR(int &numIndices, unsigned short *TileIndices, int step)
{
	int x;
	int v0;		// 頂点インデックス基準
	
	//======================= 右上 ======================//
	v0 = m_Dim/2*m_Dim + m_Dim/2;

	for(x=0; x<m_Dim/2; x+=step)
	{
		AddQuadIndex1(numIndices, TileIndices, v0, step);// 四角
		if(x>0)// 三角
		{
			AddTriangleIndex_BR(numIndices, TileIndices, v0 - step, step);
			AddTriangleIndex_TL(numIndices, TileIndices, v0 - m_Dim*step, step);
		}

		v0 += step*(m_Dim+1);
	}// end of x loop
	
}


void MeshGenerator::ConstructSymmetricStitch_BR(int &numIndices, unsigned short *TileIndices, int step)
{
	int x;
	int v0;		// 頂点インデックス基準

	//======================= 右下 ======================//
	v0 = m_Dim/2*m_Dim + m_Dim/2;

	for(x=0; x<m_Dim/2; x+=step)
	{
		AddQuadIndex2(numIndices, TileIndices, v0 - m_Dim*step, step);// 四角
		if(x>0)// 三角
		{
			AddTriangleIndex_BL(numIndices, TileIndices, v0, step);
			AddTriangleIndex_TR(numIndices, TileIndices, v0 - m_Dim*step, step);
		}
		
		v0 += step * (1-m_Dim);
	}// end of x loop
	
}


// 時計回り方向の継ぎ目(stepは大きい側の解像度)
void MeshGenerator::ConstructCWStitch_BL(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int step;

	//====================== 左下 ========================//
	v0 = 0;
	step = step_larger/2;

	for(x=0; x<m_Dim/2; x+=2*step)
	{
		AddTriangleIndex_TL(numIndices, TileIndices, v0, step);
		AddTriangleIndex_BR(numIndices, TileIndices, v0+step*m_Dim, step);
		AddTriangleIndex_TL(numIndices, TileIndices, v0+step*(m_Dim+1), step);
		AddTriangleIndex_BOTTOM(numIndices, TileIndices, v0, step);
		AddTriangleIndex_RIGHT(numIndices, TileIndices, v0+2*step, step);

		if(x>0)
		{
			AddTriangleIndex_BR(numIndices, TileIndices, v0-step, step);
			AddTriangleIndex_TL(numIndices, TileIndices, v0-2*m_Dim*step, step*2);
		}
		
		v0 += 2 * step * (m_Dim+1);
	}// end of x loop
}




void MeshGenerator::ConstructCWStitch_TL(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int step;
	
	//====================== 左上 ========================//
	v0 = m_Dim*(m_Dim-1);
	step = step_larger/2;

	for(x=0; x<m_Dim/2; x+=2*step)
	{	
		AddTriangleIndex_TR(numIndices, TileIndices, v0+step*(1-m_Dim), step);
		AddTriangleIndex_BL(numIndices, TileIndices, v0+step*(1-m_Dim), step);
		AddTriangleIndex_TR(numIndices, TileIndices, v0+2*step*(1-m_Dim), step);
		AddTriangleIndex_LEFT(numIndices, TileIndices, v0-2*step*m_Dim, step);
		AddTriangleIndex_BOTTOM(numIndices, TileIndices, v0-2*step*m_Dim, step);
		
		if(x>0)
		{
			AddTriangleIndex_BL(numIndices, TileIndices, v0, step);
			AddTriangleIndex_TR(numIndices, TileIndices, v0-2*m_Dim*step, step*2);
		}
		
		v0 += 2 * step * (1-m_Dim);
	}// end of x loop
}


void MeshGenerator::ConstructCWStitch_TR(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int step;
	
	//======================= 右上 ======================//
	v0 = m_Dim/2*m_Dim + m_Dim/2;
	step = step_larger/2;

	for(x=0; x<m_Dim/2; x+=2*step)
	{
		AddTriangleIndex_BR(numIndices, TileIndices, v0, step);
		AddTriangleIndex_TL(numIndices, TileIndices, v0+step, step);
		AddTriangleIndex_BR(numIndices, TileIndices, v0+step*(1+m_Dim), step);
		AddTriangleIndex_LEFT(numIndices, TileIndices, v0, step);
		AddTriangleIndex_TOP(numIndices, TileIndices, v0+step*(1+m_Dim), step);
		
		if(x>0)
		{
			AddTriangleIndex_TL(numIndices, TileIndices, v0-step*m_Dim, step);
			AddTriangleIndex_BR(numIndices, TileIndices, v0-2*step, step*2);
		}
		
		v0 += 2*step*(m_Dim+1);
	}// end of x loop
}


void MeshGenerator::ConstructCWStitch_BR(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int step;
	
	//======================= 右下 ======================//
	v0 = m_Dim/2*m_Dim + m_Dim/2;
	step = step_larger/2;

	for(x=0; x<m_Dim/2; x+=2*step)
	{
		AddTriangleIndex_BL(numIndices, TileIndices, v0-step*m_Dim, step);
		AddTriangleIndex_TR(numIndices, TileIndices, v0+step*(1-2*m_Dim), step);
		AddTriangleIndex_BL(numIndices, TileIndices, v0+step*(1-2*m_Dim), step);
		AddTriangleIndex_TOP(numIndices, TileIndices, v0+step*(1-m_Dim), step);
		AddTriangleIndex_RIGHT(numIndices, TileIndices, v0+2*step*(1-m_Dim), step);
		
		if(x>0)
		{
			AddTriangleIndex_TR(numIndices, TileIndices, v0-step*m_Dim, step);
			AddTriangleIndex_BL(numIndices, TileIndices, v0, step*2);
		}
		
		v0 += 2 * step * (1-m_Dim);
	}// end of x loop
}





// 時計回り方向の継ぎ目
void MeshGenerator::ConstructCCWStitch_BL(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int step;

	//====================== 左下 ========================//
	v0 = 0;
	step = step_larger/2;

	for(x=0; x<m_Dim/2; x+=2*step)
	{
		AddTriangleIndex_BR(numIndices, TileIndices, v0, step);
		AddTriangleIndex_TL(numIndices, TileIndices, v0+step, step);
		AddTriangleIndex_BR(numIndices, TileIndices, v0+step*(1+m_Dim), step);
		AddTriangleIndex_LEFT(numIndices, TileIndices, v0, step);
		AddTriangleIndex_TOP(numIndices, TileIndices, v0+step*(1+m_Dim), step);
		
		if(x>0)
		{
			AddTriangleIndex_TL(numIndices, TileIndices, v0-step*m_Dim, step);
			AddTriangleIndex_BR(numIndices, TileIndices, v0-2*step, step*2);
		}
		
		v0 += 2*step*(m_Dim+1);
	}// end of x loop
}


void MeshGenerator::ConstructCCWStitch_TL(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int step;
	
	//====================== 左上 ========================//
	v0 = m_Dim*(m_Dim-1);
	step = step_larger/2;

	for(x=0; x<m_Dim/2; x+=2*step)
	{
		AddTriangleIndex_BL(numIndices, TileIndices, v0-step*m_Dim, step);
		AddTriangleIndex_TR(numIndices, TileIndices, v0+step*(1-2*m_Dim), step);
		AddTriangleIndex_BL(numIndices, TileIndices, v0+step*(1-2*m_Dim), step);
		AddTriangleIndex_TOP(numIndices, TileIndices, v0+step*(1-m_Dim), step);
		AddTriangleIndex_RIGHT(numIndices, TileIndices, v0+2*step*(1-m_Dim), step);
		
		if(x>0)
		{
			AddTriangleIndex_TR(numIndices, TileIndices, v0-step*m_Dim, step);
			AddTriangleIndex_BL(numIndices, TileIndices, v0, step*2);
		}
		
		v0 += 2 * step * (1-m_Dim);
	}// end of x loop
}


void MeshGenerator::ConstructCCWStitch_TR(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int	step;
	
	//======================= 右上 ======================//
	v0 = m_Dim/2*m_Dim + m_Dim/2;
	step = step_larger/2;
	
	for(x=0; x<m_Dim/2; x+=2*step)
	{
		AddTriangleIndex_TL(numIndices, TileIndices, v0, step);
		AddTriangleIndex_BR(numIndices, TileIndices, v0+step*m_Dim, step);
		AddTriangleIndex_TL(numIndices, TileIndices, v0+step*(m_Dim+1), step);
		AddTriangleIndex_BOTTOM(numIndices, TileIndices, v0, step);
		AddTriangleIndex_RIGHT(numIndices, TileIndices, v0+2*step, step);

		if(x>0)
		{
			AddTriangleIndex_BR(numIndices, TileIndices, v0-step, step);
			AddTriangleIndex_TL(numIndices, TileIndices, v0-2*m_Dim*step, step*2);
		}
		
		v0 += 2*step*(m_Dim+1);
	}// end of x loop
}


void MeshGenerator::ConstructCCWStitch_BR(int &numIndices, unsigned short *TileIndices, int step_larger)
{
	int x;
	int v0;		// 頂点インデックス基準
	int step;

	//======================= 右下 ======================//
	v0 = m_Dim/2*m_Dim + m_Dim/2;
	step = step_larger/2;

	for(x=0; x<m_Dim/2; x+=2*step)
	{	
		AddTriangleIndex_TR(numIndices, TileIndices, v0+step*(1-m_Dim), step);
		AddTriangleIndex_BL(numIndices, TileIndices, v0+step*(1-m_Dim), step);
		AddTriangleIndex_TR(numIndices, TileIndices, v0+2*step*(1-m_Dim), step);
		AddTriangleIndex_LEFT(numIndices, TileIndices, v0-2*step*m_Dim, step);
		AddTriangleIndex_BOTTOM(numIndices, TileIndices, v0-2*step*m_Dim, step);
		
		if(x>0)
		{
			AddTriangleIndex_BL(numIndices, TileIndices, v0, step);
			AddTriangleIndex_TR(numIndices, TileIndices, v0-2*m_Dim*step, step*2);
		}
		
		v0 += 2 * step * (1-m_Dim);
	}// end of x loop
}