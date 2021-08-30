#include	"GLSamplerState.h"



namespace OreOreLib
{


GLSamplerState::GLSamplerState()
{
	sampler_state	= 0;
	m_refTexUnit	= 0;

}


GLSamplerState::~GLSamplerState()
{
	Release();

}


void GLSamplerState::Init()
{
	GLuint sampler_state = 0;
	glGenSamplers( 1, &sampler_state );
	glSamplerParameteri( sampler_state, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glSamplerParameteri( sampler_state, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glSamplerParameteri( sampler_state, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glSamplerParameteri( sampler_state, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	//glSamplerParameterf( sampler_state, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f );
}


void GLSamplerState::Release()
{
	glDeleteSamplers( 1, &sampler_state );
	sampler_state	= 0;
	m_refTexUnit	= 0;
}










}// end of namespace