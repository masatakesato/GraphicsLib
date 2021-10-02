
//#################### バーテックスシェーダ ##################//
#ifdef _VERTEX_SHADER_

#pragma include "Common.glsl"

// 入力
in vec4			POSITION;


// 出力


// メイン関数
void main()
{	
	gl_Position		= POSITION * g_MatWorld * g_ViewTransform.MatViewProjection;//g_MatViewProj;
}


#endif




//##################### フラグメントシェーダ #################//
#ifdef _FRAGMENT_SHADER_

// 入力


// 出力
//out vec4	outColor;// 必要ない。デバッグ用

// メイン関数
void main()
{
	//outColor	= vec4(0.5);// 必要ない。デバッグ用
}


#endif
