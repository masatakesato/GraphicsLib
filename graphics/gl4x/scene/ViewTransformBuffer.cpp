#include	"ViewTransformBuffer.h"


#include	"GLBindPointManager.h"


namespace OreOreLib
{

	//const tstring	ViewTransformBuffer::m_TransformUniformBlockNames[ 2 ] =
	//{
	//	_T("ubViewTransform"),
	//	_T("ubViewAttribute"),
	//};

	const char*	ViewTransformBuffer::m_TransformUniformBlockNames[2] =
	{
		"ubViewTransform",
		"ubViewAttribute",
	};


	GLuint	ViewTransformBuffer::m_BindPoins[2] = { 0, 0, };



	ViewTransformBuffer::ViewTransformBuffer()
	{
		m_refFrustum	= NULL;
	}


	ViewTransformBuffer::~ViewTransformBuffer()
	{
		Release();
	}

	

	void ViewTransformBuffer::Init()
	{
		Release();

		for( int i=0; i<2; ++i )	if( m_BindPoins[i]==0 )	m_BindPoins[i]	= GLBindPointManager::Reserve( GL_UNIFORM_BUFFER );

		m_uboTransformMatrix.Init( GL_DYNAMIC_DRAW, m_BindPoins[0] );
		m_uboAttributes.Init( GL_DYNAMIC_DRAW, m_BindPoins[1] );
	}
	


	void ViewTransformBuffer::Release()
	{
		m_uboTransformMatrix.Release();
		m_uboAttributes.Release();
	}


	void ViewTransformBuffer::BindToShader( GLuint program_id )
	{
		m_uboTransformMatrix.BindToShaderByName( program_id, m_TransformUniformBlockNames[0] );
		m_uboAttributes.BindToShaderByName( program_id, m_TransformUniformBlockNames[1] );
	}


	void ViewTransformBuffer::UpdateSubresource()
	{
		if( !m_refFrustum )
			return;

		m_uboTransformMatrix.UpdateSubresource( m_refFrustum->GetTransformMatrix(), sizeof(ViewTransformMatrix) );
		m_uboAttributes.UpdateSubresource( m_refFrustum->GetAttribute(), sizeof(FrustumAttribute) );
	}


}// end of namespace