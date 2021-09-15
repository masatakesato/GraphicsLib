/*=================================================================================
	クラス名：	SPH
	作者：		佐藤晶威
	作成日：	2008.5.9
	内容：		Smoothed Particle Hydrodynamicsによる非圧縮性流体の数値解計算
	更新履歴：
			ファイル説明コメントの追加.......................2008.1.14
			カーネル関数の追加..............................2008.03.22
			近傍粒子探索用変数（Elms, Neighbors)を追加.......2008.05.20
			外力項，粘性項，移流項，圧力項の計算機能を追加....2008.05.24
			表面張力を追加..................................2008.08.16
==================================================================================*/


#ifndef SPH_SOLVER_H
#define SPH_SOLVER_H

#include <iostream>
using namespace std;

#include "Particle.h"
#include "RegularGrid.h"


// 近傍粒子
typedef struct SPH_Neighbor
{
	int		NeighborID;	// 近傍粒子の配列インデックス
	double	Distance;	// 近傍粒子までの距離

}SPH_Neighbor;


// 近傍粒子リスト
typedef struct SPH_NeighborList
{
	SPH_Neighbor	**Neighbors;// 粒子ごとの近傍リスト
	int				*Elms;		// 各粒子の近傍粒子数
	int				*Capacity;	// 近傍粒子格納数の上限
}NeighborList;


// 粒子の加速度
typedef struct SPH_Accerelation
{
	Vec3d	G;			// 外力
	Vec3d	Visc;		// 粘性
	Vec3d	Press;		// 圧力
	Vec3d	Surface;	// 表面張力

}ACC;


// 粒子～壁面の距離に応じた密度を格納する構造体
typedef struct SPH_WallDensity
{
	double	h[10];			// 壁面からの距離
	double	Wr[10];			// 距離hの時のラプラシアン分母

}SPH_WallDensity;


// SPH本体
typedef struct SmoothedParticleHydrodynamics
{

	//double		Mu;				// 絶対粘度
	//double		Rho0;			// 初期密度
	double		dT;					// タイムステップ

	Particle	*Particles;			// 粒子配列
	int			pNum;				// 粒子の数
	int			pCapacity;			// 粒子の上限個数
	double		SmoothLength;		// カーネル半径
	double		ParticleDistance;	// 粒子間距離
	
	Vec3d		Boundary_Min;	// 境界ボックス（最小）
	Vec3d		Boundary_Max;	// 境界ボックス（最大）
	RegularGrid	Grid;				// 一様グリッド
	NeighborList NList;				// 隣接粒子の組み合わせ
	
	SPH_Accerelation	*Acc;		// 各粒子の加速度
	SPH_WallDensity		WallDensity;// 壁付近の粒子にかかる重み
	double		K;					// 表面張力の強さ
	
	//double		*C;					// 表面張力計算用のなにか
	//double		*C2;				// 表面張力計算用のなにか
	//double		*ParticleDensity;	// 表面粒子判定用の粒子数密度
	//double		Pd0;				// 粒子数密度の初期値
	
} SPH;





//####################### 前処理 #####################//
void NeighborList_Init(NeighborList *nlist, int numParticle);//
void NeighborList_GetNeighbor(NeighborList *nlist, 
							  Particle *particles, int start, int end, 
							  RegularGrid *rg, double maxradius);
void NeighborList_Info(NeighborList *nlist, int pnum);
void WallDensity_Init(SPH_WallDensity *wdensity);


//###################### 初期化 ######################//
SPH *newSPH();
void SPH_Init(SPH *sph);
void SPH_Initialize(SPH *sph);
void SPH_SetSmoothLength(SPH *sph, double h);// カーネル半径を設定
void SPH_SetBoundary(SPH *sph, 
					 double xmin, double ymin, double zmin,
					 double xmax, double ymax, double zmax);// シミュレーション領域を設定
void SPH_AddParticle(SPH *sph, double pos[3]);
void SPH_GenParticle(SPH *sph, 
					 double xmin, double ymin, double zmin,
					 double xmax, double ymax, double zmax);// 粒子を発生させる


//##################### 計算　########################//
void SPH_Run(SPH *sph);						// 1タイムステップぶんＳＰＨシミュレーションを実行
void SPH_Run( SPH *sph, const Vec3d& f_ext );	// 外力を与えてSPHシミュレーション実行？

void SPH_UpdateParticleDensity(SPH *sph);	// 密度計算
void SPH_AddForce( SPH *sph, const Vec3d& f );	// 外力項計算
void SPH_Viscosity(SPH *sph);				// 粘性項(拡散項)計算
void SPH_Advection(SPH *sph);				// 移流項の計算
void SPH_Pressure(SPH *sph);				// 圧力項計算


void SPH_SurfaceTension_BT07(SPH *sph);		// 表面張力計算 [BT07]
//void SPH_SurfaceTension_MCG03(SPH *sph);	// 表面張力計算 [MCG03]

//##################### 表示 #########################//
void SPH_RenderParticles(const SPH *sph);
void SPH_RenderBoundary(const SPH *sph);


//################### まだ実験中 #####################//
double SPH_DefaultDensity(double re, double dist, double m);
void SPH_CalcBoundaryWeight(SPH *sph);// 固定壁からの粒子重みを前計算する
double SPH_WallDensity_At(double dist, SPH_WallDensity *wd);// 前計算した重みを取り出す
double ParticleBoxIntersection(double origin[3], double direction[3], double bb_min[3], double bb_max[3]);




#endif /* SPH_SOLVER_H */