#version 150

//############################ �e��ϐ� ##########################//


// ���_�V�F�[�_����̓���
in Vertex_Out
{
	vec4		Position;	// ���_���W
	vec4		TexCoord;	// �e�N�X�`�����W
	
//	vec3		Tangent;	// �ڐ��x�N�g��
//	vec3		Binormal;	// �]�@���x�N�g��
	vec3		Normal;		// �@���x�N�g��
	
}IN;


// �t���O�����g�V�F�[�_���o�͂���F�f�[�^
out vec4 Color_out;





//###################### �t���O�����g�V�F�[�_ ####################//
void main()
{
	Color_out.rgb = IN.Normal;
	Color_out.a = 0.0;
	
}