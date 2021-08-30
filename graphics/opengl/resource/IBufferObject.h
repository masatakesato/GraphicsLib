#ifndef I_BUFFER_OBJECT_H
#define	I_BUFFER_OBJECT_H


#include	"./common/Utility.h"


namespace OreOreLib
{

	class IBufferObject
	{
	public:

		IBufferObject(){}
		virtual ~IBufferObject(){}// 仮想関数を作っとかないと、派生クラスのtype_info識別できない
		//virtual void Bind() {}
		//virtual void Unbind() {}
		// virtual void Draw() {}
	};


}// end of namespace



#endif // !I_BUFFER_OBJECT_H
