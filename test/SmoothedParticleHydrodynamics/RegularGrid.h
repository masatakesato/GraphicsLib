/*=====================================================
	クラス名：	RegularGrid
	作者：		佐藤晶威
	作成日：	2008.5.12
	内容：		一様グリッド（空間データ構造）を用いて粒子検索を高速化する
	更新履歴：
			クラスの追加...2008.5.12
			メンバ変数/関数/コンストラクタの追加....2008.5.13
======================================================*/
#ifndef REGULARGRID_H
#define REGULARGRID_H




#include <malloc.h>

#include <iostream>
using namespace std;
#include <GL/glut.h>

#include <oreore/MathLib.h>




typedef struct RegularGrid
{
	int		**Cells;	// セルの1次元配列．セルごとに粒子ID(int型)を登録する
	int		*Capacity;	// セルに格納できる粒子の最大個数
	int		*Elms;		// 実際にセルに格納されている粒子個数

	double	Bin;		// セル辺の長さ
	int		Size[3];	// 幅, 高さ，奥行き方向のセル分割数
	int		cellNum;	// セルの総数

	Vec3d Boundary_min;// 領域最小点
	Vec3d Boundary_max;// 領域最大点
	
} RegularGrid;



void	RegularGrid_Init(RegularGrid *rg, Vec3d bb[2], double bin);	// RegularGridオブジェクトの初期化
int		RegularGrid_InsertID( RegularGrid *rg, int id, const Vec3d& pos );	//　粒子（座標pos，ID：id）をグリッドに登録する
void	RegularGrid_InsertID(RegularGrid *rg, int id, int cellindex);	//　cellindexで指定したセルにidを登録する
void	RegularGrid_Clear(RegularGrid *rg);								// 各セルの要素数を0にする	
void	RegularGrid_Info(const RegularGrid *rg);
void	RegularGrid_RenderGL(const RegularGrid *rg, int x, int y, int z);


#endif /* REGULARGRID_H */