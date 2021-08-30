#include	"TileCache.h"

#include	<graphics/gl4x/resource/GLTexture.h>
using namespace MyGlTexture;



TileCache::TileCache(int cache_width, int cache_height, int tile_size)
{
cout << "TileCache::TileCache()..." << endl;

	//=============== パラメータ初期化 ============//
	m_TexSize		= tile_size;
	m_CacheWidth	= cache_width;
	m_CacheHeight	= cache_height;
	
	m_numTiles		= 0;
	m_numMaxTiles	= m_CacheWidth * m_CacheHeight;// TileCacheに格納できるタイル数
	
	//============== テクスチャキャッシュの初期化 ================//
	m_TileCache			= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_CacheWidth*m_TexSize, m_CacheHeight*m_TexSize, GL_R16F, GL_RED, GL_FLOAT);
	m_TileCache_normal	= AllocateTexture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_CacheWidth*m_TexSize, m_CacheHeight*m_TexSize, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);

	//================ キャッシュステータスの初期化 ===============//
	m_CacheStatus = new CacheStatus[ m_numMaxTiles ];
	InitCacheStatus();
}


TileCache::~TileCache()
{
cout << "TileCache::~TileCache()..." << endl;

	//================ タイルキャッシュの削除 ================//
	if(m_TileCache){ glDeleteTextures(1,	&m_TileCache); m_TileCache = NULL; }
	if(m_TileCache_normal){ glDeleteTextures(1, &m_TileCache_normal); m_TileCache_normal = NULL; }
	
	//================= キャッシュ情報の削除 ================//
	if(m_CacheStatus){ delete [] m_CacheStatus; m_CacheStatus = NULL; }
}


void TileCache::InitCacheStatus()
{
cout << "TileCache::InitCacheStatus()..." << endl;

	int i;
	
	for(i=0; i<m_numMaxTiles;  i++)
	{
		m_CacheStatus[i].reserved	= false;// 使用可能
		m_CacheStatus[i].dirty		= false;// 誰もアクセスしてない
		m_CacheStatus[i].mapid		= -1;
		m_CacheStatus[i].miplv		= -1;	// MIPMAPレベル（登録してないタイルのレベルはゼロ）
		m_CacheStatus[i].px			= -1;
		m_CacheStatus[i].py			= -1;

	}// end of i loop
}