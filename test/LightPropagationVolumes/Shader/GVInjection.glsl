#version 150


// GV��^��(���[���h���Wz��)���猩���낵�āC�Ή�����z�X���C�X�ɉ��z������z�u����C���[�W


// �w�b�_�[�t�@�C���C���N���[�h
#pragma include "Shader/HelperFunctions.glslh"


// RSM�̃O���[�o���ϐ�
uniform ivec2		g_RsmSize;			// RSM�̉𑜓x
uniform sampler2D	g_PositionSampler;	// ���[���h���W
uniform sampler2D	g_NormalSampler;	// �@���x�N�g��


// LPV�̃O���[�o���ϐ�
uniform float		g_LpvCellDim;		// LPV�Z���̕ӂ̒���
//uniform mat4		g_matGVScreen;		// ���_���W��LPV�X�N���[�����W�n�ɕϊ�����s��


uniform matParam
{
	mat4	g_matLPVScreen;
}matparam;



//#################### �o�[�e�b�N�X�V�F�[�_ ##################//
#ifdef _VERTEX_SHADER_

// �ϐ�


// ����
in vec2			POSITION;	// ���_���W


// �o��
out vec3		vNormal;	// �@���x�N�g��
out float		vArea;		// �T�[�t�F���̖ʐ�
out int			vLayer;


// ���C���֐�
void main()
{
	//============= �@���x�N�g����g_NormalSampler����擾���� =================//
	vNormal		= texture(g_NormalSampler, POSITION).xyz;


	//=========== LPV�X�N���[����ԏ�ł̉��z�_�����̈ʒu���v�Z���� ===========//
	// g_PositionSampler���烏�[���h���W���擾����
	vec4 worldpos	= texture(g_PositionSampler, POSITION).xyzw;
	
	// �@���x�N�g��������1�Z�����̋��������ړ�����
	worldpos.xyz -= 1.0 * g_LpvCellDim * vNormal;

	// GV�̈ʒu�ɑ΂��āCVPL�̔z�u�ꏊ�𔼃Z���������ړ�����
	worldpos.xyz -= 0.5 * vec3(g_LpvCellDim);
	
	// ���[���h���W����LPV�X�N���[����ԍ��W�ɕϊ�����
	gl_Position		= vec4(worldpos.xyz, 1.0) * matparam.g_matLPVScreen;//g_matGVScreen;

	//=====================  ���z�_�����̖ʐς��v�Z���� =======================//
	vArea		= 4.0 * worldpos.w * worldpos.w / float(g_RsmSize.x * g_RsmSize.y);

	//===================== ���C�����v�Z���� ==============================//
	vLayer		= int( (gl_Position.z*0.5 + 0.5) * 31.0 );

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
in float	vArea[1];	// �T�[�t�F���̖ʐ�
in int		vLayer[1];


// �o��
out vec3	gNormal;		// �@���x�N�g��
out float	gArea;			// �T�[�t�F���̖ʐ�


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
		gArea			= vArea[0];

		EmitVertex();

		EndPrimitive();
	}
}



#endif





//##################### �t���O�����g�V�F�[�_ #################//
#ifdef _FRAGMENT_SHADER_

// ����
in vec3		gNormal;		// �@���x�N�g��
in float	gArea;			// �T�[�t�F���̖ʐ�

// �o��
out vec4	fSH_GV;		// ���ʒ��a�֐��̌W��(�`��f�[�^)


// ���C���֐�
void main()
{
	// �Օ����@��������SH�W�� * �Z���ɐ�߂�Surfel�̖ʐ�
	fSH_GV = CalcSHCoeff_CosineLobe(gNormal) * clamp( gArea / (g_LpvCellDim*g_LpvCellDim), 0.0, 1.0);
}


#endif
