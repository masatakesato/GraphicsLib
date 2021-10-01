#version 150

//############################ 各種変数 ##########################//


// 頂点シェーダからの入力
in Vertex_Out
{
	vec4		Position;	// 頂点座標
	vec4		TexCoord;	// テクスチャ座標
	
//	vec3		Tangent;	// 接線ベクトル
//	vec3		Binormal;	// 従法線ベクトル
	vec3		Normal;		// 法線ベクトル
	
}IN;


// フラグメントシェーダが出力する色データ
out vec4 Color_out;





//###################### フラグメントシェーダ ####################//
void main()
{
	Color_out.rgb = IN.Normal;
	Color_out.a = 0.0;
	
}