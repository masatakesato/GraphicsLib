#ifndef	SSDO_BUFFER_H
#define	SSDO_BUFFER_H

#include	<graphics/gl4x/resource/GLUniformBufferObject.h>
#include	<graphics/gl4x/resource/Texture.h>



namespace OreOreLib
{
	struct SSDOParams
	{
		float	OcclusionRadius;
		float	MaxOcclusionDistance;
		Vec2f	TexSize;
		//Vec3f	ViewPosition;	// いずれいらなくなる
	};


	class SSDOBuffer
	{
	public:

		SSDOBuffer();
		SSDOBuffer( int texwidth, int texheight );
		~SSDOBuffer();

		void Init( int texwidth, int texheight );
		void Release();


		inline void SetOcclusionRadius( float val )			{ m_Param.OcclusionRadius = val; }
		inline void SetMaxOcclusionDistance( float val )	{ m_Param.MaxOcclusionDistance = val; }
		inline void SetTexSize( int w, int h )				{ InitVec( m_Param.TexSize, (float)w, (float)h ); }
		//inline void SetViewPosition( const Vec3f& vpos )	{ m_Param.ViewPosition = vpos; }

		inline Texture2D *GetTexOcclusionSH() const			{ return (Texture2D *)&m_TexOcclusionSH; }
		inline Texture2D *GetTexBlur() const				{ return (Texture2D *)&m_TexBlurBuffer; }
		inline Texture2D *GetResult() const					{ return (Texture2D *)&m_TexFinalOcclusion; }
		inline SSDOParams *GetParam() const					{ return (SSDOParams *)&m_Param; }


	//	void BindToShader( GLuint program_id );
	//	void UpdateSubresource();
	//{
	//	m_uboTransformMatrix.BindToShaderByName( program_id, m_TransformUniformBlockNames[0] );
	//	m_uboAttributes.BindToShaderByName( program_id, m_TransformUniformBlockNames[1] );
	//}



	private:

		SSDOParams	m_Param;
		Texture2D	m_TexOcclusionSH;
		Texture2D	m_TexBlurBuffer;
		Texture2D	m_TexFinalOcclusion;
	};



}// end of namespace


#endif	// SSDO_BUFFER_H //