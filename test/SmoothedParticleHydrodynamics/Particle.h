/*=====================================================
	クラス名：	Particle
	作者：		佐藤晶威
	作成日：	2008.4.1
	内容：		流体を離散化した単位要素である粒子
	更新履歴：
			ファイル説明コメントの追加...2008.4.1(Mon)
			粒子の属性を定義...2008.4.20
			アクセッサの追加...2008.05.23
======================================================*/


#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <malloc.h>

using namespace std;

#include <oreore/MathLib.h>



typedef struct Particle
{
	double	Radius;		// 粒子の半径
	Vec3d	Position;	// 位置
	Vec3d	Uvec;		// 速度ベクトル
	double	Pressure;	// 圧力
	double	Rho;		// 密度
	double	m;			// 質量


	// 以下試験的に実装．複相流のための流体属性
	double	viscosity;	// 粘度
	double	K;			// 表面張力係数
	double	Rho0;		// 初期密度

	// 流体表面粒子のレンダリングのため粒子属性
	//double	n[3];		// 法線ベクトル
	//double	curvature;	// 曲率

}Particle;




Particle *newParticle();									// 粒子を作成する
Particle *newParticle( const Vec3d& pos );					// 位置posに粒子を作成する

void Particle_Init( Particle *p, const Vec3d& pos );			// 粒子を初期化する
void Particle_Init( Particle *p, const Vec3d& pos, const Vec3d& u );// 粒子を初期化する

void Particle_Info( Particle *p );



#endif /* PARTICLE_H */