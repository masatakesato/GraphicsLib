#version 430


// �����p�����[�^
uniform vec3	g_LightIntensity;	// �����̋����i���ˑ��j
uniform vec3	g_LightPos;			// �ʒu
//uniform vec3	g_LightDir;			// �����ւ̃x�N�g��(���s�����̏ꍇ�̂ݎg�p)

uniform mat4	g_matLight;

// RSM�e�N�X�`���̉𑜓x
uniform ivec2	g_RsmSize;


#define M_PI 3.141592


//#################### �o�[�e�b�N�X�V�F�[�_ ##################//
#ifdef _VERTEX_SHADER_

// �ϐ�
uniform mat4	g_matMVP;


// ����
in vec4			POSITION;	// ���_���W
in vec3			NORMAL;		// �@���x�N�g��
in vec2			TEXCOORD;	// �e�N�X�`�����W
in vec4			COLOR;		// �F


// �o��
out vec3		vWorldPos;	// ���[���h���W�n���_���W
out vec4		vViewPos;	// �r���[���W�n�̒��_���W
out vec2		vTexCoord;	// �e�N�X�`�����W
out vec3		vNormal;	// �@���x�N�g��
out vec4		vColor;		// ���_���W


// ���C���֐�
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







//##################### �t���O�����g�V�F�[�_ #################//
#ifdef _FRAGMENT_SHADER_

// ����
in vec3		vWorldPos;	// ���[���h���W�n���_���W
in vec4		vViewPos;	// �r���[���W�n�̒��_���W
in vec2		vTexCoord;	// �e�N�X�`�����W
in vec3		vNormal;	// �@���x�N�g��
in vec4		vColor;		// ���_���W


// �o��
out vec4	fPosition;	// ���[���h���W
out vec3	fNormal;	// �@���x�N�g��
out vec3	fFlux;		// ���ˑ�
out float	fDepth;		// ���s��


// ���C���֐�
void main()
{
	// �ʒu
	fPosition.xyz	= vWorldPos;
	fPosition.w		= vViewPos.w;

	// �@���x�N�g�� TODO: �|���S���̗��ʂɑΉ��ł���悤�ɂ���H
	fNormal		= vNormal;
	//if( dot(normalize(g_LightPos - vWorldPos), fNormal) < 0.0 )	fNormal = -fNormal;


	// ���ˑ�
	vec3 LightDir = normalize(g_LightPos - vWorldPos);
	float flux_delta = 1.0 / (6.0 * float(g_RsmSize.x * g_RsmSize.y));
	fFlux = g_LightIntensity * vColor.rgb * max(dot(fNormal, LightDir), 0.0) / M_PI * flux_delta;// �����̋��� * �ʂ̐F * DOT(�@���x�N�g��, �����x�N�g���̓���) *  1�s�N�Z��������̕��ˑ��̗�
	
	// ���s��
	fDepth		= vViewPos.z/*vViewPos.w*/;
	
}


#endif
