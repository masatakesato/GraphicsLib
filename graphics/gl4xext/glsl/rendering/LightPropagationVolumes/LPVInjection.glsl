
// VPLを真上(ワールド座標z軸)から見下ろして，対応するzスライスに仮想光源を配置するイメージ


// ヘッダーファイルインクルード
#pragma include "lpv/LPVHelperFunctions.glslh"

// 光源情報
uniform vec3		g_LightPos;		// 光源の位置


// RSMのグローバル変数
uniform sampler2D	g_PositionSampler;	// ワールド座標
uniform sampler2D	g_NormalSampler;	// 法線ベクトル
uniform sampler2D	g_FluxSampler;		// 放射束


// LPVのグローバル変数
uniform float		g_LpvCellDim;		// LPVセルの辺の長さ
uniform vec3		g_LpvCellSize;

//uniform mat4		g_matLPVScreen;		// 頂点座標をLPVスクリーン座標系に変換する行列


uniform matParam
{
	mat4	g_matLPVScreen;
}matparam;




// 
uniform float		g_VPLScale;		// VPL点群の点が，RSMテクスチャのピクセルいくつに相当するか(例：RSM:256^2, VPL:128^2なら，4.0)







//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_

// 変数


// 入力
in vec2			POSITION;	// 頂点座標. VPLスプライト座標 (0.0, 0.0) - (1.0, 1.0)


// 出力
out vec3		vNormal;	// 法線ベクトル
out vec3		vFlux;		// 頂点座標
out int			vLayer;


// メイン関数
void main()
{
	//============= 法線ベクトルをg_NormalSamplerから取得する =================//
	vNormal			= texture( g_NormalSampler, POSITION ).xyz;
	

	//=========== LPVスクリーン空間上での仮想点光源の位置を計算する ===========//
	// g_PositionSamplerからワールド座標を取得する
	vec4 worldpos	= texture( g_PositionSampler, POSITION ).xyzw;	//worldpos	= vec4(-1.9, 0.0, -0.6, 0);	//worldpos = vec4(1.4, POSITION.x-0.5, POSITION.y-0.7, 1);

	// 法線ベクトル方向に0.25セル分の距離だけ移動する
	worldpos.xyz	+= 0.25 * g_LpvCellDim * vNormal;
	
	// 光線ベクトル方向に0.25セル分の距離だけ移動
	worldpos.xyz	+= 0.25 * g_LpvCellDim * normalize( g_LightPos - worldpos.xyz );
	
	// ワールド座標をLPVスクリーン空間座標に変換する
	gl_Position		= vec4(worldpos.xyz, 1.0) * matparam.g_matLPVScreen;


	//=============== g_FluxSamplerから仮想点光源の放射束を取得する ===========//
	vFlux			= texture( g_FluxSampler, POSITION ).rgb * g_VPLScale;


	//===================== レイヤを計算する ==============================//
	// ワールド座標→LPV空間座標
	// lpvscreenpos.zは,[-1, 1]の間の数字
	// これを，[0,1]にしてLPVの高さをかければ,,,
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
in vec3		vFlux[1];	// 頂点座標
in int		vLayer[1];


// 出力
out vec3	gNormal;		// 法線ベクトル
out vec3	gFlux;			// 放射束


// メイン関数
void main()
{
	//============ 明るさのある頂点だけ出力する =============//
	if(length(vFlux[0])>0.0)
	{
		// レイヤの割り当て
		gl_Layer = vLayer[0];// テクスチャレイヤーの指定. 必ずEmitVertexの前に実行する

		// VPL情報の書き出し
		gl_Position		= gl_in[0].gl_Position;
		gNormal			= vNormal[0];
		gFlux			= vFlux[0];

		EmitVertex();

		EndPrimitive();
	}
}



#endif





//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 入力
in vec3		gNormal;		// 法線ベクトル
in vec3		gFlux;			// 放射束


// 出力
out vec4	fSH_R;		// 球面調和関数の係数(赤)
out vec4	fSH_G;		// 球面調和関数の係数(緑)
out vec4	fSH_B;		// 球面調和関数の係数(青)
out vec4	fSHaccum_R;	// 球面調和関数の係数(赤)
out vec4	fSHaccum_G;	// 球面調和関数の係数(緑)
out vec4	fSHaccum_B;	// 球面調和関数の係数(青)


// メイン関数
void main()
{
	// 疑問：RSMのFluxを球面調和関数に変換するってことでOK？→OK

	// Flux * 法線ベクトル方向のコサインローブ / PI
	vec4 SHcoeffs = CalcSHCoeff_CosineLobe(gNormal) / M_PI;

	// 色毎にSH係数(2-band)を出力する.
	fSH_R		= gFlux.r * SHcoeffs;
	fSH_G		= gFlux.g * SHcoeffs;
	fSH_B		= gFlux.b * SHcoeffs;
	fSHaccum_R	= fSH_R;
	fSHaccum_G	= fSH_G;
	fSHaccum_B	= fSH_B;


	// デバッグ
	//vec4 out_col = vec4(1);

	//fSH_R		= out_col;
	//fSH_G		= out_col;
	//fSH_B		= out_col;
	//fSHaccum_R	= out_col;
	//fSHaccum_G	= out_col;
	//fSHaccum_B	= out_col;

}


#endif
