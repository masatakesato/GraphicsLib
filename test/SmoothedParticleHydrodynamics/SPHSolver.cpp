#include "SPHSolver.h"


#include <math.h>
#include <float.h>

#include <oreore/MathLib.h>





#define WATER_DENSITY		1000.0	// [kg/m^3]
#define SMOOTH_LENGTH		0.01	// [m]
#define PARTICLE_DISTANCE	0.005	// [m]
#define TIME_STEP			0.001	// [s]
#define VISCOSITY			1000		// ?絶対粘度？
#define COEFF_ST			0.01


#define K_SPRING	30000//30000
#define K_DAMPER	512//128



static double W(double r, double h)
{
	if(r > h) return 0.0;
	if(r >= 1.0e-6) return h/r - 1.0;
	
	return h/1.0e-9 - 1.0;
}


#define W_poly(r, h) ( h<r ? 0.0 : 315.0/(64.0*M_PI*pow(h,9)) * pow((h*h-r*r),3) )
#define W_poly_grad(r, h) ( h<r ? 0.0 : -945.0/(32.0*M_PI*pow(h,9)) * (h*h-r*r)*(h*h-r*r) )  
#define W_poly_lap(r, h) ( h<r ? 0.0 : 315.0/(64.0*M_PI*pow(h,9)) * (h*h-r*r)*(7*r*r-3*h*h) )

#define W_spiky(r, h) ( h<r ? 0.0 : 15.0/(M_PI*h*h*h*h*h*h) * (h-r)*(h-r)*(h-r) )
#define W_spiky_grad(r, h) ( h<=r ? 0.0 : -45.0/(M_PI*h*h*h*h*h*h) * (h-r)*(h-r)/r)

#define W_visc_lap(r, h) ( h<r ? 0.0 : 45.0/(M_PI*h*h*h*h*h*h) * (h-r) )


static double W_cspline(double r, double h)
{
	double q = r/h;
	if(q > 1) return 0.0;
	if(q >= 0.5) return 8.0/(M_PI*h*h*h) * 2*(1.0-q)*(1.0-q)*(1.0-q);
	return 8.0/(M_PI*h*h*h) * (6*q*q*q - 6*q*q + 1.0);
}



static void SPH_EvadeParticleCollision(SPH *sph);
static void SPH_EvadeParticleCollision_fromMPS(SPH *sph);


void NeighborList_Init(NeighborList *nlist, int numParticle)
{
	int i;

	nlist->Neighbors = (SPH_Neighbor **) malloc (sizeof(SPH_Neighbor*) * numParticle);
	nlist->Elms = (int *) malloc (sizeof(int) * numParticle);
	nlist->Capacity = (int *) malloc (sizeof(int) * numParticle);
	
	for(i=0; i<numParticle; i++)
	{
		nlist->Elms[i] = 0;
		nlist->Capacity[i] = 100;
		nlist->Neighbors[i] = (SPH_Neighbor *) malloc (sizeof(SPH_Neighbor)*100);
	}

}




//	[関数名]	NeighborList_GetNeighbor
//	[引き数]	*nlist：各粒子隣接粒子を保持する配列，start/end：配列の先頭/末尾，rg：グリッド，re：粒子有効半径
//	[戻り値]	なし
//	[内容]		近傍粒子(半径re内)の組み合わせリストを作る．
void NeighborList_GetNeighbor(NeighborList *nlist, 
							  Particle *particles, int start, int end, 
							  RegularGrid *rg, double re)
{
	double	bin_inv		= 1.0 / rg->Bin;
	const Vec3d& bb_min	= rg->Boundary_min;
	
	
	//========================== グリッドへの粒子ID登録と，近傍粒子の探索を同時に行う ======================//
	for(int i=start; i<=end; ++i)
	{
		const Vec3d& pos = particles[i].Position;
		
		//================== 既に一様グリッドに登録された粒子の中から，粒子iの近傍粒子を見つける ==================//
		int x = (int)( (pos.x - bb_min.x) * bin_inv );
		int y = (int)( (pos.y - bb_min.y) * bin_inv );
		int z = (int)( (pos.z - bb_min.z) * bin_inv );

		int cell_id = rg->Size[1]*rg->Size[2]*x + rg->Size[2]*y + z;// 粒子がいるセルのID
		if(cell_id < 0 || cell_id >= rg->cellNum)	continue;// 領域外の粒子は無視する


		for(x=-1; x<=1; x++) {
			for(y=-1; y<=1; y++) {
				for(z=-1; z<=1; z++)
				{
					int neighbor_cell = cell_id + rg->Size[1]*rg->Size[2]*x + rg->Size[2]*y + z;
					if(neighbor_cell < 0 || neighbor_cell >= rg->cellNum)	continue;

					int idnum = rg->Elms[neighbor_cell];
					
					if(nlist->Elms[i] + idnum >= nlist->Capacity[i])// 近傍リストの必要容量を確保する
					{
						int newsize = (nlist->Elms[i] + idnum) * 2;
						nlist->Neighbors[i] = (SPH_Neighbor *) realloc (nlist->Neighbors[i], sizeof(SPH_Neighbor) * newsize);
						nlist->Capacity[i] = newsize;
					}
					
					for( int k=0; k<idnum; ++k )
					{
						int j= rg->Cells[neighbor_cell][k];

						double dist = Distance( particles[i].Position, particles[j].Position );//Max(0.0, Distance(particles[i].Position, particles[j].Position) - PARTICLE_DISTANCE*0.5);

						if(dist > re) continue;// 有効半径よりも大きい場合は無視
						
						nlist->Neighbors[i][nlist->Elms[i]].NeighborID = j;
						nlist->Neighbors[i][nlist->Elms[i]].Distance = dist;
						
						nlist->Elms[i]++;
					}// end of k loop
					
				}// end of z loop
			}// end of y loop
		}// end of x loop
		
		//================================== 近傍探索の終わった粒子iをセルに登録する ================================//
		RegularGrid_InsertID(rg, i, cell_id);

	}// end of i loop

}



void NeighborList_Clear(NeighborList *nlist, int pnum)
{
	int i;
	for(i=0; i<pnum; i++) nlist->Elms[i] = 0;
}

void NeighborList_Info(NeighborList *nlist, int pnum)
{
	int i;
	int j;

	std::cout << "################# NeighborListクラス情報 ##############" << std::endl;

	for(i=0; i<pnum; i++)
	{
		std::cout << "[" <<i << "] ：..." << std::endl;
		
		for(j=0; j<nlist->Elms[i]; j++)
			std::cout << nlist->Neighbors[i][j].NeighborID << std::endl;
	}

}


void Init_ParticleAccerelation(SPH_Accerelation *pf, int pnum)
{
	int i;

	for(i=0; i<pnum; i++)
	{
		InitZero( pf->G );
		InitZero( pf->Visc );
		InitZero( pf->Press );
		InitZero( pf->Surface );
	}
}





//########################################################################################################
//########################################### Public Functions  ############################################
//#########################################################################################################

//	[関数名]	SPH
//　[内容]		デフォルトコンストラクタ．動作するような初期条件を実装
void SPH_Init(SPH *sph)
{
	Vec3d bb[2];
	
	
	//sph->Mu = VISCOSITY;------------------------------>Particleのメンバ変数にした
	//sph->Rho0 = WATER_DENSITY;------------------------------>Particleのメンバ変数にした
	sph->dT = TIME_STEP;
	sph->SmoothLength = SMOOTH_LENGTH;
	sph->ParticleDistance = PARTICLE_DISTANCE;
	//sph->Stiff = STIFFNESS;------------------------------>Particleのメンバ変数にした
	//sph->K = COEFF_ST;------------------------------>Particleのメンバ変数にした
	
	//InitVec(sph->Boundary_Min, -0.05, -0.06, -0.05);
	//InitVec(sph->Boundary_Max,	0.05, 0.06, 0.05);
	InitVec(sph->Boundary_Min, -0.5, -0.1, -0.007);
	InitVec(sph->Boundary_Max, 0.5, 0.2, 0.007);
	
	bb[0]	= sph->Boundary_Min;
	bb[1]	= sph->Boundary_Max;
	
	RegularGrid_Init(&sph->Grid, bb, sph->SmoothLength);// 一様グリッドを生成する
	//sph->Rho0 = SPH_DefaultDensity(sph->Re, sph->ParticleDistance, pow(sph->ParticleDistance,3)*sph->Rho0 );
	
	//SPH_GenParticle(sph,  -0.05, -0.05, -0.05, 0.05, 0.05, 0.05);
	SPH_GenParticle(sph, -0.5, -0.2, 0.0, -0.1, 0.2, 0.05);
	

	sph->Acc = (SPH_Accerelation *) malloc (sizeof(SPH_Accerelation) * sph->pNum);
	Init_ParticleAccerelation(sph->Acc, sph->pNum);
	
	//sph->C = (double *) malloc (sizeof(double) * sph->pNum);
	//sph->C2 = (double *) malloc (sizeof(double) * sph->pNum);
	//sph->ParticleDensity = (double *) malloc (sizeof(double) * sph->pNum);
	//sph->Pd0 = SPH_DefaultParticleDensity(sph->Re, sph->ParticleDistance);
	
	
	SPH_CalcBoundaryWeight(sph);
	
	NeighborList_Init(&sph->NList, sph->pNum);
	
	cout << "粒子数:    " << sph->pNum << " [個]"<<endl;
	cout << "粒子の質量:" << sph->Particles[0].m << " [kg]" << endl;
	//cout << "初期の粒子密度:  " << sph->Rho0 << endl;
	
}


//	[関数名]	SPH_SetSmoothLength
//	[引数]		h:カーネル計算時の近傍粒子探索半径
//	[戻り値]	なし
//　[内容]		reの値をメンバ変数Reに代入する．不正な入力の時はfalseを返す
void SPH_SetSmoothLength(SPH *sph, double h)
{
	sph->SmoothLength = h;
}


//	[関数名]	SPH_SetBoundary
//	[引数]		xmin, ymin, zmin: バウンディングボックス最小端点
//				xmax, ymax, zmax: バウンディングボックス最大端点
//	[戻り値]	なし
//　[内容]		境界を設定する
void SPH_SetBoundary(SPH *sph, 
					double xmin, double ymin, double zmin,
					double xmax, double ymax, double zmax)
{
	InitVec(sph->Boundary_Min, xmin, ymin, zmin);
	InitVec(sph->Boundary_Max, xmax, ymax, zmax);
}


//	[関数名]	SPH_AddParticle
//	[引数]		pos: 粒子の位置
//	[戻り値]	なし
//	[内容]		粒子を追加する
void SPH_AddParticle(SPH *sph, const Vec3d& pos)
{
	int newsize;
	
	if (sph->pNum == sph->pCapacity)
	{
		newsize = sph->pNum+1*2;
		sph->Particles = (Particle *) realloc (sph->Particles, sizeof(Particle) * newsize); 
		sph->pCapacity = newsize;
	}
	
	Particle_Init(&sph->Particles[sph->pNum],pos);
	sph->pNum++;
}


//	[関数名]	Run()
//	[引数]	なし
//	[戻り値]	なし
//　[内容]	SPH法のシミュレーションを動かす
void SPH_Run(SPH *sph)
{
	Vec3d f( 0.0, -9.8, 0.0 );
	
	SPH_Initialize(sph);
	
	// 1. 近傍粒子を探索する
	RegularGrid_Clear(&sph->Grid);// グリッド初期化
	NeighborList_Clear(&sph->NList, sph->pNum);// 各粒子の近傍リスト初期化
	NeighborList_GetNeighbor(&sph->NList,sph->Particles,0,sph->pNum-1,&sph->Grid,sph->SmoothLength);
	
	// 2. 各粒子の密度を計算
	SPH_UpdateParticleDensity(sph);
	
	// 3. 外力，粘性，圧力による力を計算
	SPH_AddForce(sph, f);
	SPH_Viscosity(sph);
	SPH_Pressure(sph);

	// 4. 表面張力を計算
	SPH_SurfaceTension_BT07(sph);	//SPH_SurfaceTension_MCG03(sph);
	
	// 4.5. 粒子の衝突計算
	SPH_EvadeParticleCollision_fromMPS(sph);
	//SPH_EvadeParticleCollision(sph);
	
	// 5. 各粒子の位置と速度を更新
	SPH_Advection(sph);
	
}




//########################################################################################################
//########################################## Private Functions  ############################################
//#########################################################################################################



//	[関数名]	SPH_Initialize()
//	[引数]		?
//	[戻り値]	なし
//　[内容]		
void SPH_Initialize(SPH *sph)
{
	int i;

	for(i=0; i<sph->pNum; i++)
	{
		//=========== 粒子のパラメータ初期化 ===========//
		sph->Particles[i].Rho = 0.0;// 密度ゼロ
		
		//InitVec(sph->Particles[i].n, 0,0,0);// 法線ゼロ
		//sph->ParticleDensity[i] = 0.0;
		
		//=============== 粒子を初期化 ==============//
		InitZero( sph->Acc[i].G );
		InitZero( sph->Acc[i].Press );
		InitZero( sph->Acc[i].Visc );
		InitZero( sph->Acc[i].Surface );
	}
}


//	[関数名]	SPH_AddForce()
//	[引数]	sph：SPH構造体，a：加速度
//	[戻り値]	なし
//　[内容]	指定領域内の粒子に外力を加えて速度ベクトルを更新する
void SPH_AddForce( SPH *sph, const Vec3d& acc )
{
	int i;
	for(i=0; i<sph->pNum; i++)
	{
		sph->Acc[i].G = acc;
	}
}



//	[関数名]	SPH_Viscosity()
//	[引数]	sph
//	[戻り値]	なし
//　[内容]	粘性項（拡散項）から粒子の加速度ベクトルを計算する
void SPH_Viscosity(SPH *sph)
{
	int				i, j, k;
	double			h;
	double			scale;
	double			dt;
	Vec3d			vec;
	NeighborList	*nlist;
	Particle		*p;
	ACC				*acc;

	p = sph->Particles;
	acc = sph->Acc;
	h = sph->SmoothLength;
	nlist = &sph->NList;
	dt = sph->dT;
	
	//======== μΣ[ m_j * (vj-vi)/ρj * ∇W(|rj-ri|) ] ===========//
	
	// i番目粒子と近傍粒子の間でラプラシアンを計算する
	for(i=0; i< sph->pNum; i++)
	{
		//================= 粒子間に働く力 ===================//
		for(k=0; k<nlist->Elms[i]; k++)
		{
			j = nlist->Neighbors[i][k].NeighborID;
			Subtract(vec, p[j].Uvec, p[i].Uvec);
			Scale(vec, dt);
			scale = 0.5*(p[i].viscosity + p[j].viscosity) / (p[j].Rho*p[i].Rho) * W_visc_lap(nlist->Neighbors[i][k].Distance, h);
			//scale = /*sph->Mu*/ / (p[j].Rho*p[i].Rho) * W_visc_lap(nlist->Neighbors[i][k].Distance, h);
			
			AddScaled( acc[i].Visc, vec, scale*p[j].m );// 粒子jから粒子iに働く粘性力
			AddScaled( acc[j].Visc, vec, -scale*p[i].m );// 粒子iから粒子jに働く粘性力
		}
	}// end of i loop
	
}



//	[関数名]	SPH_UpateParticleDensity()
//	[引数]	sph
//	[戻り値]	なし
//　[内容]	粒子の密度を計算する
void SPH_UpdateParticleDensity(SPH *sph)
{
	int	i, j, k;
	double Wr;
	double h;
	Particle	*p;
	//double		*pd;
	NeighborList *nlist;
	
	p = sph->Particles;
	h = sph->SmoothLength;
	//pd = sph->ParticleDensity;
	nlist = &sph->NList;
	
	//========= 各粒子の密度を計算する ==========//
	for(i=0; i<sph->pNum; i++)
	{
		p[i].Rho += p[i].m * W_poly(0,h);// 自分も入れる
		//========== 近傍粒子からの寄与を足し合わせる ===========//
		for(k=0; k<nlist->Elms[i]; k++)
		{
			
			j = nlist->Neighbors[i][k].NeighborID;

			Wr = //0.5*(p[i].m + p[j].m) * 
				W_poly(nlist->Neighbors[i][k].Distance,h);// 変更点：質量の平均を取って足し合わせ
			
			p[j].Rho += p[j].m*
						Wr;
			p[i].Rho += p[i].m*
						Wr;
			
			//Wr = W(r, sph->Re);
			//pd[i] += Wr;
			//pd[j] += Wr;
		}
		
		/*
		//=========== 壁面からの寄与を足し合わせる ===========//
		for(k=0; k<3; k++)
		{
			double dist = p[i].Position[k] - sph->Boundary_Min[k];
			if(dist < sph->Re)
			{
				p[i].Rho += SPH_WallDensity_At(dist,&sph->WallDensity);
				break;
			}
			dist = sph->Boundary_Max[k] - p[i].Position[k];
			if(dist < sph->Re)
			{
				p[i].Rho += SPH_WallDensity_At(dist,&sph->WallDensity);
				break;
			}	
		}
		*/
	}// end of i loop
	
	
	//========= 粒子の圧力を計算する ==========//
	double gamma = 7.0;
	double B;
	
	for(i=0; i<sph->pNum; i++)
	{	
		B = 500;//Max(1000.0, p[i].Rho0 * 10.0 * sqrt(2.0*9.81*(sph->Boundary_Max[1]-sph->Boundary_Min[1]) ) / gamma);
		//p[i].Rho = Max(p[i].Rho, p[i].Rho0);
		
		if(p[i].Rho < p[i].Rho0) p[i].Pressure = 0.0;
		else
			p[i].Pressure = 50.0*(p[i].Rho - p[i].Rho0);
			//p[i].Pressure = B * ( pow( Max(p[i].Rho, p[i].Rho0) /p[i].Rho0, gamma) - 1.0);
	}
}




//	[関数名]	SPH_Pressure()
//	[引数]	なし
//	[戻り値]	なし
//　[内容]	圧力項から粒子の加速度を計算する
void SPH_Pressure(SPH *sph)
{
	int	i, j, k;
	double scale;
	double h;
	double dt_inv;
	Vec3d vec;
	double dist;
	SPH_NeighborList *nlist;
	ACC			*acc;
	Particle	*p;
	
	h = sph->SmoothLength;
	p = sph->Particles;
	acc = sph->Acc;
	nlist = &sph->NList;
	dt_inv = 1.0 / sph->dT;
	
	for(i=0; i<sph->pNum; i++)
	{
		// 圧力項の計算
		for(k=0; k<nlist->Elms[i]; k++)
		{
			j = nlist->Neighbors[i][k].NeighborID;
			Subtract(vec, p[i].Position, p[j].Position);
			dist = nlist->Neighbors[i][k].Distance;
			
			scale = - 0.5 * (p[i].Pressure + p[j].Pressure) / (p[i].Rho*p[j].Rho) * W_spiky_grad(dist, h);
			
			AddScaled( acc[i].Press, vec, p[j].m * scale );// 粒子iが粒子jから受ける力を計算する
			AddScaled( acc[j].Press, vec, -p[i].m * scale );// 粒子jが粒子iから受ける力を計算する
		}
		
		//====== 壁からの圧力の計算 ========//
		for(k=0; k<3; k++)
		{	
			double v0;
			
			InitZero( vec );
			vec.xyz[k] = 1.0;
			v0 = DotProduct(vec, p[i].Uvec);
			dist = sph->ParticleDistance - (p[i].Position.xyz[k] - sph->Boundary_Min.xyz[k]);// 最小壁面までの距離
			
			if(dist < 4.0*sph->ParticleDistance && dist>1.0e-6)
			{
				acc[i].Press.xyz[k] += (K_SPRING*dist - K_DAMPER*v0);//vec[k] = stiff*dist - damp*v0; ←stiff = 30000, damp = 128.0
			}
			
			InitVec(vec, 0.0,0.0,0.0);
			vec.xyz[k] = -1.0;
			v0 = DotProduct(vec, p[i].Uvec);
			dist = sph->ParticleDistance - (sph->Boundary_Max.xyz[k] - p[i].Position.xyz[k]);// 最大壁面までの距離
			
			if(dist < 4.0*sph->ParticleDistance && dist>1.0e-6)
			{
				acc[i].Press.xyz[k] -= (K_SPRING*dist - K_DAMPER*v0);//vec[k] = stiff*dist - damp*v0; ←stiff = 30000, damp = 128.0
			}
			
		}// end of k loop

	}// end of i loop
	
}



//	[関数名]	SPH_SurfaceTension_BT07()
//	[引数]	なし
//	[戻り値]	なし
//　[内容]	表面張力を計算する
void SPH_SurfaceTension_BT07(SPH *sph)
{
	int			i, j, k;
	double		h;
	Vec3d		vec;
	double		k_st;
	double		scale;
	Particle	*p;
	ACC			*acc;
	SPH_NeighborList	*nlist;
	
	h = 4.0*sph->SmoothLength;
	p = sph->Particles;
	acc = sph->Acc;
	nlist = &sph->NList;
	k_st = sph->K;
	
	for(i=0; i<sph->pNum; i++)
	{
		for(k=0; k<nlist->Elms[i]; k++)
		{
			j = nlist->Neighbors[i][k].NeighborID;
			
			if(p[i].Rho0 != p[j].Rho0) continue;//....................同じ密度の流体にだけ適用

			Subtract(vec, p[i].Position, p[j].Position);
			
			scale = p[i].K * W_cspline(nlist->Neighbors[i][k].Distance,h) * sph->dT;
			//scale = k_st * W_cspline(nlist->Neighbors[i][k].Distance,h) * sph->dT;
			
			AddScaled(acc[i].Surface, vec, -scale * p[j].m/p[i].m);
			AddScaled(acc[j].Surface, vec, scale * p[i].m/p[j].m);
		}
	}
}





//	[関数名]	SPH_Advection()
//	[引数]	なし
//	[戻り値]	なし
//　[内容]	粒子位置を移動させる
void SPH_Advection(SPH *sph)
{
	const double& dt = sph->dT;
	Vec3d uvec;
	//RegularGrid *rg =&sph->Grid;


	//========= 各パーティクルの位置を更新する　=========//
	for( int i=0; i<sph->pNum; ++i )
	{	
		Particle* p = &sph->Particles[i];
		InitZero( uvec );
		
		//============= 粒子の速度を更新する =============//
		const ACC& acc = sph->Acc[i];
		// v = Δt * f/m
		uvec.x = dt * ( acc.G.x + acc.Press.x + acc.Visc.x + acc.Surface.x );
		uvec.y = dt * ( acc.G.y + acc.Press.y + acc.Visc.y + acc.Surface.y );
		uvec.z = dt * ( acc.G.z + acc.Press.z + acc.Visc.z + acc.Surface.z );
	
		Add( p->Uvec, uvec );

		//============= 粒子位置を更新する =============//
		AddScaled(p->Position, p->Uvec, dt);
		/*
		for( int k=0; k<3; ++k )
		{
			p->Position[k] = Clamp( sph->Boundary_Min[k],  sph->Boundary_Max[k], p->Position[k]);
		}
		*/
	}// end of i loop

}






//#########################################################################################################//
//######################################### Test Implementation  ##########################################//
//#########################################################################################################//

void SPH_GenParticle(SPH *sph, 
					 double xmin, double ymin, double zmin,
					 double xmax, double ymax, double zmax)
{
	double	i,j,k;
	double	bin;//粒子間隔
	Vec3d	bbmin, bbmax;
	Vec3d	pos;
	int		sizex, sizey, sizez;
	int		l;
	
	if(sph->ParticleDistance==0.0) return;

	bin = sph->ParticleDistance;
	bbmin.x = Max( sph->Boundary_Min.x, xmin );
	bbmin.y = Max( sph->Boundary_Min.y, ymin );
	bbmin.z = Max( sph->Boundary_Min.z, zmin );
	bbmax.x = Min( sph->Boundary_Max.x, xmax );
	bbmax.y = Min( sph->Boundary_Max.y, ymax );
	bbmax.z = Min( sph->Boundary_Max.z, zmax );
	

	sizex = (int)floor( ( bbmax.x - bbmin.x ) / bin );
	sizey = (int)floor( ( bbmax.y - bbmin.y ) / bin );
	sizez = (int)floor( ( bbmax.z - bbmin.z ) / bin );
	
	
	sph->pNum = sizex*sizey*sizez;
	sph->pCapacity = sizex*sizey*sizez;
	sph->Particles = (Particle *) malloc (sizeof(Particle) * sph->pCapacity);
	cout << sizez << endl;
	l = 0;
	// 粒子位置を設定する
	for(i=sizex-1; i>=0; i--){
		for(j=sizey-1; j>=0; j--){
			for(k=sizez-1; k>=0; k--)
			{
				InitVec(pos,
						(double)i * bin + bbmin.x,
						(double)j * bin + bbmin.y,
						(double)k * bin + bbmin.z
					);
				
				Particle_Init(&sph->Particles[l], pos);
				
				if(/*i>=sizex*0.25 && i<=sizex*0.75 && j>sizey*0.20 &&*/ j<=sizey*0.5 /*&& k>=sizez*0.25 && k<=sizez*0.75*/ )// 水以外の流体/////////////////////////////////////////////////////
				{	
					sph->Particles[l].Rho0 = 500.0;
					sph->Particles[l].m = pow(bin,3.0)*sph->Particles[l].Rho0;
					sph->Particles[l].viscosity = 500.0;
					sph->Particles[l].K = 8.0;
					
				}else
				{// 水/////////////////////////////////////////////////////////////////////////////////
					sph->Particles[l].Rho0 = 1000.0;
					sph->Particles[l].m = pow(bin,3.0)*sph->Particles[l].Rho0;
					sph->Particles[l].viscosity = 50.0;
					sph->Particles[l].K = 2.0;
				}
				
				l++;
			}
		}
	}

}



//	[関数名]	SPH_RenderParticles
//	[引数]		なし
//	[戻り値]	なし
//　[内容]		OpenGLを使って粒子を表示する
void SPH_RenderParticles(const SPH *sph)
{
	//const Vec3d& bb_min = sph->Boundary_Min;
	//const Vec3d& bb_max = sph->Boundary_Max;
	
	//=========================== 点を描く ============================//
	glPushMatrix();
	glPointSize(3.5);
	glBegin(GL_POINTS);
	
	for( int i=0; i<sph->pNum; ++i )
	{
		
		//if(sph->Particles[i].Position.x < bb_min[0] || sph->Particles[i].Position.y < bb_min[1] || sph->Particles[i].Position.z < bb_min[2]
		//	|| sph->Particles[i].Position.x > bb_max[0] || sph->Particles[i].Position.y > bb_max[1] || sph->Particles[i].Position.z > bb_max[2])
		//	glColor3d(1,0,0);
		//else
			//glColor3d(0.5,0.5,0);
		if(sph->Particles[i].Rho0 < 700)
			glColor3d(0.2,0.2,0.2);
		else
			glColor3d(0.4,0.5,1.0);
		//glColor3d(1.0-sph->C[i],1.0-sph->C[i], 1.0-sph->C[i]);
		glVertex3dv( sph->Particles[i].Position.xyz );
		
	}
	glEnd();
	glPopMatrix();
	

	//========================== 法線を描く ===========================//
	/*
	glPushMatrix();
	glBegin(GL_LINES);
	
	for( int i=0; i<sph->pNum; ++i )
	{
		glColor3d(1,1,1);
		glVertex3d( sph->Particles[i].Position.x, sph->Particles[i].Position.y, sph->Particles[i].Position.z );
		glColor3d(0.1,0.1,0.1);
		glVertex3d( sph->Particles[i].Position.x + sph->Particles[i].n[0]/sph->C[i]*0.00001,
					sph->Particles[i].Position.y + sph->Particles[i].n[1]/sph->C[i]*0.00001,
					sph->Particles[i].Position.z + sph->Particles[i].n[2]/sph->C[i]*0.00001);
	}
	glEnd();
	glPopMatrix();
	*/
}



//	[関数名]	SPH_RenderParticles
//	[引数]	なし
//	[戻り値]	なし
//　[内容]	OpenGLを使って粒子を表示する
void SPH_RenderBoundary(const SPH *sph)
{
	const Vec3d& min = sph->Boundary_Min,
				max = sph->Boundary_Max;
	
	glPushMatrix();
	glColor3d(0.4,0.4,0.4);
	glBegin(GL_QUADS);
		
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, min.y, max.z);
		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, max.y, min.z);
		
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, max.y, min.z);
		glVertex3d(max.x, max.y, min.z);
		
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(max.x, max.y, max.z);
		
		glVertex3d(min.x, min.y, max.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);
		
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(min.x, max.y, min.z);
		glVertex3d(min.x, max.y, max.z);
		
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(min.x, min.y, max.z);
		
	glEnd();
	glPopMatrix();
	
}



// 次のタイムステップで粒子がぶつからないように，位置と速度を修正する
// 別に反射しなくてもいい
// でも，めり込んだときは戻って欲しい
void SPH_EvadeParticleCollision(SPH *sph)
{
	int				i, j, k;
	int				pnum;
	double			dist;
	double			dt;
	NeighborList	*nlist;
	Particle		*p;
	
	dt = sph->dT;
	pnum = sph->pNum;
	nlist = &sph->NList;
	p = sph->Particles;
	
	for(i=0; i<pnum; i++)// 各粒子について．．．
	{
		for(k=0; k<nlist->Elms[i]; k++)// 各近傍粒子について．．．
		{
			Vec3d du;
			Vec3d c;// 法線方向ベクトル(i→j)
			double scale;
			Vec3d uvec;
			
			j = nlist->Neighbors[i][k].NeighborID;
			dist = nlist->Neighbors[i][k].Distance;
			
			if(dist > sph->ParticleDistance*0.5) continue;
			
			Subtract(c, p[j].Position, p[i].Position);// 粒子i→j方向のベクトル
			Normalize(c);
			
			Subtract(du, p[j].Uvec, p[i].Uvec);// 速度ベクトルの差分
			//Scale(du, dt);
			
			if(DotProduct(c,p[i].Uvec) < 0.0 && DotProduct(c, p[j].Uvec) > 0.0) continue;

			scale = 0.5 * (1.0  + 1.0) * DotProduct(du, c);// 反発係数
			Scale(uvec, c, scale);// 移動ベクトルを求める

			// 粒子iの位置と速度を更新
			Add( p[i].Uvec, uvec );
			AddScaled(p[i].Position, uvec, dt*p[i].m);
			

			// 粒子jの位置と速度を更新
			Add( p[j].Uvec, uvec );
			AddScaled(p[j].Position, uvec, -dt*p[j].m);
		}
		
	}

}





void SPH_EvadeParticleCollision_fromMPS(SPH *sph)
{
	double			rho0_i_inv, rho0_j_inv;
	double			dist;
	double			dt;
	NeighborList	*nlist;
	
	dt = sph->dT;
	nlist = &sph->NList;

	double rlim = sph->ParticleDistance*0.5;// 粒子間距離の最小値 =規定の1/2
	
	for( int i=0; i<sph->pNum; ++i )
	{
		Particle& pi = sph->Particles[i];
		rho0_i_inv = 1.0 / pi.Rho0;

		for( int k=0; k<nlist->Elms[i]; ++k )
		{
			int j = nlist->Neighbors[i][k].NeighborID;
			dist = nlist->Neighbors[i][k].Distance;

			Particle& pj = sph->Particles[j];

			// 粒子位置i→jへのベクトル
			double x = pj.Position.x - pi.Position.x;
			double y = pj.Position.y - pi.Position.y;
			double z = pj.Position.z - pi.Position.z;
			
			if(dist > rlim) continue;
			
			double vgx = 0.5 * (pi.Uvec.x + pj.Uvec.x);//本来は速度を粘度で按分（2速度の平均を粘度で重み付き足し合わせ）
			double vgy = 0.5 * (pi.Uvec.y + pj.Uvec.y);
			double vgz = 0.5 * (pi.Uvec.z + pj.Uvec.z);

			double vrx = pi.Rho0 * (pi.Uvec.x - vgx);
			double vry = pi.Rho0 * (pi.Uvec.y - vgy);
			double vrz = pi.Rho0 * (pi.Uvec.z - vgz);

			double vabs = (vrx*x + vry*y + vrz*z) / dist;

			if(vabs < 0) continue;

			double colrat = 0.15;// 反発係数
			double vrat = 1 + colrat;
			double vmx = vrat * vabs * x / dist;
			double vmy = vrat * vabs * y / dist;
			double vmz = vrat * vabs * z / dist;
			
			//=========== 粒子の位置と速度を更新 ============//
			
			rho0_j_inv = 1.0 / pj.Rho0; 

			pi.Uvec.x -= vmx * rho0_i_inv;
			pi.Uvec.y -= vmy * rho0_i_inv;
			pi.Uvec.z -= vmz * rho0_i_inv;
			pi.Position.x -= dt * vmx * rho0_i_inv;
			pi.Position.y -= dt * vmy * rho0_i_inv;
			pi.Position.z -= dt * vmz * rho0_i_inv;
			
			pj.Uvec.x += vmx * rho0_j_inv;
			pj.Uvec.y += vmy * rho0_j_inv;
			pj.Uvec.z += vmz * rho0_j_inv;
			pj.Position.x += dt * vmx * rho0_j_inv;
			pj.Position.y += dt * vmy * rho0_j_inv;
			pj.Position.z += dt * vmz * rho0_j_inv;
		}

	}
}








// [関数名]	SPH_DefaultParticleDensity
// [引数]	re：粒子の影響半径，dist：粒子間距離，m：粒子の質量
// [戻り値]	粒子数密度
// [内容]	粒子の初期密度を計算する（正確な密度から誤差が生じるので，粒子ベースで近似値を計算する）
double SPH_DefaultDensity(double re, double dist, double m)
{
	double x,y,z;
	double size_xyz;
	double rho0;
	
	size_xyz = (int)(re / dist);
	rho0 = 0.0;

	//cout << "sample_width: " << size_xyz << endl;
	
	for(x=-size_xyz; x<=size_xyz; x++) {
		for(y=-size_xyz; y<=size_xyz; y++) {
			for(z=-size_xyz; z<=size_xyz; z++)
			{
				//InitVec(curr_pos, dist*x, dist*y, dist*z);
				rho0 += W_poly( sqrt(dist*x*dist*x + dist*y*dist*y + dist*z*dist*z), re);
			}
		}
	}
	
	return m * rho0;
}





//	[関数名]	SPH_CalcBoundaryWeight
//	[引数]		sph
//	[戻り値]	なし
//	[内容]		壁面付近の粒子にかかる重み関数を前計算する(壁面との距離Re～ParticleDistanceのテーブル)
void SPH_CalcBoundaryWeight(SPH *sph)
{
	double bin;
	double x,y,z;
	double h;
	double size_xyz;
	Vec3d curr_pos, origin;
	double dist = sph->ParticleDistance;
	
	h = sph->SmoothLength;
	size_xyz = (int)(h / dist);
	bin = (h - dist) / 10.0;// 1辺りの幅を計算する
	

	int i;
	
	//===================== 壁面からの距離を計算する =========================//
	for(i=9; i>=0; i--)
		sph->WallDensity.h[i] = dist + bin*(double)i;
	
	//===================== 壁面からの距離を計算する =========================//
	for(i=0; i<10; i++)
	{	
		InitVec( origin, 0.0, sph->WallDensity.h[i], 0.0 );
		
		sph->WallDensity.Wr[i] = 0;

		for(x=-size_xyz; x<=size_xyz; x++) {
			for(y=0; y>=-size_xyz; y--) {
				for(z=-size_xyz; z<=size_xyz; z++)
				{
					InitVec(curr_pos, dist*x, dist*y, dist*z);
					sph->WallDensity.Wr[i] += W( Distance(origin,curr_pos), h);
				}
			}
		}
		cout << "壁からの距離 " << sph->WallDensity.h[i] << " の粒子の重み関数 = " << sph->WallDensity.Wr[i] << endl;
		
	}// end of h loop

}



//	[関数名]	SPH_WallDensity_At
//	[引数]		dist：壁面～粒子の距離，wd：壁密度関数データ（離散値）
//	[戻り値]	粒子密度の補正値
//	[内容]		壁面付近の粒子にかかる重み関数を前計算する(壁面との距離Re～ParticleDistanceのテーブル)
double SPH_WallDensity_At(double dist, SPH_WallDensity *wd)
{
	int i;

	for(i=0; i<10; i++)
	{
		if(dist > wd->h[i]) break;
	}
	
	return wd->Wr[Min(9,i)];
}







// [関数名]	SPH_Pressure_Implicit
// [引き数]	
// [戻り値]	
// [内容]	圧力項を陰解法で解く
void SPH_Pressure_Implicit(SPH *sph)
{
	int loop, iter;// 反復計算用
	int i, j, k;
	double	Pab;
	
	
	// ∇^2 P_n+1 = rho0/dT * ∇u'
	
}








//#################################################################################################
//##################################### 以下試験的に実装 ##########################################
//#################################################################################################



/*
//	[関数名]	SPH_SurfaceTension_MCG03()
//	[引数]	なし
//	[戻り値]	なし
//　[内容]	表面張力を計算する
void SPH_SurfaceTension_MCG03(SPH *sph)
{
	int i, j, k;
	double dist;
	double h;
	Vec3d vec;
	double *c, *c2;
	double Wr;
	Particle	*p;
	SPH_Accerelation *f;
	SPH_NeighborList *nlist;

	h = sph->SmoothLength;
	p = sph->Particles;
	f = sph->Acc;
	nlist = &sph->NList;
	c = sph->C;
	c2 = sph->C2;

	for(i=0; i<sph->pNum; i++)
	{
		c[i] = 0.0;
		InitVec(p[i].n, 0,0,0);// 法線ベクトル初期化
		c2[i] = 0.0;
	}

	//======================== カラー関数を求める ====================//
	for(i=0; i<sph->pNum; i++)
	{
		for(k=0; k<nlist->Elms[i]; k++)
		{
			j = nlist->Neighbors[i][k].NeighborID;
			dist = nlist->Neighbors[i][k].Distance;
			Wr = W_poly(dist, h);
			
			c[i] += p[j].m / (p[j].Rho) * Wr;
			c[j] += p[i].m / (p[i].Rho) * Wr;
		}
	}

	//====================== 法線ベクトルを計算する ====================//
	for(i=0; i<sph->pNum; i++)
	{	
		for(k=0; k<nlist->Elms[i]; k++)
		{
			j = nlist->Neighbors[i][k].NeighborID;
			dist = nlist->Neighbors[i][k].Distance;
			Subtract(vec, p[i].Position, p[j].Position);
			
			// 法線の計算．．．//
			AddScaled(p[i].n, vec, p[j].m/p[j].Rho * c[j] * W_poly_grad(dist,h) );
			AddScaled(p[j].n, vec, -p[i].m/p[i].Rho * c[i] * W_poly_grad(dist,h) );
		
			// ∇2Cの計算．．．//
			c2[i] += p[j].m/p[j].Rho * c[j] * W_poly_lap(dist, h);
			c2[j] += p[i].m/p[i].Rho * c[i] * W_poly_lap(dist, h);
		}
	}
	
	double sigma = 2.2;
	//======================== 表面張力を算出する ====================//
	for(i=0; i<sph->pNum; i++)
	{
		if(LengthSqrd(p[i].n) >= 1.0e+4  && p[i].Rho < sph->Rho0)
		{
			Normalize(p[i].n);
			AddScaled(f[i].Surface, p[i].n, -sigma*c2[i]/p[i].Rho);
		}
	}

}
*/


/*

液層上の圧力計算　→　重い粒子から計算する
・



*/

