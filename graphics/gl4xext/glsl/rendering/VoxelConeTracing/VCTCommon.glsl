#ifndef	VCT_COMMON_GLSL
#define	VCT_COMMON_GLSL




struct VCTVoxelAttribute
{
	ivec4	Resolution;
	float	CellDim;	// セルの1辺の長さ
};


struct VCTVoxelTransform
{
	mat4	MatVoxelSpace;	// ボクセル空間への座標変換行列
	mat4	MatTexSpace;	// テクスチャ空間への座標変換行列
	mat4	MatAxisSpace[3];// XYZ各軸方向のボクセル平面空間へ座標変換行列
	mat4	MatCellIdToWorldSpace;
};



uniform ubVCTVoxelAttribute
{
	VCTVoxelAttribute	g_VCTVoxelAttribute;
};


uniform ubVCTVoxelTransform
{
	VCTVoxelTransform	g_VCTVoxelTransform;
};



#define	g_VoxelResolution	g_VCTVoxelAttribute.Resolution.xyz	//uniform ivec3	g_VoxelResolution;
#define	g_numCascadeLevels	g_VCTVoxelAttribute.Resolution.w
#define g_VoxelDim			g_VCTVoxelAttribute.CellDim
#define	g_MatVoxelSpace		g_VCTVoxelTransform.MatVoxelSpace	//uniform mat4	g_MatVoxelSpace;	// ボクセル空間への座標変換行列
#define	g_MatTexSpace		g_VCTVoxelTransform.MatTexSpace		//uniform mat4	g_MatTexSpace;		// テクスチャ空間への座標変換行列
#define	g_MatAxisSpace(i)	g_VCTVoxelTransform.MatAxisSpace[i]	//uniform mat4	g_MatAxisSpace[3];	// XYZ各軸方向のボクセル平面空間へ座標変換行列
#define	g_DominantAxis(i)	g_MatAxisSpace(i)[2].xyz			//uniform vec3	g_DominantAxis[3];	// 軸


#define g_MatCellToWorldSpace	g_VCTVoxelTransform.MatCellIdToWorldSpace;




// Helper Functions

// レイ始点から、AABBの進入/退出位置それぞれへの距離を計算し、AABBとレイが交差判定を行う。
// (http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
bool bIntersectRayWithAABB( in vec3 rayOrigin, in vec3 rayDirection,
							in vec3 bbMin, in vec3 bbMax,
							out float tEnter, out float tExit )
{
	vec3 tempMin	= ( bbMin - rayOrigin ) / rayDirection;
	vec3 tempMax	= ( bbMax - rayOrigin ) / rayDirection;

	vec3 v3Max		= max( tempMax, tempMin );
	vec3 v3Min		= min( tempMax, tempMin );

	tExit			= min( v3Max.x, min(v3Max.y, v3Max.z) );
	tEnter			= max( max( v3Min.x, 0.0 ), max( v3Min.y, v3Min.z ) );

	return tExit >= tEnter;
}




#endif	// VCT_COMMON_GLSL //
