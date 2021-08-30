#ifndef SEAMLESS_PATCHES_H
#define SEAMLESS_PATCHES_H

#include	<oreore/mathlib/GraphicsMath.h>


// "Seamless Patches for GPU Based Terrain Tessellation"




#define BUFFER_OFFSET(offset) ((unsigned short *)NULL + (offset))

#define	VERTEX_BUFFER		0
#define	INDEX_BUFFER_TILE	1
#define	INDEX_BUFFER_STITCH	2



// 時計回り頂点インデックス
#define	BOTTOM_LEFT		0x00// 00
#define	TOP_LEFT		0x01// 01
#define	TOP_RIGHT		0x02// 10
#define	BOTTOM_RIGHT	0x03// 11







class MeshGenerator
{
private:
	
	int	m_Dim;		// タイル頂点の解像度
	int	m_numLevels;	// 解像度レベル


	// 頂点データ
	int		m_numVertices;
	Vec2f	*m_Vertices;
	
	// 頂点インデックス
	int				m_numPatchIndices;
	unsigned short	*m_PatchIndices;	// タイルを構成する三角形の頂点インデックス
	int				m_PBlockSize;		// タイルの頂点インデックス数
	unsigned short	m_POffsetMR[3];		// 多重解像度タイル開始インデックス
	unsigned short	m_PSizeMR[3];		// 解像度ごとのタイル頂点数
	
	int				m_numStitchIndices;
	unsigned short	*m_StitchIndices;	// タイル継ぎ目を構成する三角形のインデックス
	int				m_SBlockSize;		// タイル継ぎ目の頂点インデックス数
	unsigned short	m_SSizeMR[3][3];	// 解像度ごとの継ぎ目頂点数
	unsigned short	m_SOffsetMR[3][3];	// 多重解像度継ぎ目開始インデックス

	unsigned int	m_buffers[3];// 頂点バッファ/パッチインデックスバッファ/継ぎ目インデックスバッファのOpenGLインスタンス

	//================== メンバ関数 =============//

	// 3角形１枚分の頂点インデックスを生成する
	void AddTriangleIndex_BL(int &curr_index, unsigned short *TileIndices, int startid, int step);// 左下側
	void AddTriangleIndex_TL(int &curr_index, unsigned short *TileIndices, int startid, int step);// 左上側
	void AddTriangleIndex_TR(int &curr_index, unsigned short *TileIndices, int startid, int step);// 右上側
	void AddTriangleIndex_BR(int &curr_index, unsigned short *TileIndices, int startid, int step);// 右下側

	void AddTriangleIndex_LEFT(int &curr_index, unsigned short *TileIndices, int startid, int step);	// 左側
	void AddTriangleIndex_TOP(int &curr_index, unsigned short *TileIndices, int startid, int step);		// 上側
	void AddTriangleIndex_RIGHT(int &curr_index, unsigned short *TileIndices, int startid, int step);	// 右側
	void AddTriangleIndex_BOTTOM(int &curr_index, unsigned short *TileIndices, int startid, int step);	// 下側

	// 4角形から三角形2枚分の頂点インデックスを生成する
	void AddQuadIndex1(int &curr_index, unsigned short *TileIndices, int v0, int step);// 左上と右下に分割
	void AddQuadIndex2(int &curr_index, unsigned short *TileIndices, int v0, int step);// 左下と右上に分割

	// パッチの頂点インデックスを生成する
	void ConstructTile_LEFT(int &numIndices, unsigned short *TileIndices, int step);	// 左
	void ConstructTile_TOP(int &numIndices, unsigned short *TileIndices, int step);		// 上
	void ConstructTile_RIGHT(int &numIndices, unsigned short *TileIndices, int step);	// 右
	void ConstructTile_BOTTOM(int &numIndices, unsigned short *TileIndices, int step);	// 下
	
	// 継ぎ目(同じ解像度のパッチ間を接続)の頂点インデックスを生成する
	void ConstructSymmetricStitch_BL(int &numIndices, unsigned short *TileIndices, int step);	// 左下
	void ConstructSymmetricStitch_TL(int &numIndices, unsigned short *TileIndices, int step);	// 左上
	void ConstructSymmetricStitch_TR(int &numIndices, unsigned short *TileIndices, int step);	// 右上
	void ConstructSymmetricStitch_BR(int &numIndices, unsigned short *TileIndices, int step);	// 右下

	// 継ぎ目(異なる解像度のパッチ間を接続)の頂点インデックスを生成する
	void ConstructCWStitch_BL(int &numIndices, unsigned short *TileIndices, int step_larger);	// 左下	
	void ConstructCWStitch_TL(int &numIndices, unsigned short *TileIndices, int step_larger);	// 左上
	void ConstructCWStitch_TR(int &numIndices, unsigned short *TileIndices, int step_larger);	// 右上
	void ConstructCWStitch_BR(int &numIndices, unsigned short *TileIndices, int step_larger);	// 右下
	
	void ConstructCCWStitch_BL(int &numIndices, unsigned short *TileIndices, int step_larger);	// 左下	
	void ConstructCCWStitch_TL(int &numIndices, unsigned short *TileIndices, int step_larger);	// 左上
	void ConstructCCWStitch_TR(int &numIndices, unsigned short *TileIndices, int step_larger);	// 右上
	void ConstructCCWStitch_BR(int &numIndices, unsigned short *TileIndices, int step_larger);	// 右下

	// 継ぎ目/タイルの頂点インデックスの配列上オフセットを計算する

	// VBOを作成する
	void CreateVertexBufferObject();

	MeshGenerator();// デフォルトコンストラクタは使用禁止


public:
	
	MeshGenerator(int dim, int lv, float scale = 1.0);
	~MeshGenerator();

	
	// レンダリング前後処理をする(On/Off)
	void Bind();
	void Unbind();

	// レンダリングする
	void Draw(int tileres[]);

	
	//=================== タイル内トライアングル数の取得 ========================//
	int numTriangles_Tile(int step);		// タイルのトライアングル数を計算する
	int numTriangles_StitchCWCCW(int step_larger);// 非対称パッチのトライアングル数を計算する
	int	numTriangles_StitchSYM(int step);// 対称パッチのトライアングル数を計算する
	

	// 頂点配列とインデックス配列を出力するメソッド
	void GenerateTileVertices(int &numPoints, Vec2f **Vertices, int dimx, int dimy, float scale);
	void GenerateTileIndices(int &numElements, unsigned short **TileIndices, int numLevels, int dimx, int dimy);
	void GenerateStitchIndices(int &numElements, unsigned short **StitchIndices, int numLevels, int dimx, int dimy);

};



#endif // SEAMLESS_PATCHES_H //