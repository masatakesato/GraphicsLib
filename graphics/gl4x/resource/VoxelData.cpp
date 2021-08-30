#include	"VoxelData.h"

#include	"GLHelperFunctions.h"




namespace OreOreLib
{

	VoxelData::VoxelData()
	{
		InitZero( m_Attrib.Resolution );
		InitZero( m_Origin );
		m_Attrib.CellDim	= 0.0f;

		InitVec( m_Basis[X_SPACE], 1.0f, 0.0f, 0.0f );// x axis
		InitVec( m_Basis[Y_SPACE], 0.0f, 1.0f, 0.0f );// y axis
		InitVec( m_Basis[Z_SPACE], 0.0f, 0.0f, 1.0f );// z axis

		m_numTextures	= 0;
		m_pTextures		= NULL;
	}



	VoxelData::~VoxelData()
	{
		Release();
	}



	void VoxelData::Init( int numtex, const DATA_FORMAT formats[], const Vec3i &size, float dim )
	{
		Init( numtex, formats, size.x, size.y, size.z, dim );
	}



	void VoxelData::Init( int numtex, const DATA_FORMAT formats[], int w, int h, int d, float dim )
	{
		if( numtex==0 || !formats )
			return;

		Release();

		InitVec( m_Attrib.Resolution, w, h, d );
		InitZero( m_Origin );
		m_Attrib.CellDim	= dim;

		//=======================	Init 3D Textures	==========================//
		m_numTextures	= numtex;
		m_pTextures		= new Texture3D[m_numTextures];

		for( int i=0; i< m_numTextures; ++i )
		{
			m_pTextures[i].Allocate( m_Attrib.Resolution.x, m_Attrib.Resolution.y, m_Attrib.Resolution.z, 0, formats[i] );
			//m_pTextures[i].SetFilterMode( TEX_FILTER_LINEAR, TEX_FILTER_LINEAR );
			m_pTextures[i].GenHardwareTexture();
		}// end of i loop


		UpdateMatrix();
	}



	void VoxelData::Release()
	{
		InitZero( m_Attrib.Resolution );
		InitZero( m_Origin );
		m_Attrib.CellDim	= 0.0f;

		for( int i=0; i<NUM_SPACE; ++i )
			InitZero( m_Basis[i] );

		m_numTextures	= 0;
		SafeDeleteArray( m_pTextures );
	}



	void VoxelData::SetBasis( const Vec3f& vecX, const Vec3f& vecY, const Vec3f& vecZ )
	{
		m_Basis[X_SPACE]	= vecX;// x axis
		m_Basis[Y_SPACE]	= vecY;// y axis
		m_Basis[Z_SPACE]	= vecZ;// z axis
	}



	void VoxelData::SetBasis( float vecX_x, float vecX_y, float vecX_z, float vecY_x, float vecY_y, float vecY_z, float vecZ_x, float vecZ_y, float vecZ_z )
	{
		InitVec( m_Basis[X_SPACE], vecX_x, vecX_y, vecX_z );// x axis
		InitVec( m_Basis[Y_SPACE], vecY_x, vecY_y, vecY_z );// y axis
		InitVec( m_Basis[Z_SPACE], vecZ_x, vecZ_y, vecZ_z );// z axis	
	}



	void VoxelData::UpdateMatrix()
	{
		float half_width	= 0.5f * m_Attrib.CellDim * float( m_Attrib.Resolution.x );
		float half_height	= 0.5f * m_Attrib.CellDim * float( m_Attrib.Resolution.y );
		float half_depth	= 0.5f * m_Attrib.CellDim * float( m_Attrib.Resolution.z );


		//============ ワールド空間から正規化ボクセル空間座標への変換行列を計算する =========//
		const Vec3f traslate	={ -m_Origin.x, -m_Origin.y, -m_Origin.z };
		Mat4f matTrans, matScale;

		MatView( matTrans, m_Basis[X_SPACE], m_Basis[Y_SPACE], m_Basis[Z_SPACE], traslate );// ボクセル中心点基準に平行移動 + 座標変換
		MatScale( matScale, 1.0f / half_width, 1.0f / half_height, 1.0f / half_depth );// [-1.0,1.0]の範囲に正規化

		Multiply( m_Transform.MatVoxelSpace, matScale, matTrans );
		

		//============ 正規化ボクセル空間からワールド空間への変換行列を計算する =========//
		MatInverse( m_MatVoxelToWorldSpace, m_Transform.MatVoxelSpace );


		//============ ワールド空間から3Dテクスチャ座標への変換行列を計算する =========//
		Mat4f	matShrinkTexCoord,	// [-0.5, +0.5]の範囲にテクスチャ座標を収める
			matOffsetTexCoord,	// テクスチャ座標を+0.5して、[0.0, 1.0]の範囲に補正する
			matVoxelToTexSpace;

		MatScale( matShrinkTexCoord, 0.5f, 0.5f, 0.5f );
		MatTranslation( matOffsetTexCoord, 0.5f, 0.5f, 0.5f );
		Multiply( matVoxelToTexSpace, matOffsetTexCoord, matShrinkTexCoord );

		Multiply( m_Transform.MatTexSpace, matVoxelToTexSpace, m_Transform.MatVoxelSpace );


		//============ 3Dテクスチャ座標からワールド空間への変換行列を計算する =========//
		MatInverse( m_MatTexToWorldSpace, m_Transform.MatTexSpace );


		//================ Calculate X/Y/Z Axis space Matrix ====================//
		Vec3f u, v, n, pos;
		Mat4f matView, matOrtho;

		// X-Axis
		Reverse( u, m_Basis[2] );
		v	= m_Basis[1];
		n	= m_Basis[0];
		AddScaled( pos, traslate, -half_width, n );
		MatView( matView, u, v, n, pos );
		MatOrtho( matOrtho, -half_depth, +half_depth, -half_height, +half_height, 0.0f, m_Attrib.CellDim * float( m_Attrib.Resolution.x ) );
		Multiply( m_Transform.MatAxisSpace[X_SPACE], matOrtho, matView );

		// Y-Axis
		u	= m_Basis[0];
		Reverse( v, m_Basis[2] );
		n	= m_Basis[1];
		AddScaled( pos, traslate, -half_height, n );
		MatView( matView, u, v, n, pos );
		MatOrtho( matOrtho, -half_width, +half_width, -half_depth, +half_depth, 0.0f, m_Attrib.CellDim * float( m_Attrib.Resolution.y ) );
		Multiply( m_Transform.MatAxisSpace[Y_SPACE], matOrtho, matView );

		// Z-Axis
		u	= m_Basis[0];
		v	= m_Basis[1];
		n	= m_Basis[2];
		AddScaled( pos, traslate, -half_depth, n );
		MatView( matView, u, v, n, pos );
		MatOrtho( matOrtho, -half_width, +half_width, -half_height, +half_height, 0.0f, m_Attrib.CellDim * float( m_Attrib.Resolution.z ) );
		Multiply( m_Transform.MatAxisSpace[Z_SPACE], matOrtho, matView );


	}


}// end of namespace