#include	"IShader.h"

namespace OreOreLib
{

	GLShader			*IShader::m_pCurrShader			= 0;
	GLVertexArrayObject *IShader::m_refScreenSpaceQuad	= NULL;
	GLVertexArrayObject	*IShader::m_refUnitCube			= NULL;


	void IShader::Init()
	{

	}


	void IShader::Release()
	{
		m_pCurrShader			= NULL;
		m_refScreenSpaceQuad	= NULL;
		m_refUnitCube			= NULL;
	}




}// end of namespace