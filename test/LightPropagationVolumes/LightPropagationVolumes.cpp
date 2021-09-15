#include	"LightPropagationVolumes.h"



#include	<oreore/GLTexture.h>
using namespace MyGlTexture;







//########################################################################################################################//
//										 Light Propagation Volumes Texture class
//########################################################################################################################//

// デフォルトコンストラクタ
LPVTexture::LPVTexture()
{
	m_numCascades		= 0;
	InitVec(m_LpvSize, 0, 0, 0);
//	InitVec(m_RsmSize, 0, 0);

	m_LpvCellDim		= NULL;
	m_LpvOrigin			= NULL;

}


// デストラクタ
LPVTexture::~LPVTexture()
{
	Release();
}



// LPVテクスチャを生成する
void LPVTexture::Init(int num_lpvs, const Vec3i &lpv_size/*, const Vec2i &rsm_size, const Vec2i &vpl_size*/)
{
	int i, j;

	m_numCascades	= num_lpvs;
	m_LpvSize		= lpv_size;


	//========= TODO: 別関数で実装してコールバックする．いずれ ===========//
	m_LpvCellDim	= new float[m_numCascades];
	m_LpvOrigin		= new Vec3f[m_numCascades];
	for(int i=0; i<m_numCascades; i++)
	{
		m_LpvCellDim[i] = 0.4f * pow(2.0f, float(i));
		InitVec(m_LpvOrigin[i], 0.0f, 0.0f, 0.0f);
	}
	//====================================================================//







	GLint lfilter = GL_LINEAR;
	GLint nfilter = GL_NEAREST;
	GLint wrap = GL_CLAMP_TO_EDGE;


	//====================== テクスチャオブジェクトの初期化 ============================//
	// LPVテクスチャ
	m_SH_LPVs		= new SH_LPV*[2];// Src/Dst分のLPVバッファを用意する
	m_SH_LPVs[0]	= new SH_LPV[m_numCascades];
	m_SH_LPVs[1]	= new SH_LPV[m_numCascades];

	m_SHaccum		= new SH_LPV[2];


	for(j=0; j<3; j++)
	{
		for(i=0; i<m_numCascades; i++)
		{
			m_SH_LPVs[0][i].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z,	GL_RGBA16F, GL_RGBA, GL_FLOAT);
			m_SH_LPVs[1][i].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z,	GL_RGBA16F, GL_RGBA, GL_FLOAT);
		}// end of i loop

		m_SHaccum[0].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_SHaccum[1].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, GL_RGBA16F, GL_RGBA, GL_FLOAT);

	}// end of j loop

	// GVテクスチャ
	m_SH_GV	= new GLuint[m_numCascades];

	for(i=0; i<m_numCascades; i++)
		m_SH_GV[i] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, GL_RGBA16F, GL_RGBA, GL_FLOAT);

}



// LPVテクスチャを削除する
void LPVTexture::Release()
{
	//=============== テクスチャの削除 =================//


	cout << "  delete m_SH_LPVs..." << endl;
	if(m_SH_LPVs)
	{
		for(int i=0; i<2; i++)
		{
			for(int j=0; j<m_numCascades; j++)	glDeleteTextures(3, m_SH_LPVs[i][j].rgb);
			delete [] m_SH_LPVs[i];
		}
		delete [] m_SH_LPVs;
		m_SH_LPVs = NULL;
	}

	cout << "  delete m_SHaccum..." << endl;
	if(m_SHaccum)
	{	
		for(int i=0; i<2; i++)	glDeleteTextures(3, m_SHaccum[i].rgb);
		delete [] m_SHaccum;
		m_SHaccum = NULL;
	}


	cout << "  delete m_SH_GV..." << endl;
	if(m_SH_GV)
	{
		glDeleteTextures(m_numCascades, m_SH_GV);
		delete [] m_SH_GV;
		m_SH_GV = NULL;
	}


	//=============== LPV情報の削除 =================//
	if(m_LpvCellDim)
	{
		delete	[] m_LpvCellDim;
		m_LpvCellDim = NULL;
	}

	if(m_LpvOrigin)
	{
		delete [] m_LpvOrigin;
		m_LpvOrigin = NULL;
	}


}




//########################################################################################################################//
//										 Light Propagation Volumes Shader class
//########################################################################################################################//


// 光源の強さ（仮決め） TODO: シーングラフデータから光源情報を取り込むように改良すること
const float scale = 3500.0;
const Vec3f LightIntensity = {255.0/255.0 * scale, 241/255.0 * scale, 225/255.0 * scale};



// テクスチャユニット
const GLenum TextureUnit[] =
{
	GL_TEXTURE0,	GL_TEXTURE1,	GL_TEXTURE2,	GL_TEXTURE3, 
	GL_TEXTURE4,	GL_TEXTURE5,	GL_TEXTURE6,	GL_TEXTURE7,
	GL_TEXTURE8,	GL_TEXTURE9,	GL_TEXTURE10,	GL_TEXTURE11,
	GL_TEXTURE12,	GL_TEXTURE13,	GL_TEXTURE14,	GL_TEXTURE15,
};

// フレームバッファオブジェクトのアタッチメントポイント
const GLenum Attachments[] = 
{
	GL_COLOR_ATTACHMENT0,	GL_COLOR_ATTACHMENT1,	GL_COLOR_ATTACHMENT2,	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,	GL_COLOR_ATTACHMENT5,	GL_COLOR_ATTACHMENT6,	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,	GL_COLOR_ATTACHMENT9,	GL_COLOR_ATTACHMENT10,	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,	GL_COLOR_ATTACHMENT13,	GL_COLOR_ATTACHMENT14,	GL_COLOR_ATTACHMENT15,
};



#define VPL_RESOLUTION 256// 仮想点光源の配置解像度








struct Vertex
{
	float	Position[3];
	float	Normal[3];
	float	Color[3];
};




// 頂点配列を使って接空間ベクトル付き立方体を描画する
static void DrawCube_with_tangent(float dimx, float dimy, float dimz, GLint vertLoc, GLint normLoc, GLint colorLoc)
{
	float dx = +0.0;
	float dy = -0.0;
	float dz = -0.7;



	Vertex	vertices[] =
	{
		{ {-dimx+dx, -dimy+dy, -dimz+dz}, {0,0,-1}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, -dimy+dy, -dimz+dz}, {0,0,-1}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, +dimy+dy, -dimz+dz}, {0,0,-1}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, +dimy+dy, -dimz+dz}, {0,0,-1}, {0.5, 1.0, 1.0} },
		
		{ {+dimx+dx, -dimy+dy, -dimz+dz}, {1,0,0}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, -dimy+dy, +dimz+dz}, {1,0,0}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, +dimy+dy, +dimz+dz}, {1,0,0}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, +dimy+dy, -dimz+dz}, {1,0,0}, {0.5, 1.0, 1.0} },
		
		{ {+dimx+dx, -dimy+dy, +dimz+dz}, {0,0,1}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, -dimy+dy, +dimz+dz}, {0,0,1}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, +dimy+dy, +dimz+dz}, {0,0,1}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, +dimy+dy, +dimz+dz}, {0,0,1}, {0.5, 1.0, 1.0} },
	
		{ {-dimx+dx, -dimy+dy, +dimz+dz}, {-1,0,0}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, -dimy+dy, -dimz+dz}, {-1,0,0}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, +dimy+dy, -dimz+dz}, {-1,0,0}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, +dimy+dy, +dimz+dz}, {-1,0,0}, {0.5, 1.0, 1.0} },

		{ {-dimx+dx, +dimy+dy, -dimz+dz}, {0,1,0}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, +dimy+dy, -dimz+dz}, {0,1,0}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, +dimy+dy, +dimz+dz}, {0,1,0}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, +dimy+dy, +dimz+dz}, {0,1,0}, {0.5, 1.0, 1.0} },

		{ {-dimx+dx, -dimy+dy, -dimz+dz}, {0,-1,0}, {0.5, 1.0, 1.0} },
		{ {-dimx+dx, -dimy+dy, +dimz+dz}, {0,-1,0}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, -dimy+dy, +dimz+dz}, {0,-1,0}, {0.5, 1.0, 1.0} },
		{ {+dimx+dx, -dimy+dy, -dimz+dz}, {0,-1,0}, {0.5, 1.0, 1.0} },
	};

	GLint	index[] =
	{
		0,1,2, 3,0,2,
		4,5,6, 7,4,6,
		8,9,10, 11,8,10,
		12,13,14, 15,12,14,
		16,17,18, 19,16,18,
		20,21,22, 23,20,22,
	};


	glEnableVertexAttribArray(vertLoc);
	glEnableVertexAttribArray(normLoc);
	glEnableVertexAttribArray(colorLoc);	

	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Position) );
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Normal) );
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Color) );
	
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, index);

	glDisableVertexAttribArray(vertLoc);
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(colorLoc);


}



// 頂点配列を使って接空間ベクトル付き立方体を描画する
static void DrawSlitRect(float dimx, float dimy, float dimz, GLint vertLoc, GLint normLoc, GLint colorLoc)
{
	//float dz = 0.1f * (2.0f*dimz);


	Vertex	vertices[] =
	{
		// -z底面
		{ {-dimx, -dimy, -dimz},			{0,0,+1},	{1.0, 1.0, 1.0} }, //{1.0, 1.0, 1.0}
		{ {+dimx, -dimy, -dimz},			{0,0,+1},	{1.0, 1.0, 1.0} },
		{ {+dimx, +dimy, -dimz},			{0,0,+1},	{1.0, 1.0, 1.0} },
		{ {-dimx, +dimy, -dimz},			{0,0,+1},	{1.0, 1.0, 1.0} },
	
		// -x側面1
		{ {-dimx, -dimy, +dimz},			{+1,0,0},	{0.2, 1.0, 0.2} },
		{ {-dimx, -dimy, -dimz},			{+1,0,0},	{0.2, 1.0, 0.2} },
		{ {-dimx, +dimy, -dimz},			{+1,0,0},	{0.2, 1.0, 0.2} },
		{ {-dimx, +dimy, +dimz},			{+1,0,0},	{0.2, 1.0, 0.2} },
		
		// -y側面1
		{ {-dimx, -dimy, -dimz},			{0,+1,0},	{0.2, 0.2, 1.0} },
		{ {-dimx, -dimy, +dimz},			{0,+1,0},	{0.2, 0.2, 1.0} },
		{ {+dimx, -dimy, +dimz},			{0,+1,0},	{0.2, 0.2, 1.0} },
		{ {+dimx, -dimy, -dimz},			{0,+1,0},	{0.2, 0.2, 1.0} },
		
		// +x側面1
		{ {+dimx, -dimy, +dimz},			{-1,0,0},	{1.0, 0.2, 0.2} },
		{ {+dimx, -dimy, -dimz},			{-1,0,0},	{1.0, 0.2, 0.2} },
		{ {+dimx, +dimy, -dimz},			{-1,0,0},	{1.0, 0.2, 0.2} },
		{ {+dimx, +dimy, +dimz},			{-1,0,0},	{1.0, 0.2, 0.2} },

	};

	GLint	index[] =
	{
		0,1,2, 3,0,2,		// -z底面
		4,5,6, 7,4,6,		// +x側面1
		8,9,10, 11,8,10,	// +z上面
		12,13,14, 15,12,14,	// -x側面
		16,17,18, 19,16,18,	// +y側面
		20,21,22, 23,20,22,	// -y側面

		//24,25,26, 27,24,26,	// +x側面2
		//28,29,30, 31,28,30,	// -x側面2
		//32,33,34, 35,32,34,	// +y側面2
		//36,37,38, 39,36,38,	// -y側面2
	};
	
	glEnableVertexAttribArray(vertLoc);
	glEnableVertexAttribArray(normLoc);
	glEnableVertexAttribArray(colorLoc);	

	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Position) );
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Normal) );
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Color) );
	
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, index);

	glDisableVertexAttribArray(vertLoc);
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(colorLoc);

}


/*
// 頂点配列を使って接空間ベクトル付き立方体を描画する
static void DrawSlitRect(float dimx, float dimy, float dimz, GLint vertLoc, GLint normLoc, GLint colorLoc)
{
	float dz = 0.1f * (2.0f*dimz);


	Vertex	vertices[] =
	{
		// -z底面
		{ {-dimx, -dimy, -dimz},			{0,0,-1},	{1.0, 0.4, 0.4} },
		{ {+dimx, -dimy, -dimz},			{0,0,-1},	{1.0, 0.4, 0.4} },
		{ {+dimx, +dimy, -dimz},			{0,0,-1},	{1.0, 0.4, 0.4} },
		{ {-dimx, +dimy, -dimz},			{0,0,-1},	{1.0, 0.4, 0.4} },
		
		// +x側面1
		{ {+dimx, -dimy, -dimz},			{1,0,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, -dimy, -dimz+dz*1.5f},	{1,0,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, -dimz+dz*5.0f},	{1,0,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, -dimz},			{1,0,0},	{0.8, 0.8, 0.8} },
		
		// +z上面
		{ {+dimx, -dimy, +dimz},			{0,0,1},	{0.4, 0.4, 1.0} },
		{ {-dimx, -dimy, +dimz},			{0,0,1},	{0.4, 0.4, 1.0} },
		{ {-dimx, +dimy, +dimz},			{0,0,1},	{0.4, 0.4, 1.0} },
		{ {+dimx, +dimy, +dimz},			{0,0,1},	{0.4, 0.4, 1.0} },
	
		// -x側面1
		{ {-dimx, -dimy, -dimz+dz*8.0f},	{-1,0,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, -dimy, -dimz},			{-1,0,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, +dimy, -dimz},			{-1,0,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, +dimy, -dimz+dz*6.0f},	{-1,0,0},	{0.8, 0.8, 0.8} },

		// +y側面1
		{ {-dimx, +dimy, -dimz},			{0,1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, -dimz},			{0,1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, -dimz+5.0f*dz},	{0,1,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, +dimy, -dimz+6.0f*dz},	{0,1,0},	{0.8, 0.8, 0.8} },

		// -y側面1
		{ {-dimx, -dimy, -dimz},			{0,-1,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, -dimy, -dimz+0.5f*dz},	{0,-1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, -dimy, -dimz+1.5f*dz},	{0,-1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, -dimy, -dimz},			{0,-1,0},	{0.8, 0.8, 0.8} },


		
		// +x側面2
		{ {+dimx, -dimy, -dimz+dz*2.8f},	{1,0,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, -dimy, +dimz},			{1,0,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, +dimz},			{1,0,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, -dimz+dz*7.5f},	{1,0,0},	{0.8, 0.8, 0.8} },

		
		// -x側面2
		{ {-dimx, -dimy, +dimz},			{-1,0,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, -dimy, -dimz+dz*9.5f},	{-1,0,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, +dimy, -dimz+dz*8.0f},	{-1,0,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, +dimy, +dimz},			{-1,0,0},	{0.8, 0.8, 0.8} },

		
		// +y側面2
		{ {-dimx, +dimy, -dimz+8.0f*dz},	{0,1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, -dimz+7.5f*dz},	{0,1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, +dimy, +dimz},			{0,1,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, +dimy, +dimz},			{0,1,0},	{0.8, 0.8, 0.8} },

		
		// -y側面2
		{ {-dimx, -dimy, -dimz+2.8f*dz},	{0,-1,0},	{0.8, 0.8, 0.8} },
		{ {-dimx, -dimy, +dimz},			{0,-1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, -dimy, +dimz},			{0,-1,0},	{0.8, 0.8, 0.8} },
		{ {+dimx, -dimy, -dimz+2.8f*dz},	{0,-1,0},	{0.8, 0.8, 0.8} },
		

	};

	GLint	index[] =
	{
		0,1,2, 3,0,2,		// -z底面
		4,5,6, 7,4,6,		// +x側面1
		8,9,10, 11,8,10,	// +z上面
		12,13,14, 15,12,14,	// -x側面
		16,17,18, 19,16,18,	// +y側面
		20,21,22, 23,20,22,	// -y側面

		24,25,26, 27,24,26,	// +x側面2
		28,29,30, 31,28,30,	// -x側面2
		32,33,34, 35,32,34,	// +y側面2
		36,37,38, 39,36,38,	// -y側面2
	};
	
	glEnableVertexAttribArray(vertLoc);
	glEnableVertexAttribArray(normLoc);
	glEnableVertexAttribArray(colorLoc);	

	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Position) );
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Normal) );
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(vertices->Color) );
	
	glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, index);

	glDisableVertexAttribArray(vertLoc);
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(colorLoc);

}
*/




























// デフォルトコンストラクタ
LightPropagationVolumes::LightPropagationVolumes()
{
	m_numIter			= 8;
//	m_numLpvLevels		= 0;
//	InitVec(m_LpvSize, 0, 0, 0);
//	InitVec(m_RsmSize, 0, 0);
	InitVec(m_VplSize, 0, 0);

	m_Mode				= NORMAL;

	m_ulMatMVP			= -1;
	m_ulLightIntensity	= -1;
	m_ulLightPos		= -1;
	m_ulLightDir		= -1;
	m_ulRsmSize			= -1;

	m_ulRsmSizeInj		= -1;
//m_ulPositionSampler	= -1;
//m_ulNormalSampler	= -1;
//m_ulFluxSampler		= -1;
	m_ulLpvCellDim		= -1;
//	m_ulMatLPVScreen	= -1;
	m_ulVPLScale		= -1;

	m_ulIter			= -1;
//	m_ulSH_R			= -1;
//	m_ulSH_G			= -1;
//	m_ulSH_B			= -1;
//	m_ulSH_GV			= -1;
//	m_ulSHaccum_R		= -1;
//	m_ulSHaccum_G		= -1;
//	m_ulSHaccum_B		= -1;
	m_ulLpvCellSize		= -1;

	m_RSM				= NULL;
	
	m_Fbo_rsm			= 0;
	m_Rb_rsm			= 0;
	m_Fbo_lpv			= 0;

	m_VAO[0]			= 0;
	m_VAO[1]			= 0;
	m_VBO[0]			= 0;
	m_VBO[1]			= 0;
	eao					= 0;

//m_LpvCellDim		= NULL;
//m_LpvOrigin			= NULL;
}



// デストラクタ
LightPropagationVolumes::~LightPropagationVolumes()
{
	cout << "~LightPropagationVolumes()..." << endl;
	
	//======= フレームバッファオブジェクトの削除 =======//
	cout << "  delete m_Fbo_rsm..." << endl;
	if(m_Fbo_rsm)
	{
		glDeleteFramebuffers(1, &m_Fbo_rsm);
		m_Fbo_rsm = NULL;
	}
	
	cout << "  delete m_Rb_rsm..." << endl;
	if(m_Rb_rsm)
	{
		glDeleteRenderbuffers(1, &m_Rb_rsm);
		m_Rb_rsm = NULL;
	}

	cout << "  delete m_Fbo_lpv..." << endl;
	if(m_Fbo_lpv)
	{
		glDeleteFramebuffers(1, &m_Fbo_lpv);
		m_Fbo_lpv = NULL;
	}


	//=============== テクスチャの削除 =================//
	cout << "  delete m_RSM..." << endl;
	if(m_RSM)
	{
		glDeleteTextures(4, m_RSM);
		delete [] m_RSM;
		m_RSM = NULL;
	}
	/*
	cout << "  delete m_SH_LPVs..." << endl;
	if(m_SH_LPVs)
	{
		for(int i=0; i<2; i++)
		{
			for(int j=0; j<m_numLpvLevels; j++)	glDeleteTextures(3, m_SH_LPVs[i][j].rgb);
			delete [] m_SH_LPVs[i];
		}
		delete [] m_SH_LPVs;
		m_SH_LPVs = NULL;
	}

	cout << "  delete m_SHaccum..." << endl;
	if(m_SHaccum)
	{	
		for(int i=0; i<2; i++)	glDeleteTextures(3, m_SHaccum[i].rgb);
		delete [] m_SHaccum;
		m_SHaccum = NULL;
	}


	cout << "  delete m_SH_GV..." << endl;
	if(m_SH_GV)
	{
		glDeleteTextures(m_numLpvLevels, m_SH_GV);
		delete [] m_SH_GV;
		m_SH_GV = NULL;
	}
	*/
	
	//=============== 仮想点光源の削除 =================//
	m_VPLSprites.clear();
	if(m_VBO[0]){ glDeleteBuffers(1, &m_VBO[0]); m_VBO[0] = 0; }//VBOを削除

	m_LPVSlices.clear();
	if(m_VBO[1]){ glDeleteBuffers(1, &m_VBO[1]); m_VBO[1] = 0;}	//VBOを削除

	m_LPVIndices.clear();
	if(eao){ glDeleteBuffers(1, &eao); eao = 0; }	//VBOを削除

	/*
	//=============== パラメータの削除 =================//
	if(m_LpvCellDim)
	{
		delete	[] m_LpvCellDim;
		m_LpvCellDim = NULL;
	}

	if(m_LpvOrigin)
	{
		delete [] m_LpvOrigin;
		m_LpvOrigin = NULL;
	}
	*/
	

}





bool LightPropagationVolumes::InitShader()
{
	GLuint program_id;

	//===================== RSMパスの初期化 =======================//
	// シェーダオブジェクト生成
	m_Pass[DRAW_RSM].Init("Shader/ReflectiveShadowMap.glsl");
	program_id		= m_Pass[DRAW_RSM].ID();

	// 入力データの設定
	glBindAttribLocation( program_id,	m_alPosition,	"POSITION" );
	glBindAttribLocation( program_id,	m_alNormal,		"NORMAL" );
	glBindAttribLocation( program_id,	m_alTexCoord,	"TEXCOORD" );
	glBindAttribLocation( program_id,	m_alColor,		"COLOR" );

	// シェーダ変数の設定
	m_ulMatMVP			= glGetUniformLocation( program_id, "g_matMVP");
	m_ulLightIntensity	= glGetUniformLocation( program_id, "g_LightIntensity");
	m_ulLightPos		= glGetUniformLocation( program_id, "g_LightPos");
	m_ulLightDir		= glGetUniformLocation( program_id, "g_LightDir");
	m_ulRsmSize			= glGetUniformLocation( program_id, "g_RsmSize");

	// 出力データの設定
	glBindFragDataLocation( program_id,	RsmPosition,	"fPosition");	// ワールド座標
	glBindFragDataLocation( program_id,	RsmNormal,		"fNormal");		// 法線
	glBindFragDataLocation( program_id,	RsmFlux,		"fFlux");		// 放射束
	glBindFragDataLocation( program_id,	RsmDepth,		"fDepth");		// 奥行き

	// プログラムのリンク
	m_Pass[DRAW_RSM].Link();

	// シェーダ変数の設定
	m_Pass[DRAW_RSM].Bind();
		glUniform2iv(m_ulRsmSize, 1, m_RsmSize.xy);
	m_Pass[DRAW_RSM].Unbind();

	
	//=================== LPVInjectionパスの初期化 ===================//
	// シェーダオブジェクト生成
	m_Pass[LPV_INJECTION].Init("Shader/LPVInjection.glsl");
	program_id		= m_Pass[LPV_INJECTION].ID();
	
	// 入力データの設定
	glBindAttribLocation( program_id,	m_alPosition,	"POSITION" );
	
	// シェーダ変数の設定
	//m_ulPositionSampler	= glGetUniformLocation( program_id, "g_PositionSampler" );
	//m_ulNormalSampler	= glGetUniformLocation( program_id, "g_NormalSampler" );
	//m_ulFluxSampler		= glGetUniformLocation( program_id, "g_FluxSampler" );
	m_ulLpvCellDim		= glGetUniformLocation( program_id, "g_LpvCellDim" );
	//m_ulMatLPVScreen	= glGetUniformLocation( program_id, "g_matLPVScreen" );
	m_ulVPLScale		= glGetUniformLocation( program_id, "g_VPLScale" );
	m_ulLightPos_	= glGetUniformLocation( program_id, "g_LightPos" );

	// 出力データの設定
	glBindFragDataLocation( program_id, SH_R, "fSH_R" );
	glBindFragDataLocation( program_id, SH_G, "fSH_G" );
	glBindFragDataLocation( program_id, SH_B, "fSH_B" );
	glBindFragDataLocation( program_id, SHaccum_R, "fSHaccum_R" );
	glBindFragDataLocation( program_id, SHaccum_G, "fSHaccum_G" );
	glBindFragDataLocation( program_id, SHaccum_B, "fSHaccum_B" );
	
	// プログラムのリンク
	m_Pass[LPV_INJECTION].Link();

//########################### UBOテスト #############################//

// シェーダーからuniform blockを取得して、バインディングポイントUBP_MATRIXに対応付ける
matBlockIndex = glGetUniformBlockIndex( m_Pass[LPV_INJECTION].ID(), "matParam" );
glUniformBlockBinding( m_Pass[LPV_INJECTION].ID(), matBlockIndex, UBP_MATRIX );

// UniformBufferObjectを作成する
glGetActiveUniformBlockiv( m_Pass[LPV_INJECTION].ID(), matBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &matBlockSize );
 
glGenBuffers(1, matUboID);
glBindBuffer(GL_UNIFORM_BUFFER, matUboID[0]);
{
	glBufferData(GL_UNIFORM_BUFFER, matBlockSize, NULL, GL_DYNAMIC_DRAW); 
	glBindBufferBase( GL_UNIFORM_BUFFER, UBP_MATRIX, matUboID[0] );
}
glBindBuffer(GL_UNIFORM_BUFFER, 0);

//###################################################################//




	// シェーダ変数およびテクスチャユニットの設定
	m_Pass[LPV_INJECTION].Bind();
	{
		// RSM Position
		glUniform1i( glGetUniformLocation( program_id, "g_PositionSampler" ), RsmPosition );
		glActiveTexture(TextureUnit[RsmPosition]);
		glBindTexture(GL_TEXTURE_2D, m_RSM[RsmPosition]);

		// RSM Noraml
		glUniform1i( glGetUniformLocation( program_id, "g_NormalSampler" ), RsmNormal );
		glActiveTexture(TextureUnit[RsmNormal]);
		glBindTexture(GL_TEXTURE_2D, m_RSM[RsmNormal]);

		// RSM Flux
		glUniform1i( glGetUniformLocation( program_id, "g_FluxSampler" ), RsmFlux );
		glActiveTexture(TextureUnit[RsmFlux]);
		glBindTexture(GL_TEXTURE_2D, m_RSM[RsmFlux]);

		glUniform1f(m_ulVPLScale, float(m_RsmSize.x*m_RsmSize.y) / float(m_VplSize.x*m_VplSize.y));
	}
	m_Pass[LPV_INJECTION].Unbind();
	
	
	//=================== GVInjectionパスの初期化 ===================//
	// シェーダオブジェクト生成
	m_Pass[GV_INJECTION].Init("Shader/GVInjection.glsl");
	program_id		= m_Pass[GV_INJECTION].ID();
	
	// 入力データの設定
	glBindAttribLocation( program_id,	m_alPosition,	"POSITION" );
	
	// シェーダ変数の設定
	m_ulRsmSizeInj		= glGetUniformLocation( program_id, "g_RsmSize" );
	//m_ulPositionSampler2	= glGetUniformLocation( program_id, "g_PositionSampler" );
	//m_ulNormalSampler2	= glGetUniformLocation( program_id, "g_NormalSampler" );
	m_ulLpvCellDim2		= glGetUniformLocation( program_id, "g_LpvCellDim" );
//	m_ulMatLPVScreen2		= glGetUniformLocation( program_id, "g_matGVScreen" );

	// 出力データの設定
	glBindFragDataLocation( program_id, SH_GV, "fSH_GV");
	
	// プログラムのリンク
	m_Pass[GV_INJECTION].Link();
	

//########################### UBOテスト #############################//

// シェーダーからuniform blockを取得して、バインディングポイントUBP_MATRIXに対応付ける
matBlockIndex = glGetUniformBlockIndex( m_Pass[GV_INJECTION].ID(), "matParam" );
glUniformBlockBinding( m_Pass[GV_INJECTION].ID(), matBlockIndex, UBP_MATRIX );
/*
// UniformBufferObjectを作成する
glGetActiveUniformBlockiv( m_Pass[GV_INJECTION].ID(), matBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &matBlockSize );

glGenBuffers(1, matUboID);
glBindBuffer(GL_UNIFORM_BUFFER, matUboID[0]);
{
	glBufferData(GL_UNIFORM_BUFFER, matBlockSize, NULL, GL_DYNAMIC_DRAW); 
	glBindBufferBase( GL_UNIFORM_BUFFER, UBP_MATRIX, matUboID[0] );
}
glBindBuffer(GL_UNIFORM_BUFFER, 0);
*/
//###################################################################//


	// シェーダ変数およびテクスチャユニットの設定
	m_Pass[GV_INJECTION].Bind();
	{
		glUniform2iv(m_ulRsmSizeInj, 1, m_RsmSize.xy);
		
		glUniform1i( glGetUniformLocation( program_id, "g_PositionSampler" ), RsmPosition );
		glActiveTexture(TextureUnit[RsmPosition]);
		glBindTexture(GL_TEXTURE_2D, m_RSM[RsmPosition]);

		glUniform1i( glGetUniformLocation( program_id, "g_NormalSampler" ), RsmNormal );
		glActiveTexture(TextureUnit[RsmNormal]);
		glBindTexture(GL_TEXTURE_2D, m_RSM[RsmNormal]);
	}
	m_Pass[GV_INJECTION].Unbind();
	
	
	//================== Propagationパスの初期化 ==================//
	// シェーダオブジェクト生成
	m_Pass[PROPAGATION].Init("Shader/Propagation.glsl");
	program_id		= m_Pass[PROPAGATION].ID();
	
	// 入力データの設定
	glBindAttribLocation( program_id,	m_alPosition,	"POSITION" );
	glBindAttribLocation( program_id,	m_alTexCoord,	"TEXCOORD" );
	
	// シェーダ変数の設定
	m_ulIter		= glGetUniformLocation( program_id, "g_Iter" );
	//m_ulSH_R		= glGetUniformLocation( program_id, "g_LPV_SH_R" );
	//m_ulSH_G		= glGetUniformLocation( program_id, "g_LPV_SH_G" );
	//m_ulSH_B		= glGetUniformLocation( program_id, "g_LPV_SH_B" );
	//m_ulSHaccum_R	= glGetUniformLocation( program_id, "g_SHaccum_R" );
	//m_ulSHaccum_G	= glGetUniformLocation( program_id, "g_SHaccum_G" );
	//m_ulSHaccum_B	= glGetUniformLocation( program_id, "g_SHaccum_B" );
	//m_ulSH_GV		= glGetUniformLocation( program_id, "g_GV_SH" );
	m_ulLpvCellSize	= glGetUniformLocation( program_id, "g_LpvSize" );

	// 出力データの設定
	glBindFragDataLocation( program_id, SH_R, "fSH_R");
	glBindFragDataLocation( program_id, SH_G, "fSH_G");
	glBindFragDataLocation( program_id, SH_B, "fSH_B");
	glBindFragDataLocation( program_id, SHaccum_R, "fSHaccum_R");
	glBindFragDataLocation( program_id, SHaccum_G, "fSHaccum_G");
	glBindFragDataLocation( program_id, SHaccum_B, "fSHaccum_B");


	// プログラムのリンク
	m_Pass[PROPAGATION].Link();

	// シェーダ変数およびテクスチャユニットの設定
	m_Pass[PROPAGATION].Bind();
	{
		glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_R" ), SH_R );
		glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_G" ), SH_G );
		glUniform1i( glGetUniformLocation( program_id, "g_LPV_SH_B" ), SH_B );
		glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_R" ), SHaccum_R );
		glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_G" ), SHaccum_G );
		glUniform1i( glGetUniformLocation( program_id, "g_SHaccum_B" ), SHaccum_B );
		glUniform1i( glGetUniformLocation( program_id, "g_GV_SH" ), SH_GV);

		glUniform3f(m_ulLpvCellSize, float(m_pLPVTex->m_LpvSize.x), float(m_pLPVTex->m_LpvSize.y), float(m_pLPVTex->m_LpvSize.z));
	}
	m_Pass[PROPAGATION].Unbind();
	

	//================ SurfaceShaderパスの初期化 ================//
	// シェーダオブジェクト生成
	m_Pass[SURFACE_SHADER].Init("Shader/SurfaceShader.glsl");
	program_id = m_Pass[SURFACE_SHADER].ID();

	// 入力データの設定
	glBindAttribLocation( program_id,	m_alPosition,	"POSITION" );
	glBindAttribLocation( program_id,	m_alNormal,		"NORMAL" );
	glBindAttribLocation( program_id,	m_alColor,		"COLOR" );

	// シェーダ変数の設定
	m_ulMatMVP2		= glGetUniformLocation( program_id, "g_matMVP");
	m_ulEyePos2		= glGetUniformLocation( program_id, "g_EyePos");
	m_ulLightPos2	= glGetUniformLocation( program_id, "g_LightPos");

	m_SHR			= glGetUniformLocation( program_id, "g_LPV_SH_R" );
	m_SHG			= glGetUniformLocation( program_id, "g_LPV_SH_G" );
	m_SHB			= glGetUniformLocation( program_id, "g_LPV_SH_B" );
	m_ShadowMap		= glGetUniformLocation( program_id, "g_ShadowMap" );

	// 出力データの設定


	// プログラムのリンク
	m_Pass[SURFACE_SHADER].Link();

	// シェーダ変数およびテクスチャユニットの設定
	m_Pass[SURFACE_SHADER].Bind();
	{
		glUniform1i(m_SHR, SH_R);
		glUniform1i(m_SHG, SH_G);
		glUniform1i(m_SHB, SH_B);
		glUniform1i(m_ShadowMap, 3);
	}	
	m_Pass[SURFACE_SHADER].Unbind();


	return true;

}



// テクスチャオブジェクトを初期化する
bool LightPropagationVolumes::InitTexture()
{
//	int i, j;
	GLint lfilter = GL_LINEAR;
	GLint nfilter = GL_NEAREST;
	GLint wrap = GL_CLAMP_TO_EDGE;


	//====================== テクスチャオブジェクトの初期化 ============================//
	// RSMテクスチャ
	m_RSM = new GLuint[4];
	m_RSM[ RsmPosition ] = AllocateTexture2D(nfilter, nfilter, wrap, wrap, m_RsmSize.x, m_RsmSize.y, GL_RGBA16F, GL_RGBA, GL_FLOAT);// Psition(RGB:xyz座標, A:モデルビュー変換後のw)
	m_RSM[ RsmNormal ] = AllocateTexture2D(nfilter, nfilter, wrap, wrap, m_RsmSize.x, m_RsmSize.y, GL_RGB16F, GL_RGB, GL_FLOAT);	// Normal
	m_RSM[ RsmFlux ] = AllocateTexture2D(lfilter, lfilter, wrap, wrap, m_RsmSize.x, m_RsmSize.y, GL_RGB16F, GL_RGB, GL_FLOAT);		// Flux
	m_RSM[ RsmDepth ] = AllocateTexture2D(lfilter, lfilter, wrap, wrap, m_RsmSize.x, m_RsmSize.y, GL_R16F, GL_RED, GL_FLOAT);		// Depth
	/*
	// LPVテクスチャ
	m_SH_LPVs		= new SH_LPV*[2];// Src/Dst分のLPVバッファを用意する
	m_SH_LPVs[0]	= new SH_LPV[m_numLpvLevels];
	m_SH_LPVs[1]	= new SH_LPV[m_numLpvLevels];

	m_SHaccum		= new SH_LPV[2];


	for(j=0; j<3; j++)
	{
		for(i=0; i<m_numLpvLevels; i++)
		{
			m_SH_LPVs[0][i].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z,	GL_RGBA16F, GL_RGBA, GL_FLOAT);
			m_SH_LPVs[1][i].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z,	GL_RGBA16F, GL_RGBA, GL_FLOAT);
		}// end of i loop

		m_SHaccum[0].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_SHaccum[1].rgb[j] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, GL_RGBA16F, GL_RGBA, GL_FLOAT);

	}// end of j loop

	// GVテクスチャ
	m_SH_GV	= new GLuint[m_numLpvLevels];

	for(i=0; i<m_numLpvLevels; i++)
		m_SH_GV[i] = AllocateTexture3D(lfilter, lfilter, wrap, wrap, wrap, m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, GL_RGBA16F, GL_RGBA, GL_FLOAT);
*/

	Src = 0;
	Dst = 1;

	//================== RSM用フレームバッファオブジェクトの初期化 ===================//
	// fbo, rboの生成
	glGenFramebuffers(1, &m_Fbo_rsm);
	glGenRenderbuffers(1, &m_Rb_rsm);

	// rboの初期化
	glBindRenderbuffer(GL_RENDERBUFFER, m_Rb_rsm);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_RsmSize.x, m_RsmSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	// fboにrboとテクスチャを割り当てる
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo_rsm);
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Rb_rsm);
		glFramebufferTexture(GL_FRAMEBUFFER, Attachments[RsmPosition],	m_RSM[RsmPosition], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, Attachments[RsmNormal],	m_RSM[RsmNormal], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, Attachments[RsmFlux],		m_RSM[RsmFlux], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, Attachments[RsmDepth],		m_RSM[RsmDepth], 0);
		
		glDrawBuffers(4, Attachments);
		glClearColor(0,0,0,0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//================== LPV用フレームバッファオブジェクトの初期化 ===================//
	glGenFramebuffers(1, &m_Fbo_lpv);
	
	return true;
}


// RSM仮想点光源スプライトの初期化
bool LightPropagationVolumes::InitVPLSprites()
{
	int x, y, i;
	float dx, dy, fx, fy;
	float scale_x, scale_y;
	float offset_x, offset_y;

	
	//================ メモリを確保する =================//
	m_VPLSprites.clear();
	m_VPLSprites.resize(m_VplSize.x*m_VplSize.y);


	//========== 座標計算用パラメータを準備する =========//
	scale_x		= float(m_VplSize.x-1)/float(m_VplSize.x);
	scale_y		= float(m_VplSize.x-1)/float(m_VplSize.x);

	offset_x	= 0.5f / float(m_VplSize.x);
	offset_y	= 0.5f / float(m_VplSize.y);

	dx = 1.0f / float(m_VplSize.x-1);
	dy = 1.0f / float(m_VplSize.y-1);


	//========= 頂点(テクスチャ座標)を設定する =========//
	i=0;
	for(y=0; y<m_VplSize.y; y++)
	{
		fy = dy * float(y) * scale_y + offset_y; // y方向内側に0.5テクセルだけ縮める

		for(x=0; x<m_VplSize.x; x++)
		{
			fx = dx * float(x) * scale_x + offset_x;

			InitVec(m_VPLSprites[i], fx, fy);
			i++;
		}// end of x loop
	}// end of y loop


	//=============== VAOを初期化する ================//
	// VBOの作成
	glGenBuffers(1, &m_VBO[0]);					//VBOを1つ作成
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);	// VBOをバインド
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2f)*m_VPLSprites.size(), &m_VPLSprites[0], GL_STATIC_DRAW);// 頂点データを登録
	glBindBuffer(GL_ARRAY_BUFFER, 0); // VBOをアンバインド
	
	// VAOの作成
	glGenVertexArrays(1, &m_VAO[0]);// VAOを1つ作成
	glBindVertexArray(m_VAO[0]);	//VAOをバインド
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]); // VBOをバインド
		{
			glVertexAttribPointer(m_alPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), 0);
			glEnableVertexAttribArray(m_alPosition);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0); // VBOをアンバインド
	}
	glBindVertexArray(0);// VAOをアンバインド

	return true;
}



// LPVセル中心点スプライトの初期化
bool LightPropagationVolumes::InitLPVSlices()
{
	int z;
	int i, j; // i:頂点配列の要素カウント, j:インデックス配列の要素カウント
	float dx,dy,dz, posz, texz;
	float tex_scale_z;
	Vec3f tex_offset;

	//================ メモリを確保する =================//
	m_LPVSlices.clear();
	m_LPVSlices.resize(m_pLPVTex->m_LpvSize.z*4);// zスライスあたり4頂点を確保する
	m_LPVIndices.clear();
	m_LPVIndices.resize(m_pLPVTex->m_LpvSize.z*6);//zスライスあたり6つ必要


	//========== 座標計算用パラメータを準備する =========//
	dx = 1.0f / float(m_pLPVTex->m_LpvSize.x-1);
	dy = 1.0f / float(m_pLPVTex->m_LpvSize.y-1);
	dz = 1.0f / float(m_pLPVTex->m_LpvSize.z-1);

	tex_scale_z = float(m_pLPVTex->m_LpvSize.z-1)/float(m_pLPVTex->m_LpvSize.z);
		
	InitVec(tex_offset, 0.5f/float(m_pLPVTex->m_LpvSize.x), 0.5f/float(m_pLPVTex->m_LpvSize.y), 0.5f/float(m_pLPVTex->m_LpvSize.z) );

	//========= 頂点(スクリーン空間座標[-1,1])を設定する =========//
	i=0; j=0;
	for(z=0; z<m_pLPVTex->m_LpvSize.z; z++)// z平面のスライス毎に，頂点座標と頂点インデックスを生成する
	{
		posz = 2.0f*dz*float(z) - 1.0f;					// 頂点座標z値[-1,1]
		texz = dz*float(z) * tex_scale_z + tex_offset.z;// テクスチャ座標z値

		//========= 頂点インデックスを格納 =========//
		m_LPVIndices[j++] = i;
		m_LPVIndices[j++] = i+1;
		m_LPVIndices[j++] = i+3;
		m_LPVIndices[j++] = i+1;
		m_LPVIndices[j++] = i+2;
		m_LPVIndices[j++] = i+3;

		//============= 頂点座標を格納 =============//
		// v0(-1,-1)
		InitVec(m_LPVSlices[i].Position, -1.0f, -1.0f, posz);
		InitVec(m_LPVSlices[i].TexCoord, tex_offset.x, tex_offset.y, texz);
		i++;
		// v1(+1,-1)
		InitVec(m_LPVSlices[i].Position, +1.0f, -1.0f, posz);
		InitVec(m_LPVSlices[i].TexCoord, 1.0f-tex_offset.x, tex_offset.y, texz);
		i++;
		// v2(+1,+1)
		InitVec(m_LPVSlices[i].Position, +1.0f, +1.0f, posz);
		InitVec(m_LPVSlices[i].TexCoord, 1.0f-tex_offset.x, 1.0f-tex_offset.y, texz);
		i++;
		// v3(-1,+1)
		InitVec(m_LPVSlices[i].Position, -1.0f, +1.0f, posz);
		InitVec(m_LPVSlices[i].TexCoord, tex_offset.x, 1.0f-tex_offset.y, texz);
		i++;

	}// end of z loop


	//=============== VAOを初期化する ================//
	// VBOの作成
	glGenBuffers(1, &m_VBO[1]);					//VBOを1つ作成
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);	// VBOをバインド
		glBufferData(GL_ARRAY_BUFFER, sizeof(stLPVSlice)*m_LPVSlices.size(), &m_LPVSlices[0], GL_STATIC_DRAW);// 頂点データを登録
	glBindBuffer(GL_ARRAY_BUFFER, 0); // VBOをアンバインド
	
	// インデックスバッファの作成
	glGenBuffers(1, &eao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eao);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_LPVIndices.size()*sizeof(GLuint), &m_LPVIndices[0], GL_STATIC_DRAW);// インデックスを登録
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// VAOの作成
	glGenVertexArrays(1, &m_VAO[1]);// VAOを1つ作成
	glBindVertexArray(m_VAO[1]);	//VAOをバインド
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]); // VBOをバインド
		{
			glVertexAttribPointer(m_alPosition, 3, GL_FLOAT, GL_FALSE, sizeof(stLPVSlice), 0);
			glVertexAttribPointer(m_alTexCoord, 3, GL_FLOAT, GL_FALSE, sizeof(stLPVSlice), (GLfloat*)0 + 3);
			glEnableVertexAttribArray(m_alPosition);
			glEnableVertexAttribArray(m_alTexCoord);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0); // VBOをアンバインド
	}
	glBindVertexArray(0);// VAOをアンバインド


	return true;
}



bool LightPropagationVolumes::Init(int num_lpvs, const Vec3i &lpv_size, const Vec2i &rsm_size, const Vec2i &vpl_size)
{
//	m_numLpvLevels	= num_lpvs;
//	m_LpvSize		= lpv_size;
	m_RsmSize		= rsm_size;
	m_VplSize		= vpl_size;
	
/*
//========= TODO: 別関数で実装してコールバックする．いずれ ===========//
m_LpvCellDim	= new float[m_pLPVTex->m_numCascades];
m_LpvOrigin	= new Vec3f[m_pLPVTex->m_numCascades];
for(int i=0; i<m_pLPVTex->m_numCascades; i++)
{
	m_LpvCellDim[i] = 0.4f * pow(2.0f, float(i));
	InitVec(m_LpvOrigin[i], 0.0f, 0.0f, 0.0f);
}
//====================================================================//
*/
	InitTexture();
	InitShader();

	InitVPLSprites();
	InitLPVSlices();
	
	return true;
}






// RSMパス. TDOO: LPVシェーダーの外へ移動。RMSはLPVシェーダーへの入力データにする
void LightPropagationVolumes::DrawRSM(const Camera &lightcamera)
{
	//==================== RSM計算 ==========================//
	glViewport(0, 0, m_RsmSize.x, m_RsmSize.y);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo_rsm);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	m_Pass[DRAW_RSM].Bind();
	{
		// シェーダへ光源パラメータを渡す
		glUniformMatrix4fv(m_ulMatMVP, 1, GL_FALSE, lightcamera.GetViewProjectionMatrix()->m );// 座標変換行列
		glUniform3fv(m_ulLightIntensity, 1, LightIntensity.xyz);// Flux
		glUniform3fv(m_ulLightPos, 1, lightcamera.GetPosition()->xyz );// 位置
		//glUniform3fv(m_ulLightDir, );// 向き

		// 図形を描画する(テスト用の立方体)
		DrawSlitRect(1.5874, 1.2599, 1.0, m_alPosition, m_alNormal, m_alColor);
		DrawCube_with_tangent(0.4, 0.4, 0.5, m_alPosition, m_alNormal, m_alColor);
	}
	m_Pass[DRAW_RSM].Unbind();
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}



// LPVInjectionパス
// LPVのセルに仮想点光源の放射束を書き込む．
// 同一セルに複数の点光源を書き込む場合は加算合成する
void LightPropagationVolumes::LPVInjection(const Vec3f &lightpos)
{
	Mat4f	m_matLvpScr;// LPVスクリーン座標空間(x,y,zが[-1,1],z軸上方向から見下ろす)への座標変換行列
	MatIdentity(m_matLvpScr);


	//============ GL設定初期化 =============//
	// アルファブレンド有効化
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	// ビューポート設定
	glViewport(0, 0, m_pLPVTex->m_LpvSize.x, m_pLPVTex->m_LpvSize.y);

	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo_lpv);
	glDrawBuffers(6, Attachments);
	
	glBindVertexArray(m_VAO[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, matUboID[0]);


	//============ シェーダ起動 =============//
	m_Pass[LPV_INJECTION].Bind();
	{
		for(int i=0; i<m_pLPVTex->m_numCascades; i++)
		{
			//============ LPVスクリーン座標変換行列の計算 ==========//
			// LPVの大きさに合わせたスケーリング係数の追記
			m_matLvpScr.m00	= 1.0 / (0.5f * float(m_pLPVTex->m_LpvSize.x) * m_pLPVTex->m_LpvCellDim[i]);
			m_matLvpScr.m11	= 1.0 / (0.5f * float(m_pLPVTex->m_LpvSize.y) * m_pLPVTex->m_LpvCellDim[i]);
			m_matLvpScr.m22	= 1.0 / (0.5f * float(m_pLPVTex->m_LpvSize.z) * m_pLPVTex->m_LpvCellDim[i]);

			// LPV位置に併せた平行移動係数の追記
			m_matLvpScr.m03	= -m_pLPVTex->m_LpvOrigin[i].x * m_matLvpScr.m00;
			m_matLvpScr.m13	= -m_pLPVTex->m_LpvOrigin[i].y * m_matLvpScr.m11;
			m_matLvpScr.m23	= -m_pLPVTex->m_LpvOrigin[i].z * m_matLvpScr.m22;
			
			//=============== FBOにテクスチャをアタッチ =============//
			glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SH_R], m_pLPVTex->m_SH_LPVs[Dst][i].r, 0);	// GL_COLOR_ATTACHMENT0: RのSH係数
			glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SH_G], m_pLPVTex->m_SH_LPVs[Dst][i].g, 0);	// GL_COLOR_ATTACHMENT1: GのSH係数
			glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SH_B],	m_pLPVTex->m_SH_LPVs[Dst][i].b, 0);	// GL_COLOR_ATTACHMENT2: BのSH係数
			glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SHaccum_R], m_pLPVTex->m_SHaccum[Dst].r, 0);	// GL_COLOR_ATTACHMENT0: RのSH係数
			glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SHaccum_G], m_pLPVTex->m_SHaccum[Dst].g, 0);	// GL_COLOR_ATTACHMENT1: GのSH係数
			glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SHaccum_B], m_pLPVTex->m_SHaccum[Dst].b, 0);	// GL_COLOR_ATTACHMENT2: BのSH係数
			
			glClear( GL_COLOR_BUFFER_BIT );


			//================== シェーダ変数の設定 =================//
glBufferData(GL_UNIFORM_BUFFER, matBlockSize, m_matLvpScr.m, GL_DYNAMIC_DRAW);
//glUniformMatrix4fv(m_ulMatLPVScreen, 1, GL_FALSE, *(m_matLvpScr.m) );// TODO: UniformBlockを用いた処理へ変更。削除予定
			glUniform1f(m_ulLpvCellDim, m_pLPVTex->m_LpvCellDim[i]);
			glUniform3fv(m_ulLightPos_, 1, lightpos.xyz);
			
			//===================== VPLを描画 ==================//
			glDrawArrays(GL_POINTS, 0, m_VPLSprites.size());
			
		}// end of i loop
		
	}
	m_Pass[LPV_INJECTION].Unbind();


glBindBuffer(GL_UNIFORM_BUFFER, 0);


	//=================== 後片付け =================//
	glBindVertexArray(0);// 仮想点光源VAOの無効化
//glBindFramebuffer(GL_FRAMEBUFFER, 0);// フレームバッファオブジェクトの解除.後続のPropagation処理でも同じFBOを利用するので，ここで明示的に解除する必要はない
	glDisable(GL_BLEND);// アルファブレンド無効化

	Src ^= 1;//Dst;
	Dst ^= 1;//(Src+1)%2;

}




// フレームバッファ（頂点座標と法線）からGeometryVolumeを計算する
void LightPropagationVolumes::GVInjection()
{
	Mat4f	m_matLvpScr;// LPVスクリーン座標空間(x,y,zが[-1,1],z軸上方向から見下ろす)への座標変換行列
	MatIdentity(m_matLvpScr);
	
	//============ GL設定初期化 =============//
	// アルファブレンド有効化
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	// ビューポート設定
	glViewport(0, 0, m_pLPVTex->m_LpvSize.x, m_pLPVTex->m_LpvSize.y);

//	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo_lpv);// FBOのバインド．先行のLPVInjectionで使ったFBOをそのまま使うので，ここで明示的にバインドする必要はない
	glDrawBuffers(1, &Attachments[SH_GV]);

	glBindVertexArray(m_VAO[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, matUboID[0]);


	//============ シェーダ起動 =============//
	m_Pass[GV_INJECTION].Bind();
	{
		
		for(int i=0; i<m_pLPVTex->m_numCascades; i++)
		{
			//============ LPVスクリーン座標変換行列の計算 ==========//
			// LPVの大きさに合わせたスケーリング係数の追記
			m_matLvpScr.m00	= 1.0 / (0.5f * float(m_pLPVTex->m_LpvSize.x) * m_pLPVTex->m_LpvCellDim[i]);
			m_matLvpScr.m11	= 1.0 / (0.5f * float(m_pLPVTex->m_LpvSize.y) * m_pLPVTex->m_LpvCellDim[i]);
			m_matLvpScr.m22	= 1.0 / (0.5f * float(m_pLPVTex->m_LpvSize.z) * m_pLPVTex->m_LpvCellDim[i]);

			// LPV位置に併せた平行移動係数の追記
			m_matLvpScr.m03	= -m_pLPVTex->m_LpvOrigin[i].x * m_matLvpScr.m00;
			m_matLvpScr.m13	= -m_pLPVTex->m_LpvOrigin[i].y * m_matLvpScr.m11;
			m_matLvpScr.m23	= -m_pLPVTex->m_LpvOrigin[i].z * m_matLvpScr.m22;
			
			//=============== FBOにテクスチャをアタッチ =============//
			glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SH_GV], m_pLPVTex->m_SH_GV[i], 0);	// GL_COLOR_ATTACHMENT6: GVのSH係数
			
			glClear(GL_COLOR_BUFFER_BIT);


			//================== シェーダ変数の設定 =================//
//glUniformMatrix4fv(m_ulMatLPVScreen2, 1, GL_FALSE, *(m_matLvpScr.m) );// TODO: UniformBlockを用いた処理へ変更。削除予定
			glUniform1f(m_ulLpvCellDim2, m_pLPVTex->m_LpvCellDim[i]);
glBufferData(GL_UNIFORM_BUFFER, matBlockSize, m_matLvpScr.m, GL_DYNAMIC_DRAW);

			//===================== VPLを描画 ==================//
			glDrawArrays(GL_POINTS, 0, m_VPLSprites.size());
			
		}// end of i loop
	
	}
	m_Pass[GV_INJECTION].Unbind();

	
	//=================== 後片付け =================//
	glBindVertexArray(0);// 仮想点光源VAOの無効化
//glBindFramebuffer(GL_FRAMEBUFFER, 0);// フレームバッファオブジェクトの解除.後続のPropagation処理でも同じFBOを利用するので，ここで明示的に解除する必要はない
	glDisable(GL_BLEND);// アルファブレンド無効化

}




// Propagationパス: 

// Src,Dstで切り替えるシェーダー

void LightPropagationVolumes::Propagation()
{
	
	int i, k;

	//============ GL設定初期化 =============//
	// ビューポート設定
	glViewport(0, 0, m_pLPVTex->m_LpvSize.x, m_pLPVTex->m_LpvSize.y);
	
	// 描画する形状データのバインド(頂点とインデックス)
	glBindVertexArray(m_VAO[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eao);

	// レンダーターゲットの設定
//glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo_lpv);// FBOのバインド．先行のInjectionで使ったFBOをそのまま使うので，ここで明示的にバインドする必要はない
	glDrawBuffers(6, Attachments);

	//============ シェーダ起動 =============//
	m_Pass[PROPAGATION].Bind();
	{
		//============= シェーダ変数のアタッチ ==============//
		// セルあたりのテクスチャ座標移動量

		for(i=0; i<m_pLPVTex->m_numCascades; i++)
		{
			glActiveTexture(TextureUnit[SH_GV]);
			glBindTexture(GL_TEXTURE_3D, *(m_pLPVTex->m_SH_GV + i));

for(k=0; k<m_numIter; k++)
			{
				glUniform1i(m_ulIter, m_numIter);

				//====== 参照するLPVテクスチャのシェーダ変数へのアタッチ ======//
				glActiveTexture(TextureUnit[SH_R]);			glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SH_LPVs[Src][i].r);
				glActiveTexture(TextureUnit[SH_G]);			glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SH_LPVs[Src][i].g);
				glActiveTexture(TextureUnit[SH_B]);			glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SH_LPVs[Src][i].b);
				glActiveTexture(TextureUnit[SHaccum_R]);	glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SHaccum[Src].r);
				glActiveTexture(TextureUnit[SHaccum_G]);	glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SHaccum[Src].g);
				glActiveTexture(TextureUnit[SHaccum_B]);	glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SHaccum[Src].b);
				glActiveTexture(TextureUnit[SH_GV]);		glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SH_GV[i]);

				//====== 伝播結果を書き込むLPVテクスチャのFBOへのアタッチ =====//
				glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SH_R], m_pLPVTex->m_SH_LPVs[Dst][i].r, 0);// GL_COLOR_ATTACHMENT0: RのSH係数
				glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SH_G], m_pLPVTex->m_SH_LPVs[Dst][i].g, 0);// GL_COLOR_ATTACHMENT1: GのSH係数
				glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SH_B], m_pLPVTex->m_SH_LPVs[Dst][i].b, 0);// GL_COLOR_ATTACHMENT2: BのSH係数
				glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SHaccum_R], m_pLPVTex->m_SHaccum[Dst].r, 0);	// GL_COLOR_ATTACHMENT0: RのSH係数
				glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SHaccum_G], m_pLPVTex->m_SHaccum[Dst].g, 0);	// GL_COLOR_ATTACHMENT1: GのSH係数
				glFramebufferTexture(GL_FRAMEBUFFER, Attachments[SHaccum_B], m_pLPVTex->m_SHaccum[Dst].b, 0);	// GL_COLOR_ATTACHMENT2: BのSH係数

				//======================== 描画処理を実行 =====================//
				glDrawElements(GL_TRIANGLES, m_LPVIndices.size(), GL_UNSIGNED_INT, 0);
				//glDrawElementsInstanced(GL_TRIANGLES, m_LPVIndices.size(), GL_UNSIGNED_INT, 0, m_LpvSize.z);

				// LPVテクスチャの参照/書き込みバッファの入れ替え
				Src ^= 1;//Src = Dst;
				Dst ^= 1;//Dst = (Src+1)%2;
				
			}// end of k loop

		}// end of i loop

	}
	m_Pass[PROPAGATION].Unbind();
	
	//=================== 後片付け =================//
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	
}



void LightPropagationVolumes::Draw(const Camera &camera)
{
	//================= 1. RSMを描画する =================//
	DrawRSM(camera);
	
	//=============== 2. LPVにRSMを投影する ==============//
	LPVInjection(*camera.GetPosition());

	//================2.5 GVにRSMを投影する ==============//
	GVInjection();

	
	//====== 3. 反復計算でLPV上で放射束を伝播させる ======//
	Propagation();

	Src ^= 1;//Dst;
	Dst ^= 1;//(Src+1)%2;
	
	

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	//glViewport(0, 0, 256, 256);
	//MyGlTexture::DrawTexture2D(m_RsmSize.x, m_RsmSize.y, m_RSM[RsmPosition]);
	//glViewport(256, 0, 256, 256);
	//MyGlTexture::DrawTexture2D(m_RsmSize.x, m_RsmSize.y, m_RSM[RsmNormal]);
	//glViewport(512, 0, 256, 256);
	//MyGlTexture::DrawTexture2D(m_RsmSize.x, m_RsmSize.y, m_RSM[RsmFlux]);
	//glViewport(512+256, 0, 256, 256);
	//MyGlTexture::DrawTexture2D(m_RsmSize.x, m_RsmSize.y, m_RSM[RsmDepth]);


	//glClearColor(0.2,0.2,0.2,0);
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//
	//glViewport(0, 0, 256, 256);
	//MyGlTexture::DrawTexture3D(m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, m_SH_LPVs[Dst][0].R, 14);
	//glViewport(256, 0, 256, 256);
	//MyGlTexture::DrawTexture3D(m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, m_SH_LPVs[Src][0].R, 14);

	//glViewport(256, 0, 256, 256);
	//MyGlTexture::DrawTexture3D(m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, m_SH_GV[0], 14);



}





void LightPropagationVolumes::DrawSurface(const Camera &camera, const Camera &lightcam)
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	m_Pass[SURFACE_SHADER].Bind();
	{
		glActiveTexture(TextureUnit[SH_R]);
		glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SH_LPVs[Src][0].r);
		
		glActiveTexture(TextureUnit[SH_G]);
		glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SH_LPVs[Src][0].g);
		
		glActiveTexture(TextureUnit[SH_B]);
		glBindTexture(GL_TEXTURE_3D, m_pLPVTex->m_SH_LPVs[Src][0].b);
		
		glActiveTexture(TextureUnit[3]);
		glBindTexture(GL_TEXTURE_2D, m_RSM[RsmDepth]);


		//================== シェーダ変数の設定 =================//
		glUniformMatrix4fv(m_ulMatMVP2, 1, GL_FALSE, camera.GetViewProjectionMatrix()->m );
glUniformMatrix4fv( glGetUniformLocation(m_Pass[SURFACE_SHADER].ID(), "g_matLight"), 1, GL_FALSE, lightcam.GetViewProjectionMatrix()->m );
glUniform3fv( glGetUniformLocation(m_Pass[SURFACE_SHADER].ID(), "g_LightIntensity"), 1, LightIntensity.xyz);

glUniform1i( glGetUniformLocation(m_Pass[SURFACE_SHADER].ID(), "g_Mode"), m_Mode);


		glUniform3fv(m_ulLightPos2, 1, lightcam.GetPosition()->xyz);
		glUniform3fv(m_ulEyePos2, 1, camera.GetPosition()->xyz);

		DrawSlitRect(1.5874, 1.2599, 1.0, m_alPosition, m_alNormal, m_alColor);
		DrawCube_with_tangent(0.4, 0.4, 0.5, m_alPosition, m_alNormal, m_alColor);

	}
	m_Pass[SURFACE_SHADER].Unbind();




	//glViewport(0, 0, 256, 256);
	//MyGlTexture::DrawTexture3D(m_LpvSize.x, m_LpvSize.y, m_LpvSize.z, m_SH_GV[0], 14);
}






/*
// SHテクスチャの中身を表示する(OpenGL2.0ベース)
void LightPropagationVolumes::ViewSH(const Camera &camera)
{
	int x, y, z;
	
	glViewport(0,0,512,512);
	//glClearColor(0.2, 0.2, 0.2, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);// テクスチャユニット0を指定
	glEnable(GL_TEXTURE_3D);

	glBindTexture(GL_TEXTURE_3D, m_SH_LPVs[Dst][0].G);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, camera.GetNearClip(), camera.GetFarClip());
	
	
	glMatrixMode(GL_MODELVIEW);


	const Vec3f	*pos = camera.GetPosition(),
					*dir = camera.GetForward(),
					*up =camera.GetVertical();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	pos->x, pos->y, pos->z,
				pos->x + dir->x, pos->y + dir->y, pos->z + dir->z,
				up->x, up->y, up->z);

	

	glPointSize(1.5);
	glColor3f(1,1,1);
	glBegin(GL_POINTS);

	for(z=0; z<m_LpvSize.z; z++)
	{
		for(y=0; y<m_LpvSize.y; y++)
		{
			for(x=0; x<m_LpvSize.x; x++)
			{
				glTexCoord3f( float(x)/float(m_LpvSize.x), float(y)/float(m_LpvSize.y), float(z)/float(m_LpvSize.z) );
				glVertex3f( float(x)/float(m_LpvSize.x), float(y)/float(m_LpvSize.y), float(z)/float(m_LpvSize.z) );
			}// end of x loop
		}// end of y loop
	}// end of z loop

	glEnd();

	glDisable(GL_TEXTURE_3D);

}
*/