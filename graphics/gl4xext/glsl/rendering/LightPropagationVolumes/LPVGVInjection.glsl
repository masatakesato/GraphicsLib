
// GVを真上(ワールド座標z軸)から見下ろして，対応するzスライスに仮想光源を配置するイメージ


// ヘッダーファイルインクルード
#pragma include "LPVHelperFunctions.glslh"


// RSMのグローバル変数
uniform ivec2		g_RsmSize;			// RSMの解像度
uniform sampler2D	g_PositionSampler;	// ワールド座標
uniform sampler2D	g_NormalSampler;	// 法線ベクトル


// LPVのグローバル変数
uniform float		g_LpvCellDim;		// LPVセルの辺の長さ
uniform vec3		g_LpvCellSize;
//uniform mat4		g_matGVScreen;		// 頂点座標をLPVスクリーン座標系に変換する行列


uniform matParam
{
	mat4	g_matLPVScreen;
}matparam;



//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_

// 変数


// 入力
in vec2			POSITION;	// 頂点座標


// 出力
out vec3		vNormal;	// 法線ベクトル
out float		vArea;		// サーフェルの面積
out int			vLayer;


// メイン関数
void main()
{
	//============= 法線ベクトルをg_NormalSamplerから取得する =================//
	vNormal			= texture(g_NormalSampler, POSITION).xyz;


	//=========== LPVスクリーン空間上での仮想点光源の位置を計算する ===========//
	// g_PositionSamplerからワールド座標を取得する
	vec4 worldpos	= texture(g_PositionSampler, POSITION).xyzw;
	
	// 法線ベクトル方向に1セル分の距離だけ移動する
	worldpos.xyz	-= 1.0 * g_LpvCellDim * vNormal;

	// GVの位置に対して，VPLの配置場所を半セル分だけ移動する
	worldpos.xyz	-= 0.5 * vec3(g_LpvCellDim);
	
	// ワールド座標からLPVスクリーン空間座標に変換する
	gl_Position		= vec4(worldpos.xyz, 1.0) * matparam.g_matLPVScreen;//g_matGVScreen;

	//=====================  仮想点光源の面積を計算する. TODO:間違い!!!これはFOV90度の場合. =======================//
	vArea			= 4.0 * worldpos.w * worldpos.w / float(g_RsmSize.x * g_RsmSize.y);

	//TODO: ReflectiveShadowMapShaderでテクセルの面積を計算し、Flux.w成分に入れておく. 2015.01.23

	//===================== レイヤを計算する ==============================//
	vLayer			= int( (gl_Position.z*0.5 + 0.5) * g_LpvCellSize.z );

}


#endif



//#################### ジオメトリシェーダ ##################//
#ifdef _GEOMETRY_SHADER_


// 入出力プリミティブタイプ
layout(points) in;
layout(points, max_vertices=1) out;

// 変数


// 入力
in vec3		vNormal[1];	// 法線ベクトル
in float	vArea[1];	// サーフェルの面積
in int		vLayer[1];


// 出力
out vec3	gNormal;		// 法線ベクトル
out float	gArea;			// サーフェルの面積


// メイン関数
void main()
{
	//============ 面上の頂点だけ出力する =============//
	if(length(vNormal[0])>0.0)
	{
		// レイヤの割り当て
		gl_Layer = vLayer[0];// テクスチャレイヤーの指定. 必ずEmitVertexの前に実行する

		// VPL情報の書き出し
		gl_Position		= gl_in[0].gl_Position;
		gNormal			= vNormal[0];
		gArea			= vArea[0];

		EmitVertex();

		EndPrimitive();
	}
}



#endif





//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 入力
in vec3		gNormal;		// 法線ベクトル
in float	gArea;			// サーフェルの面積

// 出力
out vec4	fSH_GV;		// 球面調和関数の係数(形状データ)


// メイン関数
void main()
{
	// 遮蔽物法線方向のSH係数 * セルに占めるSurfelの面積
	fSH_GV = CalcSHCoeff_CosineLobe(gNormal) * clamp( gArea / (g_LpvCellDim*g_LpvCellDim), 0.0, 1.0);
}


#endif
