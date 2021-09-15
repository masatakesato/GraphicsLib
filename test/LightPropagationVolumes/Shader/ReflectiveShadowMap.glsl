#version 430


// 光源パラメータ
uniform vec3	g_LightIntensity;	// 光源の強さ（放射束）
uniform vec3	g_LightPos;			// 位置
//uniform vec3	g_LightDir;			// 光源へのベクトル(平行光源の場合のみ使用)

uniform mat4	g_matLight;

// RSMテクスチャの解像度
uniform ivec2	g_RsmSize;


#define M_PI 3.141592


//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_

// 変数
uniform mat4	g_matMVP;


// 入力
in vec4			POSITION;	// 頂点座標
in vec3			NORMAL;		// 法線ベクトル
in vec2			TEXCOORD;	// テクスチャ座標
in vec4			COLOR;		// 色


// 出力
out vec3		vWorldPos;	// ワールド座標系頂点座標
out vec4		vViewPos;	// ビュー座標系の頂点座標
out vec2		vTexCoord;	// テクスチャ座標
out vec3		vNormal;	// 法線ベクトル
out vec4		vColor;		// 頂点座標


// メイン関数
void main()
{	
	gl_Position	= POSITION * g_matMVP;
	vWorldPos	= POSITION.xyz;
	vViewPos	= gl_Position;
	vTexCoord	= TEXCOORD;
	vNormal		= NORMAL;
	vColor		= COLOR;
}


#endif







//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 入力
in vec3		vWorldPos;	// ワールド座標系頂点座標
in vec4		vViewPos;	// ビュー座標系の頂点座標
in vec2		vTexCoord;	// テクスチャ座標
in vec3		vNormal;	// 法線ベクトル
in vec4		vColor;		// 頂点座標


// 出力
out vec4	fPosition;	// ワールド座標
out vec3	fNormal;	// 法線ベクトル
out vec3	fFlux;		// 放射束
out float	fDepth;		// 奥行き


// メイン関数
void main()
{
	// 位置
	fPosition.xyz	= vWorldPos;
	fPosition.w		= vViewPos.w;

	// 法線ベクトル TODO: ポリゴン板の両面に対応できるようにする？
	fNormal		= vNormal;
	//if( dot(normalize(g_LightPos - vWorldPos), fNormal) < 0.0 )	fNormal = -fNormal;


	// 放射束
	vec3 LightDir = normalize(g_LightPos - vWorldPos);
	float flux_delta = 1.0 / (6.0 * float(g_RsmSize.x * g_RsmSize.y));
	fFlux = g_LightIntensity * vColor.rgb * max(dot(fNormal, LightDir), 0.0) / M_PI * flux_delta;// 光源の強さ * 面の色 * DOT(法線ベクトル, 光源ベクトルの内積) *  1ピクセルあたりの放射束の量
	
	// 奥行き
	fDepth		= vViewPos.z/*vViewPos.w*/;
	
}


#endif
