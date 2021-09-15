#version 150

#pragma include "Shader/HelperFunctions.glslh"


uniform int			g_Mode;// �����_�����O���[�h



// RSM
uniform vec2		g_RsmTexelDim = {1/2048, 1/2048};	// �e�N�Z��1�̕�
uniform mat4		g_matLight;		// �����n�̍��W�ϊ��}�g���N�X
uniform sampler2D	g_ShadowMap;	// �V���h�E�}�b�v

// LPV
uniform sampler3D	g_LPV_SH_R;
uniform sampler3D	g_LPV_SH_G;
uniform sampler3D	g_LPV_SH_B;



uniform vec3	g_EyePos;
uniform vec3	g_LightPos;
uniform vec3	g_LightIntensity;


uniform float	g_Magnitude = 1.0;// TODO:�@�ԐڏƖ��̋��x�����[�U�[���������邽�߂̃p�����[�^�D�O������V�F�[�_�ɒl����荞��
uniform float	g_LpvCellDim = 0.4;// TODO: �O������ �V
uniform vec4	g_LpvCellSize = vec4(32.0, 32.0, 32.0, 1.0/32.0);


#define M_PI 3.141592




vec2 neighbors[25] = vec2[25]
(
	vec2(-2, -2),
	vec2(-1, -2),
	vec2(0,  -2),
	vec2(+1, -2),
	vec2(+2, -2),

	vec2(-2, -1),
	vec2(-1, -1),
	vec2(0,  -1),
	vec2(+1, -1),
	vec2(+2, -1),

	vec2(-2, 0),
	vec2(-1, 0),
	vec2(0,  0),
	vec2(+1, 0),
	vec2(+2, 0),

	vec2(-2, +1),
	vec2(-1, +1),
	vec2(0,  +1),
	vec2(+1, +1),
	vec2(+2, +1),

	vec2(-2, +2),
	vec2(-1, +2),
	vec2(0,  +2),
	vec2(+1, +2),
	vec2(+2, +2)
);



//#################### �o�[�e�b�N�X�V�F�[�_ ##################//
#ifdef _VERTEX_SHADER_

// �ϐ�
uniform mat4	g_matMVP;



// ����
in vec4		POSITION;
in vec3		NORMAL;
in vec4		COLOR;


// �o��
out vec3	vPosition;	// ���_���W
out vec4	vLightPos;	// �����n�̒��_���W
out vec3	vNormal;	// �@���x�N�g��
out vec4	vColor;		// �F


// ���C���֐�
void main()
{	
	gl_Position	= POSITION * g_matMVP;
	vPosition	= POSITION.xyz;
	vLightPos	= POSITION * g_matLight;
	vNormal		= NORMAL;
	vColor		= COLOR;
}


#endif







//##################### �t���O�����g�V�F�[�_ #################//
#ifdef _FRAGMENT_SHADER_

// �ϐ�


// ����
in vec3		vPosition;	// ���_���W
in vec4		vLightPos;	// �����n�̒��_���W
in vec3		vNormal;	// �@���x�N�g��
in vec4		vColor;		// �F


// �o��
out vec4 Color_out;





// Variance Shadow map
float VSM_Filter(vec2 texcoord, float fragDepth)
{/*
	float depth = texture(g_ShadowMap, texcoord).x;
	
	float depth_sq = depth * depth;
	float variance = depth - depth_sq;
	float md = fragDepth - depth;
	float p = -variance / (variance + (md * md));

	return 1.0 - max(p, float(depth<=fragDepth));
	*/
	
	float occluder = texture(g_ShadowMap, texcoord).r;
	float overdark = 0.001;
	float    lit = exp(overdark * (occluder - fragDepth));
	lit = 2*(clamp(lit, 0, 1) - 0.5);

	return lit;
	
}



// LPV�e�N�X�`���֐����v�Z����֐��DTODO: �V�F�[�_�O������LPV�̃p�����[�^���擾���Čv�Z����
vec3 GetLpvIndex(vec3 worldpos)
{
	vec3 idx = vec3(0);		// ���S

	vec3 origin = vec3(0.0);
	
	// LPV���_���S�Ɉړ����ăX�P�[�����O
	idx = (worldpos - origin) * (g_LpvCellSize.xyz - vec3(1.0)) / g_LpvCellSize.xyz;
	idx = idx / (0.5f * g_LpvCellSize.xyz * g_LpvCellDim);// [-1, 1]
	idx = idx * 0.5 + vec3(0.5);// [0, 1]
	
	return idx;
}



/*
// s��0�̏ꍇ��a, 1�̏ꍇ��b
float lerp(float a, float b, float s)
{
    return a + s * (b - a);
}
*/

// ���C���֐�
void main()
{
	
	//================= �e�t�� =================//
	vec2 rsmtexcoord = 0.5*(vLightPos.xy/vLightPos.w) + vec2(0.5);
	float Depth_RSM = texture(g_ShadowMap, rsmtexcoord).r;
	float Depth_Frag = vLightPos.z/*vLightPos.w*/ - 1.0e-2;
	float islit = 1.0;

	if(Depth_RSM < Depth_Frag) islit = 0.0;


	//================= �Ɩ��v�Z ===============//
	vec3 L = g_LightPos - vPosition;
	float LightDistance = length(L);
	L /= LightDistance;
	vec3 V = normalize(g_EyePos - vPosition);
	vec3 N = vNormal;

	vec3 lpvtexcoord = GetLpvIndex(vPosition);
	vec4 SH_R = texture(g_LPV_SH_R, lpvtexcoord);
	vec4 SH_G = texture(g_LPV_SH_G, lpvtexcoord);
	vec4 SH_B = texture(g_LPV_SH_B, lpvtexcoord);

	if(dot(V, N) < 0.0) N = -N;// �ʂ̖@������Ɏ��_�������悤�ɂ���

	vec4 SampleDirSH = CalcSHCoeff(-N);
	
	vec3 LpvIntensity = vec3(
							max(dot(SampleDirSH, SH_R),0),
							max(dot(SampleDirSH, SH_G),0),
							max(dot(SampleDirSH, SH_B),0)
							);
	
	
	//=============== ���ʂɌ����R��o���Ȃ��悤�ɂ��� =================//
	//vec4 SH_Gradient_R = SH_Gradient( g_LPV_SH_R, N, lpvtexcoord, 1.0/g_LpvCellSize.xyz );
	//vec4 SH_Gradient_G = SH_Gradient( g_LPV_SH_R, N, lpvtexcoord, 1.0/g_LpvCellSize.xyz );
	//vec4 SH_Gradient_B = SH_Gradient( g_LPV_SH_R, N, lpvtexcoord, 1.0/g_LpvCellSize.xyz );

	//float Visibility = (dot(SH_Gradient_R, SampleDirSH) + dot(SH_Gradient_G, SampleDirSH) + dot(SH_Gradient_B, SampleDirSH)) / 3.0;
	//float flag = 1.0;

	float drawdirect = float(g_Mode==0 || g_Mode==1);
	float drawindirect = float(g_Mode==0 || g_Mode==2);

	vec3 lightRadiance = g_LightIntensity / (4 * M_PI) / LightDistance / LightDistance;// ��������̋����ɔ�Ⴕ�Č���
	vec3 LpvRadiance = LpvIntensity * 4.0 / g_LpvCellDim / g_LpvCellDim * g_Magnitude;

	Color_out.rgb = (islit * lightRadiance) * (vColor.rgb * max(dot(L,N),0) / M_PI) * drawdirect+
					vColor.rgb * LpvRadiance / M_PI * drawindirect+
					vec3(0.05);
}


#endif
