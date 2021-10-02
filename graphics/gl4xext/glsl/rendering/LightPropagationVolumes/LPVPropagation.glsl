
// TODO: 計算に必要なデータを予め定数にしておく
// SH_DirectionalConstraint = CalcSHCoeff_CosineLobe(PropDirection) * g_FrontFacingArea; → 伝播方向に制約をかけるSH
// SH_EvalVec = CalcSHCoeff(g_EvaluationVector[0]);										→// 評価ベクトルのSH
// nl_SH	= CalcSHCoeff_CosineLobe(g_ReprojectionVector[0]);						→再投影ベクトルのコサインローブSH


// VPLを真上(ワールド座標z軸)から見下ろして，対応するzスライスに仮想光源を配置するイメージ


// ヘッダーファイルインクルード
#pragma include "LPVHelperFunctions.glslh"


//uniform int			g_Iter;// 伝播計算の反復回数. 未使用. 0回目だけGeometryVolumeを使った遮蔽計算を無視していた->必要ない


uniform sampler3D	g_LPV_SH_R;	// LPVテクスチャ(R)
uniform sampler3D	g_LPV_SH_G;	// LPVテクスチャ(G)
uniform sampler3D	g_LPV_SH_B;	// LPVテクスチャ(B)

uniform sampler3D	g_SHaccum_R;// LPV差分
uniform sampler3D	g_SHaccum_G;// LPV差分
uniform sampler3D	g_SHaccum_B;// LPV差分

uniform sampler3D	g_GV_SH;	// GeometryVolume


// LPVのグローバル変数
uniform vec3		g_LpvSize;	// LPVセルの解像度(xyz:解像度,w:1セル分の)


// xyz各方向の，セル1個あたりのテクスチャ


#define M_PI 3.141592


//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_

// 変数

// 入力
in vec4		POSITION;	// LPVスクリーン空間上の頂点座標(x,y,z各成分が[-1,1]に正規化されている)
in vec3		TEXCOORD;	// 3次元テクスチャ座標(各頂点に対応したテクスチャ座標)

// 出力
out vec3	vTexCoord;	// テクスチャ座標
out int		vLayer;		// 3Dテクスチャのレイヤ


// メイン関数
void main()
{
	//=========== 頂点座標はPOSITIONをそのまま出力 ===========//
	gl_Position		= POSITION;
	
	//============== テクスチャ座標もそのまま出力 ============//
	vTexCoord		= TEXCOORD;
	//vTexCoord.xy	+= 0.5 / g_LpvSize.xy;

	//======= 頂点に対応する3Dテクスチャのレイヤを計算 =======//
	vLayer = int( TEXCOORD.z  * g_LpvSize.z );
}

#endif



//#################### ジオメトリシェーダ ##################//
#ifdef _GEOMETRY_SHADER_

// 入出力プリミティブタイプ
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

// 変数

// 入力
in vec3		vTexCoord[3];	// テクスチャ座標
in int		vLayer[3];		// 3Dテクスチャのレイヤ

// 出力
out vec3	gTexCoord;		// テクスチャ座標


// メイン関数
void main()
{
	gl_Position	= gl_in[0].gl_Position;
	gl_Layer	= vLayer[0];// 書き込むテクスチャレイヤーの指定(必ずEmitVertexの前に実行すること)
	gTexCoord	= vTexCoord[0];// テクスチャ座標の出力
	EmitVertex();

	gl_Position	= gl_in[1].gl_Position;
	gl_Layer	= vLayer[1];// 書き込むテクスチャレイヤーの指定(必ずEmitVertexの前に実行すること)
	gTexCoord	= vTexCoord[1];// テクスチャ座標の出力
	EmitVertex();

	gl_Position	= gl_in[2].gl_Position;
	gl_Layer	= vLayer[2];// 書き込むテクスチャレイヤーの指定(必ずEmitVertexの前に実行すること)
	gTexCoord	= vTexCoord[2];// テクスチャ座標の出力
	EmitVertex();
	
	EndPrimitive();
}

#endif





//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 変数


// 入力
in vec3		gTexCoord;	// テクスチャ座標

// 出力
out vec4	fSH_R;		// 球面調和関数の係数(赤)
out vec4	fSH_G;		// 球面調和関数の係数(緑)
out vec4	fSH_B;		// 球面調和関数の係数(青)

out vec4	fSHaccum_R;	// SHの更新差分(赤).隣接セルからの伝播結果「だけ」が格納される
out vec4	fSHaccum_G;	// SHの更新差分(緑).隣接セルからの伝播結果「だけ」が格納される
out vec4	fSHaccum_B;	// SHの更新差分(青).隣接セルからの伝播結果「だけ」が格納される



// 関数
void Propagate(inout vec4 SHaccum_R, inout vec4 SHaccum_G, inout vec4 SHaccum_B, in vec3 cellIndex, in vec3 nOffset)
{
	vec3 neighborIndex		= cellIndex + nOffset / g_LpvSize;
	vec4 SHCoeff_neighbor_R	= texture( g_SHaccum_R, neighborIndex );
	vec4 SHCoeff_neighbor_G	= texture( g_SHaccum_G, neighborIndex );
	vec4 SHCoeff_neighbor_B	= texture( g_SHaccum_B, neighborIndex );
	vec4 SH_PropCone		= Cone90Degree(-nOffset) * 2.0 * M_PI;
	vec4 SH_Reproj			= CalcSHCoeff_CosineLobe(-nOffset);
	

	//vec3 Flux = vec3(
	//				max(0, dot(SHCoeff_neighbor_R, SH_PropCone)),
	//				max(0, dot(SHCoeff_neighbor_G, SH_PropCone)),
	//				max(0, dot(SHCoeff_neighbor_B, SH_PropCone))
	//				) / 6.0;

	vec3 Flux = abs( vec3(
					dot(SHCoeff_neighbor_R, SH_PropCone),
					dot(SHCoeff_neighbor_G, SH_PropCone),
					dot(SHCoeff_neighbor_B, SH_PropCone)
					) ) / 6.0;

	float OcclusionFactor = 1.0;//1:遮蔽なし, 0: 完全に遮蔽されている
	
	
	//if(g_Iter>0)// 遮蔽の計算．自分のGVで自分を遮蔽しないように，伝播計算の最初の1回目は本処理を飛ばす->GV作成時に必要ピクセル数ずらしてあるので必要なし
	{
		vec3 OccTexCoord	= (cellIndex + 0.5*nOffset/g_LpvSize) - 0.5/g_LpvSize;
		vec4 SHocclusion	= texture(g_GV_SH, OccTexCoord);
		// 負になった場合は遮蔽が発生。それ以外は遮蔽なし
		OcclusionFactor = 1.0 + clamp( dot( SHocclusion, CalcSHCoeff(-nOffset) ), -1.0, 0.0 );
	}
	
	SHaccum_R += Flux.r * SH_Reproj * OcclusionFactor;
	SHaccum_G += Flux.g * SH_Reproj * OcclusionFactor;
	SHaccum_B += Flux.b * SH_Reproj * OcclusionFactor;

}





// メイン関数
void main()
{

	fSHaccum_R = vec4(0);
	fSHaccum_G = vec4(0);
	fSHaccum_B = vec4(0);

	// 伝播した放射束を計算する
	Propagate( fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(+1, 0, 0) );
	Propagate( fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(-1, 0, 0) );
	Propagate( fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, +1, 0) );
	Propagate( fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, -1, 0) );
	Propagate( fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, 0, +1) );
	Propagate( fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, 0, -1) );


	// 各色のSHをアクティブなセルから取得する
	fSH_R	= texture( g_LPV_SH_R, gTexCoord ).rgba + fSHaccum_R;
	fSH_G	= texture( g_LPV_SH_G, gTexCoord ).rgba + fSHaccum_G;
	fSH_B	= texture( g_LPV_SH_B, gTexCoord ).rgba + fSHaccum_B;

}

#endif
