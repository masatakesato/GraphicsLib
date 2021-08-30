#include	"TileCacheTexture.h"

//#include	<oreore/GLTexture.h>
//using namespace MyGlTexture;
#include	"./common/TString.h"




TileCacheTexture::TileCacheTexture( int cache_width, int cache_height, int tile_size )
{
tcout << "TileCacheTexture::TileCacheTexture()..." << tendl;

	//=============== パラメータ初期化 ============//
	m_TexSize		= tile_size;
	m_CacheWidth	= cache_width;
	m_CacheHeight	= cache_height;
	
//	m_numTiles		= 0;
//	m_numMaxTiles	= m_CacheWidth * m_CacheHeight;// TileCacheに格納できるタイル数
	
	//============== テクスチャキャッシュの初期化 ================//
	m_TileCache.Allocate( m_CacheWidth*m_TexSize, m_CacheHeight*m_TexSize, 0, 0, FORMAT_R16_FLOAT );//			= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_CacheWidth*m_TexSize, m_CacheHeight*m_TexSize, GL_R16F, GL_RED, GL_FLOAT);
	m_TileCache.SetFilterMode( FILTER_MAG_MIN_LINEAR );
	m_TileCache.GenHardwareTexture();
	
	m_TileCache_normal.Allocate( m_CacheWidth*m_TexSize, m_CacheHeight*m_TexSize, 0, 0, FORMAT_R16G16B16_FLOAT ); //	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_CacheWidth*m_TexSize, m_CacheHeight*m_TexSize, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
	m_TileCache_normal.SetFilterMode( FILTER_MAG_MIN_LINEAR );
	m_TileCache_normal.GenHardwareTexture();


	//================ キャッシュステータスの初期化 ===============//
	m_CacheManager.Init( m_CacheWidth * m_CacheHeight );

//	m_CacheStatus = new TileStatus[ m_numMaxTiles ];
//	InitCacheStatus();
}


TileCacheTexture::~TileCacheTexture()
{
tcout << "TileCacheTexture::~TileCacheTexture()..." << tendl;

	//================ タイルキャッシュの削除 ================//
	m_TileCache.Release();
	m_TileCache_normal.Release();

	
	//================= キャッシュ情報の削除 ================//
	m_CacheManager.Release();
	//SafeDeleteArray( m_CacheStatus );
}


//void TileCacheTexture::InitCacheStatus()
//{
//tcout << "TileCacheTexture::TileCacheTexture()..." << tendl;
//
//	int i;
//	
//	for(i=0; i<m_numMaxTiles;  i++)
//	{
//		m_CacheStatus[i].reserved	= false;// 使用可能
//		m_CacheStatus[i].dirty		= false;// 誰もアクセスしてない
//	}// end of i loop
//}