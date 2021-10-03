
//############################ 各種変数 ##########################//


// グローバル変数
uniform mat4	g_matMVP;
uniform vec3	g_LightDir;
uniform vec3	g_EyePos;


// 頂点シェーダへの入力データ
in vec4			POSITION;	//
in vec3			NORMAL;		//
in vec2			TEXCOORD0;	//
in vec3			TANGENT;	//
in vec3			BINORMAL;	//


// 頂点シェーダからの出力データ
out Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec2		TexCoord;	// テクスチャ座標
	
//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	vec3		Normal;		// 法線ベクトル
	
}OUT;



//##################### バーテックスシェーダ #####################//
void main()
{	
	gl_Position		= POSITION * g_matMVP;
	OUT.Position	= g_matMVP * POSITION;
	OUT.TexCoord	= TEXCOORD0;
	
	OUT.Normal		= NORMAL;
	


//============== 光源ベクトルを接空間に変換 ==============//


}