#ifndef	VIEW_TRANSFORM_BUFFER_H
#define	VIEW_TRANSFORM_BUFFER_H

#include	"../resource/IBufferObject.h"
#include	"../resource/GLUniformBufferObject.h"

#include	"Frustum.h"



namespace OreOreLib
{

	class ViewTransformBuffer : public IBufferObject
	{
	public:

		ViewTransformBuffer();
		~ViewTransformBuffer();

		void Init();
		void Release();

		void BindFrustum( Frustum *pfrustum )	{ m_refFrustum = pfrustum; }
		void UnbindFrustum()					{ m_refFrustum = NULL; }

		inline void Bind()		{ m_uboTransformMatrix.Bind(); m_uboAttributes.Bind(); }
		inline void Unbind()	{ m_uboAttributes.Unbind();	m_uboTransformMatrix.Unbind(); }

		void BindToShader( GLuint program_id );
		void UpdateSubresource();


	private:

		Frustum					*m_refFrustum;
		GLUniformBufferObject	m_uboTransformMatrix;
		GLUniformBufferObject	m_uboAttributes;
		
		static const char*	m_TransformUniformBlockNames[ 2 ];
		//static const tstring	m_TransformUniformBlockNames[2];
		static GLuint			m_BindPoins[2];
	};



}// end of namespace


#endif	// VIEW_TRANSFORM_BUFFER_H //