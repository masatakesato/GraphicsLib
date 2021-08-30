#ifndef I_GLBUFFER_OBJECT_H
#define	I_GLBUFFER_OBJECT_H



#include	<GL/glew.h>
#include	<GL/gl.h>
#pragma comment( lib, "glew32.lib" )

#include	"../common/GLHelperFunctions.h"



namespace OreOreLib
{

	class IGLBufferObject
	{
	public:

		virtual void Bind() const{};
		virtual void Unbind() const{};
		// virtual void Draw() const{}
	};


}// end of namespace



#endif // !I_GLBUFFER_OBJECT_H
