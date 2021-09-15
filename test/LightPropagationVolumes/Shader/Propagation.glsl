#version 150



// TODO: �v�Z�ɕK�v�ȃf�[�^��\�ߒ萔�ɂ��Ă���
// SH_DirectionalConstraint = CalcSHCoeff_CosineLobe(PropDirection) * g_FrontFacingArea; �� �`�d�����ɐ����������SH
// SH_EvalVec = CalcSHCoeff(g_EvaluationVector[0]);										��// �]���x�N�g����SH
// nl_SH	= CalcSHCoeff_CosineLobe(g_ReprojectionVector[0]);						���ē��e�x�N�g���̃R�T�C�����[�uSH


// VPL��^��(���[���h���Wz��)���猩���낵�āC�Ή�����z�X���C�X�ɉ��z������z�u����C���[�W


// �w�b�_�[�t�@�C���C���N���[�h
#pragma include "Shader/HelperFunctions.glslh"


uniform int			g_Iter;


uniform sampler3D	g_LPV_SH_R;	// LPV�e�N�X�`��(R)
uniform sampler3D	g_LPV_SH_G;	// LPV�e�N�X�`��(G)
uniform sampler3D	g_LPV_SH_B;	// LPV�e�N�X�`��(B)

uniform sampler3D	g_SHaccum_R;// LPV����
uniform sampler3D	g_SHaccum_G;// LPV����
uniform sampler3D	g_SHaccum_B;// LPV����

uniform sampler3D	g_GV_SH;	// GeometryVolume


// LPV�̃O���[�o���ϐ�
uniform vec3		g_LpvSize = vec3(32.0, 32.0, 32.0);		// LPV�Z���̉𑜓x(xyz:�𑜓x,w:1�Z������) TODO: �V�F�[�_�O������ϐ���������


// xyz�e�����́C�Z��1������̃e�N�X�`��


#define M_PI 3.141592


//#################### �o�[�e�b�N�X�V�F�[�_ ##################//
#ifdef _VERTEX_SHADER_

// �ϐ�

// ����
in vec4		POSITION;	// LPV�X�N���[����ԏ�̒��_���W(x,y,z�e������[-1,1]�ɐ��K������Ă���)
in vec3		TEXCOORD;	// 3�����e�N�X�`�����W(�e���_�ɑΉ������e�N�X�`�����W)

// �o��
out vec3	vTexCoord;	// �e�N�X�`�����W
out int		vLayer;		// 3D�e�N�X�`���̃��C��


// ���C���֐�
void main()
{
	//=========== ���_���W��POSITION�����̂܂܏o�� ===========//
	gl_Position		= POSITION;
	
	//============== �e�N�X�`�����W�����̂܂܏o�� ============//
	vTexCoord		= TEXCOORD;

	//======= ���_�ɑΉ�����3D�e�N�X�`���̃��C�����v�Z =======//
	vLayer = int( TEXCOORD.z  * g_LpvSize.z );
}

#endif



//#################### �W�I���g���V�F�[�_ ##################//
#ifdef _GEOMETRY_SHADER_

// ���o�̓v���~�e�B�u�^�C�v
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

// �ϐ�

// ����
in vec3		vTexCoord[3];	// �e�N�X�`�����W
in int		vLayer[3];		// 3D�e�N�X�`���̃��C��

// �o��
out vec3	gTexCoord;		// �e�N�X�`�����W


// ���C���֐�
void main()
{
	gl_Position	= gl_in[0].gl_Position;
	gl_Layer	= vLayer[0];// �������ރe�N�X�`�����C���[�̎w��(�K��EmitVertex�̑O�Ɏ��s���邱��)
	gTexCoord	= vTexCoord[0];// �e�N�X�`�����W�̏o��
	EmitVertex();

	gl_Position	= gl_in[1].gl_Position;
	gl_Layer	= vLayer[1];// �������ރe�N�X�`�����C���[�̎w��(�K��EmitVertex�̑O�Ɏ��s���邱��)
	gTexCoord	= vTexCoord[1];// �e�N�X�`�����W�̏o��
	EmitVertex();

	gl_Position	= gl_in[2].gl_Position;
	gl_Layer	= vLayer[2];// �������ރe�N�X�`�����C���[�̎w��(�K��EmitVertex�̑O�Ɏ��s���邱��)
	gTexCoord	= vTexCoord[2];// �e�N�X�`�����W�̏o��
	EmitVertex();
	
	EndPrimitive();
}

#endif





//##################### �t���O�����g�V�F�[�_ #################//
#ifdef _FRAGMENT_SHADER_

// �ϐ�


// ����
in vec3		gTexCoord;	// �e�N�X�`�����W

// �o��
out vec4	fSH_R;		// ���ʒ��a�֐��̌W��(��)
out vec4	fSH_G;		// ���ʒ��a�֐��̌W��(��)
out vec4	fSH_B;		// ���ʒ��a�֐��̌W��(��)

out vec4	fSHaccum_R;	// SH�̍X�V����(��).�אڃZ������̓`�d���ʁu�����v���i�[�����
out vec4	fSHaccum_G;	// SH�̍X�V����(��).�אڃZ������̓`�d���ʁu�����v���i�[�����
out vec4	fSHaccum_B;	// SH�̍X�V����(��).�אڃZ������̓`�d���ʁu�����v���i�[�����



//// �֐�
//void Propagate(inout vec4 SHaccum_R, inout vec4 SHaccum_G, inout vec4 SHaccum_B, in vec3 cellIndex, in vec3 nOffset)
//{
//	vec3 neighborIndex		= cellIndex + nOffset / g_LpvSize;
//
//	vec4 SHCoeff_neighbor_R	= texture(g_SHaccum_R, neighborIndex);
//	vec4 SHCoeff_neighbor_G	= texture(g_SHaccum_G, neighborIndex);
//	vec4 SHCoeff_neighbor_B	= texture(g_SHaccum_B, neighborIndex);
//
//	vec4 SH_PropVec			= CalcSHCoeff(-nOffset);
//	vec4 SH_ReprojVec		= Cone90Degree(-nOffset);//CalcSHCone(-nOffset, 0.25*M_PI);//
//
//	vec3 Luminance			= vec3(	max(0, dot(SHCoeff_neighbor_R, SH_PropVec)),
//									max(0, dot(SHCoeff_neighbor_G, SH_PropVec)),
//									max(0, dot(SHCoeff_neighbor_B, SH_PropVec)) ) * g_FrontFacingSolidAngle;
//
//	float OcclusionFactor	= 1.0;//1:�Օ��Ȃ�, 0: ���S�ɎՕ�����Ă���
//	
//	if(g_Iter>0)// �Օ��̌v�Z�D������GV�Ŏ������Օ����Ȃ��悤�ɁC�`�d�v�Z�̍ŏ���1��ڂ͖{�������΂�
//	{
//		vec3 OccTexCoord = (cellIndex + 0.5*nOffset/g_LpvSize) - 0.5/g_LpvSize;
//		vec4 SHocclusion = texture(g_GV_SH, OccTexCoord);
//		OcclusionFactor = 1.0 - min( abs(dot(SHocclusion, SH_PropVec) ), 1.0);
//	}
//	
//	SHaccum_R += OcclusionFactor * Luminance.r * SH_ReprojVec;
//	SHaccum_G += OcclusionFactor * Luminance.g * SH_ReprojVec;
//	SHaccum_B += OcclusionFactor * Luminance.b * SH_ReprojVec;
//}




// �֐� �ԈႦ�Ă�H�H�H
void Propagate(inout vec4 SHaccum_R, inout vec4 SHaccum_G, inout vec4 SHaccum_B, in vec3 cellIndex, in vec3 nOffset)
{
	vec3 neighborIndex		= cellIndex + nOffset / g_LpvSize;
	vec4 SHCoeff_neighbor_R	= texture(g_SHaccum_R, neighborIndex);
	vec4 SHCoeff_neighbor_G	= texture(g_SHaccum_G, neighborIndex);
	vec4 SHCoeff_neighbor_B	= texture(g_SHaccum_B, neighborIndex);
	vec4 SH_PropCone		= Cone90Degree(-nOffset) * 2.0 * M_PI;
	vec4 SH_Reproj			= CalcSHCoeff_CosineLobe(-nOffset);
	

	vec3 Flux = vec3(
					max(0, dot(SHCoeff_neighbor_R, SH_PropCone)),
					max(0, dot(SHCoeff_neighbor_G, SH_PropCone)),
					max(0, dot(SHCoeff_neighbor_B, SH_PropCone))
					) / 6.0;


	float OcclusionFactor = 1.0;//1:�Օ��Ȃ�, 0: ���S�ɎՕ�����Ă���
	
	
	if(g_Iter>0)// �Օ��̌v�Z�D������GV�Ŏ������Օ����Ȃ��悤�ɁC�`�d�v�Z�̍ŏ���1��ڂ͖{�������΂�
	{
		vec3 OccTexCoord = (cellIndex + 0.5*nOffset/g_LpvSize) - 0.5/g_LpvSize;
		vec4 SHocclusion = texture(g_GV_SH, OccTexCoord);
		OcclusionFactor = 1.0 - min( abs(dot(SHocclusion, CalcSHCoeff(-nOffset)) ), 1.0);
	}
	
	SHaccum_R += Flux.r * SH_Reproj * OcclusionFactor;
	SHaccum_G += Flux.g * SH_Reproj * OcclusionFactor;
	SHaccum_B += Flux.b * SH_Reproj * OcclusionFactor;

}





// ���C���֐�
void main()
{

	fSHaccum_R = vec4(0);
	fSHaccum_G = vec4(0);
	fSHaccum_B = vec4(0);

	// �`�d�������ˑ����v�Z����
	Propagate(fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(+1, 0, 0) );
	Propagate(fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(-1, 0, 0) );
	Propagate(fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, +1, 0) );
	Propagate(fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, -1, 0) );
	Propagate(fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, 0, +1) );
	Propagate(fSHaccum_R, fSHaccum_G, fSHaccum_B, gTexCoord, vec3(0, 0, -1) );


	// �e�F��SH���A�N�e�B�u�ȃZ������擾����
	fSH_R	= texture(g_LPV_SH_R, gTexCoord).rgba + fSHaccum_R;
	fSH_G	= texture(g_LPV_SH_G, gTexCoord).rgba + fSHaccum_G;
	fSH_B	= texture(g_LPV_SH_B, gTexCoord).rgba + fSHaccum_B;

}

#endif
