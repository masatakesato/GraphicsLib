#ifndef	LPV_IRRADINACE_VOLUME
#define	LPV_IRRADINACE_VOLUME


#include	"Texture.h"



namespace OreOreLib
{

	class SH_LPV
	{
	public:

		SH_LPV();
		~SH_LPV();
		void Init( int width, int height, int depth );
		void Release();


	private:

		Texture3D	rgb[3];

		friend class LPVIrradianceVolume;
	};

	
	// LPV Voxel Data
	class LPVIrradianceVolume
	{

	public:

		LPVIrradianceVolume();
		~LPVIrradianceVolume();

		void Init( int num_cascades, const Vec3i &lpv_size );	// LPVテクスチャを生成する
		void Init( int num_cascades, int width, int height, int depth );
		void Release();	// LPVテクスチャを削除する

		inline void SwapBuffers()		{ Src ^= 1;	Dst ^= 1; }

		void SetOrigin( int lv, float x, float y, float z );
		void SetCellDim( float dim );

		inline int NumCascades() const			{ return m_numCascades; }
		inline int Width() const				{ return m_LpvSize.x; }
		inline int Height() const				{ return m_LpvSize.y; }
		inline int Depth() const				{ return m_LpvSize.z; }

		inline float GetCellDim( int lv ) const	{ return m_LpvCellDim[lv]; }
		inline Vec3f* GetOrigin( int lv ) const	{ return &m_LpvOrigin[lv]; }
		
		inline Texture3D* GetSourceLPV( int lv, int colr_idx ) const	{ return &m_SH_LPVs[Src][lv].rgb[ colr_idx ]; }
		inline Texture3D* GetDestLPV( int lv, int colr_idx ) const		{ return &m_SH_LPVs[Dst][lv].rgb[ colr_idx ]; }
		inline Texture3D* GetSourceAccumSH( int colr_idx ) const		{ return (Texture3D*)&m_SHaccum[Src].rgb[colr_idx]; }
		inline Texture3D* GetDestAccumSH( int colr_idx ) const			{ return (Texture3D*)&m_SHaccum[Dst].rgb[colr_idx]; }
		inline Texture3D* GetGeometryVolume( int lv ) const				{ return &m_SH_GV[lv]; }
		
		inline Mat4f* GetTransformMatrix( int lv ) const				{ return &m_MatVoxelSpace[lv]; }
		inline Mat4f* GetTexCoordMatrix( int lv ) const					{ return &m_MatTextureSpace[lv]; }


		void UpdateMatrix();
		
		

	private:

		// Light Propagation Volumesの情報
		int			m_numCascades;	// LPVの多重解像度レベル数
		Vec3i		m_LpvSize;	// LPV(LighhtPropagationVolumes)のピクセル解像度
		float		*m_LpvCellDim;	// カスケード毎のLPVセルの1辺の長さ
		Vec3f		*m_LpvOrigin;	// カスケード毎のLPVの中心座標. ボクセル空間の中心にある
		Mat4f		*m_MatVoxelSpace;	// ワールド空間座標をボクセル空間座標に変換する行列
		Mat4f		*m_MatTextureSpace;	// ワールド座標をボクセルのインデックスに変換する行列


		// LightPrpagationVolumeテクスチャ
		SH_LPV		*m_SH_LPVs[2];	// LPVのSH.2次元配列(1次元目:FrontBack, 2次元目:Lv)
		SH_LPV		m_SHaccum[2];	// LPV反復計算用の一時バッファ
		Texture3D	*m_SH_GV;		// GeometryVolumesのSH



		unsigned char Src, Dst;
	};



}// end of namespace






#endif	// LPV_IRRADINACE_VOLUME //