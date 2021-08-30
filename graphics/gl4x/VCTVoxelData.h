#ifndef	VCT_VOXEL_DATA_H
#define	VCT_VOXEL_DATA_H


#include	"IVoxelData.h"


namespace OreOreLib
{

	class VCTVoxelData : public IVoxelData
	{
	public:

		VCTVoxelData();
		~VCTVoxelData();

		void Init( const Vec3i &size, int numcascades, float dim );
		void Init( int w, int h, int d, int numcascades, float dim );

		void Release();

		const Vec4i* GetResolution() const								{ return &m_Attrib.Resolution; }

		const Mat4f* GetMatAxisSpaceX() const							{ return &m_Transform.MatAxisSpace[X_SPACE]; }
		const Mat4f* GetMatAxisSpaceY() const							{ return &m_Transform.MatAxisSpace[Y_SPACE]; }
		const Mat4f* GetMatAxisSpaceZ() const							{ return &m_Transform.MatAxisSpace[Z_SPACE]; }

		const Mat4f* GetMatTexToWorldSpace( int lv=0 ) const			{ return &MatTexToWorldSpace; }
		const Mat4f* GetMatCellIdToTexCoord( int lv=0 ) const			{ return &MatCellIdToTexCoord; }
		const Mat4f* GetMatCellIdToWorldSpace( int lv=0 ) const			{ return &m_Transform.MatCellIdToWorldSpace; }

		Texture3D* GetTexPosition() const								{ return (Texture3D *)&m_TexPosition; }
		Texture3D* GetTexNormal() const									{ return (Texture3D *)&m_TexNormal; }
		Texture3D* GetTexColor() const									{ return (Texture3D *)&m_TexColor; }
		Texture3D* GetTexIrradiance() const								{ return (Texture3D *)&m_TexIrradiance; }
		Texture3D* GetTexIrradianceAniso( int i ) const					{ return (Texture3D *)&m_TexIrradianceAniso[i]; }


		// Override Virtual Functions
		virtual void SetOrigin( float x, float y, float z, int lv=0 )	{ InitVec( m_Origin, x, y, z ); }
		virtual void SetCellDim( float dim, int lv=0 )					{ m_Attrib.CellDim	= dim; }
		virtual void SetBasis( const Vec3f& vecX, const Vec3f& vecY, const Vec3f& vecZ );
		virtual void SetBasis( float vecX_x, float vecX_y, float vecX_z, float vecY_x, float vecY_y, float vecY_z, float vecZ_x, float vecZ_y, float vecZ_z );
		
		virtual const uint32 Width( int lv=0 ) const					{ return m_Attrib.Resolution.x; }
		virtual const uint32 Height( int lv=0 ) const					{ return m_Attrib.Resolution.y; }
		virtual const uint32 Depth( int lv=0 ) const					{ return m_Attrib.Resolution.z; }
		virtual const uint32 NumLevels() const							{ return m_Attrib.Resolution.w; }
		virtual const float GetCellDim( int lv=0 ) const				{ return m_Attrib.CellDim; }
		virtual const Vec3f* GetBasis() const							{ return &m_Basis[0]; }

		virtual const Mat4f* GetMatVoxelSpace( int lv=0 ) const			{ return &m_Transform.MatVoxelSpace; }
		virtual const Mat4f* GetMatTexSpace( int lv=0 ) const			{ return &m_Transform.MatTexSpace; }
		virtual const Mat4f* GetMatInvVoxelSpace( int lv=0 ) const		{ return &MatVoxelToWorldSpce; }
		virtual const Mat4f* GetMatInvTexSpace( int lv=0 ) const		{ return &MatTexToWorldSpace; }


		void UpdateMatrix();

		
	private:

		enum AXIS{ X_SPACE, Y_SPACE, Z_SPACE, NUM_SPACE };
		
		struct VCTVoxelAttribute
		{
			Vec4i	Resolution;	// xyz:ボクセル解像度, w:カスケード数
			float	CellDim;	// セルの1辺の長さ
		};
		

		struct VCTVoxelTransform
		{
			Mat4f	MatVoxelSpace;	// ワールド空間から、正規化したボクセルローカル空間への座標変換行列
			Mat4f	MatTexSpace;	// ワールド空間から、3Dテクスチャ座標空間への座標変換行列
			Mat4f	MatAxisSpace[3];// XYZ各軸方向のボクセル平面空間へ座標変換行列
			Mat4f	MatCellIdToWorldSpace;	// TODO: ボクセルインデックスからワールド空間への座標変換行列
		};

// Temporary Value
		Mat4f	MatVoxelToWorldSpce;
Mat4f	MatTexToWorldSpace;		// テクスチャ座標からワールド座標に変換する行列
Mat4f	MatCellIdToTexCoord;	// セルインデックスからテクスチャ座標に変換する行列

		Vec3f						m_Origin;				// 原点. ボクセル空間の中心にある
		Vec3f						m_Basis[ NUM_SPACE ];	// 基底ベクトル
		VCTVoxelAttribute			m_Attrib;
		VCTVoxelTransform			m_Transform;

		// Position, Normal, Color 3DTextures
		Texture3D					m_TexPosition;
		Texture3D					m_TexNormal;
		Texture3D					m_TexColor;

		// IrradianceVolumes
		Texture3D					m_TexIrradiance;		
		Texture3D					m_TexIrradianceAniso[6];// +X, -X, +Y, -Y, +Z, -Z方向のIrradianceVolume


		GLAtomicCounterBufferObject	m_SurfaceVoxelCounter;
		//GLTextureBufferObject		m_TboSurfaceVoxelList;// TBO version
		GLShaderStorageBufferObject	m_ssboSurfaceVoxelList;// SSBO version

		// Bind Points
		static GLuint				m_BindPoints[1];


		friend class VoxelConeTracing;
	};


}// end of namespace



#endif	// VCT_VOXEL_DATA_H //