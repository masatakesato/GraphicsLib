#include	"CacheDataManager.h"


#include	<oreore/common/TString.h>
#include	<oreore/common/Utility.h>


namespace OreOreLib
{


CacheDataManager::CacheDataManager()
{
	m_numMaxSlots		= 0;
	m_numReservedSlots	= 0;
	m_CacheStatus		= NULL;

m_LinkBuffer	= NULL;
}



CacheDataManager::CacheDataManager( int max_slots )
{
//tcout << "TileCacheTexture::TileCacheTexture()..." << tendl;
	Init( max_slots );
}



CacheDataManager::~CacheDataManager()
{
//tcout << "TileCacheTexture::~TileCacheTexture()..." << tendl;
	
	//================= キャッシュステータス情報の削除 ================//
	SafeDeleteArray( m_CacheStatus );

SafeDeleteArray( m_LinkBuffer );
}



void CacheDataManager::Init( int max_slots )
{
	//================= パラメータ初期化 =================//	
	m_numMaxSlots		= max_slots;// TileCacheに格納できるタイル数
	m_numReservedSlots	= 0;

	//=========== ステータス管理バッファの確保 ===========//
	SafeDeleteArray( m_CacheStatus );
	m_CacheStatus		= new CacheStatus[ m_numMaxSlots ];
	m_FreeSlots.Init( max_slots );


//============= 連続確保した要素のリンク情報 ==========//
m_LinkBuffer	= new int[ m_numMaxSlots ];



	//================ バッファの初期化 ==================//
	Clear();
}



void CacheDataManager::Release()
{
	m_numMaxSlots		= 0;
	m_numReservedSlots	= 0;
	SafeDeleteArray( m_CacheStatus );
	m_FreeSlots.Release();
}



}// end of namespace