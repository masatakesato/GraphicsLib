#version 150


//############################ �e��ϐ� ##########################//


// �O���[�o���ϐ�
uniform mat4	g_matMVP;
uniform vec3	g_LightDir;
uniform vec3	g_EyePos;


// ���_�V�F�[�_�ւ̓��̓f�[�^
in vec4			POSITION;	//
in vec3			NORMAL;		//
in vec2			TEXCOORD0;	//
in vec3			TANGENT;	//
in vec3			BINORMAL;	//


// ���_�V�F�[�_����̏o�̓f�[�^
out Vertex_Out
{
	vec4		Position;	// ���_���W
	vec2		TexCoord;	// �e�N�X�`�����W
	
//	vec3		Tangent;	// �ڐ��x�N�g��
//	vec3		Binormal;	// �]�@���x�N�g��
	vec3		Normal;		// �@���x�N�g��
	
}OUT;



//##################### �o�[�e�b�N�X�V�F�[�_ #####################//
void main()
{	
	gl_Position		= POSITION * g_matMVP;
	OUT.Position	= g_matMVP * POSITION;
	OUT.TexCoord	= TEXCOORD0;
	
	OUT.Normal		= NORMAL;
	


//============== �����x�N�g����ڋ�Ԃɕϊ� ==============//


}