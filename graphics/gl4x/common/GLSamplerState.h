#ifndef	GL_SAMPLER_SATAE_H
#define	GL_SAMPLER_SATAE_H


#include	<GL/glew.h>
#pragma comment(lib, "glew32.lib")


namespace OreOreLib
{

class GLSamplerState
{
public:
	
	GLSamplerState();
	~GLSamplerState();

	void Init();
	void Release();

	void Bind( GLuint texunit )	{ m_refTexUnit = texunit;	glBindSampler( m_refTexUnit, sampler_state ); }// bind sampler
	void Unbind()				{ if(m_refTexUnit)			glBindSampler( m_refTexUnit, 0); }		// unbind sampler


private:

	GLuint	sampler_state;
	GLuint	m_refTexUnit;

};


}// end of namespace



#endif	// GL_SAMPLER_SATAE_H //