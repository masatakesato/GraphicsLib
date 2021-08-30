#ifndef VOXEL_DATA_H
#define	VOXEL_DATA_H



#include	"Texture.h"
#include	"IVoxelData.h"
#include	"GLUniformBufferObject.h"
#include	"GLAtomicCounterBufferObject.h"
#include	"GLTextureBufferObject.h"



namespace OreOreLib
{

	class VoxelData : public IVoxelData
	{
	public:

		VoxelData();
		~VoxelData();

		void Init( int numtex, const DATA_FORMAT formats[], const Vec3i &size, float dim );
		void Init( int numtex, const DATA_FORMAT formats[], int w, int h, int d, float dim );

		void Release();

		Texture3D* GetTexture( int idx ) const							{ return idx>=m_numTextures ? NULL : ( Texture3D * )&m_pTextures[idx]; }
		const Vec3i* GetResolution() const								{ return &m_Attrib.Resolution; }

		const Mat4f* GetMatAxisSpaceX() const							{ return &m_Transform.MatAxisSpace[X_SPACE]; }
		const Mat4f* GetMatAxisSpaceY() const							{ return &m_Transform.MatAxisSpace[Y_SPACE]; }
		const Mat4f* GetMatAxisSpaceZ() const							{ return &m_Transform.MatAxisSpace[Z_SPACE]; }
		
		// Override Virtual Functions
		virtual void SetOrigin( float x, float y, float z, int lv=0 )	{ InitVec( m_Origin, x, y, z ); }
		virtual void SetCellDim( float dim, int lv=0 )					{ m_Attrib.CellDim	= dim; }
		virtual void SetBasis( const Vec3f& vecX, const Vec3f& vecY, const Vec3f& vecZ );
		virtual void SetBasis( float vecX_x, float vecX_y, float vecX_z, float vecY_x, float vecY_y, float vecY_z, float vecZ_x, float vecZ_y, float vecZ_z );

		virtual const uint32 Width( int lv=0 ) const					{ return m_Attrib.Resolution.x; }
		virtual const uint32 Height( int lv=0 ) const					{ return m_Attrib.Resolution.y; }
		virtual const uint32 Depth( int lv=0 ) const					{ return m_Attrib.Resolution.z; }
		virtual const uint32 NumLevels() const							{ return 0; }
		virtual const float GetCellDim( int lv=0 ) const				{ return m_Attrib.CellDim; }
		virtual const Vec3f* GetBasis() const							{ return &m_Basis[0]; }

		virtual const Mat4f* GetMatVoxelSpace( int lv=0 ) const			{ return &m_Transform.MatVoxelSpace; }
		virtual const Mat4f* GetMatTexSpace( int lv=0 ) const			{ return &m_Transform.MatTexSpace; }
		virtual const Mat4f* GetMatInvVoxelSpace( int lv=0 ) const		{ return &m_MatVoxelToWorldSpace; }
		virtual const Mat4f* GetMatInvTexSpace( int lv=0 ) const		{ return &m_MatTexToWorldSpace; }

		virtual void UpdateMatrix();


	private:
		
		struct VoxelAttribute
		{
			Vec3i	Resolution;	// xyz:ボクセル解像度
			float	CellDim;	// セルの1辺の長さ
		};


		struct VoxelTransform
		{
			Mat4f	MatVoxelSpace;	// ワールド空間から、正規化したボクセルローカル座標空間への座標変換行列
			Mat4f	MatTexSpace;	// ワールド空間から、3Dテクスチャ座標空間への座標変換行列
			Mat4f	MatAxisSpace[3];// XYZ各軸方向のボクセル平面空間へ座標変換行列
			//Mat4f	MatCellIdToWorldSpace;	// TODO: ボクセルインデックスからワールド空間への座標変換行列

		};
		

		Vec3f			m_Origin;			// 原点. ボクセル空間の中心にある
		Vec3f			m_Basis[NUM_SPACE];	// 基底ベクトル
		VoxelAttribute	m_Attrib;
		VoxelTransform	m_Transform;

		Mat4f			m_MatVoxelToWorldSpace;	// 正規化ボクセル空間からワールド空間への変換行列
		Mat4f			m_MatTexToWorldSpace;	// 3Dテクスチャ座標からワールド空間への変換行列

		// Volume Data
		int				m_numTextures;
		Texture3D		*m_pTextures;

	};


}// end of namespace



#endif // !VOXEL_DATA_H
