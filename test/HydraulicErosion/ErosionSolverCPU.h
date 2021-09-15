#ifndef	EROSION_SIMULATOR_H
#define	EROSION_SIMULATOR_H


#include	"oreore/MathLib.h"
#include	"oreore/Texture.h"


namespace OreOreLib
{



	class ErosionSimulator
	{
	public:

		ErosionSimulator( int dimx, int dimy, float length, int num_layers );
		~ErosionSimulator();

		void BindBuffer();
		void UnbindBuffer();

		void InitHeightExamples();
		void InitAdvectionTest();



		void SetWaterDumping( float val );


		void DropRandomWater();
		void Vaporize( float rate );

		void Update();

		void TestAdvection();


		Texture2D* GetTerrainHeightMap() const { return (Texture2D *)&m_TerrainHeight; };
		Texture2D* GetTerrainNormal() const { return (Texture2D *)&m_TerrainNormal; };
		Texture2D* GetWaterLevelMap() const { return (Texture2D *)&m_WaterLevel; };
		Texture2D* GetWaterNormal() const { return (Texture2D *)&m_WaterNormal; };
#ifdef _DEBUG
		Texture2D* GetPressureMap() const { return (Texture2D *)&m_DP; };
		Texture2D* GetAccelationMap() const { return (Texture2D *)&m_Accelation; };
#endif
		Texture2D* GetFlowAmountMap() const { return (Texture2D *)&m_WaterFlow; };
		Texture2D* GetVelocityMap() const { return (Texture2D *)&m_Velocity; };
		Texture2D* GetCapacity() const { return (Texture2D *)&m_SedimentTransportCapacity; };
		Texture2D* GetSedimentMap() const { return (Texture2D *)&m_ActualSediment; };
		Texture2D* GetRegolith() const { return (Texture2D *)&m_ActualRegolithThickness; };

		void SetMackCormakAdvection( bool flag ) { m_UseMacCormackAdvection = flag; };
		float GetToalWaterAmount();


	private:

		Vec2i	m_Dim;		// �V�~�����[�V������Ԃ̉𑜓x
		float	m_Length;	// �Z���Ԃ̋���
		int		m_numLayers;// �n�w�̐�

		float	m_timeStep;	// �^�C���X�e�b�v
		float	m_Rho;		// ���̖̂��x
		float	m_G;		// �d�͉����x
		float	m_Vaporize;	// ������

		float	m_C;		// �Z����ʐ�

		bool	m_UseMacCormackAdvection;

		float	m_WaterDumping;


		// �����f�[�^
		Texture2D	m_TerrainHeight;	// �n�ʂ̍���
		Texture2D	m_TerrainNormal;	// �n�ʂ̖@���}�b�v
		Texture2D	m_WaterLevel;		// ���̍���
		Texture2D	m_WaterNormal;		// ���̖@���}�b�v
		Texture2D	m_WaterFlow;				// �Z���Ԃ̗���

#ifdef _DEBUG
		Texture2D	m_DP;				// �אڃZ���Ƃ̐����� r:+x, g:-x, b:+y, a:-y
		Texture2D	m_Accelation;		// �����x
#endif

	// �y���͐σf�[�^
		Texture2D	m_Velocity;			// ����(m_timeStep���Ԃ�����)
		Texture2D	m_AverageWaterHeight;		// �������ƒ��O�����̐��ʕ��ϒl
		Texture2D	m_SedimentTransportCapacity;// �^���\�ȓy���̗e��
		Texture2D	m_ActualSediment;			// �����y���̗�

		Texture2D	m_MaxRegolithThickness;
		Texture2D	m_ActualRegolithThickness;
		Texture2D	m_RegolithOutflow;

		Texture2D	m_ActualSediment_tmp;			// �����y���̗�

#ifdef _DEBUG
		void CalcWaterPressure();// �f�o�b�O�p�̋@�\
		void CalcAcceleration();// �f�o�b�O�p�̋@�\
#endif

		void CalcWaterFlow();
		void CalcWaterLevel();
		void CalcWaterVelocity();
		void ForceBasedErosion();
		void DissolutionBasedErosion();// ��Ε\�ʂ̏_�炩���w�̃V�~�����[�V����


		void CalcNormalMap();
		float TiltAngle( const Vec2i &pos, const Texture2D &normalmap );
		void CalcSedimentTransportCapacity();
		void CalcMaxRegolithThickness();	// Dissolution based erosion

		void CalcDepositionDissolution();
		void CalcRegolithDepositionDissolution();
		void CalcRegolithOutflow();
		void CalcRegolithThickness();



		// Semi-Lagrangian Advection
		void Advect_SemiLagrangian( const Texture2D *in, const Texture2D *velocity, Texture2D *out );
		// MacCormack Advection
		void Advect_MacCormack( const Texture2D *in, const Texture2D *velocity, Texture2D *out );

	};



}



#endif	// EROSION_SIMULATOR_H //