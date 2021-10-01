#version 150




uniform vec3 L = vec3(1,0.7,2);

//#################### �o�[�e�b�N�X�V�F�[�_ ##################//
#ifdef _VERTEX_SHADER_



// �ϐ�
uniform mat4	g_matMVP;
uniform vec3	g_LightDir;
uniform vec3	g_EyePos;


// ����
in vec4			POSITION;	//
in vec2			TEXCOORD0;	//
in vec3			NORMAL;		//
in vec3			TANGENT;	//
in vec3			BINORMAL;	//


// �o��
out Vertex_Out
{
	vec4		Position;	// ���_���W
	vec2		TexCoord;	// �e�N�X�`�����W
	
//	vec3		Tangent;	// �ڐ��x�N�g��
//	vec3		Binormal;	// �]�@���x�N�g��
	vec3		Normal;		// �@���x�N�g��
	
}OUT;


// ���C���֐�
void main()
{	
	//vec4 pos = vec4(POSITION.xyz*0.1, POSITION.w);
	gl_Position		= POSITION * g_matMVP;
	OUT.Position	= g_matMVP * POSITION;
	OUT.TexCoord	= TEXCOORD0;
	OUT.Normal		= NORMAL;//
}


#endif







//##################### �t���O�����g�V�F�[�_ #################//
#ifdef _FRAGMENT_SHADER_

// ����
in Vertex_Out
{
	vec4		Position;	// ���_���W
	vec4		TexCoord;	// �e�N�X�`�����W
	
//	vec3		Tangent;	// �ڐ��x�N�g��
//	vec3		Binormal;	// �]�@���x�N�g��
	vec3		Normal;		// �@���x�N�g��
	
}IN;


// �o��
out vec4 Color_out;


// ���C���֐�
void main()
{
	float I = max( dot(IN.Normal, normalize(L)), 0.0);
	Color_out.rgb = vec3(I);//IN.Normal;//color_gray().rgb;//
	Color_out.a = 0.0;	
}


#endif
