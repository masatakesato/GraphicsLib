#include	"VoxelConeTracingPass.h"




namespace OreOreLib
{

	VoxelConeTracingPass::VoxelConeTracingPass()
	{
		m_numMaxRenderObjects	= 0;
		m_numRenderObjects		= 0;
		m_RenderQueue			= NULL;
	}



	VoxelConeTracingPass::~VoxelConeTracingPass()
	{


	}



	bool VoxelConeTracingPass::Init( int maxRenderObjects )
	{
		Release();

		m_numMaxRenderObjects	= maxRenderObjects;
		m_RenderQueue	= new MovableObjectPtr[m_numMaxRenderObjects];
		ClearRenderQueue();

		return true;
	}



	bool VoxelConeTracingPass::Release()
	{
		SafeDeleteArray( m_RenderQueue );
		
		m_numMaxRenderObjects	= 0;
		m_numRenderObjects		= 0;

		return true;
	}



	bool VoxelConeTracingPass::AddRenderObject( const MovableObject *pObj )
	{
		if( m_numRenderObjects >= m_numMaxRenderObjects )
			return false;

		m_RenderQueue[m_numRenderObjects++]	= pObj;
		
		return true;
	}



	void VoxelConeTracingPass::ClearRenderQueue()
	{
		memset( m_RenderQueue, 0, sizeof( MovableObjectPtr ) * m_numRenderObjects );
		m_numRenderObjects	= 0;
	}



	void VoxelConeTracingPass::BindBufferObject( const IBufferObject* pbufferobj )
	{


	}



	void VoxelConeTracingPass::UnbindBufferObject()
	{

	}



	void VoxelConeTracingPass::Execute()
	{


	}



}// end of namespace