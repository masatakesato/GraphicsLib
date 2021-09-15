#version 150


// VPL��^��(���[���h���Wz��)���猩���낵�āC�Ή�����z�X���C�X�ɉ��z������z�u����C���[�W


// �w�b�_�[�t�@�C���C���N���[�h
#pragma include "Shader/HelperFunctions.glslh"

// �������
uniform vec3		g_LightPos;		// �����̈ʒu


// RSM�̃O���[�o���ϐ�
uniform sampler2D	g_PositionSampler;	// ���[���h���W
uniform sampler2D	g_NormalSampler;	// �@���x�N�g��
uniform sampler2D	g_FluxSampler;		// ���ˑ�


// LPV�̃O���[�o���ϐ�
uniform float		g_LpvCellDim;		// LPV�Z���̕ӂ̒���
//uniform mat4		g_matLPVScreen;		// ���_���W��LPV�X�N���[�����W�n�ɕϊ�����s��


uniform matParam
{
	mat4	g_matLPVScreen;
}matparam;




// 
uniform float		g_VPLScale;		// VPL�_�Q�̓_���CRSM�e�N�X�`���̃s�N�Z�������ɑ������邩(��FRSM:256^2, VPL:128^2�Ȃ�C4.0)







//#################### �o�[�e�b�N�X�V�F�[�_ ##################//
#ifdef _VERTEX_SHADER_

// �ϐ�


// ����
in vec2			POSITION;	// ���_���W


// �o��
out vec3		vNormal;	// �@���x�N�g��
out vec3		vFlux;		// ���_���W
out int			vLayer;


// ���C���֐�
void main()
{
	//============= �@���x�N�g����g_NormalSampler����擾���� =================//
	vNormal		= texture(g_NormalSampler, POSITION).xyz;
	//vNormal = vec3(-1,0,0);

	//=========== LPV�X�N���[����ԏ�ł̉��z�_�����̈ʒu���v�Z���� ===========//
	// g_PositionSampler���烏�[���h���W���擾����
	vec4 worldpos	= texture(g_PositionSampler, POSITION).xyzw;	//worldpos	= vec4(-1.9, 0.0, -0.6, 0);	//worldpos = vec4(1.4, POSITION.x-0.5, POSITION.y-0.7, 1);

	// �@���x�N�g��������0.25�Z�����̋��������ړ�����
	worldpos.xyz += 0.25 * g_LpvCellDim * vNormal;
	
	// �����x�N�g��������0.25�Z�����̋��������ړ�
	worldpos.xyz += 0.25 * g_LpvCellDim * normalize(g_LightPos-worldpos.xyz);
	
	// ���[���h���W����LPV�X�N���[����ԍ��W�ɕϊ�����
	gl_Position		= vec4(worldpos.xyz, 1.0) * matparam.g_matLPVScreen;

	//=============== g_FluxSampler���牼�z�_�����̕��ˑ����擾���� ===========//
	vFlux		= texture(g_FluxSampler, POSITION).rgb * g_VPLScale;

	//===================== ���C�����v�Z���� ==============================//
	// ���[���h���W��LPV��ԍ��W
	// lpvscreenpos.z��,[-1, 1]�̊Ԃ̐���
	// ������C[0,1]�ɂ���LPV�̍������������,,,
	vLayer = int( (gl_Position.z*0.5 + 0.5) * 31.0 );

}


#endif



//#################### �W�I���g���V�F�[�_ ##################//
#ifdef _GEOMETRY_SHADER_


// ���o�̓v���~�e�B�u�^�C�v
layout(points) in;
layout(points, max_vertices=1) out;

// �ϐ�


// ����
in vec3		vNormal[1];	// �@���x�N�g��
in vec3		vFlux[1];	// ���_���W
in int		vLayer[1];


// �o��
out vec3	gNormal;		// �@���x�N�g��
out vec3	gFlux;			// ���ˑ�


// ���C���֐�
void main()
{
	//============ �ʏ�̒��_�����o�͂��� =============//
	if(length(vNormal[0])>0.0)
	{
		// ���C���̊��蓖��
		gl_Layer = vLayer[0];// �e�N�X�`�����C���[�̎w��. �K��EmitVertex�̑O�Ɏ��s����

		// VPL���̏����o��
		gl_Position		= gl_in[0].gl_Position;
		gNormal			= vNormal[0];
		gFlux			= vFlux[0];

		EmitVertex();

		EndPrimitive();
	}
}



#endif





//##################### �t���O�����g�V�F�[�_ #################//
#ifdef _FRAGMENT_SHADER_

// ����
in vec3		gNormal;		// �@���x�N�g��
in vec3		gFlux;			// ���ˑ�


// �o��
out vec4	fSH_R;		// ���ʒ��a�֐��̌W��(��)
out vec4	fSH_G;		// ���ʒ��a�֐��̌W��(��)
out vec4	fSH_B;		// ���ʒ��a�֐��̌W��(��)
out vec4	fSHaccum_R;	// ���ʒ��a�֐��̌W��(��)
out vec4	fSHaccum_G;	// ���ʒ��a�֐��̌W��(��)
out vec4	fSHaccum_B;	// ���ʒ��a�֐��̌W��(��)


// ���C���֐�
void main()
{
	// �^��FRSM��Flux�����ʒ��a�֐��ɕϊ�������Ă��Ƃ�OK�H��OK

	// Flux * �@���x�N�g�������̃R�T�C�����[�u / PI
	vec4 SHcoeffs = CalcSHCoeff_CosineLobe(gNormal) / M_PI;

	// �F����SH�W��(2-band)���o�͂���.
	fSH_R		= gFlux.r * SHcoeffs;
	fSH_G		= gFlux.g * SHcoeffs;
	fSH_B		= gFlux.b * SHcoeffs;
	fSHaccum_R	= fSH_R;
	fSHaccum_G	= fSH_G;
	fSHaccum_B	= fSH_B;
}


#endif
