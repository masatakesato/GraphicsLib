// Include Files
#pragma include "../../../shader/glsl/Common.glsl"



//#################### �o�[�e�b�N�X�V�F�[�_ ##################//
#ifdef _VERTEX_SHADER_


// �ϐ�


// ����
in vec4			POSITION;
in vec2			TEXCOORD;
in vec3			NORMAL;
in vec3			COLOR;

//in vec3			TANGENT;
//in vec3			BINORMAL;


// �o��
out Vertex_Out
{
	vec2		TexCoord;	// �e�N�X�`�����W
}OUT;


// ���C���֐�
void main()
{	
	gl_Position		= POSITION;
	OUT.TexCoord	= TEXCOORD;
}


#endif







//##################### �t���O�����g�V�F�[�_ #################//
#ifdef _FRAGMENT_SHADER_


// Uniform Variables
uniform sampler1D	g_TexRamp;	// texture1D
uniform sampler2D	g_TexDensity;	// density
uniform sampler2D	g_TexTemperature;	// temperature
uniform float		g_Scale;	// color scale


// Fragment Shader Input
in Vertex_Out
{
	vec2		TexCoord;	// �e�N�X�`�����W	
}IN;


// Fragment Shader Output
out vec4 outColor;


// Main Function
void main()
{
	//float scalar = clamp( texture( g_TexScalar, IN.TexCoord.xy ).x, 0.0, 0.999 );

	float temperature	= texture( g_TexTemperature, IN.TexCoord.xy ).x;
	float density		= texture( g_TexDensity, IN.TexCoord.xy).x;

	vec3 rampColor = texture( g_TexRamp, clamp( temperature*g_Scale, 0.0, 0.999 ) ).xyz;

	outColor = vec4( rampColor*density*g_Scale, 1.0 );
}


#endif
