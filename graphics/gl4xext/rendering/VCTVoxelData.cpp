#include	"VCTVoxelData.h"

#include	<graphics/gl4x/common/GLHelperFunctions.h>
#include	<graphics/gl4x/resource/GLBindPointManager.h>



namespace OreOreLib
{

	GLuint	VCTVoxelData::m_BindPoints[] ={ 0 };// SSBO bindpoints


	VCTVoxelData::VCTVoxelData()
	{
		InitZero( m_Attrib.Resolution );
		InitZero( m_Origin );
		m_Attrib.CellDim	= 0.0f;

		m_Basis[X_SPACE]	= X_AXIS;// InitVec( m_Basis[X_SPACE], 1.0f, 0.0f, 0.0f );// x axis
		m_Basis[Y_SPACE]	= Y_AXIS;// InitVec( m_Basis[Y_SPACE], 0.0f, 1.0f, 0.0f );// y axis
		m_Basis[Z_SPACE]	= Z_AXIS;// InitVec( m_Basis[Z_SPACE], 0.0f, 0.0f, 1.0f );// z axis

		for( int i=0; i<NUM_SPACE; ++i )
			MatZero( m_Transform.MatAxisSpace[i] );


		if( m_BindPoints[0]==0 )	m_BindPoints[0]	= GLBindPointManager::Reserve( GL_SHADER_STORAGE_BUFFER );// required for ssbo bindpoint
	}
	


	VCTVoxelData::~VCTVoxelData()
	{
		Release();
	}



	void VCTVoxelData::Init( const Vec3i &size, int numcascades, float dim )
	{
		Init( size.x, size.y, size.z, numcascades, dim );
	}
	


	void VCTVoxelData::Init( int w, int h, int d, int numcascades, float dim )
	{
		InitVec( m_Attrib.Resolution, w, h, d, numcascades );
		InitZero( m_Origin );
		m_Attrib.CellDim	= dim;
		
		//=======================	Init Position, Normal, and Color 3D Textures	==========================//
		m_TexPosition.Allocate( m_Attrib.Resolution.x, m_Attrib.Resolution.y, m_Attrib.Resolution.z, 0, DATA_FORMAT::FORMAT_R32_UINT );//FORMAT_R16G16B16A16_FLOAT );//
		//m_TexPosition.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_TexPosition.GenHardwareTexture();
		
		m_TexNormal.Allocate( m_Attrib.Resolution.x, m_Attrib.Resolution.y, m_Attrib.Resolution.z, 0, DATA_FORMAT::FORMAT_R32_UINT );//FORMAT_R16G16B16A16_FLOAT );
		//m_TexNormal.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_TexNormal.GenHardwareTexture();
		
		m_TexColor.Allocate( m_Attrib.Resolution.x, m_Attrib.Resolution.y, m_Attrib.Resolution.z, 0, DATA_FORMAT::FORMAT_R32_UINT );//FORMAT_R16G16B16A16_FLOAT );
		//m_TexColor.SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
		m_TexColor.GenHardwareTexture();
		
		//==============================	Init Isotropic Irradiance Texture	==============================//
		m_TexIrradiance.Allocate( m_Attrib.Resolution.x, m_Attrib.Resolution.y, m_Attrib.Resolution.z, m_Attrib.Resolution.w, FORMAT_R16G16B16A16_FLOAT );//DATA_FORMAT::FORMAT_R8G8B8_UNORM );
		m_TexIrradiance.SetFilterMode( FILTER_MODE::FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_LINEAR );
		m_TexIrradiance.SetAddressMode( TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER );
		m_TexIrradiance.GenHardwareTexture();

		//==============================	Init Anisotropic Irradiance Texture	==============================//
		for( int i=0; i<6; ++i )
		{
			m_TexIrradianceAniso[i].Allocate( m_Attrib.Resolution.x, m_Attrib.Resolution.y, m_Attrib.Resolution.z, m_Attrib.Resolution.w, FORMAT_R16G16B16A16_FLOAT );//FORMAT_R32G32B32A32_FLOAT );//DATA_FORMAT::FORMAT_R8G8B8_UNORM );
			m_TexIrradianceAniso[i].SetFilterMode( FILTER_MODE::FILTER_MAG_LINEAR_MIN_LINEAR_MIPMAP_LINEAR );
			m_TexIrradianceAniso[i].SetAddressMode( TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER, TEX_ADDRESS_BORDER );
			m_TexIrradianceAniso[i].GenHardwareTexture();
		}

		m_SurfaceVoxelCounter.Init();
		//m_TboSurfaceVoxelList.Init( FORMAT_R32_UINT, w*h*d, GL_DYNAMIC_DRAW, NULL );// TBO version
		m_ssboSurfaceVoxelList.Init( sizeof( uint32 )*w*h*d, GL_DYNAMIC_COPY, m_BindPoints[0] );// SSBO version

		UpdateMatrix();
	}
	


	void VCTVoxelData::Release()
	{
		InitZero( m_Attrib.Resolution );
		InitZero( m_Origin );
		m_Attrib.CellDim	= 0.0f;
		for( int i=0; i<NUM_SPACE; ++i )
		{
			InitZero( m_Basis[i] );
			MatZero( m_Transform.MatAxisSpace[i] );
		}
	
		
		m_TexPosition.Release();
		m_TexNormal.Release();
		m_TexColor.Release();
		
		m_TexIrradiance.Release();
		
		for( int i=0; i<6; ++i )
			m_TexIrradianceAniso[i].Release();
		
		m_SurfaceVoxelCounter.Release();
		//m_TboSurfaceVoxelList.Release();// TBO version
		m_ssboSurfaceVoxelList.Release();// SSBO version
	}



	void VCTVoxelData::SetBasis( const Vec3f& vecX, const Vec3f& vecY, const Vec3f& vecZ )
	{
		m_Basis[X_SPACE]	= vecX;// x axis
		m_Basis[Y_SPACE]	= vecY;// y axis
		m_Basis[Z_SPACE]	= vecZ;// z axis
	}
	
	
	
	void VCTVoxelData::SetBasis(	float vecX_x, float vecX_y, float vecX_z, float vecY_x, float vecY_y, float vecY_z, float vecZ_x, float vecZ_y, float vecZ_z )
	{
		InitVec( m_Basis[X_SPACE], vecX_x, vecX_y, vecX_z );// x axis
		InitVec( m_Basis[Y_SPACE], vecY_x, vecY_y, vecY_z );// y axis
		InitVec( m_Basis[Z_SPACE], vecZ_x, vecZ_y, vecZ_z );// z axis	
	}



	void VCTVoxelData::UpdateMatrix()
	{
		const float half_width	= 0.5f * m_Attrib.CellDim * float( m_Attrib.Resolution.x );
		const float half_height	= 0.5f * m_Attrib.CellDim * float( m_Attrib.Resolution.y );
		const float half_depth	= 0.5f * m_Attrib.CellDim * float(m_Attrib.Resolution.z);
		const Vec3f traslate	= { -m_Origin.x, -m_Origin.y, -m_Origin.z };


		//====== Calculate World-to-VoxelSpace Transform Matrix(NEW!!!! 2015.02.13) ======//
		Mat4f matTrans, matScale;

		MatView( matTrans, m_Basis[X_SPACE], m_Basis[Y_SPACE], m_Basis[Z_SPACE], traslate );// ボクセル中心点基準に平行移動 + 座標変換
		MatScale( matScale, 1.0f / half_width, 1.0f / half_height, 1.0f / half_depth );// [-1.0,1.0]の範囲に正規化

		Multiply( m_Transform.MatVoxelSpace, matScale, matTrans );


		//============ Calculate Voxel-to-WorldSpace Transform Matrix ============//
		MatInverse( MatVoxelToWorldSpce, m_Transform.MatVoxelSpace );


		//============ Calculate World-to-TextureSpace Transform Matrix =========//
		Mat4f	matShrinkTexCoord,	// [-0.5, +0.5]の範囲にテクスチャ座標を収める
				matOffsetTexCoord,	// テクスチャ座標を+0.5して、[0.0, 1.0]の範囲に補正する
				matVoxelToTexSpace;

		MatScale( matShrinkTexCoord, 0.5f, 0.5f, 0.5f );
		MatTranslation( matOffsetTexCoord, 0.5f, 0.5f, 0.5f );
		Multiply( matVoxelToTexSpace, matOffsetTexCoord, matShrinkTexCoord );

		Multiply( m_Transform.MatTexSpace, matVoxelToTexSpace, m_Transform.MatVoxelSpace );


		//========== Calculate TexCoord-to-WorldSpace Transform Matrix ==========//
		MatInverse( MatTexToWorldSpace, m_Transform.MatTexSpace );


		//================ Calculate X/Y/Z Axis space Matrix ====================//
		Vec3f u, v, n, pos;
		Mat4f matView, matOrtho;

		// X-Axis
		Reverse( u, m_Basis[2] );
		v	= m_Basis[1];
		n	= m_Basis[0];
		AddScaled( pos, traslate, -half_width, n );
		MatView( matView, u, v, n, pos );
		MatOrtho( matOrtho, -half_depth, +half_depth, -half_height, +half_height, 0.0f, m_Attrib.CellDim * float(m_Attrib.Resolution.x) );
		Multiply( m_Transform.MatAxisSpace[ X_SPACE ], matOrtho, matView );

		// Y-Axis
		u	= m_Basis[0];
		Reverse( v, m_Basis[2] );
		n	= m_Basis[1];
		AddScaled( pos, traslate, -half_height, n );
		MatView( matView, u, v, n, pos );
		MatOrtho( matOrtho, -half_width, +half_width, -half_depth, +half_depth, 0.0f, m_Attrib.CellDim * float(m_Attrib.Resolution.y) );
		Multiply( m_Transform.MatAxisSpace[ Y_SPACE ], matOrtho, matView );

		// Z-Axis
		u	= m_Basis[0];
		v	= m_Basis[1];
		n	= m_Basis[2];
		AddScaled( pos, traslate, -half_depth, n );
		MatView( matView, u, v, n, pos );
		MatOrtho( matOrtho, -half_width, +half_width, -half_height, +half_height, 0.0f, m_Attrib.CellDim * float(m_Attrib.Resolution.z) );
		Multiply( m_Transform.MatAxisSpace[ Z_SPACE ], matOrtho, matView );


		//============== Calculate CellIndex-to-TexCoord Transform Matrix ===============//
		Mat4f matCellIdOffset;// セルインデックスを+0.5だけオフセットする行列
		MatTranslation( matCellIdOffset, 0.5f, 0.5f, 0.5f );

		Mat4f matCellIdNormalize;// セルインデックスを正規化する行列
		MatScale( matCellIdNormalize, 1.0f/float(m_Attrib.Resolution.x), 1.0f/float(m_Attrib.Resolution.y), 1.0f/float(m_Attrib.Resolution.z) );

		Multiply( MatCellIdToTexCoord, matCellIdNormalize, matCellIdOffset );


		//============== Calculate CellIndex-to-WorldSpace Transform Matrix ===============//
		Multiply( m_Transform.MatCellIdToWorldSpace, MatTexToWorldSpace, MatCellIdToTexCoord );


	}


}// end of namespace