#ifndef VOXEL_CONE_TRACING_PASS_H

#include	<graphics/gl4x/common/Defines.h>



namespace OreOreLib
{

	// TODO: ボクセルコーンとレーシングのパス.レンダーキュー管理機能の実験中. 2015.05.04
	class VoxelConeTracingPass
	{
	public:

		VoxelConeTracingPass();
		~VoxelConeTracingPass();

		bool Init( int maxRenderObjects=MAX_RENDER_QUEUE );
		bool Release();

		int NumRenderObjects() const					{ return m_numRenderObjects; }
		int NumMaxRenderObjects() const					{ return m_numMaxRenderObjects; }
		const MovableObject** GetRenderQueue() const	{ return m_RenderQueue; }


		// Override Virtual Functions
		virtual bool AddRenderObject( const MovableObject *pObj );
		virtual void ClearRenderQueue();

		virtual void BindBufferObject( const IBufferObject* pbufferobj );
		virtual void UnbindBufferObject();

		virtual void Execute();

	private:

		typedef const MovableObject* MovableObjectPtr;

		int					m_numMaxRenderObjects;
		int					m_numRenderObjects;
		MovableObjectPtr	*m_RenderQueue;



	};



}// end of namespace


#endif // !VOXEL_CONE_TRACING_PASS_H
