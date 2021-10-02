#include	"LPVIrradianceVolume.h"

#include	<oreore/common/TString.h>



namespace OreOreLib
{

	//########################################################################################################################//
	//										SH_LPV class
	//########################################################################################################################//

	SH_LPV::SH_LPV()
	{
		
	}



	SH_LPV::~SH_LPV()
	{
		Release();
	}



	void SH_LPV::Init( int width, int height, int depth )
	{
		for( int i=0; i<3; ++i )
		{
			rgb[i].Allocate( width, height, depth, 0, FORMAT_R16G16B16A16_FLOAT );
			rgb[i].SetAddressMode( TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER );
			rgb[i].SetFilterMode( FILTER_MAG_MIN_LINEAR );
			rgb[i].GenHardwareTexture();
		}
	}



	void SH_LPV::Release()
	{
		for( int i=0; i<3; ++i )
			rgb[i].Release();
	}





	//########################################################################################################################//
	//										 Light Propagation Volumes Shader
	//########################################################################################################################//

	// デフォルトコンストラクタ
	LPVIrradianceVolume::LPVIrradianceVolume()
	{
#ifdef	_DEBUG
		tcout << _T("LPVIrradianceVolume::LPVIrradianceVolume()...") << tendl;
#endif

		m_numCascades		= 0;
		InitVec( m_LpvSize, 0, 0, 0 );
		m_LpvCellDim		= NULL;
		m_LpvOrigin			= NULL;
		m_MatVoxelSpace		= NULL;
		m_MatTextureSpace	= NULL;

		memset( m_SH_LPVs, 0, sizeof(SH_LPV*) * 2 );
		m_SH_GV				= NULL;

		Src					= 0;
		Dst					= 1;
	}


	// デストラクタ
	LPVIrradianceVolume::~LPVIrradianceVolume()
	{
#ifdef	_DEBUG
		tcout << _T("LPVIrradianceVolume::~LPVIrradianceVolume()...") << tendl;
#endif

		Release();
	}



	// LPVテクスチャを生成する
	void LPVIrradianceVolume::Init(int num_cascades, const Vec3i &lpv_size/*, const Vec2i &rsm_size, const Vec2i &vpl_size*/)
	{
		Init( num_cascades, lpv_size.x, lpv_size.y, lpv_size.z );
	}



	void LPVIrradianceVolume::Init( int num_cascades, int width, int height, int depth )
	{
		m_numCascades	= num_cascades;
		InitVec( m_LpvSize, width, height, depth );

		m_LpvCellDim		= new float[ m_numCascades ];
		m_LpvOrigin			= new Vec3f[ m_numCascades ];
		m_MatVoxelSpace		= new Mat4f[ m_numCascades ];
		m_MatTextureSpace	= new Mat4f[ m_numCascades ];

		for( int i=0; i<m_numCascades; ++i )
		{
			m_LpvCellDim[i] = 1.0f * pow( 2.0f, float(i) );
			InitVec( m_LpvOrigin[i], 0.0f, 0.0f, 0.0f );
			MatIdentity( m_MatVoxelSpace[i] );
			MatIdentity( m_MatTextureSpace[i] );
		}


		//====================== テクスチャオブジェクトの初期化 ============================//
		// Init LPV Texture
		m_SH_LPVs[Src]	= new SH_LPV[ m_numCascades ];
		m_SH_LPVs[Dst]	= new SH_LPV[ m_numCascades ];

		for( int i=0; i<m_numCascades; ++i )
		{
			m_SH_LPVs[Src][i].Init( m_LpvSize.x, m_LpvSize.y, m_LpvSize.z );
			m_SH_LPVs[Dst][i].Init( m_LpvSize.x, m_LpvSize.y, m_LpvSize.z );
		}

		// Init SH Accumulation Texture
		m_SHaccum[Src].Init( m_LpvSize.x, m_LpvSize.y, m_LpvSize.z );
		m_SHaccum[Dst].Init( m_LpvSize.x, m_LpvSize.y, m_LpvSize.z );


		// Init GeometryVolume Texture
		m_SH_GV	= new Texture3D[ m_numCascades ];

		for( int i=0; i<m_numCascades; ++i )
		{
			m_SH_GV[i].Allocate( m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, 0, FORMAT_R16G16B16A16_FLOAT );
			m_SH_GV[i].SetAddressMode( TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER );
			m_SH_GV[i].SetFilterMode( FILTER_MAG_MIN_LINEAR );
			m_SH_GV[i].GenHardwareTexture();
		}
	
	}



	// LPVテクスチャを削除する
	void LPVIrradianceVolume::Release()
	{
		//=============== テクスチャの削除 =================//
		if(m_SH_LPVs)
		{
			for(int i=0; i<2; i++)
				SafeDeleteArray( m_SH_LPVs[i] );
		}

		if(m_SHaccum)
		{	
			for(int i=0; i<2; i++)
			{
				for( int k=0; k<3; ++k )	m_SHaccum[i].rgb[k].Release();
			}
		}


		SafeDeleteArray( m_SH_GV );

		if(m_SH_GV)
		{
			for( int k=0; k<m_numCascades; ++k )	m_SH_GV[k].Release();
			//glDeleteTextures(m_numCascades, m_SH_GV);
			delete [] m_SH_GV;
			m_SH_GV = NULL;
		}


		//=============== LPV情報の削除 =================//
		SafeDeleteArray( m_LpvCellDim );
		SafeDeleteArray( m_LpvOrigin );

		SafeDeleteArray( m_MatVoxelSpace );
		SafeDeleteArray( m_MatTextureSpace );

	}



	void LPVIrradianceVolume::SetOrigin( int lv, float x, float y, float z )
	{
		InitVec( m_LpvOrigin[lv], x, y, z );
	}



	void LPVIrradianceVolume::SetCellDim( float dim )
	{
		for(int i=0; i<m_numCascades; i++)
		{
			m_LpvCellDim[i] = dim * pow(2.0f, float(i));
		}
	
	}



	void LPVIrradianceVolume::UpdateMatrix()
	{
		for( int i=0; i<m_numCascades; ++i )
		{
		
			//============ Calculate World-to-VoxelSpace Transform Matrix =========//
			Mat4f matTranslate, matScale;

			// Init Translation Matrix
			MatTranslation( matTranslate, -m_LpvOrigin[i].x, -m_LpvOrigin[i].y, -m_LpvOrigin[i].z );

			// Init Scaling Matrix
			MatScale( matScale,
						1.0f / ( 0.5f * float(m_LpvSize.x) * m_LpvCellDim[i] ),
						1.0f / ( 0.5f * float(m_LpvSize.y) * m_LpvCellDim[i] ),
						1.0f / ( 0.5f * float(m_LpvSize.z) * m_LpvCellDim[i] )
						);

			Multiply( m_MatVoxelSpace[i], matScale, matTranslate );


			//============ Calculate World-to-TexrureSpace Transform Matrix =========//
			Mat4f	matShrinkTexCoord,	// [-0.5, +0.5]の範囲にテクスチャ座標を収める
					matOffsetTexCoord,	// テクスチャ座標を+0.5して、[0.0, 1.0]の範囲に補正する
					matVoxelToTexSpace;

			MatScale( matShrinkTexCoord,
						(float(m_LpvSize.x) - 1.0f) / float(m_LpvSize.x) * 0.5f,
						(float(m_LpvSize.y) - 1.0f) / float(m_LpvSize.y) * 0.5f,
						(float(m_LpvSize.z) - 1.0f) / float(m_LpvSize.z) * 0.5f
						);
			MatTranslation( matOffsetTexCoord, 0.5f, 0.5f, 0.5f );
			Multiply( matVoxelToTexSpace, matOffsetTexCoord, matShrinkTexCoord );

			Multiply( m_MatTextureSpace[i], matVoxelToTexSpace, m_MatVoxelSpace[i] );

		}// end of i loop
	}



	// LPVボクセル空間への座標変換行列. ボクセル中心点が空間の原点
	//void LPVIrradianceVolume::GetTransformMatrix( Mat4f* result, int lv )
	//{
	//	Mat4f matScale, matTranslate;

	//	MatIdentity( *result );
	//	
	//	// Init Translation Matrix
	//	MatTranslation( matTranslate, -m_LpvOrigin[lv].x, -m_LpvOrigin[lv].y, -m_LpvOrigin[lv].z );

	//	// Init Scaling Matrix
	//	MatScaling( matScale,
	//				1.0f / ( 0.5f * float(m_LpvSize.x) * m_LpvCellDim[lv] ),
	//				1.0f / ( 0.5f * float(m_LpvSize.y) * m_LpvCellDim[lv] ),
	//				1.0f / ( 0.5f * float(m_LpvSize.z) * m_LpvCellDim[lv] )
	//				);

	//	MatMultiply( *result, matScale, matTranslate );

	//}


}// end of namespace