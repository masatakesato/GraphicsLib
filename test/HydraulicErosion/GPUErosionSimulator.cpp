#include	"GPUErosionSimulator.h"


#include	"oreore/MathLib.h"


namespace OreOreLib
{


	const int numNs	= 4;
	const Vec2i	Ns[] =
	{
		{+1, 0},
		{-1, 0},
		{0, +1},
		{0, -1},
	};


	const int Ninv[] =
	{
		1,
		0,
		3,
		2,
	};


	//#define STILLWATER_EROSION


#define	DEFAULT_TIMESTEP	0.05f

#define DAMPING				0.999f
#define DAMPING_REGOLITH	0.85f
#define SedimentCapacity	0.001f // 0.0001(岩) - 0.1(砂)
#define DepositionConst		0.5f	// 0.15(岩) - 0.9(砂)
#define DissolutionConst	0.5f	// 0.15(岩) - 0.9(砂)
#define Ck					0.01f	// 0.0001 - 0.01

#define minTiltAngle	0.05f



	//######################################################################################################//
	//																										//
	//											Public Functions											//
	//																										//
	//######################################################################################################//


	// Default Constructor
	GPUErosionSimulator::GPUErosionSimulator()
	{
	
	}



	// Constructor
	GPUErosionSimulator::GPUErosionSimulator( int dimx, int dimy, float length, int num_layers )
	{
		InitVec( m_Dim, dimx, dimy );
		m_Length	= length;
		m_numLayers	= num_layers;

		m_timeStep	= DEFAULT_TIMESTEP;
		m_Rho		= 1.0f;
		m_G			= 10.0f;
		m_Vaporize	= 0.5f;//0.01f;

		m_C			= m_Length * m_Length;

		m_UseMacCormackAdvection	= true;

	}


	// destructor
	GPUErosionSimulator::~GPUErosionSimulator()
	{
		Release();
	}




	void GPUErosionSimulator::Init();
	
	
	void GPUErosionSimulator::Release();

	
	
	void GPUErosionSimulator::BindVoxelData( ErosionVoxelData* pdata );
	
	
	void GPUErosionSimulator::UnbindVoxelData();















	// run simulation
	void GPUErosionSimulator::Update()
	{

		//=================== 隣接セルの水位から圧力差分を計算する ======================//
#ifdef _DEBUG
		CalcWaterPressure();
#endif

		//======================== 圧力差分をもとに加速度を計算する =====================//
#ifdef _DEBUG
		CalcAcceleration();
#endif

		//==================== 加速度をもとに"隣接セルへの流量"を計算する ===============//
		CalcWaterFlow();

		//=========================== 各セルの水位を更新する ============================//
		CalcWaterLevel();

		//========================== 法線マップを更新する ============================//
		CalcNormalMap();

		//========================= 各セルの水の速度を計算する =======================//
		CalcWaterVelocity();

		//================= 水流に基づいて浸食効果を計算する =========================//
		ForceBasedErosion();

		//========================= レゴリスの移動を計算する =========================//
		DissolutionBasedErosion();

	}



	// 
	//void GPUErosionSimulator::TestAdvection()
	//{
	//	if( m_UseMacCormackAdvection==true )
	//	{
	//		Advect_MacCormack( &m_ActualSediment_tmp, &m_Velocity, &m_ActualSediment );
	//	}
	//	else
	//	{
	//		memcpy( m_ActualSediment_tmp.m_pData, m_ActualSediment.m_pData, m_ActualSediment.size * m_Dim.x*m_Dim.y );
	//		Advect_SemiLagrangian( &m_ActualSediment_tmp, &m_Velocity, &m_ActualSediment );
	//	}
	//}



	// 地面を傾斜付きにする
	void GPUErosionSimulator::InitTerrain( const Texture2D* pTexTerrain )
	{
		if( !pTexTerrain->HasCPUBuffer() )
			return;


		const int center_x = m_Dim.x / 2;
		const int center_y = m_Dim.y / 2; 

		//m_TerrainHeight.Load_Luminance( "../texture/heightmap.png" );

		for( int y=0; y<m_Dim.y; ++y )
		{
			for( int x=0; x<m_Dim.x; ++x )
			{
				//==================== ハイトマップの初期化 ====================//
				// ピラミッド型
				//*(float *)m_TerrainHeight(x, y)	= (abs(center_x - x) + abs(center_y - y));//10.5f * max( ( float(center_x + center_y) - ((float)abs(center_x - x) + (float)abs(center_y - y)) ), 0.0f ) / float(center_x + center_y);
				// x方向の傾斜
				//*(float *)m_TerrainHeight.Data(x, y) = float(x) * 0.2f;
				// 平面
				*(float *)m_TerrainHeight.Data(x, y) *= 50.0f * m_Length;

				//==================== 水分布の初期化 ==========================// 
				// 一様
				*(float *)m_WaterLevel(x,y) = m_C * 0.0f;
				*(float *)m_AverageWaterHeight(x,y) = *(float *)m_WaterLevel(x,y);

				//=============== 運搬中の土砂量を初期化 =======================//
				*(float *)m_ActualSediment(x,y) = 0.0f;

			}// end of x loop
		}// end of y loop

	}



	void GPUErosionSimulator::InitAdvectionTest()
	{/*
	 int x, y;

	 //============== 速度場を適当に初期化する ==================//
	 for( y=0; y<m_Dim.y; ++y )
	 {
	 for( x=0; x<m_Dim.x; ++x )
	 {
	 Vec2f *velocity = (Vec2f *)m_Velocity(	x, y );	

	 InitVec( *velocity, 5.0f, 5.0f );

	 }// end of x loop
	 }// end of y loop


	 //============= 土砂の堆積量を適当に初期化する =============//
	 Vec2f center = {30.0f, 30.0f};

	 for( y=0; y<m_Dim.y; ++y )
	 {
	 for( x=0; x<m_Dim.x; ++x )
	 {
	 Vec2f pos = { float(x), float(y) };

	 //if( Distance( pos, center ) < 25.0f )
	 *(float *)m_ActualSediment(x,y) = 1.0f;

	 }// end of x loop
	 }// end of y loop
	 */

		for( int y=0; y<m_Dim.y; ++y )
		{
			for( int x=0; x<m_Dim.x; ++x )
			{
				Vec2f pos = { float(x), float(y) };

				//if( Distance( pos, center ) < 25.0f )
				*(float *)m_WaterLevel(x, y) += 0.5f;

			}// end of x loop
		}// end of y loop
	}



	void GPUErosionSimulator::DropRandomWater()
	{

		for( int i=0; i<50; ++i )
		{
			int x = rand()%(m_Dim.x-1);
			int y = rand()%(m_Dim.y-1);

			//*(float *)m_WaterLevel(x, y) += m_C * 2.0f;

			for( int y_=y; y_<y+2; ++y_)
			{
				for( int x_=x; x_<x+2; ++x_ )
				{
					*(float *)m_WaterLevel(x_, y_) += m_C * 2.5f;
				}
			}

		}

		/*
		for( int y=250; y<300; ++y)
		{
		for( int x=275; x<300; ++x )
		{
		if( *(float *)m_WaterLevel(x, y) < m_C * 5.0f )
		*(float *)m_WaterLevel(x, y) = m_C * 5.0f;

		}
		}
		*/
	}



	float GPUErosionSimulator::GetToalWaterAmount()
	{
		float result = 0.0f;

		for( int y=0; y<m_Dim.y; ++y )
			for( int x=0; x<m_Dim.x; ++x )
				result += *(float *)m_WaterLevel(x, y);


		return result;
	}





	//######################################################################################################//
	//																										//
	//											Private Functions											//
	//																										//
	//######################################################################################################//


	void GPUErosionSimulator::CalcNormalMap( )
	{
		int x, y;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{

				Vec3f ut = 
				{
					1.0f,
					0.0f,
					( *(float *)m_TerrainHeight(x+1, y) - *(float *)m_TerrainHeight(x-1, y) ) / ( m_Length * 2.0f )
				};

				Vec3f vt =
				{
					0.0f,
					1.0f,
					( *(float *)m_TerrainHeight(x, y+1) - *(float *)m_TerrainHeight(x, y-1) ) / ( m_Length * 2.0f )
				};

				Vec3f uw =
				{
					1.0f,
					0.0f,
					( *(float *)m_WaterLevel(x, y+1) - *(float *)m_WaterLevel(x, y-1) ) / ( m_Length * 2.0f ) + ut.z
				};

				Vec3f vw =
				{
					0.0f,
					1.0f,
					( *(float *)m_WaterLevel(x, y+1) - *(float *)m_WaterLevel(x, y-1) ) / ( m_Length * 2.0f ) + vt.z
				};

				Normalize(ut);
				Normalize(vt);
				Normalize(uw);
				Normalize(vw);

				//================== 地形の法線マップを計算する ===================//
				Vec3f *n_terrain = (Vec3f *)m_TerrainNormal(x,y);
				CrossProduct( *n_terrain, ut, vt );

				//=================== 水面の法線マップを計算する ==================//
				Vec3f *n_water = (Vec3f *)m_WaterNormal(x,y);
				CrossProduct( *n_water, uw, vw );

			}// end of x loop
		}// end of y loop

	}



	float GPUErosionSimulator::TiltAngle( const Vec2i &pos, const Texture2D &normalmap )
	{
		const Vec3f up = {0,0,1};
		const Vec3f n  = *(Vec3f *)normalmap.Data( pos.x, pos.y );

		return acos( DotProduct(n, up) );
	}




	// 水流が土砂を運べる量を計算する. ピクセル単位で計算可能
	void GPUErosionSimulator::CalcSedimentTransportCapacity()
	{
		Vec2i pos;
		const Vec3f up = {0,0,1};

		for( pos.y=0; pos.y<m_Dim.y; ++pos.y )
		{
			for( pos.x=0; pos.x<m_Dim.x; ++pos.x )
			{
				const Vec2f *velocity	= (Vec2f *)m_Velocity(pos.x, pos.y);
				float *S				= (float *)m_SedimentTransportCapacity(pos.x, pos.y);

				float tiltAngle = TiltAngle( pos, m_TerrainNormal );
				//tiltAngle =	(tiltAngle < minTiltAngle) ? minTiltAngle : tiltAngle;

				*S = SedimentCapacity * Length( *velocity ) * sin( tiltAngle );

			}// end of x loop
		}// end of y loop

	}



	void GPUErosionSimulator::CalcMaxRegolithThickness()
	{
		Vec2i pos;
		const Vec3f up = {0,0,1};

		for( pos.y=0; pos.y<m_Dim.y; ++pos.y )
		{
			for( pos.x=0; pos.x<m_Dim.x; ++pos.x )
			{
				const Vec2f *velocity	= (Vec2f *)m_Velocity(pos.x, pos.y);
				float *regolith			= (float *)m_MaxRegolithThickness(pos.x, pos.y);

				*regolith = min( Ck, *(float *)m_WaterLevel(pos.x, pos.y) );

			}// end of x loop
		}// end of y loop
	}







	// 土砂の浸食と堆積を計算する
	void GPUErosionSimulator::CalcDepositionDissolution()
	{
		int x, y;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				Vec2i pos = {x,y};

				float capacity		= *(float *)m_SedimentTransportCapacity( x, y );// * sedimentCapacityFactor;
				float *sediment	= (float *)m_ActualSediment( x, y );


				if( *sediment > capacity )// 流され中の土砂量が流送限度量を超えた場合
				{
					// 土砂を置いてく
					float sedimentDif				= DepositionConst * ( *sediment - capacity );

					*(float *)m_TerrainHeight(x,y)	+= sedimentDif;// 置いてく分は地形に足す
					*sediment						-= sedimentDif;// 運んでいる土砂からは引く
				}
				else// まだ輸送限度まで余力がある
				{
					// 地面を削る
					float maxSediment	= capacity;// - *(float *)m_TerrainHeight(x,y);
					if( maxSediment < *sediment )	continue;

					float sedimentDif	= DissolutionConst * ( maxSediment - *sediment );	// 水に溶かせる土砂の量
					sedimentDif			= min( *(float *)m_TerrainHeight(x,y), sedimentDif );// 但し、地層以下

					*(float *)m_TerrainHeight(x,y)	-= sedimentDif;// 浸食ぶんだけ地面を低くする
					*sediment						+= sedimentDif;// 流送土砂を増やす
				}

			}// end of x loop
		}// end of y loop


	}



	void GPUErosionSimulator::CalcRegolithDepositionDissolution()
	{
		int x, y;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				float max_regolith_thickness	= *(float *)m_MaxRegolithThickness( x, y );	// レゴリスの堆積許容限界
				float *curr_regorith_thickness	= (float *)m_ActualRegolithThickness( x, y );// レゴリスの堆積量

				if( *curr_regorith_thickness > max_regolith_thickness )// レゴリスの量が許容限界を超えた場合
				{
					// レゴリスを地面に降ろす
					float regolithDif				=  ( *curr_regorith_thickness - max_regolith_thickness );// 許容限界オーバー分

					*(float *)m_TerrainHeight(x,y)	+= regolithDif;// 余剰レゴリスを地形に足す
					*curr_regorith_thickness		-= regolithDif;// レゴリス堆積量から余剰分を減算する
				}
				else// レゴリス許容限界に達していない場合
				{
					float maxRegolith	= max_regolith_thickness;
					if( maxRegolith < *curr_regorith_thickness )	continue;

					float regolithDif	= ( max_regolith_thickness - *curr_regorith_thickness );
					regolithDif			= min( *(float *)m_TerrainHeight(x,y), regolithDif );

					*(float *)m_TerrainHeight(x,y)	-= regolithDif;// 地層を削る
					*curr_regorith_thickness		+= regolithDif;// 削った分レゴリス層に足す

					// 地面を削る
					//float regolithDif				= min(  ( max_regolith_thickness - *curr_regorith_thickness ), *(float *)m_TerrainHeight(x,y) );

					//*(float *)m_TerrainHeight(x,y)	-= regolithDif;// 浸食ぶんだけ地面を低くする
					//*curr_regorith_thickness		+= regolithDif;// 流送土砂を増やす
				}

			}// end of x loop
		}// end of y loop


	}



	void GPUErosionSimulator::CalcRegolithThickness()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos			= {x, y};
				float flowChange	= 0.0f;
				Vec4f *prevFlow		= (Vec4f *)m_RegolithOutflow(pos.x, pos.y);	// 現在の水の流出量
				float prevThickness	= *(float *)m_ActualRegolithThickness(pos.x, pos.y);

				for( int n=0; n<numNs; ++n )
				{
					float inFlow;
					float outFlow;

					// 自分から隣接セルへの流出量を取得
					outFlow		= prevFlow->xyzw[n];

					// 隣接セルから自分への流入量を取得
					Vec4f* neighbor_cell = (Vec4f *)m_RegolithOutflow( pos.x+Ns[n].x, pos.y+Ns[n].y );

					// 境界条件を加味。境界方向からの流入量はゼロにする
					if( n==0 && x==m_Dim.x-1 )		inFlow	= outFlow;// 右端
					else if( n==1 && x==0 )			inFlow	= outFlow;// 左端
					else if( n==2 && y==m_Dim.y-1 )	inFlow	= outFlow;// 上端
					else if( n==3 && y==0 )			inFlow	= outFlow;// 下端
					else							inFlow	= neighbor_cell->xyzw[ Ninv[n] ];	// それ以外

					// トータルの流出量 += 隣接セルから自分への流量 - 自分から隣接セルへの流量
					flowChange += ( inFlow - outFlow );
				}// end of neighbor loop

				// トータル流出量を計算する
				flowChange *= ( m_timeStep / m_C );

				// 新しい水位を計算する
				float newThickness = max( ( prevThickness + flowChange ), 0.0f );
				*(float *)m_ActualRegolithThickness(pos.x, pos.y) = newThickness;

			}// end of x loop
		}// end of y loop

	}






	void GPUErosionSimulator::ForceBasedErosion()
	{
		//================= 各セルが運搬可能な土砂容量を更新する ==================//
		CalcSedimentTransportCapacity();

		//=================== 地面を削るor土砂を置くの判断をする ==================//
		CalcDepositionDissolution();

		//============================== 土砂を移動する ===========================//
		if( m_UseMacCormackAdvection==true )
		{
			Advect_MacCormack( &m_ActualSediment_tmp, &m_Velocity, &m_ActualSediment );
		}
		else
		{
			memcpy( m_ActualSediment_tmp.m_pData, m_ActualSediment.m_pData, m_ActualSediment.size * m_Dim.x*m_Dim.y );
			Advect_SemiLagrangian( &m_ActualSediment_tmp, &m_Velocity, &m_ActualSediment );
		}

	}



	void GPUErosionSimulator::DissolutionBasedErosion()
	{
		CalcMaxRegolithThickness();

		// regotith deposition and dissolution
		CalcRegolithDepositionDissolution();

		// calc regolith outflow
		CalcRegolithOutflow();

		// update regolith revel
		CalcRegolithThickness();

	}




	void GPUErosionSimulator::CalcRegolithOutflow()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos = {x, y};

				const Vec4f* oldoutFlow = (Vec4f *)m_RegolithOutflow(x,y);
				Vec4f outFlow;

				for( int n=0; n<numNs; ++n )
				{
					Vec2i n_pos;
					Add( n_pos, pos, Ns[n] );

					//============== 近傍セルのリゴリスの高低差から圧力を計算 ==============//
					float curr_regolith_lv = *(float *)m_ActualRegolithThickness( pos.x, pos.y );
					float neighbor_regolith_lv = *(float *)m_ActualRegolithThickness( n_pos.x, n_pos.y );

					// posセルの高さを計算する.現状1レイヤーだけ
					float h		= curr_regolith_lv + *(float *)m_TerrainHeight( pos.x, pos.y );

					// neighborセルの高さを計算する
					float n_h	= neighbor_regolith_lv + *(float *)m_TerrainHeight( n_pos.x, n_pos.y );

					// 圧力差分を計算する
					//float dP	= m_Rho * m_G * ( h - n_h );
					//float accel	= dP / ( m_Rho * m_Length );

					//============== 圧力をもとに水の加速度を計算 ================//
					float accel	= m_G * ( h - n_h ) / m_Length;

					//================== 加速度から流量を計算 ====================//
					// ダンピングを加えてシミュレーション安定させる & 流出(正の値)の場合だけ値を登録する
					outFlow.xyzw[n]	= max( DAMPING_REGOLITH * oldoutFlow->xyzw[n] + m_timeStep * m_C * accel, 0.0f );// 論文中の(

				}// end of neighbor loop

				// 流出の総量が水位を超えないようにスケーリングする
				float maxRegolith	= *(float *)m_ActualRegolithThickness(x,y) * m_C;
				float regolithOut	= ( outFlow.x + outFlow.y + outFlow.z + outFlow.w ) * m_timeStep;

				if( regolithOut==0.0f )
				{
					InitVec( outFlow, 0.0f, 0.0f, 0.0f, 0.0f );
				}
				else
				{
					float regolithScaleFactor = maxRegolith / regolithOut;
					if( regolithScaleFactor < 1.0f )	Scale( outFlow, regolithScaleFactor );				
				}

				*(Vec4f *)m_RegolithOutflow(x,y) = outFlow;

			}// end of x loop
		}// end of y loop



	}



#ifdef _DEBUG

	void GPUErosionSimulator::CalcWaterPressure()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos = {x, y};

				float *dP = (float *)m_DP.Data(x,y);

				for( int n=0; n<numNs; ++n )
				{
					Vec2i n_pos;
					Add( n_pos, pos, Ns[n] );


					float curr_water_lv = *(float *)m_WaterLevel( pos.x, pos.y );
					float neighbor_water_lv = *(float *)m_WaterLevel( n_pos.x, n_pos.y );

					// posセルの高さを計算する.現状1レイヤーだけ
					float h		= curr_water_lv + *(float *)m_TerrainHeight( pos.x, pos.y );

					// neighborセルの高さを計算する
					float n_h	= neighbor_water_lv + *(float *)m_TerrainHeight( n_pos.x, n_pos.y );

					// 圧力差分を計算する
					*( dP + n )	= m_Rho * m_G * ( h - n_h );

				}// end of neighbor loop

			}// end of x loop
		}// end of y loop

	}



	void GPUErosionSimulator::CalcAcceleration()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos = {x, y};

				float *pressure = (float *)m_DP(x,y);
				float *newAccel	= (float *)m_Accelation(x,y);

				for( int n=0; n<numNs; ++n )
				{
					*( newAccel + n )	= *( pressure + n ) / ( m_Rho * m_Length );	
				}// end of neighbor loop

			}// end of x loop
		}// end of y loop

	}

#endif


	void GPUErosionSimulator::CalcWaterFlow()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos = {x, y};

				const Vec4f* oldoutFlow = (Vec4f *)m_WaterFlow(x,y);
				Vec4f outFlow;

				for( int n=0; n<numNs; ++n )
				{
					Vec2i n_pos;
					Add( n_pos, pos, Ns[n] );

					//============== 近傍セルの水位差から圧力を計算 ==============//
					float curr_water_lv = *(float *)m_WaterLevel( pos.x, pos.y );
					float neighbor_water_lv = *(float *)m_WaterLevel( n_pos.x, n_pos.y );

					// posセルの高さを計算する.現状1レイヤーだけ
					float h		= curr_water_lv + *(float *)m_TerrainHeight( pos.x, pos.y );

					// neighborセルの高さを計算する
					float n_h	= neighbor_water_lv + *(float *)m_TerrainHeight( n_pos.x, n_pos.y );

					// 圧力差分を計算する
					//float dP	= m_G * ( h - n_h );
					//float accel	= min( dP / ( m_Length ), m_Length );

					//============== 圧力をもとに水の加速度を計算 ================//
					float accel	= m_G * ( h - n_h ) / m_Length;

					//================== 加速度から流量を計算 ====================//

					// ダンピングを加えてシミュレーション安定させる & 流出(正の値)の場合だけ値を登録する
					outFlow.xyzw[n]	= max( DAMPING * oldoutFlow->xyzw[n] + m_timeStep * m_C * accel, 0.0f );// 論文中の(1)

				}// end of neighbor loop

				// 現在の水量
				float maxWater	= m_Length * m_Length * *(float *)m_WaterLevel(x,y);
				// 流出させたい水量
				float waterOut	= ( outFlow.x + outFlow.y + outFlow.z + outFlow.w ) * m_timeStep;

				if( waterOut == 0.0f )
				{
					InitVec( outFlow, 0.0f, 0.0f, 0.0f, 0.0f );
				}
				else
				{
					float waterScaleFactor = maxWater / waterOut;
					if( waterScaleFactor < 1.0f )	Scale( outFlow, waterScaleFactor );
				}

				*(Vec4f *)m_WaterFlow(x,y) = outFlow;

			}// end of x loop
		}// end of y loop

	}


	void GPUErosionSimulator::CalcWaterLevel()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos				= {x, y};
				Vec4f *prevFlow			= (Vec4f *)m_WaterFlow(pos.x, pos.y);	// 現在の水の流出量
				float prevWaterLevel	= *(float *)m_WaterLevel(pos.x, pos.y);

				float inFlows	= 0.0f;
				float outFlows	= 0.0f;

				for( int n=0; n<numNs; ++n )
				{
					float inFlow;
					float outFlow;

					// 自分から隣接セルへの流出量を取得
					outFlow		= prevFlow->xyzw[n];

					// 隣接セルから自分への流入量を取得
					Vec4f* neighbor_cell = (Vec4f *)m_WaterFlow( pos.x+Ns[n].x, pos.y+Ns[n].y );

					// 境界条件を加味。境界方向からの流入量はゼロにする
					if( n==0 && x==m_Dim.x-1 )		inFlow	= outFlow;// 右端
					else if( n==1 && x==0 )			inFlow	= outFlow;// 左端
					else if( n==2 && y==m_Dim.y-1 )	inFlow	= outFlow;// 上端
					else if( n==3 && y==0 )			inFlow	= outFlow;// 下端
					else							inFlow	= neighbor_cell->xyzw[ Ninv[n] ];	// それ以外

					inFlows	+= inFlow;
					outFlows += outFlow;

				}// end of neighbor loop

				// 新しい水位を計算する
				float newWaterLevel = prevWaterLevel + ( m_timeStep * ( inFlows-outFlows)) / (m_Length * m_Length);// 式(1)の1個下の式
				*(float *)m_WaterLevel(pos.x, pos.y) = newWaterLevel * (1.0f - m_Vaporize );

				// 直前タイムステップと現タイムステップとの水位平均値を更新する
				*(float *)m_AverageWaterHeight(pos.x, pos.y) = 0.5f * ( prevWaterLevel + newWaterLevel );

			}// end of x loop
		}// end of y loop


	}


	void GPUErosionSimulator::CalcWaterVelocity()
	{
		int k;
		Vec2i pos;

		for( pos.y=0; pos.y<m_Dim.y; ++pos.y )
		{
			for( pos.x=0; pos.x<m_Dim.x; ++pos.x )
			{
				const Vec4f *f_curr	= (Vec4f *)m_WaterFlow(pos.x, pos.y);
				Vec2f *velocity		= (Vec2f *)m_Velocity(pos.x, pos.y);

				for( k=0; k<numNs; k+=2 )// 軸方向毎に
				{
					const float velocityFactor = m_Length * m_Length * *(float *)m_AverageWaterHeight(pos.x, pos.y);
					float W = 0.0f;
					Vec2i n_pos;

					//============= 軸方向隣接セル間の水位変化量を計算する ================//
					// 直前セル→自分セルへの流入量を加算
					Add( n_pos, pos, Ns[k+1] );// 直前セルへのインデックス取得
					Vec4f *f_prev = (Vec4f *)m_WaterFlow(n_pos.x, n_pos.y);// 直前セルの流出量取得				
					if( pos.x > 0 &&  pos.y > 0)// 境界条件
						W += *(f_prev->xyzw + k);// 自分セルへの流入量取得

					// 直後セル->自分セルへの流入量を加算
					Add( n_pos, pos, Ns[k] );// 直後セルへのインデックス取得
					Vec4f *f_next = (Vec4f *)m_WaterFlow(n_pos.x, n_pos.y);// 直後セルの流出量取得
					if( pos.x <m_Dim.x-1 &&  pos.y < m_Dim.y-1 )// 境界条件
						W += *(f_next->xyzw + k+1);// 自分セルへの流入量取得

					// 直前直後両セルへの流出量を減算
					if( pos.x > 0 &&  pos.y > 0)// 境界条件
						W -= *(f_curr->xyzw + k+1);

					if( pos.x <m_Dim.x-1 &&  pos.y < m_Dim.y-1 )// 境界条件
						W -= *(f_curr->xyzw + k);

					W *= 0.5f;

					//========================= 軸方向の速度を計算する ====================//
					if( velocityFactor>0.0 )	W /= velocityFactor;

					velocity->xy[k]	= W;

				}// end of k loop

			}// end of x loop
		}// end of y loop

	}

	/*
	// おかしい　破棄?
	float bilinear( const Vec2f &pos, const Texture2D *in )
	{
	const int x_floor	= (int)floor(pos.x);
	const int x_ceil	= (int)ceil(pos.x);
	const int y_floor	= (int)floor(pos.y);
	const int y_ceil	= (int)ceil(pos.y);

	const float x		= max(min(pos.x - float(x_floor),1.0f),0.0f);
	const float y		= max(min(pos.y - float(y_floor),1.0f),0.0f);
	const float f00		= *(float *)in->Data( x_floor, y_floor );
	const float f10		= *(float *)in->Data( x_ceil, y_floor );
	const float f01		= *(float *)in->Data( x_floor, y_ceil );
	const float f11		= *(float *)in->Data( x_ceil, y_ceil );

	return f00 * (1.0f-x) * (1.0f-y) + f10 * x * (1.0f-y) + f01 * (1.0f-x) * y + f11 * x * y;
	}
	*/


	template<typename T>
	T bilinear( const Vec2<T> &pos, const T &c00, const T &c10, const T &c01, const T &c11)
	{
		//#if 1
		T a = c00 * (T(1) - pos.x) + c10 * pos.x;
		T b = c01 * (T(1) - pos.x) + c11 * pos.x;
		return a * (T(1) - pos.y) + b * pos.y;
		//#else
		//    return (T(1) - pos.x) * (T(1) - pos.y) * c00 + 
		//        pos.x * (T(1) - pos.y) * c10 +
		//        (T(1) - pos.x) * pos.y * c01 +
		//        pos.x * pos.y * c11;
		//#endif
	}



	// Semi-Lagrangian Advection
	void GPUErosionSimulator::Advect_SemiLagrangian( const Texture2D *in, const Texture2D *velocity, Texture2D *out )
	{
		int x, y;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				// セル位置ベクトルの逆方向にさかのぼる
				const Vec2f *v = (Vec2f *)velocity->Data(x,y);
				const Vec2f prev_pos =
				{
					float(x) - m_timeStep * v->x,
					float(y) - m_timeStep * v->y
				};

				float c00 = *(float *)in->Data( max(prev_pos.x,0),					max(prev_pos.y,0) );
				float c01 = *(float *)in->Data( max(prev_pos.x,0),					min(prev_pos.y+1, in->Height()-1) );
				float c10 = *(float *)in->Data( min(prev_pos.x+1,in->Width()-1),	max(prev_pos.y,0) );
				float c11 = *(float *)in->Data( min(prev_pos.x+1,in->Width()-1),	min(prev_pos.y+1, in->Height()-1) );

				const Vec2f p = { prev_pos.x - floor(prev_pos.x), prev_pos.y - floor(prev_pos.y) };
				*(float *)out->Data(x,y)	= bilinear<float>( p, c00, c10, c01, c11 ) * 0.25f;

				// 遡った位置で、近傍セルの値をバイリニア補間
				//*(float *)out->Data(x,y) = bilinear( prev_pos, in );

			}// end of x loop
		}// end of y loop

	}


	// MacCormack Advection
	void GPUErosionSimulator::Advect_MacCormack( const Texture2D *in, const Texture2D *velocity, Texture2D *out )
	{
		int x, y;

		Texture2D *Sn		= &m_ActualSediment;
		Texture2D *Sn1_hat	= &m_ActualSediment_tmp;

		//=============== Sn+1_hatを前計算する ====================//
		Advect_SemiLagrangian( Sn, &m_Velocity, Sn1_hat );


		//==================== Sn+1を計算する =====================//
		Texture2D *Sn1		= &m_ActualSediment;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				// Sn_hatを計算
				const Vec2f *v = (Vec2f *)velocity->Data(x,y);


				//##################### 計算結果のクランプ幅を計算する ###################//
				const Vec2f npos =
				{
					float(x) - m_timeStep * v->x,
					float(y) - m_timeStep * v->y
				};

				const int x_floor	= floor(npos.x);
				const int x_ceil	= ceil(npos.x);
				const int y_floor	= floor(npos.y);
				const int y_ceil	= ceil(npos.y);

				const float s00		= *(float *)Sn->Data( x_floor, y_floor );
				const float s10		= *(float *)Sn->Data( x_ceil, y_floor );
				const float s01		= *(float *)Sn->Data( x_floor, y_ceil );
				const float s11		= *(float *)Sn->Data( x_ceil, y_ceil );

				const float Sn_min	= min(min(min(s00,s10),s01),s11);
				const float Sn_max	= max(max(max(s00,s10),s01),s11);


				const Vec2f next_pos =
				{
					float(x) + m_timeStep * v->x,
					float(y) + m_timeStep * v->y
				};


				float c00 = *(float *)in->Data( max(next_pos.x,0),					max(next_pos.y,0) );
				float c01 = *(float *)in->Data( max(next_pos.x,0),					min(next_pos.y+1, in->Height()-1) );
				float c10 = *(float *)in->Data( min(next_pos.x+1,in->Width()-1),	max(next_pos.y,0) );
				float c11 = *(float *)in->Data( min(next_pos.x+1,in->Width()-1),	min(next_pos.y+1, in->Height()-1) );

				const Vec2f p = { next_pos.x - floor(next_pos.x), next_pos.y - floor(next_pos.y) };

				float Sn_hat = bilinear<float>( p, c00, c10, c01, c11 );//bilinear( next_pos, Sn1_hat );

				// Sn+1を計算
				*(float *)Sn1->Data(x,y) = Clamp( *(float *)Sn1_hat->Data(x,y) + 0.5f * ( *(float *)Sn->Data(x,y) - Sn_hat ),
					Sn_min, Sn_max );			

			}// end of x loop
		}// end of y loop

	}







}// end of namespace