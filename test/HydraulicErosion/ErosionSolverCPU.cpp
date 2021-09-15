#include	"ErosionSolverCPU.h"

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
#define SedimentCapacity	0.0001f // 0.0001(��) - 0.1(��)
#define DepositionConst		0.15f	// 0.15(��) - 0.9(��)
#define DissolutionConst	0.15f	// 0.15(��) - 0.9(��)
#define Ck					0.01f	// 0.0001 - 0.01

#define minTiltAngle	0.05f



//######################################################################################################//
//																										//
//											Public Functions											//
//																										//
//######################################################################################################//

// default constructor
	ErosionSimulator::ErosionSimulator( int dimx, int dimy, float length, int num_layers )
	{
		InitVec( m_Dim, dimx, dimy );
		m_Length	= length;
		m_numLayers	= num_layers;

		m_timeStep	= DEFAULT_TIMESTEP;
		m_Rho		= 1.0f;
		m_G			= 10.0f;
		m_Vaporize	= 0.0001f;//0.5f;

		m_C			= m_Length * m_Length;

		m_UseMacCormackAdvection	= true;


		m_TerrainHeight.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );
		m_TerrainHeight.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );

		m_TerrainNormal.Allocate( dimx, dimy, 0, 0, FORMAT_R32G32B32_FLOAT );
		m_TerrainNormal.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );

		m_WaterLevel.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );
		m_WaterLevel.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );

		m_WaterNormal.Allocate( dimx, dimy, 0, 0, FORMAT_R32G32B32_FLOAT );
		m_WaterNormal.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );

		m_WaterFlow.Allocate( dimx, dimy, 0, 0, FORMAT_R32G32B32A32_FLOAT );
		m_WaterFlow.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );

#ifdef _DEBUG
		m_DP.Allocate( dimx, dimy, 0, 0, FORMAT_R32G32B32A32_FLOAT );
		m_Accelation.Allocate( dimx, dimy, 0, 0, FORMAT_R32G32B32A32_FLOAT );
#endif

		m_Velocity.Allocate( dimx, dimy, 0, 0, FORMAT_R32G32_FLOAT );

		m_AverageWaterHeight.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );

		m_SedimentTransportCapacity.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );

		m_ActualSediment.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );
		m_ActualSediment.SetFilterMode( FILTER_MODE::FILTER_MAG_MIN_LINEAR );

		m_MaxRegolithThickness.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );

		m_ActualRegolithThickness.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );

		m_RegolithOutflow.Allocate( dimx, dimy, 0, 0, FORMAT_R32G32B32A32_FLOAT );

		m_ActualSediment_tmp.Allocate( dimx, dimy, 0, 0, FORMAT_R32_FLOAT );
	}


	// destructor
	ErosionSimulator::~ErosionSimulator()
	{
		m_TerrainHeight.Release();
		m_TerrainNormal.Release();
		m_WaterLevel.Release();
		m_WaterNormal.Release();
		m_WaterFlow.Release();
#ifdef _DEBUG
		m_DP.Release();
		m_Accelation.Release();
#endif

		m_Velocity.Release();
		m_AverageWaterHeight.Release();
		m_SedimentTransportCapacity.Release();
		m_ActualSediment.Release();

		m_MaxRegolithThickness.Release();
		m_ActualRegolithThickness.Release();
		m_RegolithOutflow.Release();

		m_ActualSediment_tmp.Release();
	}






	// run simulation
	void ErosionSimulator::Update()
	{

		//=================== �אڃZ���̐��ʂ��爳�͍������v�Z���� ======================//
#ifdef _DEBUG
		CalcWaterPressure();
#endif

		//======================== ���͍��������Ƃɉ����x���v�Z���� =====================//
#ifdef _DEBUG
		CalcAcceleration();
#endif

		//==================== �����x�����Ƃ�"�אڃZ���ւ̗���"���v�Z���� ===============//
		CalcWaterFlow();

		//=========================== �e�Z���̐��ʂ��X�V���� ============================//
		CalcWaterLevel();

		//========================== �@���}�b�v���X�V���� ============================//
		CalcNormalMap();

		//========================= �e�Z���̐��̑��x���v�Z���� =======================//
		CalcWaterVelocity();

		//================= �����Ɋ�Â��ĐZ�H���ʂ��v�Z���� =========================//
		ForceBasedErosion();

		//========================= ���S���X�̈ړ����v�Z���� =========================//
		DissolutionBasedErosion();

	}



	// 
	void ErosionSimulator::TestAdvection()
	{
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



	// �n�ʂ��X�Εt���ɂ���
	void ErosionSimulator::InitHeightExamples()
	{
		const int center_x = m_Dim.x / 2;
		const int center_y = m_Dim.y / 2;

		for( int y=0; y<m_Dim.y; ++y )
		{
			for( int x=0; x<m_Dim.x; ++x )
			{
				//==================== �n�C�g�}�b�v�̏����� ====================//
				// �s���~�b�h�^
				//*(float *)m_TerrainHeight(x, y)	= (abs(center_x - x) + abs(center_y - y));//10.5f * Max( ( float(center_x + center_y) - ((float)abs(center_x - x) + (float)abs(center_y - y)) ), 0.0f ) / float(center_x + center_y);
				// x�����̌X��
				//*(float *)m_TerrainHeight.Data(x, y) = float(x) * 0.2f;
				// ����
				//*(float *)m_TerrainHeight.Data( x, y ) *= 50.0f * m_Length;
				//
				*(float *)m_TerrainHeight( x, y )	= 0.005f * ( pow( center_x - x, 2 ) / 20.0f + pow( center_y - y, 2 ) );

				//==================== �����z�̏����� ==========================// 
				// ��l
				*(float *)m_WaterLevel( x, y ) = m_C * 0.0f;
				*(float *)m_AverageWaterHeight( x, y ) = *(float *)m_WaterLevel( x, y );

				//=============== �^�����̓y���ʂ������� =======================//
				*(float *)m_ActualSediment( x, y ) = 0.0f;

			}// end of x loop
		}// end of y loop

	}



	void ErosionSimulator::InitAdvectionTest()
	{/*
		int x, y;

		//============== ���x���K���ɏ��������� ==================//
		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				Vec2f *velocity = (Vec2f *)m_Velocity(	x, y );

				InitVec( *velocity, 5.0f, 5.0f );

			}// end of x loop
		}// end of y loop


		//============= �y���̑͐ϗʂ�K���ɏ��������� =============//
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
				Vec2f pos ={ float( x ), float( y ) };

				//if( Distance( pos, center ) < 25.0f )
				*(float *)m_WaterLevel( x, y ) += 0.5f;

			}// end of x loop
		}// end of y loop
	}



	void ErosionSimulator::SetWaterDumping( float val )
	{
		m_WaterDumping	= val;
	}



	void ErosionSimulator::DropRandomWater()
	{

		for( int i=0; i<50; ++i )
		{
			int x = rand()%( m_Dim.x-1 );
			int y = rand()%( m_Dim.y-1 );

			//*(float *)m_WaterLevel(x, y) += m_C * 2.0f;

			for( int y_=y; y_<y+2; ++y_ )
			{
				for( int x_=x; x_<x+2; ++x_ )
				{
					*(float *)m_WaterLevel( x_, y_ ) += m_C * 2.5f;
				}
			}

		}

		
		for( int y=10; y<15; ++y)
		{
			for( int x=275; x<285; ++x )
			{
				if( *(float *)m_WaterLevel(x, y) < m_C * 5.0f )
					*(float *)m_WaterLevel(x, y) = m_C * 5.0f;

			}
		}

		
		for( int x=0; x<m_WaterLevel.Width(); ++x )
			*(float *)m_WaterLevel( x, 0 ) = 0.0f;
		
	}



	float ErosionSimulator::GetToalWaterAmount()
	{
		float result = 0.0f;

		for( int y=0; y<m_Dim.y; ++y )
			for( int x=0; x<m_Dim.x; ++x )
				result += *(float *)m_WaterLevel( x, y );


		return result;
	}





	//######################################################################################################//
	//																										//
	//											Private Functions											//
	//																										//
	//######################################################################################################//


	void ErosionSimulator::CalcNormalMap()
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
					( *(float *)m_TerrainHeight( x+1, y ) - *(float *)m_TerrainHeight( x-1, y ) ) / ( m_Length * 2.0f )
				};

				Vec3f vt =
				{
					0.0f,
					1.0f,
					( *(float *)m_TerrainHeight( x, y+1 ) - *(float *)m_TerrainHeight( x, y-1 ) ) / ( m_Length * 2.0f )
				};

				Vec3f uw =
				{
					1.0f,
					0.0f,
					( *(float *)m_WaterLevel( x, y+1 ) - *(float *)m_WaterLevel( x, y-1 ) ) / ( m_Length * 2.0f ) + ut.z
				};

				Vec3f vw =
				{
					0.0f,
					1.0f,
					( *(float *)m_WaterLevel( x, y+1 ) - *(float *)m_WaterLevel( x, y-1 ) ) / ( m_Length * 2.0f ) + vt.z
				};

				Normalize( ut );
				Normalize( vt );
				Normalize( uw );
				Normalize( vw );

				//================== �n�`�̖@���}�b�v���v�Z���� ===================//
				Vec3f *n_terrain = (Vec3f *)m_TerrainNormal( x, y );
				CrossProduct( *n_terrain, ut, vt );

				//=================== ���ʂ̖@���}�b�v���v�Z���� ==================//
				Vec3f *n_water = (Vec3f *)m_WaterNormal( x, y );
				CrossProduct( *n_water, uw, vw );

			}// end of x loop
		}// end of y loop

	}



	float ErosionSimulator::TiltAngle( const Vec2i &pos, const Texture2D &normalmap )
	{
		const Vec3f up ={ 0,0,1 };
		const Vec3f n  = *(Vec3f *)normalmap.Data( pos.x, pos.y );

		return acos( DotProduct( n, up ) );
	}




	// �������y�����^�ׂ�ʂ��v�Z����. �s�N�Z���P�ʂŌv�Z�\
	void ErosionSimulator::CalcSedimentTransportCapacity()
	{
		Vec2i pos;
		const Vec3f up ={ 0,0,1 };

		for( pos.y=0; pos.y<m_Dim.y; ++pos.y )
		{
			for( pos.x=0; pos.x<m_Dim.x; ++pos.x )
			{
				const Vec2f *velocity	= (Vec2f *)m_Velocity( pos.x, pos.y );
				float *S				= (float *)m_SedimentTransportCapacity( pos.x, pos.y );

				float tiltAngle = TiltAngle( pos, m_TerrainNormal );
				//tiltAngle =	(tiltAngle < minTiltAngle) ? minTiltAngle : tiltAngle;

				*S = SedimentCapacity * Length( *velocity ) * sin( tiltAngle );

			}// end of x loop
		}// end of y loop

	}



	void ErosionSimulator::CalcMaxRegolithThickness()
	{
		Vec2i pos;
		const Vec3f up ={ 0,0,1 };

		for( pos.y=0; pos.y<m_Dim.y; ++pos.y )
		{
			for( pos.x=0; pos.x<m_Dim.x; ++pos.x )
			{
				const Vec2f *velocity	= (Vec2f *)m_Velocity( pos.x, pos.y );
				float *regolith			= (float *)m_MaxRegolithThickness( pos.x, pos.y );

				*regolith = Min( Ck, *(float *)m_WaterLevel( pos.x, pos.y ) );

			}// end of x loop
		}// end of y loop
	}







	// �y���̐Z�H�Ƒ͐ς��v�Z����
	void ErosionSimulator::CalcDepositionDissolution()
	{
		int x, y;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				Vec2i pos ={ x,y };

				float capacity		= *(float *)m_SedimentTransportCapacity( x, y );// * sedimentCapacityFactor;
				float *sediment	= (float *)m_ActualSediment( x, y );


				if( *sediment > capacity )// �����ꒆ�̓y���ʂ��������x�ʂ𒴂����ꍇ
				{
					// �y����u���Ă�
					float sedimentDif				= DepositionConst * ( *sediment - capacity );

					*(float *)m_TerrainHeight( x, y )	+= sedimentDif;// �u���Ă����͒n�`�ɑ���
					*sediment						-= sedimentDif;// �^��ł���y������͈���
				}
				else// �܂��A�����x�܂ŗ]�͂�����
				{
					// �n�ʂ����
					float maxSediment	= capacity;// - *(float *)m_TerrainHeight(x,y);
					if( maxSediment < *sediment )	continue;

					float sedimentDif	= DissolutionConst * ( maxSediment - *sediment );	// ���ɗn������y���̗�
					sedimentDif			= Min( *(float *)m_TerrainHeight( x, y ), sedimentDif );// �A���A�n�w�ȉ�

					*(float *)m_TerrainHeight( x, y )	-= sedimentDif;// �Z�H�Ԃ񂾂��n�ʂ�Ⴍ����
					*sediment						+= sedimentDif;// �����y���𑝂₷
				}

			}// end of x loop
		}// end of y loop


	}



	void ErosionSimulator::CalcRegolithDepositionDissolution()
	{
		int x, y;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				float max_regolith_thickness	= *(float *)m_MaxRegolithThickness( x, y );	// ���S���X�̑͐ϋ��e���E
				float *curr_regorith_thickness	= (float *)m_ActualRegolithThickness( x, y );// ���S���X�̑͐ϗ�

				if( *curr_regorith_thickness > max_regolith_thickness )// ���S���X�̗ʂ����e���E�𒴂����ꍇ
				{
					// ���S���X��n�ʂɍ~�낷
					float regolithDif				=  ( *curr_regorith_thickness - max_regolith_thickness );// ���e���E�I�[�o�[��

					*(float *)m_TerrainHeight( x, y )	+= regolithDif;// �]�背�S���X��n�`�ɑ���
					*curr_regorith_thickness		-= regolithDif;// ���S���X�͐ϗʂ���]�蕪�����Z����
				}
				else// ���S���X���e���E�ɒB���Ă��Ȃ��ꍇ
				{
					float maxRegolith	= max_regolith_thickness;
					if( maxRegolith < *curr_regorith_thickness )	continue;

					float regolithDif	= ( max_regolith_thickness - *curr_regorith_thickness );
					regolithDif			= Min( *(float *)m_TerrainHeight( x, y ), regolithDif );

					*(float *)m_TerrainHeight( x, y )	-= regolithDif;// �n�w�����
					*curr_regorith_thickness		+= regolithDif;// ����������S���X�w�ɑ���

					// �n�ʂ����
					//float regolithDif				= Min(  ( max_regolith_thickness - *curr_regorith_thickness ), *(float *)m_TerrainHeight(x,y) );

					//*(float *)m_TerrainHeight(x,y)	-= regolithDif;// �Z�H�Ԃ񂾂��n�ʂ�Ⴍ����
					//*curr_regorith_thickness		+= regolithDif;// �����y���𑝂₷
				}

			}// end of x loop
		}// end of y loop


	}



	void ErosionSimulator::CalcRegolithThickness()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos			={ x, y };
				float flowChange	= 0.0f;
				Vec4f *prevFlow		= (Vec4f *)m_RegolithOutflow( pos.x, pos.y );	// ���݂̐��̗��o��
				float prevThickness	= *(float *)m_ActualRegolithThickness( pos.x, pos.y );

				for( int n=0; n<numNs; ++n )
				{
					float inFlow;
					float outFlow;

					// ��������אڃZ���ւ̗��o�ʂ��擾
					outFlow		= prevFlow->xyzw[n];

					// �אڃZ�����玩���ւ̗����ʂ��擾
					Vec4f* neighbor_cell = (Vec4f *)m_RegolithOutflow( pos.x+Ns[n].x, pos.y+Ns[n].y );

					// ���E�����������B���E��������̗����ʂ̓[���ɂ���
					if( n==0 && x==m_Dim.x-1 )		inFlow	= outFlow;// �E�[
					else if( n==1 && x==0 )			inFlow	= outFlow;// ���[
					else if( n==2 && y==m_Dim.y-1 )	inFlow	= outFlow;// ��[
					else if( n==3 && y==0 )			inFlow	= outFlow;// ���[
					else							inFlow	= neighbor_cell->xyzw[Ninv[n]];	// ����ȊO

					// �g�[�^���̗��o�� += �אڃZ�����玩���ւ̗��� - ��������אڃZ���ւ̗���
					flowChange += ( inFlow - outFlow );
				}// end of neighbor loop

				// �g�[�^�����o�ʂ��v�Z����
				flowChange *= ( m_timeStep / m_C );

				// �V�������ʂ��v�Z����
				float newThickness = Max( ( prevThickness + flowChange ), 0.0f );
				*(float *)m_ActualRegolithThickness( pos.x, pos.y ) = newThickness;

			}// end of x loop
		}// end of y loop

	}






	void ErosionSimulator::ForceBasedErosion()
	{
		//================= �e�Z�����^���\�ȓy���e�ʂ��X�V���� ==================//
		CalcSedimentTransportCapacity();

		//=================== �n�ʂ����or�y����u���̔��f������ ==================//
		CalcDepositionDissolution();

		//============================== �y�����ړ����� ===========================//
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



	void ErosionSimulator::DissolutionBasedErosion()
	{
		CalcMaxRegolithThickness();

		// regotith deposition and dissolution
		CalcRegolithDepositionDissolution();

		// calc regolith outflow
		CalcRegolithOutflow();

		// update regolith revel
		CalcRegolithThickness();

	}




	void ErosionSimulator::CalcRegolithOutflow()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos ={ x, y };

				const Vec4f* oldoutFlow = (Vec4f *)m_RegolithOutflow( x, y );
				Vec4f outFlow;

				for( int n=0; n<numNs; ++n )
				{
					Vec2i n_pos;
					Add( n_pos, pos, Ns[n] );

					//============== �ߖT�Z���̃��S���X�̍��፷���爳�͂��v�Z ==============//
					float curr_regolith_lv = *(float *)m_ActualRegolithThickness( pos.x, pos.y );
					float neighbor_regolith_lv = *(float *)m_ActualRegolithThickness( n_pos.x, n_pos.y );

					// pos�Z���̍������v�Z����.����1���C���[����
					float h		= curr_regolith_lv + *(float *)m_TerrainHeight( pos.x, pos.y );

					// neighbor�Z���̍������v�Z����
					float n_h	= neighbor_regolith_lv + *(float *)m_TerrainHeight( n_pos.x, n_pos.y );

					// ���͍������v�Z����
					//float dP	= m_Rho * m_G * ( h - n_h );
					//float accel	= dP / ( m_Rho * m_Length );

					//============== ���͂����Ƃɐ��̉����x���v�Z ================//
					float accel	= m_G * ( h - n_h ) / m_Length;

					//================== �����x���痬�ʂ��v�Z ====================//
					// �_���s���O�������ăV�~�����[�V�������肳���� & ���o(���̒l)�̏ꍇ�����l��o�^����
					outFlow.xyzw[n]	= Max( DAMPING_REGOLITH * oldoutFlow->xyzw[n] + m_timeStep * m_C * accel, 0.0f );// �_������(

				}// end of neighbor loop

				// ���o�̑��ʂ����ʂ𒴂��Ȃ��悤�ɃX�P�[�����O����
				float maxRegolith	= *(float *)m_ActualRegolithThickness( x, y ) * m_C;
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

				*(Vec4f *)m_RegolithOutflow( x, y ) = outFlow;

			}// end of x loop
		}// end of y loop



	}



#ifdef _DEBUG

	void ErosionSimulator::CalcWaterPressure()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos ={ x, y };

				float *dP = (float *)m_DP.Data( x, y );

				for( int n=0; n<numNs; ++n )
				{
					Vec2i n_pos;
					Add( n_pos, pos, Ns[n] );


					float curr_water_lv = *(float *)m_WaterLevel( pos.x, pos.y );
					float neighbor_water_lv = *(float *)m_WaterLevel( n_pos.x, n_pos.y );

					// pos�Z���̍������v�Z����.����1���C���[����
					float h		= curr_water_lv + *(float *)m_TerrainHeight( pos.x, pos.y );

					// neighbor�Z���̍������v�Z����
					float n_h	= neighbor_water_lv + *(float *)m_TerrainHeight( n_pos.x, n_pos.y );

					// ���͍������v�Z����
					*( dP + n )	= m_Rho * m_G * ( h - n_h );

				}// end of neighbor loop

			}// end of x loop
		}// end of y loop

	}



	void ErosionSimulator::CalcAcceleration()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos ={ x, y };

				float *pressure = (float *)m_DP( x, y );
				float *newAccel	= (float *)m_Accelation( x, y );

				for( int n=0; n<numNs; ++n )
				{
					*( newAccel + n )	= *( pressure + n ) / ( m_Rho * m_Length );
				}// end of neighbor loop

			}// end of x loop
		}// end of y loop

	}

#endif


	void ErosionSimulator::CalcWaterFlow()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos ={ x, y };

				const Vec4f* oldoutFlow = (Vec4f *)m_WaterFlow( x, y );
				Vec4f outFlow;

				for( int n=0; n<numNs; ++n )
				{
					Vec2i n_pos;
					Add( n_pos, pos, Ns[n] );

					//============== �ߖT�Z���̐��ʍ����爳�͂��v�Z ==============//
					float curr_water_lv = *(float *)m_WaterLevel( pos.x, pos.y );
					float neighbor_water_lv = *(float *)m_WaterLevel( n_pos.x, n_pos.y );

					// pos�Z���̍������v�Z����.����1���C���[����
					float h		= curr_water_lv + *(float *)m_TerrainHeight( pos.x, pos.y );

					// neighbor�Z���̍������v�Z����
					float n_h	= neighbor_water_lv + *(float *)m_TerrainHeight( n_pos.x, n_pos.y );

					// ���͍������v�Z����
					//float dP	= m_G * ( h - n_h );
					//float accel	= Min( dP / ( m_Length ), m_Length );

					//============== ���͂����Ƃɐ��̉����x���v�Z ================//
					float accel	= m_G * ( h - n_h ) / m_Length;

					//================== �����x���痬�ʂ��v�Z ====================//

					// �_���s���O�������ăV�~�����[�V�������肳���� & ���o(���̒l)�̏ꍇ�����l��o�^����
					outFlow.xyzw[n]	= Max( DAMPING * oldoutFlow->xyzw[n] + m_timeStep * m_C * accel, 0.0f );// �_������(1)

				}// end of neighbor loop

				// ���݂̐���
				float maxWater	= m_Length * m_Length * *(float *)m_WaterLevel( x, y );
				// ���o������������
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

				*(Vec4f *)m_WaterFlow( x, y ) = outFlow;

			}// end of x loop
		}// end of y loop

	}


	void ErosionSimulator::CalcWaterLevel()
	{
		int x, y;

		for( y=0; y < m_Dim.y; ++y )
		{
			for( x=0; x < m_Dim.x; ++x )
			{
				Vec2i pos				={ x, y };
				Vec4f *prevFlow			= (Vec4f *)m_WaterFlow( pos.x, pos.y );	// ���݂̐��̗��o��
				float prevWaterLevel	= *(float *)m_WaterLevel( pos.x, pos.y );

				float inFlows	= 0.0f;
				float outFlows	= 0.0f;

				for( int n=0; n<numNs; ++n )
				{
					float inFlow;
					float outFlow;

					// ��������אڃZ���ւ̗��o�ʂ��擾
					outFlow		= prevFlow->xyzw[n];

					// �אڃZ�����玩���ւ̗����ʂ��擾
					Vec4f* neighbor_cell = (Vec4f *)m_WaterFlow( pos.x+Ns[n].x, pos.y+Ns[n].y );

					// ���E�����������B���E��������̗����ʂ̓[���ɂ���
					if( n==0 && x==m_Dim.x-1 )		inFlow	= outFlow;// �E�[
					else if( n==1 && x==0 )			inFlow	= outFlow;// ���[
					else if( n==2 && y==m_Dim.y-1 )	inFlow	= outFlow;// ��[
					else if( n==3 && y==0 )			inFlow	= outFlow;// ���[
					else							inFlow	= neighbor_cell->xyzw[Ninv[n]];	// ����ȊO

					inFlows	+= inFlow;
					outFlows += outFlow;

				}// end of neighbor loop

				// �V�������ʂ��v�Z����
				float newWaterLevel = prevWaterLevel + ( m_timeStep * ( inFlows-outFlows ) ) / ( m_Length * m_Length );// ��(1)��1���̎�
				*(float *)m_WaterLevel( pos.x, pos.y ) = newWaterLevel * ( 1.0f - m_Vaporize );

				// ���O�^�C���X�e�b�v�ƌ��^�C���X�e�b�v�Ƃ̐��ʕ��ϒl���X�V����
				*(float *)m_AverageWaterHeight( pos.x, pos.y ) = 0.5f * ( prevWaterLevel + newWaterLevel );

			}// end of x loop
		}// end of y loop


	}


	void ErosionSimulator::CalcWaterVelocity()
	{
		int k;
		Vec2i pos;

		for( pos.y=0; pos.y<m_Dim.y; ++pos.y )
		{
			for( pos.x=0; pos.x<m_Dim.x; ++pos.x )
			{
				const Vec4f *f_curr	= (Vec4f *)m_WaterFlow( pos.x, pos.y );
				Vec2f *velocity		= (Vec2f *)m_Velocity( pos.x, pos.y );

				for( k=0; k<numNs; k+=2 )// ����������
				{
					const float velocityFactor = m_Length * m_Length * *(float *)m_AverageWaterHeight( pos.x, pos.y );
					float W = 0.0f;
					Vec2i n_pos;

					//============= �������אڃZ���Ԃ̐��ʕω��ʂ��v�Z���� ================//
					// ���O�Z���������Z���ւ̗����ʂ����Z
					Add( n_pos, pos, Ns[k+1] );// ���O�Z���ւ̃C���f�b�N�X�擾
					Vec4f *f_prev = (Vec4f *)m_WaterFlow( n_pos.x, n_pos.y );// ���O�Z���̗��o�ʎ擾				
					if( pos.x > 0 &&  pos.y > 0 )// ���E����
						W += *( f_prev->xyzw + k );// �����Z���ւ̗����ʎ擾

					// ����Z��->�����Z���ւ̗����ʂ����Z
					Add( n_pos, pos, Ns[k] );// ����Z���ւ̃C���f�b�N�X�擾
					Vec4f *f_next = (Vec4f *)m_WaterFlow( n_pos.x, n_pos.y );// ����Z���̗��o�ʎ擾
					if( pos.x <m_Dim.x-1 &&  pos.y < m_Dim.y-1 )// ���E����
						W += *( f_next->xyzw + k+1 );// �����Z���ւ̗����ʎ擾

					// ���O���㗼�Z���ւ̗��o�ʂ����Z
					if( pos.x > 0 &&  pos.y > 0 )// ���E����
						W -= *( f_curr->xyzw + k+1 );

					if( pos.x <m_Dim.x-1 &&  pos.y < m_Dim.y-1 )// ���E����
						W -= *( f_curr->xyzw + k );

					W *= 0.5f;

					//========================= �������̑��x���v�Z���� ====================//
					if( velocityFactor>0.0 )	W /= velocityFactor;

					velocity->xy[k]	= W;

				}// end of k loop

			}// end of x loop
		}// end of y loop

	}

	/*
	// ���������@�j��?
	float bilinear( const Vec2f &pos, const Texture2D *in )
	{
		const int x_floor	= (int)floor(pos.x);
		const int x_ceil	= (int)ceil(pos.x);
		const int y_floor	= (int)floor(pos.y);
		const int y_ceil	= (int)ceil(pos.y);

		const float x		= Max(Min(pos.x - float(x_floor),1.0f),0.0f);
		const float y		= Max(Min(pos.y - float(y_floor),1.0f),0.0f);
		const float f00		= *(float *)in->Data( x_floor, y_floor );
		const float f10		= *(float *)in->Data( x_ceil, y_floor );
		const float f01		= *(float *)in->Data( x_floor, y_ceil );
		const float f11		= *(float *)in->Data( x_ceil, y_ceil );

		return f00 * (1.0f-x) * (1.0f-y) + f10 * x * (1.0f-y) + f01 * (1.0f-x) * y + f11 * x * y;
	}
	*/


	template<typename T>
	T bilinear( const Vec2<T> &pos, const T &c00, const T &c10, const T &c01, const T &c11 )
	{
		//#if 1
		T a = c00 * ( T( 1 ) - pos.x ) + c10 * pos.x;
		T b = c01 * ( T( 1 ) - pos.x ) + c11 * pos.x;
		return a * ( T( 1 ) - pos.y ) + b * pos.y;
		//#else
		//    return (T(1) - pos.x) * (T(1) - pos.y) * c00 + 
		//        pos.x * (T(1) - pos.y) * c10 +
		//        (T(1) - pos.x) * pos.y * c01 +
		//        pos.x * pos.y * c11;
		//#endif
	}



	// Semi-Lagrangian Advection
	void ErosionSimulator::Advect_SemiLagrangian( const Texture2D *in, const Texture2D *velocity, Texture2D *out )
	{
		int x, y;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				// �Z���ʒu�x�N�g���̋t�����ɂ����̂ڂ�
				const Vec2f *v = (Vec2f *)velocity->Data( x, y );
				const Vec2f prev_pos =
				{
					float( x ) - m_timeStep * v->x,
					float( y ) - m_timeStep * v->y
				};

				float c00 = *(float *)in->Data( (int)Max( prev_pos.x, 0.0f ), (int)Max( prev_pos.y, 0.0f ) );
				float c01 = *(float *)in->Data( (int)Max( prev_pos.x, 0.0f ), (int)Min( prev_pos.y+1.0f, in->Height()-1.0f ) );
				float c10 = *(float *)in->Data( (int)Min( prev_pos.x+1, in->Width()-1.0f ), (int)Max( prev_pos.y, 0.0f ) );
				float c11 = *(float *)in->Data( (int)Min( prev_pos.x+1, in->Width()-1.0f ), (int)Min( prev_pos.y+1, in->Height()-1.0f ) );

				const Vec2f p ={ prev_pos.x - floor( prev_pos.x ), prev_pos.y - floor( prev_pos.y ) };
				*(float *)out->Data( x, y )	= bilinear<float>( p, c00, c10, c01, c11 ) * 0.25f;

				// �k�����ʒu�ŁA�ߖT�Z���̒l���o�C���j�A���
				//*(float *)out->Data(x,y) = bilinear( prev_pos, in );

			}// end of x loop
		}// end of y loop

	}


	// MacCormack Advection
	void ErosionSimulator::Advect_MacCormack( const Texture2D *in, const Texture2D *velocity, Texture2D *out )
	{
		int x, y;

		Texture2D *Sn		= &m_ActualSediment;
		Texture2D *Sn1_hat	= &m_ActualSediment_tmp;

		//=============== Sn+1_hat��O�v�Z���� ====================//
		Advect_SemiLagrangian( Sn, &m_Velocity, Sn1_hat );


		//==================== Sn+1���v�Z���� =====================//
		Texture2D *Sn1		= &m_ActualSediment;

		for( y=0; y<m_Dim.y; ++y )
		{
			for( x=0; x<m_Dim.x; ++x )
			{
				// Sn_hat���v�Z
				const Vec2f *v = (Vec2f *)velocity->Data( x, y );


				//##################### �v�Z���ʂ̃N�����v�����v�Z���� ###################//
				const Vec2f npos =
				{
					float( x ) - m_timeStep * v->x,
					float( y ) - m_timeStep * v->y
				};

				const int x_floor	= (int)floor( npos.x );
				const int x_ceil	= (int)ceil( npos.x );
				const int y_floor	= (int)floor( npos.y );
				const int y_ceil	= (int)ceil( npos.y );

				const float s00		= *(float *)Sn->Data( x_floor, y_floor );
				const float s10		= *(float *)Sn->Data( x_ceil, y_floor );
				const float s01		= *(float *)Sn->Data( x_floor, y_ceil );
				const float s11		= *(float *)Sn->Data( x_ceil, y_ceil );

				float Sn_min	= Min( Min( Min( s00, s10 ), s01 ), s11 );
				float Sn_max	= Max( Max( Max( s00, s10 ), s01 ), s11 );


				const Vec2f next_pos =
				{
					float( x ) + m_timeStep * v->x,
					float( y ) + m_timeStep * v->y
				};


				float c00 = *(float *)in->Data( (int)Max( next_pos.x, .0f ), (int)Max( next_pos.y, .0f ) );
				float c01 = *(float *)in->Data( (int)Max( next_pos.x, .0f ), (int)Min( next_pos.y+1, in->Height()-1.0f ) );
				float c10 = *(float *)in->Data( (int)Min( next_pos.x+1, in->Width()-1.0f ), (int)Max( next_pos.y, .0f ) );
				float c11 = *(float *)in->Data( (int)Min( next_pos.x+1, in->Width()-1.0f ), (int)Min( next_pos.y+1, in->Height()-1.0f ) );

				const Vec2f p ={ next_pos.x - floor( next_pos.x ), next_pos.y - floor( next_pos.y ) };

				float Sn_hat = bilinear<float>( p, c00, c10, c01, c11 );//bilinear( next_pos, Sn1_hat );

				// Sn+1���v�Z
				*(float *)Sn1->Data( x, y ) = Clamp( *(float *)Sn1_hat->Data( x, y ) + 0.5f * ( *(float *)Sn->Data( x, y ) - Sn_hat ),
					Sn_min, Sn_max );

			}// end of x loop
		}// end of y loop

	}







}// end of namespace