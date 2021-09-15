#ifndef GL_BIND_POINT_MANAGER_H
#define	GL_BIND_POINT_MANAGER_H


#include	<GL/glew.h>
#include	<GL/gl.h>
#pragma comment( lib, "glew32.lib" )


#include	<oreore/common/Utility.h>
#include	<oreore/container/RingQueue.h>



namespace OreOreLib
{

	struct GLBindPoint
	{
		GLuint	Index;
		GLenum	BufferType;
	};

	class GLBindPointManager
	{
	public:

		GLBindPointManager(){}
		virtual ~GLBindPointManager(){}

		static void InitBindPoints();
		static void ReleaseBindPoints();

		static GLuint Reserve( GLenum bufferType );
		static void Free( GLuint bindpoint, GLenum bufferType );
		

	private:
		
		// GL_UNIFORM_BUFFER
		static RingQueue<GLuint>	m_UniformBufferBindPoints;
		//static bool					*m_bUniformBufferBindPointReserved;

		// GL_SHADER_STORAGE_BUFFER
		static RingQueue<GLuint>	m_ShaderStorageBufferBindPoints;
		//static bool					*m_bShaderStorageBufferBindPointReserved;
	
		// GL_ATOMIC_COUNTER_BUFFER


		// GL_TEXTURE_BUFFER?


		static void InitBindPoints( RingQueue<GLuint> *queue, /*bool** breserved,*/ GLenum bufferType );
	};








}// end of namespace



#endif // !GL_BIND_POINT_MANAGER_H
